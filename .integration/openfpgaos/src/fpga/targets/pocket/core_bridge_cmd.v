//
// bridge host/target command handler
// 2022 Analogue
//

// mapped to 0xF8xxxxxx on bridge
// the spec is loose enough to allow implementation with either
// block rams and a soft CPU, or simply hard logic with some case statements.
//
// the implementation spec is documented, and depending on your application you
// may want to completely replace this module. this is only one of many 
// possible ways to accomplish the host/target command system and data table.
//
// this module should always be clocked by a direct clock input and never a PLL, 
// because it should report PLL lock status
//

module core_bridge_cmd (

input   wire            clk,
output  reg             reset_n,

input   wire            bridge_endian_little,
input   wire    [31:0]  bridge_addr,
input   wire            bridge_rd,
output  reg     [31:0]  bridge_rd_data,
input   wire            bridge_wr,
input   wire    [31:0]  bridge_wr_data,

// Shutdown handshake: CPU flushes saves before reset
output  reg             shutdown_pending,
input   wire            shutdown_ack_s,

// all these signals should be synchronous to clk
// add synchronizers if these need to be used in other clock domains
input   wire            status_boot_done,           // assert when PLLs lock and logic is ready
input   wire            status_setup_done,          // assert when core is happy with what's been loaded into it
input   wire            status_running,             // assert when pocket's taken core out of reset and is running

output  reg             dataslot_requestread,
output  reg     [15:0]  dataslot_requestread_id,
input   wire            dataslot_requestread_ack,
input   wire            dataslot_requestread_ok,

output  reg             dataslot_requestwrite,
output  reg     [15:0]  dataslot_requestwrite_id,
output  reg     [31:0]  dataslot_requestwrite_size,
input   wire            dataslot_requestwrite_ack,
input   wire            dataslot_requestwrite_ok,

output  reg             dataslot_update,
output  reg     [15:0]  dataslot_update_id,
output  reg     [31:0]  dataslot_update_size,

output  reg             dataslot_allcomplete,
`ifdef CHISBURNER_SDRAM120
input wire storage_admission_blocked,
output wire storage_transfer_busy,
`endif

output  reg     [31:0]  rtc_epoch_seconds,
output  reg     [31:0]  rtc_date_bcd,
output  reg     [31:0]  rtc_time_bcd,
output  reg             rtc_valid,

input   wire            savestate_supported,
input   wire    [31:0]  savestate_addr,
input   wire    [31:0]  savestate_size,
input   wire    [31:0]  savestate_maxloadsize,

output  reg             osnotify_inmenu,
output  reg             osnotify_cart_toggle,
output  reg     [31:0]  osnotify_cart_value,
output  reg             osnotify_cart_valid,
output  reg     [31:0]  osnotify_cart_last_value,
output  reg     [7:0]   osnotify_cart_count,
output  reg             reset_enter_toggle,
output  reg             reset_exit_toggle,
output  reg     [7:0]   reset_enter_count,
output  reg     [7:0]   reset_exit_count,

output  reg             savestate_start,        // core should detect rising edge on this,
input   wire            savestate_start_ack,    // and then assert ack for at least 1 cycle
input   wire            savestate_start_busy,   // assert constantly while in progress after ack
input   wire            savestate_start_ok,     // assert continuously when done, and clear when new process is started
input   wire            savestate_start_err,    // assert continuously on error, and clear when new process is started

output  reg             savestate_load,
input   wire            savestate_load_ack,
input   wire            savestate_load_busy,
input   wire            savestate_load_ok,
input   wire            savestate_load_err,

input   wire            target_dataslot_read,       // rising edge triggered
input   wire            target_dataslot_write,
input   wire            target_dataslot_getfile,
input   wire            target_dataslot_openfile,
input   wire            target_dataslot_flush,
`ifdef CHISBURNER_STORAGE_TRACE
output  reg     [15:0]   storage_trace,
`endif

output  reg             target_dataslot_ack,        // asserted upon command start until completion
output  reg             target_dataslot_done,       // asserted upon command finish until next command is issued    
output  reg     [2:0]   target_dataslot_err,        // contains result of command execution. zero is OK

input   wire    [15:0]  target_dataslot_id,         // parameters for each of the read/reload/write commands
input   wire    [31:0]  target_dataslot_slotoffset,
input   wire    [31:0]  target_dataslot_bridgeaddr,
input   wire    [31:0]  target_dataslot_length,

input   wire    [31:0]  target_buffer_param_struct, // bus address of the memory region APF will fetch additional parameter struct from
input   wire    [31:0]  target_buffer_resp_struct,  // bus address of the memory region APF will write its response struct to
                                                    // this should be mapped by the developer, the buffer is not implemented in this file

input   wire    [9:0]   datatable_addr,
input   wire            datatable_wren,
input   wire    [31:0]  datatable_data,
output  wire    [31:0]  datatable_q

);

