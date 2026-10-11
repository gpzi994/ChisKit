//------------------------------------------------------------------------------
// ChisBurner - fixed-command GB/GBC MBC5 NOR identifier.
//
// The host may request repeatable 32-byte IDENTIFY snapshots.  It cannot supply a
// cartridge write address or byte: every write below is a fixed reset, JEDEC
// autoselect, or CFI-query command copied from Mori's standard MBC5 path.
// Erase/program opcodes and generic ROM/save access are structurally absent.
//------------------------------------------------------------------------------

`default_nettype none

module chisburner_cart_gb_mbc5_identify #(
    parameter integer GEOMETRY_PROFILE = 0,
    parameter integer ADDRESS_SETUP_CYCLES = 18,
    parameter integer SELECT_SETUP_CYCLES  = 9,
    parameter integer DATA_WAIT_CYCLES     = 36,
    parameter integer RELEASE_CYCLES       = 9,
    parameter integer WRITE_SETUP_CYCLES   = 18,
    parameter integer WRITE_LOW_CYCLES     = 36,
    parameter integer WRITE_RELEASE_CYCLES = 18
) (
    input  wire        clk,
    input  wire        reset_n,
    input  wire        lifecycle_ok,
    input  wire [31:0] lifecycle_status,
    input  wire [31:0] host_b1_raw,
    input  wire [31:0] host_diag,
    input  wire        reg_wr,
    input  wire        reg_rd,
    input  wire [4:0]  reg_addr,
    // Dedicated registered write address.  The read address remains purely
    // combinational and no longer feeds write/state control timing paths.
    input  wire [4:0]  reg_wr_addr,
    input  wire [31:0] reg_wdata,
    output reg  [31:0] reg_rdata,
    input  wire [7:0]  bank3_in,
    input  wire [7:0]  bank2_in,
    output wire [7:0]  bank3_out,
    output wire        bank3_dir,
    output wire [7:0]  bank2_out,
    output wire        bank2_dir,
    output wire [7:0]  bank1_out,
    output wire        bank1_dir,
    output wire [3:0]  bank0_out,
    output wire        bank0_dir,
    output wire        pin30_out,
    output wire        pin30_dir,
    output wire        pin30_pwroff_reset,
    output wire        pin31_out,
    output wire        pin31_dir,
    output wire        writes_enabled
);

localparam [4:0] REG_ID=0, REG_CAPS=1, REG_STATUS=2, REG_COMMAND=3,
                 REG_BUS=4, REG_ADDRESS=5, REG_LENGTH=6, REG_TIMEOUT=7,
                 REG_RX_DATA=8, REG_RX_LEVEL=9, REG_ERROR=10,
                 REG_LIFECYCLE=11, REG_HOST_B1_RAW=12, REG_HOST_DIAG=13,
                 REG_QUERY_ARG=14, REG_QUERY_RESULT=15, REG_SPACE=16,
                 REG_PROFILE=20;

localparam [31:0] CART_ID=32'h3142_4746;
// GB/GBC bus + read/safe/abort/query + GB IDENTIFY space only.
localparam [31:0] CART_CAPS=32'h0010_0F01;
localparam [31:0] CMD_SAFE_IDLE=1, CMD_READ=2, CMD_ABORT=3,
                 CMD_QUERY_FW_INFO=32'hA1, CMD_PING=32'hFE;
localparam [31:0] ST_IDLE=1, ST_BUSY=2, ST_DONE=4, ST_ERROR=8,
                 ST_ABORTED=16, ST_TIMEOUT=32;
localparam [31:0] ERR_NONE=0, ERR_BAD_COMMAND=1, ERR_LIFECYCLE=3,
                 ERR_TIMEOUT=4;
localparam [31:0] QUERY_SIGNATURE=32'h5042_4746,
                 QUERY_VERSION=32'h0001_0000,
                 QUERY_FEATURES=32'h0000_0003;

localparam [3:0] S_IDLE=0, S_R_ADDRESS=1, S_R_SELECT=2, S_R_DATA=3,
                 S_R_RELEASE=4, S_W_ADDRESS=5, S_W_DATA=6,
                 S_W_PULSE=7, S_W_RELEASE=8;
localparam [4:0] P_NONE=0, P_DUMMY=1, P_RESET_BEGIN=2, P_BASE=3,
                 P_ID_AA=4, P_ID_55=5, P_ID_90=6, P_ID_READ=7,
                 P_RESET_ID=8, P_CFI1_98=9, P_CFI_QRY=10,
                 P_CFI_RETRY_RESET=11, P_CFI2_98=12, P_CFI_BODY=13,
                 P_RESET_END=14, P_FINAL_READ=15, P_RECOVERY_F0=16,
                 P_RECOVERY_FF=17;

reg [3:0] state;
reg [4:0] probe_phase;
reg [31:0] status_reg, error_reg, bus_reg, space_reg, address_reg,
           length_reg, timeout_reg, query_arg_reg, query_result_reg;
reg [31:0] phase_counter, watchdog_counter, fifo_count, fifo_read_index;
reg [5:0] probe_index;
reg [1:0] cfi_method;
reg q_ok, r_ok, y_ok, id_changed, final_normal;
reg [31:0] recovery_status, recovery_error;
reg [7:0] rx_fifo [0:31];
reg [7:0] base_byte [0:3];

reg [15:0] gb_address;
reg        gb_address_dir;
reg [7:0]  gb_write_data;
reg        gb_data_dir, gb_rd_n, gb_wr_n;

wire active = state != S_IDLE;
wire timing_valid = ADDRESS_SETUP_CYCLES > 0 && SELECT_SETUP_CYCLES > 0 &&
                    DATA_WAIT_CYCLES > 0 && RELEASE_CYCLES > 0 &&
                    WRITE_SETUP_CYCLES > 0 && WRITE_LOW_CYCLES > 0 &&
                    WRITE_RELEASE_CYCLES > 0;
wire identify_shape_valid = bus_reg == 32'd1 && space_reg == 32'd4 &&
                            address_reg == 0 && length_reg == 32 &&
                            timeout_reg != 0;
wire qry_valid = q_ok && r_ok && y_ok;
wire [31:0] profile_value = {27'd0, final_normal, cfi_method,
                             id_changed, qry_valid};

function [15:0] snapshot_address;
    input [5:0] index;
    begin
        if (GEOMETRY_PROFILE != 0) begin
            case (index)
                4,12: snapshot_address=16'h005A;
                5,13: snapshot_address=16'h005C;
                6,14: snapshot_address=16'h005E;
                7,15: snapshot_address=16'h0060;
                8,16: snapshot_address=16'h0062;
                9,17: snapshot_address=16'h0064;
                10,18: snapshot_address=16'h0066;
                11,19: snapshot_address=16'h0068;
                20: snapshot_address=16'h0080;
                21: snapshot_address=16'h0082;
                22: snapshot_address=16'h0084;
                23: snapshot_address=16'h0086;
                24: snapshot_address=16'h0088;
                25: snapshot_address=16'h008A;
                26: snapshot_address=16'h008C;
                27: snapshot_address=16'h009E;
                28: snapshot_address=16'h004E;
                29: snapshot_address=16'h0054;
                30: snapshot_address=16'h0056;
                default: snapshot_address=16'h0058;
            endcase
        end else case (index)
            0,4: snapshot_address=16'h0000;
            1,5: snapshot_address=16'h0002;
            2,6: snapshot_address=16'h001C;
            3,7: snapshot_address=16'h001E;
            8: snapshot_address=16'h0020;
            9: snapshot_address=16'h0022;
            10: snapshot_address=16'h0024;
            11: snapshot_address=16'h0026;
            12: snapshot_address=16'h0028;
            13: snapshot_address=16'h002A;
            14: snapshot_address=16'h002C;
            15: snapshot_address=16'h002E;
            16: snapshot_address=16'h0030;
            17: snapshot_address=16'h0032;
            18: snapshot_address=16'h0034;
            19: snapshot_address=16'h0036;
            20: snapshot_address=16'h0038;
            21: snapshot_address=16'h003A;
            22: snapshot_address=16'h003C;
            23: snapshot_address=16'h003E;
            24: snapshot_address=16'h004E;
            25: snapshot_address=16'h0054;
            26: snapshot_address=16'h0056;
            27: snapshot_address=16'h0058;
            28: snapshot_address=16'h005A;
            29: snapshot_address=16'h005C;
            30: snapshot_address=16'h005E;
            default: snapshot_address=16'h0060;
        endcase
    end
endfunction

function [15:0] id_address;
    input [1:0] index;
    begin
        case (index)
            0: id_address=16'h0000;
            1: id_address=16'h0002;
            2: id_address=16'h001C;
            default: id_address=16'h001E;
        endcase
    end
endfunction

// Every write address and byte is selected solely by an internal phase.
always @(*) begin
    gb_address=16'd0; gb_address_dir=1'b0; gb_write_data=8'd0;
    gb_data_dir=1'b0; gb_rd_n=1'b1; gb_wr_n=1'b1;
    case (state)
        S_R_ADDRESS, S_R_SELECT, S_R_DATA, S_R_RELEASE: begin
            gb_address = (probe_phase == P_DUMMY ||
                          probe_phase == P_FINAL_READ) ? 16'h0000 :
                         (probe_phase == P_BASE ||
                          probe_phase == P_ID_READ) ?
                             id_address(probe_index[1:0]) :
                         (probe_phase == P_CFI_QRY) ?
                             (16'h0010 + {9'd0,probe_index,1'b0}) :
                         snapshot_address(probe_index);
            gb_address_dir=1'b1;
            gb_rd_n=(state == S_R_DATA) ? 1'b0 : 1'b1;
        end
        S_W_ADDRESS, S_W_DATA, S_W_PULSE, S_W_RELEASE: begin
            gb_address_dir=1'b1; gb_data_dir=1'b1;
            case (probe_phase)
                P_ID_AA: begin gb_address=16'h0AAA; gb_write_data=8'hAA; end
                P_ID_55: begin gb_address=16'h0555; gb_write_data=8'h55; end
                P_ID_90: begin gb_address=16'h0AAA; gb_write_data=8'h90; end
                P_CFI1_98: begin gb_address=16'h00AA; gb_write_data=8'h98; end
                P_CFI2_98: begin gb_address=16'h0000; gb_write_data=8'h98; end
                P_RESET_END: begin
                    gb_address=16'h0000;
                    gb_write_data=(cfi_method == 2) ? 8'hFF : 8'hF0;
                end
                P_RECOVERY_FF: begin gb_address=16'h0000; gb_write_data=8'hFF; end
                default: begin gb_address=16'h0000; gb_write_data=8'hF0; end
            endcase
            gb_wr_n=(state == S_W_PULSE) ? 1'b0 : 1'b1;
        end
        default: begin end
    endcase
end

always @(*) begin
    reg_rdata=0;
    case (reg_addr)
        REG_ID: reg_rdata=CART_ID;
        REG_CAPS: reg_rdata=CART_CAPS;
        REG_STATUS: reg_rdata=status_reg;
        REG_BUS: reg_rdata=bus_reg;
        REG_SPACE: reg_rdata=space_reg;
        REG_ADDRESS: reg_rdata=address_reg;
        REG_LENGTH: reg_rdata=length_reg;
        REG_TIMEOUT: reg_rdata=timeout_reg;
        REG_RX_LEVEL: reg_rdata=fifo_count;
        REG_ERROR: reg_rdata=error_reg;
        REG_LIFECYCLE: reg_rdata=lifecycle_status;
        REG_HOST_B1_RAW: reg_rdata=host_b1_raw;
        REG_HOST_DIAG: reg_rdata=host_diag;
        REG_QUERY_ARG: reg_rdata=query_arg_reg;
        REG_QUERY_RESULT: reg_rdata=query_result_reg;
        REG_PROFILE: reg_rdata=profile_value;
        REG_RX_DATA: begin
            if (fifo_count > 0) reg_rdata[7:0]=rx_fifo[fifo_read_index];
            if (fifo_count > 1) reg_rdata[15:8]=rx_fifo[fifo_read_index+1];
            if (fifo_count > 2) reg_rdata[23:16]=rx_fifo[fifo_read_index+2];
            if (fifo_count > 3) reg_rdata[31:24]=rx_fifo[fifo_read_index+3];
        end
        default: reg_rdata=0;
    endcase
end

always @(posedge clk or negedge reset_n) begin
    if (!reset_n) begin
        state<=S_IDLE; probe_phase<=P_NONE; status_reg<=ST_IDLE;
        error_reg<=ERR_NONE; bus_reg<=0; space_reg<=0; address_reg<=0;
        length_reg<=0; timeout_reg<=0; query_arg_reg<=0;
        query_result_reg<=0; phase_counter<=0; watchdog_counter<=0;
        fifo_count<=0; fifo_read_index<=0; probe_index<=0; cfi_method<=0;
        q_ok<=0; r_ok<=0; y_ok<=0; id_changed<=0; final_normal<=0;
        recovery_status<=ST_IDLE; recovery_error<=0;
    end else if (reg_wr && reg_wr_addr == REG_COMMAND && reg_wdata == CMD_ABORT) begin
        if (active && lifecycle_ok) begin
            state<=S_W_ADDRESS; probe_phase<=P_RECOVERY_F0;
            recovery_status<=ST_IDLE|ST_ABORTED; recovery_error<=ERR_NONE;
            status_reg<=ST_BUSY; error_reg<=ERR_NONE; phase_counter<=0;
            watchdog_counter<=0;
        end else begin
            state<=S_IDLE; probe_phase<=P_NONE;
            status_reg<=ST_IDLE|ST_ABORTED; error_reg<=ERR_NONE;
            phase_counter<=0; watchdog_counter<=0;
        end
    end else if (active && !lifecycle_ok) begin
        state<=S_IDLE; probe_phase<=P_NONE; status_reg<=ST_IDLE|ST_ERROR;
        error_reg<=ERR_LIFECYCLE; phase_counter<=0; watchdog_counter<=0;
    end else if (active && watchdog_counter + 1 >= timeout_reg &&
                 probe_phase != P_RECOVERY_F0 &&
                 probe_phase != P_RECOVERY_FF) begin
        state<=S_W_ADDRESS; probe_phase<=P_RECOVERY_F0;
        recovery_status<=ST_IDLE|ST_ERROR|ST_TIMEOUT;
        recovery_error<=ERR_TIMEOUT; status_reg<=ST_BUSY;
        error_reg<=ERR_NONE; phase_counter<=0; watchdog_counter<=0;
    end else begin
        if (active) watchdog_counter<=watchdog_counter+1'b1;

        if (reg_rd && reg_addr == REG_RX_DATA && !active && fifo_count != 0) begin
            if (fifo_count > 4) begin
                fifo_read_index<=fifo_read_index+4; fifo_count<=fifo_count-4;
            end else begin fifo_read_index<=0; fifo_count<=0; end
        end

        if (reg_wr && !active) begin
            case (reg_wr_addr)
                REG_BUS: bus_reg<=reg_wdata;
                REG_SPACE: space_reg<=reg_wdata;
                REG_ADDRESS: address_reg<=reg_wdata;
                REG_LENGTH: length_reg<=reg_wdata;
                REG_TIMEOUT: timeout_reg<=reg_wdata;
                REG_QUERY_ARG: query_arg_reg<=reg_wdata;
                REG_COMMAND: begin
                    if (reg_wdata == CMD_SAFE_IDLE) begin
                        status_reg<=ST_IDLE; error_reg<=ERR_NONE;
                        fifo_count<=0; fifo_read_index<=0;
                    end else if (reg_wdata == CMD_READ) begin
                        if (!timing_valid || !lifecycle_ok ||
                            !identify_shape_valid) begin
                            status_reg<=ST_IDLE|ST_ERROR;
                            error_reg<=lifecycle_ok ? ERR_BAD_COMMAND :
                                                   ERR_LIFECYCLE;
                        end else begin
                            state<=S_R_ADDRESS; probe_phase<=P_DUMMY;
                            status_reg<=ST_BUSY; error_reg<=ERR_NONE;
                            phase_counter<=0; watchdog_counter<=0;
                            fifo_count<=0; fifo_read_index<=0; probe_index<=0;
                            cfi_method<=0; q_ok<=0; r_ok<=0; y_ok<=0;
                            id_changed<=0; final_normal<=0;
                        end
                    end else if (reg_wdata == CMD_QUERY_FW_INFO) begin
                        error_reg<=ERR_NONE; status_reg<=ST_IDLE|ST_DONE;
                        case (query_arg_reg)
                            0: query_result_reg<=QUERY_SIGNATURE;
                            1: query_result_reg<=QUERY_VERSION;
                            2: query_result_reg<=QUERY_FEATURES;
                            default: begin query_result_reg<=0;
                                error_reg<=ERR_BAD_COMMAND;
                                status_reg<=ST_IDLE|ST_ERROR; end
                        endcase
                    end else if (reg_wdata == CMD_PING) begin
                        error_reg<=ERR_NONE; status_reg<=ST_IDLE|ST_DONE;
                        query_result_reg<=~query_arg_reg;
                    end else if (reg_wdata != CMD_ABORT) begin
                        status_reg<=ST_IDLE|ST_ERROR; error_reg<=ERR_BAD_COMMAND;
                    end
                end
                default: begin end
            endcase
        end

        case (state)
            S_R_ADDRESS: begin
                if (phase_counter + 1 >= ADDRESS_SETUP_CYCLES) begin
                    state<=S_R_SELECT; phase_counter<=0;
                end else phase_counter<=phase_counter+1'b1;
            end
            S_R_SELECT: begin
                if (phase_counter + 1 >= SELECT_SETUP_CYCLES) begin
                    state<=S_R_DATA; phase_counter<=0;
                end else phase_counter<=phase_counter+1'b1;
            end
            S_R_DATA: begin
                if (phase_counter + 1 >= DATA_WAIT_CYCLES) begin
                    if (GEOMETRY_PROFILE == 0 &&
                        (probe_phase == P_BASE || probe_phase == P_ID_READ ||
                         probe_phase == P_CFI_QRY || probe_phase == P_CFI_BODY))
                        rx_fifo[probe_index[4:0]]<=bank3_in;
                    if (GEOMETRY_PROFILE != 0 && probe_phase == P_BASE)
                        base_byte[probe_index[1:0]]<=bank3_in;
                    if (GEOMETRY_PROFILE != 0 && probe_phase == P_ID_READ)
                        rx_fifo[probe_index-4]<=bank3_in;
                    if (GEOMETRY_PROFILE != 0 && probe_phase == P_CFI_BODY)
                        rx_fifo[probe_index[4:0]]<=bank3_in;
                    if (probe_phase == P_ID_READ && bank3_in !=
                        (GEOMETRY_PROFILE != 0 ?
                         base_byte[probe_index-4] : rx_fifo[probe_index-4]))
                        id_changed<=1'b1;
                    if (probe_phase == P_CFI_QRY) begin
                        if (probe_index == 8) q_ok<=bank3_in == 8'h51;
                        if (probe_index == 9) r_ok<=bank3_in == 8'h52;
                        if (probe_index == 10) y_ok<=bank3_in == 8'h59;
                    end
                    if (probe_phase == P_FINAL_READ)
                        final_normal<=bank3_in ==
                            (GEOMETRY_PROFILE != 0 ? base_byte[0] : rx_fifo[0]);
                    state<=S_R_RELEASE; phase_counter<=0;
                end else phase_counter<=phase_counter+1'b1;
            end
            S_R_RELEASE: begin
                if (phase_counter + 1 >= RELEASE_CYCLES) begin
                    phase_counter<=0;
                    if (probe_phase == P_DUMMY) begin
                        state<=S_W_ADDRESS; probe_phase<=P_RESET_BEGIN;
                    end else if (probe_phase == P_BASE && probe_index < 3) begin
                        probe_index<=probe_index+1'b1; state<=S_R_ADDRESS;
                    end else if (probe_phase == P_BASE) begin
                        probe_index<=4; state<=S_W_ADDRESS; probe_phase<=P_ID_AA;
                    end else if (probe_phase == P_ID_READ && probe_index < 7) begin
                        probe_index<=probe_index+1'b1; state<=S_R_ADDRESS;
                    end else if (probe_phase == P_ID_READ) begin
                        state<=S_W_ADDRESS; probe_phase<=P_RESET_ID;
                    end else if (probe_phase == P_CFI_QRY && probe_index < 10) begin
                        probe_index<=probe_index+1'b1; state<=S_R_ADDRESS;
                    end else if (probe_phase == P_CFI_QRY && qry_valid) begin
                        probe_index<=(GEOMETRY_PROFILE != 0) ? 4 : 11;
                        state<=S_R_ADDRESS;
                        probe_phase<=P_CFI_BODY;
                    end else if (probe_phase == P_CFI_QRY && cfi_method == 0) begin
                        state<=S_W_ADDRESS; probe_phase<=P_CFI_RETRY_RESET;
                    end else if (probe_phase == P_CFI_QRY) begin
                        state<=S_W_ADDRESS; probe_phase<=P_RESET_END;
                    end else if (probe_phase == P_CFI_BODY && probe_index < 31) begin
                        probe_index<=probe_index+1'b1; state<=S_R_ADDRESS;
                    end else if (probe_phase == P_CFI_BODY) begin
                        state<=S_W_ADDRESS; probe_phase<=P_RESET_END;
                    end else if (probe_phase == P_FINAL_READ) begin
                        state<=S_IDLE; probe_phase<=P_NONE;
                        status_reg<=ST_IDLE|ST_DONE; error_reg<=ERR_NONE;
                        watchdog_counter<=0; fifo_count<=32;
                    end else begin
                        state<=S_IDLE; probe_phase<=P_NONE;
                        status_reg<=ST_IDLE|ST_ERROR; error_reg<=ERR_BAD_COMMAND;
                    end
                end else phase_counter<=phase_counter+1'b1;
            end
            S_W_ADDRESS: begin
                if (phase_counter + 1 >= WRITE_SETUP_CYCLES) begin
                    state<=S_W_DATA; phase_counter<=0;
                end else phase_counter<=phase_counter+1'b1;
            end
            S_W_DATA: begin
                if (phase_counter + 1 >= SELECT_SETUP_CYCLES) begin
                    state<=S_W_PULSE; phase_counter<=0;
                end else phase_counter<=phase_counter+1'b1;
            end
            S_W_PULSE: begin
                if (phase_counter + 1 >= WRITE_LOW_CYCLES) begin
                    state<=S_W_RELEASE; phase_counter<=0;
                end else phase_counter<=phase_counter+1'b1;
            end
            S_W_RELEASE: begin
                if (phase_counter + 1 >= WRITE_RELEASE_CYCLES) begin
                    phase_counter<=0;
                    case (probe_phase)
                        P_RESET_BEGIN: begin probe_index<=0;
                            state<=S_R_ADDRESS; probe_phase<=P_BASE; end
                        P_ID_AA: begin state<=S_W_ADDRESS; probe_phase<=P_ID_55; end
                        P_ID_55: begin state<=S_W_ADDRESS; probe_phase<=P_ID_90; end
                        P_ID_90: begin probe_index<=4;
                            state<=S_R_ADDRESS; probe_phase<=P_ID_READ; end
                        P_RESET_ID: begin state<=S_W_ADDRESS;
                            probe_phase<=P_CFI1_98; end
                        P_CFI1_98: begin cfi_method<=0; probe_index<=8;
                            q_ok<=0; r_ok<=0; y_ok<=0;
                            state<=S_R_ADDRESS; probe_phase<=P_CFI_QRY; end
                        P_CFI_RETRY_RESET: begin state<=S_W_ADDRESS;
                            probe_phase<=P_CFI2_98; end
                        P_CFI2_98: begin cfi_method<=2; probe_index<=8;
                            q_ok<=0; r_ok<=0; y_ok<=0;
                            state<=S_R_ADDRESS; probe_phase<=P_CFI_QRY; end
                        P_RESET_END: begin state<=S_R_ADDRESS;
                            probe_phase<=P_FINAL_READ; end
                        P_RECOVERY_F0: begin state<=S_W_ADDRESS;
                            probe_phase<=P_RECOVERY_FF; end
                        P_RECOVERY_FF: begin state<=S_IDLE; probe_phase<=P_NONE;
                            status_reg<=recovery_status;
                            error_reg<=recovery_error; watchdog_counter<=0; end
                        default: begin state<=S_IDLE; probe_phase<=P_NONE;
                            status_reg<=ST_IDLE|ST_ERROR;
                            error_reg<=ERR_BAD_COMMAND; end
                    endcase
                end else phase_counter<=phase_counter+1'b1;
            end
            default: begin end
        endcase

        // Mark the standard 0x00AA entry after a successful QRY.  Method 2 is
        // latched before its query; method 1 is otherwise implicit.
        if (state == S_R_RELEASE && probe_phase == P_CFI_QRY &&
            phase_counter + 1 >= RELEASE_CYCLES && probe_index == 10 &&
            qry_valid && cfi_method == 0)
            cfi_method<=1;
    end
end

assign bank3_out=gb_address[7:0];
assign bank3_dir=gb_address_dir;
assign bank2_out=gb_address[15:8];
assign bank2_dir=gb_address_dir;
assign bank1_out=gb_write_data;
assign bank1_dir=gb_data_dir;
assign bank0_out={1'b1,gb_wr_n,gb_rd_n,1'b1};
assign bank0_dir=1'b1;
assign pin30_out=1'b0; assign pin30_dir=1'b0;
assign pin30_pwroff_reset=1'b1;
assign pin31_out=1'b0; assign pin31_dir=1'b0;
assign writes_enabled=1'b0;
wire unused_bank2=^bank2_in;

endmodule

`default_nettype wire