// handle endianness
    reg     [31:0]  bridge_wr_data_in;
    reg     [31:0]  bridge_rd_data_out;
    
    wire endian_little_s;
synch_3 s01(bridge_endian_little, endian_little_s, clk);

always @(*) begin
    bridge_rd_data <= endian_little_s ? {
        bridge_rd_data_out[7:0], 
        bridge_rd_data_out[15:8], 
        bridge_rd_data_out[23:16], 
        bridge_rd_data_out[31:24]
    } : bridge_rd_data_out;

    bridge_wr_data_in <= endian_little_s ? {
        bridge_wr_data[7:0], 
        bridge_wr_data[15:8], 
        bridge_wr_data[23:16], 
        bridge_wr_data[31:24]
    } : bridge_wr_data;
end                             

    
// minimalistic approach here - 
// keep the commonly used registers in logic, but data table in BRAM.
// implementation could be changed quite a bit for a more advanced use case

// host

    reg     [31:0]  host_0;
    reg     [31:0]  host_4 = 'h20; // host cmd parameter data at 0x20 
    reg     [31:0]  host_8 = 'h40; // host cmd response data at 0x40 
    
    reg     [31:0]  host_20; // parameter data
    reg     [31:0]  host_24;
    reg     [31:0]  host_28;

    reg     [31:0]  host_40; // response data
    reg     [31:0]  host_44;
    reg     [31:0]  host_48;
    
    reg             host_cmd_start;
    reg     [15:0]  host_cmd_startval;
    reg     [15:0]  host_cmd;
    reg     [15:0]  host_resultcode;
    
localparam  [3:0]   ST_IDLE         = 'd0;
localparam  [3:0]   ST_PARSE        = 'd1;
localparam  [3:0]   ST_WORK         = 'd2;
localparam  [3:0]   ST_CLEAR_DATATABLE = 'd3;
localparam  [3:0]   ST_DONE_OK      = 'd13;
localparam  [3:0]   ST_DONE_CODE    = 'd14;
localparam  [3:0]   ST_DONE_ERR     = 'd15;
    reg     [3:0]   hstate;
    
// target
    
    reg     [31:0]  target_0;
    reg     [31:0]  target_4 = 'h20; // target cmd parameter data at 0x20 
    reg     [31:0]  target_8 = 'h40; // target cmd response data at 0x40 
    
    reg     [31:0]  target_20; // parameter data
    reg     [31:0]  target_24;
    reg     [31:0]  target_28;
    reg     [31:0]  target_2C;
    
localparam  [3:0]   TARG_ST_IDLE        = 'd0;
localparam  [3:0]   TARG_ST_READYTORUN  = 'd1;
localparam  [3:0]   TARG_ST_DATASLOTOP  = 'd2;
localparam  [3:0]   TARG_ST_WAITRESULT_RTR  = 'd14;
localparam  [3:0]   TARG_ST_WAITRESULT_DSO  = 'd15;
    reg     [3:0]   tstate;
`ifdef CHISBURNER_SDRAM120
    reg storage_host_active = 1'b0;
    assign storage_transfer_busy = storage_host_active || (tstate != TARG_ST_IDLE);
    wire storage_can_launch = !storage_admission_blocked;
`else
    wire storage_can_launch = 1'b1;
`endif
    
// Shutdown timeout: ~100ms at 74.25MHz ≈ 7,425,000 cycles (23 bits)
    reg     [22:0]  shutdown_timeout;
    wire            shutdown_timeout_reached = shutdown_timeout[22];  // ~56ms conservative

    reg             status_setup_done_1, status_setup_done_queue;
    reg             warm_reset_ready_pending;
    reg             target_dataslot_read_1, target_dataslot_read_queue;
    reg             target_dataslot_write_1, target_dataslot_write_queue;
    reg             target_dataslot_getfile_1, target_dataslot_getfile_queue;
    reg             target_dataslot_openfile_1, target_dataslot_openfile_queue;
    reg             target_dataslot_flush_1, target_dataslot_flush_queue;

`ifdef CHISBURNER_STORAGE_TRACE
// Observation only: never feeds the command FSM. Sticky flags describe the
// most recent FLUSH, retained until the next FLUSH request. Sample only after
// a software timeout (the debug word crosses domains without a snapshot lock).
// [15:12] tstate; [11:9] prefix (1=cm,2=bu,3=ok,0=other);
// [8] current low command == 0188;
// [7:0] request, launch, host-read-command, host-read-slot, host-write-status,
//       host-wrote-busy, host-wrote-ok, queue-observed.
reg [7:0] storage_trace_flags = 8'd0;
reg storage_trace_window = 1'b0;
wire [2:0] storage_trace_prefix =
    target_0[31:16] == 16'h636d ? 3'd1 :
    target_0[31:16] == 16'h6275 ? 3'd2 :
    target_0[31:16] == 16'h6f6b ? 3'd3 : 3'd0;
wire storage_trace_cmd_addr =
    (bridge_addr & 32'hff00ffff) == 32'hf8001000;
wire storage_trace_slot_addr =
    (bridge_addr & 32'hff00ffff) == 32'hf8001020;
initial storage_trace = 16'd0;
always @(posedge clk) begin
    storage_trace <= {tstate, storage_trace_prefix,
                      target_0[15:0] == 16'h0188, storage_trace_flags};
    if (target_dataslot_flush && !target_dataslot_flush_1) begin
        storage_trace_window <= 1'b1;
        storage_trace_flags <= 8'h80;
    end else if (storage_trace_window) begin
        if (target_dataslot_flush_queue)
            storage_trace_flags[0] <= 1'b1;
        if (tstate == TARG_ST_DATASLOTOP && target_0[15:0] == 16'h0188)
            storage_trace_flags[6] <= 1'b1;
        if (storage_trace_flags[6]) begin
            if (bridge_rd && storage_trace_cmd_addr)
                storage_trace_flags[5] <= 1'b1;
            if (bridge_rd && storage_trace_slot_addr)
                storage_trace_flags[4] <= 1'b1;
            if (bridge_wr && storage_trace_cmd_addr) begin
                storage_trace_flags[3] <= 1'b1;
                if (bridge_wr_data_in[31:16] == 16'h6275)
                    storage_trace_flags[2] <= 1'b1;
                if (bridge_wr_data_in[31:16] == 16'h6f6b)
                    storage_trace_flags[1] <= 1'b1;
            end
        end
    end
end
`endif
    
    
initial begin
    reset_n <= 0;
    dataslot_requestread <= 0;
    dataslot_requestwrite <= 0;
    dataslot_update <= 0;
    dataslot_allcomplete <= 0;
    rtc_valid <= 0;
    savestate_start <= 0;
    savestate_load <= 0;
    osnotify_inmenu <= 0;
    osnotify_cart_toggle <= 0;
    osnotify_cart_value <= 0;
    osnotify_cart_valid <= 0;
    osnotify_cart_last_value <= 0;
    osnotify_cart_count <= 0;
    reset_enter_toggle <= 0;
    reset_exit_toggle <= 0;
    reset_enter_count <= 0;
    reset_exit_count <= 0;
    shutdown_pending <= 0;

    status_setup_done_queue <= 0;
    warm_reset_ready_pending <= 0;
    target_dataslot_read_queue <= 0;
    target_dataslot_write_queue <= 0;
    target_dataslot_getfile_queue <= 0;
    target_dataslot_openfile_queue <= 0;
    target_dataslot_flush_queue <= 0;
    target_dataslot_ack <= 0;
    target_dataslot_done <= 0;
    target_dataslot_err <= 0;
    b_datatable_clear_active <= 0;
    b_datatable_clear_addr <= 0;
end
    
always @(posedge clk) begin

    // detect a rising edge on the input signal
    // and flag a queue that will be cleared later
    status_setup_done_1 <= status_setup_done;
    target_dataslot_read_1 <= target_dataslot_read;
    target_dataslot_write_1 <= target_dataslot_write;
    target_dataslot_getfile_1 <= target_dataslot_getfile;
    target_dataslot_openfile_1 <= target_dataslot_openfile;
    target_dataslot_flush_1 <= target_dataslot_flush;
    
    if(status_setup_done & ~status_setup_done_1) begin
        status_setup_done_queue <= 1;
    end
    if(target_dataslot_read & ~target_dataslot_read_1) begin
        target_dataslot_read_queue <= 1;
    end
    if(target_dataslot_write & ~target_dataslot_write_1) begin
        target_dataslot_write_queue <= 1;
    end
    if(target_dataslot_getfile & ~target_dataslot_getfile_1) begin
        target_dataslot_getfile_queue <= 1;
    end
    if(target_dataslot_openfile & ~target_dataslot_openfile_1) begin
        target_dataslot_openfile_queue <= 1;
    end
    if(target_dataslot_flush & ~target_dataslot_flush_1) begin
        target_dataslot_flush_queue <= 1;
    end
    
    
    b_datatable_wren <= 0;
    b_datatable_addr <= bridge_addr >> 2;
        
    if(bridge_wr) begin
        casex(bridge_addr)
        32'hF8xx00xx: begin
            case(bridge_addr[7:0])
            8'h0: begin
                host_0 <= bridge_wr_data_in; // command/status
                // check for command 
                if(bridge_wr_data_in[31:16] == 16'h434D) begin
                    // host wants us to do a command
                    host_cmd_startval <= bridge_wr_data_in[15:0];
                    host_cmd_start <= 1;
                end
            end
            8'h20: host_20 <= bridge_wr_data_in; // parameter data regs
            8'h24: host_24 <= bridge_wr_data_in;
            8'h28: host_28 <= bridge_wr_data_in;
            endcase
        end
        32'hF8xx10xx: begin
            case(bridge_addr[7:0])
            8'h0: target_0 <= bridge_wr_data_in; // command/status
            8'h4: target_4 <= bridge_wr_data_in; // parameter data pointer
            8'h8: target_8 <= bridge_wr_data_in; // response data pointer
            endcase
        end
        32'hF8xx2xxx: begin
            b_datatable_wren <= 1;
        end
        endcase
    end
    if(b_datatable_clear_active) begin
        if(b_datatable_clear_addr < 5'd16) begin
            b_datatable_wren <= 1;
            b_datatable_addr <= {5'd0, b_datatable_clear_addr};
            b_datatable_clear_addr <= b_datatable_clear_addr + 5'd1;
        end else begin
            b_datatable_clear_active <= 0;
        end
    end
    if(bridge_rd) begin
        casex(bridge_addr)
        32'hF8xx00xx: begin
            case(bridge_addr[7:0])
            8'h0: bridge_rd_data_out <= host_0; // command/status
            8'h4: bridge_rd_data_out <= host_4; // parameter data pointer
            8'h8: bridge_rd_data_out <= host_8; // response data pointer
            8'h40: bridge_rd_data_out <= host_40; // response data regs
            8'h44: bridge_rd_data_out <= host_44;
            8'h48: bridge_rd_data_out <= host_48;
            8'h4C: bridge_rd_data_out <= 32'h0;
            endcase
        end
        32'hF8xx10xx: begin
            case(bridge_addr[7:0])
            8'h0: bridge_rd_data_out <= target_0;
            8'h4: bridge_rd_data_out <= target_4;
            8'h8: bridge_rd_data_out <= target_8;
            8'h20: bridge_rd_data_out <= target_20; // parameter data regs
            8'h24: bridge_rd_data_out <= target_24;
            8'h28: bridge_rd_data_out <= target_28;
            8'h2C: bridge_rd_data_out <= target_2C;
            endcase
        end
        32'hF8xx2xxx: begin
            bridge_rd_data_out <= b_datatable_q;
        
        end
        endcase
    end


    
    
    
    // host > target command executer
    case(hstate)
    ST_IDLE: begin
    
        dataslot_requestread <= 0;
        dataslot_requestwrite <= 0;
        dataslot_update <= 0;
        savestate_start <= 0;
        savestate_load <= 0;
        
        // there is no queueing. pocket will always make sure any outstanding host
        // commands are finished before starting another
        if(host_cmd_start) begin
            host_cmd_start <= 0;
            // save the command in case it gets clobbered later
            host_cmd <= host_cmd_startval;
            hstate <= ST_PARSE;
        end
    
    end
    ST_PARSE: begin
        // overwrite command semaphore with busy flag
        host_0 <= {16'h4255, host_cmd};
        
        case(host_cmd)
        16'h0000: begin
            // Request Status
            host_resultcode <= 1; // default: booting
            if(status_boot_done) begin
                host_resultcode <= 2; // setup
                if(status_setup_done) begin
                    host_resultcode <= 3; // idle
                end else if(status_running) begin
                    host_resultcode <= 4; // running
                end 
            end
            hstate <= ST_DONE_CODE;
        end
        16'h0010: begin
            // Reset Enter — shutdown handshake
            // Assert shutdown_pending so CPU can flush saves.
            // Wait for shutdown_ack or timeout (~100ms ≈ 7.4M cycles at 74.25MHz).
            if (!shutdown_pending) begin
                shutdown_pending <= 1;
                shutdown_timeout <= 0;
            end else if (shutdown_ack_s || shutdown_timeout_reached) begin
                // CPU acknowledged or timeout — proceed with reset
                reset_n <= 0;
                reset_enter_toggle <= ~reset_enter_toggle;
                reset_enter_count <= reset_enter_count + 1'b1;
                osnotify_cart_valid <= 0;
                osnotify_cart_value <= 0;
                shutdown_pending <= 0;
                dataslot_allcomplete <= 0;
                tstate <= TARG_ST_IDLE;
                target_dataslot_ack <= 0;
                target_dataslot_done <= 0;
                target_dataslot_err <= 0;
                // A menu "Load Game" / instance switch can reset and reload
                // slots without reconfiguring the FPGA.  In that warm path
                // status_setup_done is already high, so its edge detector will
                // not fire again.  Delay the fresh Ready-To-Run until after
                // stale read-only datatable entries have been cleared.
                status_setup_done_queue <= 0;
                warm_reset_ready_pending <= reset_n && status_setup_done;
                target_dataslot_read_queue <= 0;
                target_dataslot_write_queue <= 0;
                target_dataslot_getfile_queue <= 0;
                target_dataslot_openfile_queue <= 0;
                target_dataslot_flush_queue <= 0;
                // A warm instance switch can leave datatable entries for
                // undeclared read-only slots from the previous instance.
                // Clear entries 0..7 while preserving pre-save config and
                // save slots at entries 8..18; APF reload writes will
                // repopulate only the current instance's read-only slots.
                b_datatable_clear_active <= 1;
                b_datatable_clear_addr <= 0;
                hstate <= ST_CLEAR_DATATABLE;
            end else begin
                shutdown_timeout <= shutdown_timeout + 1;
            end
        end
        16'h0011: begin
            // Reset Exit
            reset_n <= 1;
            reset_exit_toggle <= ~reset_exit_toggle;
            reset_exit_count <= reset_exit_count + 1'b1;
            shutdown_pending <= 0;
            hstate <= ST_DONE_OK;
        end
        16'h0080: begin
            // Data slot request read
            dataslot_allcomplete <= 0;
            dataslot_requestread <= 1;
            dataslot_requestread_id <= host_20[15:0];
            if(dataslot_requestread_ack) begin
                host_resultcode <= 0;
                if(!dataslot_requestread_ok || !storage_can_launch) host_resultcode <= 2;
`ifdef CHISBURNER_SDRAM120
                else storage_host_active <= 1'b1;
`endif
                hstate <= ST_DONE_CODE;
            end
        end
        16'h0082: begin
            // Data slot request write
            dataslot_allcomplete <= 0;
            dataslot_requestwrite <= 1;
            dataslot_requestwrite_id <= host_20[15:0];
            dataslot_requestwrite_size <= host_24;
            if(dataslot_requestwrite_ack) begin
                host_resultcode <= 0;
                if(!dataslot_requestwrite_ok || !storage_can_launch) host_resultcode <= 2;
`ifdef CHISBURNER_SDRAM120
                else storage_host_active <= 1'b1;
`endif
                hstate <= ST_DONE_CODE;
            end
        end
        16'h008A: begin
            // Data slot update (sent on deferload marked slots only)
            dataslot_update <= 1;
            dataslot_update_id <= host_20[15:0];
            dataslot_update_size <= host_24;
            hstate <= ST_DONE_OK;
        end
        16'h008F: begin
            // Data slot access all complete
            dataslot_allcomplete <= 1;
`ifdef CHISBURNER_SDRAM120
            storage_host_active <= 1'b0;
`endif
            hstate <= ST_DONE_OK;
        end
        16'h0090: begin
            // Real-time Clock Data
            // user logic should detect rising edge, it is not continuously updated
            rtc_valid <= 1;
            rtc_epoch_seconds <= host_20;
            rtc_date_bcd <= host_24;
            rtc_time_bcd <= host_28;
            hstate <= ST_DONE_OK;
        end
        16'h00A0: begin
            // Savestate: Start/Query
            host_40 <= savestate_supported; 
            host_44 <= savestate_addr;
            host_48 <= savestate_size;
            
            host_resultcode <= 0;
            if(savestate_start_busy) host_resultcode <= 1;
            if(savestate_start_ok) host_resultcode <= 2;
            if(savestate_start_err) host_resultcode <= 3;
            
            if(host_20[0]) begin
                // Request Start!
                savestate_start <= 1;
                // stay in this state until ack'd
                if(savestate_start_ack) begin
                    hstate <= ST_DONE_CODE;
                end
            end else begin
                hstate <= ST_DONE_CODE;
            end
        end
        16'h00A4: begin
            // Savestate: Load/Query
            host_40 <= savestate_supported; 
            host_44 <= savestate_addr;
            host_48 <= savestate_maxloadsize;
            
            host_resultcode <= 0;
            if(savestate_load_busy) host_resultcode <= 1;
            if(savestate_load_ok) host_resultcode <= 2;
            if(savestate_load_err) host_resultcode <= 3;
            
            if(host_20[0]) begin
                // Request Load!
                savestate_load <= 1;
                // stay in this state until ack'd
                if(savestate_load_ack) begin
                    hstate <= ST_DONE_CODE;
                end
            end else begin
                hstate <= ST_DONE_CODE;
            end
        end
        16'h00B0: begin
            // OS Notify: Menu State
            osnotify_inmenu <= host_20[0];
            hstate <= ST_DONE_OK;
        end
        // DOCK FIX 2026-06-11: the three OS-Notify commands below used to
        // fall into `default` and answer 0x4F4BFFFF ("unknown command").
        // 0x00B2 is sent by the host exactly when the Pocket is docked —
        // an ERR reply there is the one signal path that fires ONLY in the
        // dock-and-broken case (LCD never sends it), and is the prime
        // suspect for the host refusing to bring core video up on HDMI
        // (orange placeholder).  Notifies must be ACKed OK like 0x00B0.
        16'h00B1: begin
            // OS Notify: Cartridge Adapter.  Latch the parameter and toggle
            // an event bit so another clock domain cannot miss a short pulse.
            osnotify_cart_value <= host_20;
            osnotify_cart_valid <= 1;
            osnotify_cart_last_value <= host_20;
            osnotify_cart_count <= osnotify_cart_count + 1'b1;
            osnotify_cart_toggle <= ~osnotify_cart_toggle;
            hstate <= ST_DONE_OK;
        end
        16'h00B2: begin
            // OS Notify: Docked State — informational; ACK OK.  No docked-VRR
            // policy, so the docked flag is not latched.
            hstate <= ST_DONE_OK;
        end
        16'h00B8: begin
            // OS Notify: Display Mode — the docs note some modes mandate
            // acknowledgment; ACK OK.  (Grayscale-mandating modes would
            // additionally want grayscale rendering — not gated here.)
            hstate <= ST_DONE_OK;
        end
        default: begin
            hstate <= ST_DONE_ERR;
        end
        endcase
    end
    ST_WORK: begin
        hstate <= ST_IDLE;
    end
    ST_CLEAR_DATATABLE: begin
        if(!b_datatable_clear_active) begin
            status_setup_done_queue <= warm_reset_ready_pending;
            warm_reset_ready_pending <= 0;
            hstate <= ST_DONE_OK;
        end
    end
    ST_DONE_OK: begin
        host_0 <= 32'h4F4B0000; // result code 0
        hstate <= ST_IDLE;
    end
    ST_DONE_CODE: begin
        host_0 <= {16'h4F4B, host_resultcode};
        hstate <= ST_IDLE;
    end
    ST_DONE_ERR: begin
        host_0 <= 32'h4F4BFFFF; // result code FFFF = unknown command
        hstate <= ST_IDLE;
    end
    endcase
    
    
    // target > host command executer
    case(tstate)
    TARG_ST_IDLE: begin
        // ACK is a level handshake, not a completion pulse. Normally APF
        // spends long enough in BUSY that the CPU has already seen ACK and
        // dropped its request before DONE. FLUSH can return OK immediately,
        // though; in that path keep ACK asserted until the CPU-side request
        // has crossed back low. This prevents a fast completion from becoming
        // an unobservable one-clock pulse at the CDC boundary.
        if (!(target_dataslot_read || target_dataslot_write ||
              target_dataslot_getfile || target_dataslot_openfile ||
              target_dataslot_flush))
            target_dataslot_ack <= 0;
    
        if(status_setup_done_queue) begin
            status_setup_done_queue <= 0;
            tstate <= TARG_ST_READYTORUN;
            
        end else if(target_dataslot_read_queue && storage_can_launch) begin
            target_dataslot_read_queue <= 0;
            // Keep the boot/small-file path on the proven 32-bit APF
            // command.  Switch to the 48-bit form only for offsets with
            // bit31 set, where some host paths appear to mishandle the
            // legacy command as signed.
            target_0[15:0] <= target_dataslot_slotoffset[31] ? 16'h0181 : 16'h0180;

            target_20 <= {16'd0, target_dataslot_id};
            target_24 <= target_dataslot_slotoffset;
            target_28 <= target_dataslot_bridgeaddr;
            target_2C <= target_dataslot_length;
            
            tstate <= TARG_ST_DATASLOTOP;
            
        end else if(target_dataslot_write_queue && storage_can_launch) begin
            target_dataslot_write_queue <= 0;
            target_0[15:0] <= target_dataslot_slotoffset[31] ? 16'h0185 : 16'h0184;

            target_20 <= {16'd0, target_dataslot_id};
            target_24 <= target_dataslot_slotoffset;
            target_28 <= target_dataslot_bridgeaddr;
            target_2C <= target_dataslot_length;

            tstate <= TARG_ST_DATASLOTOP;

        end else if(target_dataslot_getfile_queue && storage_can_launch) begin
            target_dataslot_getfile_queue <= 0;
            target_0[15:0] <= 16'h0190;
            
            target_20 <= target_dataslot_id;
            target_24 <= target_buffer_resp_struct; // pointer to the bram that will hold the response struct
                                                    // which will contain the requested filename before command completion
            tstate <= TARG_ST_DATASLOTOP;
            
        end else if(target_dataslot_openfile_queue && storage_can_launch) begin
            target_dataslot_openfile_queue <= 0;
            target_0[15:0] <= 16'h0192;
            
            target_20 <= target_dataslot_id;
            target_24 <= target_buffer_param_struct; // pointer to the bram that will hold the parameter struct
                                                    // which must contain the desired filename and flag/size before command execution
            tstate <= TARG_ST_DATASLOTOP;

        // FLUSH commits a host-side deferred file and carries no bridge
        // payload address.  It must not wait on the SDRAM ingress/snapshot
        // admission lock: the preceding WRITE has already completed and the
        // firmware has observed WR_IDLE before issuing this command.  Gating
        // FLUSH here can strand the request forever in the short ownership
        // hand-back CDC window (CPU sees the lock released before the bridge
        // domain sees admission reopen), leaving ACK/DONE clear even though
        // DS_COMMAND was accepted by the CPU peripheral.
        end else if(target_dataslot_flush_queue) begin
            target_dataslot_flush_queue <= 0;
            target_0[15:0] <= 16'h0188;

            target_20 <= {16'd0, target_dataslot_id};
            target_24 <= 32'd0;
            target_28 <= 32'd0;
            target_2C <= 32'd0;

            tstate <= TARG_ST_DATASLOTOP;
        end 
    end
    TARG_ST_READYTORUN: begin
        target_0 <= 32'h636D_0140;
        tstate <= TARG_ST_WAITRESULT_RTR;
    end
    TARG_ST_DATASLOTOP: begin
        target_0[31:16] <= 16'h636D;
        
        target_dataslot_done <= 0;
        target_dataslot_err <= 0;
        tstate <= TARG_ST_WAITRESULT_DSO;
    end
    TARG_ST_WAITRESULT_DSO: begin
        if(target_0[31:16] == 16'h6275) begin
            target_dataslot_ack <= 1;
        end
        if(target_0[31:16] == 16'h6F6B) begin
            // done
            // A fast command such as FLUSH may complete without the bridge
            // clock ever observing an intermediate BUSY response.  DONE is
            // itself proof that the host accepted the command, so raise ACK
            // for one clock as well; otherwise the CPU-side handshake waits
            // forever for an ACK that can no longer arrive.
            target_dataslot_ack <= 1;
            // save result code
            target_dataslot_err <= target_0[2:0];
            // assert done
            target_dataslot_done <= 1;
            tstate <= TARG_ST_IDLE;
        end
    end
    TARG_ST_WAITRESULT_RTR: begin
        if(target_0[31:16] == 16'h6F6B) begin
            // done
            tstate <= TARG_ST_IDLE;
        end
    
    end
    endcase
    

end

    wire    [31:0]  b_datatable_q;
    reg     [9:0]   b_datatable_addr;
    reg             b_datatable_wren;
    reg             b_datatable_clear_active;
    reg     [4:0]   b_datatable_clear_addr;
    wire    [31:0]  b_datatable_data =
        b_datatable_clear_active ? 32'd0 : bridge_wr_data_in;

mf_datatable idt (
    .address_a      ( datatable_addr ),
    .address_b      ( b_datatable_addr ),
    .clock_a        ( clk ),
    .clock_b        ( clk ),
    .data_a         ( datatable_data ),
    .data_b         ( b_datatable_data ),
    .wren_a         ( datatable_wren ),
    .wren_b         ( b_datatable_wren ),
    .q_a            ( datatable_q ),
    .q_b            ( b_datatable_q )
);


endmodule
