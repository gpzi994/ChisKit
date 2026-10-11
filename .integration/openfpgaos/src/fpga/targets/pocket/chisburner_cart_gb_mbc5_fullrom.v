//------------------------------------------------------------------------------
// ChisBurner - exact 2 MiB MBC5 full-ROM read verifier.
//
// The host-visible interface has no generic cartridge write operation.  It
// accepts exactly 65536 aligned 32-byte ROM reads in ascending logical order,
// covering 0x000000-0x1FFFFF once.  Bank 0 is read directly.  For banks
// 1-127, each accepted block privately selects the low MBC5 ROM-bank byte at
// 0x2000, reads 0x4000-0x7FFF, then restores bank 1 before DONE.
//
// This is a GB/GBC-only incremental profile.  The existing GBA read/program
// engines are separate core_top branches and are not instantiated here.
// Structural safety boundary:
//   * /WR can be asserted only while A == 0x2000.
//   * Write data is 1-127 (or 1 for restore); the high MBC5 bank bit is unused.
//   * 0x0000 RAM-enable, 0x3000 bank-high, 0x4000 RAM-bank, SRAM, and NOR
//     unlock/erase/program addresses are unreachable.
//------------------------------------------------------------------------------

`default_nettype none

module chisburner_cart_gb_mbc5_fullrom #(
    parameter integer ADDRESS_SETUP_CYCLES = 18,
    parameter integer SELECT_SETUP_CYCLES  = 9,
    parameter integer DATA_WAIT_CYCLES     = 36,
    parameter integer RELEASE_CYCLES       = 9,
    parameter integer WRITE_SETUP_CYCLES   = 18,
    parameter integer WRITE_LOW_CYCLES     = 36,
    parameter integer WRITE_RELEASE_CYCLES = 18,
    parameter integer MAX_READ_BYTES       = 32,
    // Some GB/GBC cartridge interfaces return unstable data on the first
    // physical ROM transaction after the adapter is claimed.  When enabled,
    // consume one complete copy of the first host block internally, keep it
    // out of the RX FIFO, then reread the same block for the host.  This does
    // not change the host-visible command count or permit any extra writes.
    parameter integer DISCARD_FIRST_BLOCK  = 0,
    // Mori-compatible MBC3/MBC5 detector.  In this profile the engine accepts
    // one fixed eleven-block sequence: two 64-byte fixed/switch-window sample
    // pairs followed by the complete 0x0100-0x015f header window.  Switch
    // samples select bank zero at 0x2000 and restore bank one afterwards.
    parameter integer MAPPER_AUTO_PROFILE  = 0,
    // UI98 shares this engine between the public, bounded 0x0100-0x015f
    // header window (space 0) and the OS-private mapper sequence (space 5).
    // Leave disabled for the accepted standalone mapper/full-ROM profiles.
    parameter integer SHARED_HEADER_PROFILE = 0,
    // UI106: OS-private space 6, sequential 2 MiB ROM capture only.
    // Same bank 0 fixed / banks 1-127 switched mapping as Mori dump.
    parameter integer SHARED_DUMP_PROFILE = 0,
    parameter integer GENERAL_DUMP_PROFILE = 0
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
    // Dedicated registered write address.  Keeping it separate from the
    // combinational read address prevents the AXI read mux from entering the
    // command/state next-state cone.
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

localparam [4:0] REG_ID           = 5'd0;
localparam [4:0] REG_CAPS         = 5'd1;
localparam [4:0] REG_STATUS       = 5'd2;
localparam [4:0] REG_COMMAND      = 5'd3;
localparam [4:0] REG_BUS          = 5'd4;
localparam [4:0] REG_ADDRESS      = 5'd5;
localparam [4:0] REG_LENGTH       = 5'd6;
localparam [4:0] REG_TIMEOUT      = 5'd7;
localparam [4:0] REG_RX_DATA      = 5'd8;
localparam [4:0] REG_RX_LEVEL     = 5'd9;
localparam [4:0] REG_ERROR        = 5'd10;
localparam [4:0] REG_LIFECYCLE    = 5'd11;
localparam [4:0] REG_HOST_B1_RAW  = 5'd12;
localparam [4:0] REG_HOST_DIAG    = 5'd13;
localparam [4:0] REG_QUERY_ARG    = 5'd14;
localparam [4:0] REG_QUERY_RESULT = 5'd15;
localparam [4:0] REG_SPACE        = 5'd16;

localparam [31:0] CART_ID           = 32'h3142_4746;
localparam [31:0] CART_CAPS         = 32'h0001_0F01;
localparam [31:0] CMD_SAFE_IDLE     = 32'd1;
localparam [31:0] CMD_READ          = 32'd2;
localparam [31:0] CMD_ABORT         = 32'd3;
localparam [31:0] CMD_QUERY_FW_INFO = 32'h0000_00A1;
localparam [31:0] CMD_PING          = 32'h0000_00FE;
localparam [31:0] QUERY_SIGNATURE   = 32'h5042_4746;
localparam [31:0] QUERY_VERSION     = 32'h0001_0000;
localparam [31:0] QUERY_FEATURES    = 32'h0000_0003;

localparam [31:0] ST_IDLE    = 32'h0000_0001;
localparam [31:0] ST_BUSY    = 32'h0000_0002;
localparam [31:0] ST_DONE    = 32'h0000_0004;
localparam [31:0] ST_ERROR   = 32'h0000_0008;
localparam [31:0] ST_ABORTED = 32'h0000_0010;
localparam [31:0] ST_TIMEOUT = 32'h0000_0020;

localparam [31:0] ERR_NONE        = 32'd0;
localparam [31:0] ERR_BAD_COMMAND = 32'd1;
localparam [31:0] ERR_LIFECYCLE   = 32'd3;
localparam [31:0] ERR_TIMEOUT     = 32'd4;

localparam [3:0] S_IDLE            = 4'd0;
localparam [3:0] S_BANK_ADDRESS    = 4'd1;
localparam [3:0] S_BANK_WRITE      = 4'd2;
localparam [3:0] S_BANK_RELEASE    = 4'd3;
localparam [3:0] S_READ_ADDRESS    = 4'd4;
localparam [3:0] S_READ_SELECT     = 4'd5;
localparam [3:0] S_READ_DATA       = 4'd6;
localparam [3:0] S_READ_RELEASE    = 4'd7;
localparam [3:0] S_RESTORE_ADDRESS = 4'd8;
localparam [3:0] S_RESTORE_WRITE   = 4'd9;
localparam [3:0] S_RESTORE_RELEASE = 4'd10;

reg [3:0]  state;
reg [31:0] status_reg;
reg [31:0] error_reg;
reg [31:0] bus_reg;
reg [31:0] space_reg;
reg [31:0] address_reg;
reg [31:0] length_reg;
reg [31:0] timeout_reg;
// Command-shape predicates are consumed on the REG_COMMAND write.  Decode
// them when their configuration register is written so a 32-bit equality
// cone is not placed directly on the FIFO/state register enables.
reg        bus_is_gb;
reg        length_is_block;
reg        timeout_is_nonzero;
reg [31:0] query_arg_reg;
reg [31:0] query_result_reg;
reg [15:0] byte_address;
reg [8:0]  target_bank;
reg general_read;
reg [2:0] mapper_kind;
reg [1:0] mapper_step, mapper_last;
reg [31:0] bytes_remaining;
// 135: opt-in width-only experiment; stage thresholds and watchdog stay unchanged.
`ifdef CHISBURNER_TIMERS135
function integer timer_max135;
 input integer a,b;
 begin timer_max135=(a>b)?a:b; end
endfunction
localparam integer TIMER_MAX135=timer_max135(timer_max135(timer_max135(timer_max135(timer_max135(timer_max135(timer_max135(1,ADDRESS_SETUP_CYCLES),SELECT_SETUP_CYCLES),DATA_WAIT_CYCLES),RELEASE_CYCLES),WRITE_SETUP_CYCLES),WRITE_LOW_CYCLES),WRITE_RELEASE_CYCLES);
// Counter holds 0..MAX-1. Keep at least one bit for zero/one-cycle parameters.
localparam integer TIMER_BITS135=(TIMER_MAX135<=1)?1:$clog2(TIMER_MAX135);
reg [TIMER_BITS135-1:0] phase_counter;
`else
reg [31:0] phase_counter;
`endif
reg [31:0] watchdog_counter;
reg [31:0] fifo_count;
reg [31:0] fifo_read_index;
reg [18:0] read_command_count;
reg        last_byte;
reg        banked_read;
reg        discarding_first_block;
reg [7:0]  rx_fifo [0:MAX_READ_BYTES-1];

wire active = state != S_IDLE;
wire timing_valid = ADDRESS_SETUP_CYCLES > 0 && SELECT_SETUP_CYCLES > 0 &&
                    DATA_WAIT_CYCLES > 0 && RELEASE_CYCLES > 0 &&
                    WRITE_SETUP_CYCLES > 0 && WRITE_LOW_CYCLES > 0 &&
                    WRITE_RELEASE_CYCLES > 0;
function [31:0] mapper_auto_address;
    input [3:0] index;
    begin
        case (index)
            4'd0: mapper_auto_address = 32'h0000_0000;
            4'd1: mapper_auto_address = 32'h0000_0020;
            4'd2: mapper_auto_address = 32'h0000_4000;
            4'd3: mapper_auto_address = 32'h0000_4020;
            4'd4: mapper_auto_address = 32'h0000_0000;
            4'd5: mapper_auto_address = 32'h0000_0020;
            4'd6: mapper_auto_address = 32'h0000_4000;
            4'd7: mapper_auto_address = 32'h0000_4020;
            4'd8: mapper_auto_address = 32'h0000_0100;
            4'd9: mapper_auto_address = 32'h0000_0120;
            default: mapper_auto_address = 32'h0000_0140;
        endcase
    end
endfunction

function mapper_auto_banked_index;
    input [3:0] index;
    begin
        mapper_auto_banked_index = index == 4'd2 || index == 4'd3 ||
                                   index == 4'd6 || index == 4'd7;
    end
endfunction

wire [31:0] fullrom_expected_address =
    {11'd0, read_command_count[15:0], 5'd0};
wire mapper_auto_banked = mapper_auto_banked_index(read_command_count[3:0]);
wire dump_space = SHARED_DUMP_PROFILE != 0 && space_reg == 32'd6;
// UI107 adds fixed mapper-specific scripts, never host-supplied writes.
wire general_space = GENERAL_DUMP_PROFILE != 0 && space_reg >= 16 && space_reg <= 23;
function [31:0] mapper_limit;
 input [2:0] kind;
 begin case(kind)
  0:mapper_limit=32768; 1,3:mapper_limit=2097152; 2:mapper_limit=262144;
  4,6:mapper_limit=4194304; 5:mapper_limit=8388608; default:mapper_limit=1048576;
 endcase end
endfunction
function [15:0] mapper_address;
 input [2:0] kind;input [1:0] step;
 begin
  if(kind==1)case(step)0:mapper_address=16'h6000;1:mapper_address=16'h4000;default:mapper_address=16'h2000;endcase
  else if(kind==5&&step==0)mapper_address=16'h3000;
  else mapper_address=kind==2?16'h2100:16'h2000;
 end
endfunction
function [7:0] mapper_data;
 input [2:0] kind;input [1:0] step;input [8:0] bank;input restore;
 begin
  if(kind==1)case(step)
   0:mapper_data=restore?0:1;
   1:mapper_data=restore?0:{6'd0,bank[6:5]};
   default:mapper_data=restore||bank[4:0]==0?8'd1:{3'd0,bank[4:0]};
  endcase
  else if(kind==5&&step==0)mapper_data=restore?0:{7'd0,bank[8]};
  else mapper_data=restore?8'd1:bank==0&&kind!=5&&kind!=7?8'd1:bank[7:0];
 end
endfunction
wire mapper_auto_space =
    space_reg == (SHARED_HEADER_PROFILE != 0 ? 32'd5 : 32'd0);
wire shared_header_read = SHARED_HEADER_PROFILE != 0 &&
                          space_reg == 32'd0 &&
                          address_reg >= 32'h0000_0100 &&
                          address_reg <= 32'h0000_0140 &&
                          address_reg[4:0] == 0;
wire read_shape_valid = timing_valid && bus_is_gb &&
                        length_is_block && timeout_is_nonzero &&
                        ((general_space && read_command_count < 19'd262144 &&
                          address_reg=={9'd0,read_command_count[17:0],5'd0} &&
                          address_reg<mapper_limit(space_reg[2:0]) &&
                          (read_command_count==0||mapper_kind==space_reg[2:0])) ||
                         (dump_space && read_command_count < 17'd65536 &&
                          address_reg == fullrom_expected_address) ||
                         (MAPPER_AUTO_PROFILE != 0 &&
                          mapper_auto_space &&
                          read_command_count < 17'd11 &&
                          address_reg == mapper_auto_address(read_command_count[3:0])) ||
                         (MAPPER_AUTO_PROFILE != 0 && shared_header_read) ||
                         (MAPPER_AUTO_PROFILE == 0 &&
                          space_reg == 32'd0 &&
                          read_command_count < 17'd65536 &&
                          address_reg == fullrom_expected_address));

reg [15:0] gb_address;
reg        gb_address_dir;
reg [7:0]  gb_write_data;
reg        gb_data_dir;
reg        gb_cs_n;
reg        gb_rd_n;
reg        gb_wr_n;

always @(*) begin
    gb_address     = 16'd0;
    gb_address_dir = 1'b0;
    gb_write_data  = 8'd0;
    gb_data_dir    = 1'b0;
    gb_cs_n        = 1'b1;
    gb_rd_n        = 1'b1;
    gb_wr_n        = 1'b1;
    case (state)
        S_BANK_ADDRESS, S_BANK_WRITE, S_BANK_RELEASE: begin
            gb_address     = general_read?mapper_address(mapper_kind,mapper_step):16'h2000;
            gb_address_dir = 1'b1;
            gb_write_data  = general_read?mapper_data(mapper_kind,mapper_step,target_bank,1'b0):target_bank[7:0];
            gb_data_dir    = 1'b1;
            gb_wr_n        = (state == S_BANK_WRITE) ? 1'b0 : 1'b1;
        end
        S_READ_ADDRESS, S_READ_SELECT, S_READ_DATA, S_READ_RELEASE: begin
            gb_address     = byte_address;
            gb_address_dir = 1'b1;
            gb_rd_n        = (state == S_READ_DATA) ? 1'b0 : 1'b1;
        end
        S_RESTORE_ADDRESS, S_RESTORE_WRITE, S_RESTORE_RELEASE: begin
            gb_address     = general_read?mapper_address(mapper_kind,mapper_step):16'h2000;
            gb_address_dir = 1'b1;
            gb_write_data  = general_read?mapper_data(mapper_kind,mapper_step,target_bank,1'b1):8'h01;
            gb_data_dir    = 1'b1;
            gb_wr_n        = (state == S_RESTORE_WRITE) ? 1'b0 : 1'b1;
        end
        default: begin end
    endcase
end

always @(*) begin
    reg_rdata = 32'd0;
    case (reg_addr)
        REG_ID:           reg_rdata = CART_ID;
        REG_CAPS:         reg_rdata = CART_CAPS;
        REG_STATUS:       reg_rdata = status_reg;
        REG_BUS:          reg_rdata = bus_reg;
        REG_SPACE:        reg_rdata = space_reg;
        REG_ADDRESS:      reg_rdata = address_reg;
        REG_LENGTH:       reg_rdata = length_reg;
        REG_TIMEOUT:      reg_rdata = timeout_reg;
        REG_RX_LEVEL:     reg_rdata = fifo_count;
        REG_ERROR:        reg_rdata = error_reg;
        REG_LIFECYCLE:    reg_rdata = lifecycle_status;
        REG_HOST_B1_RAW:  reg_rdata = host_b1_raw;
        REG_HOST_DIAG:    reg_rdata = host_diag;
        REG_QUERY_ARG:    reg_rdata = query_arg_reg;
        REG_QUERY_RESULT: reg_rdata = query_result_reg;
        REG_RX_DATA: begin
            if (fifo_count > 0) reg_rdata[7:0]   = rx_fifo[fifo_read_index];
            if (fifo_count > 1) reg_rdata[15:8]  = rx_fifo[fifo_read_index + 1];
            if (fifo_count > 2) reg_rdata[23:16] = rx_fifo[fifo_read_index + 2];
            if (fifo_count > 3) reg_rdata[31:24] = rx_fifo[fifo_read_index + 3];
        end
        default: reg_rdata = 32'd0;
    endcase
end

always @(posedge clk or negedge reset_n) begin
    if (!reset_n) begin
        state <= S_IDLE;
        status_reg <= ST_IDLE;
        error_reg <= ERR_NONE;
        bus_reg <= 0;
        space_reg <= 0;
        address_reg <= 0;
        length_reg <= 0;
        timeout_reg <= 0;
        bus_is_gb <= 0;
        length_is_block <= 0;
        timeout_is_nonzero <= 0;
        query_arg_reg <= 0;
        query_result_reg <= 0;
        byte_address <= 0;
        target_bank <= 1;
        general_read<=0;mapper_kind<=0;mapper_step<=0;mapper_last<=0;
        bytes_remaining <= 0;
        phase_counter <= 0;
        watchdog_counter <= 0;
        fifo_count <= 0;
        fifo_read_index <= 0;
        read_command_count <= 0;
        last_byte <= 0;
        banked_read <= 0;
        discarding_first_block <= 0;
    end else if (reg_wr && reg_wr_addr == REG_COMMAND && reg_wdata == CMD_ABORT) begin
        state <= S_IDLE;
        status_reg <= ST_IDLE | ST_ABORTED;
        error_reg <= ERR_NONE;
        phase_counter <= 0;
        watchdog_counter <= 0;
        discarding_first_block <= 0;
    end else if (active && !lifecycle_ok) begin
        state <= S_IDLE;
        status_reg <= ST_IDLE | ST_ERROR;
        error_reg <= ERR_LIFECYCLE;
        phase_counter <= 0;
        watchdog_counter <= 0;
        discarding_first_block <= 0;
    end else if (active && watchdog_counter + 1 >= timeout_reg) begin
        state <= S_IDLE;
        status_reg <= ST_IDLE | ST_ERROR | ST_TIMEOUT;
        error_reg <= ERR_TIMEOUT;
        phase_counter <= 0;
        watchdog_counter <= 0;
        discarding_first_block <= 0;
    end else begin
        if (active)
            watchdog_counter <= watchdog_counter + 1'b1;

        if (reg_rd && reg_addr == REG_RX_DATA && !active && fifo_count != 0) begin
            if (fifo_count > 4) begin
                fifo_read_index <= fifo_read_index + 4;
                fifo_count <= fifo_count - 4;
            end else begin
                fifo_read_index <= 0;
                fifo_count <= 0;
            end
        end

        if (reg_wr && !active) begin
            case (reg_wr_addr)
                REG_BUS: begin
                    bus_reg <= reg_wdata;
                    bus_is_gb <= reg_wdata == 32'd1;
                end
                REG_SPACE:     space_reg <= reg_wdata;
                REG_ADDRESS:   address_reg <= reg_wdata;
                REG_LENGTH: begin
                    length_reg <= reg_wdata;
                    length_is_block <= reg_wdata == 32'd32;
                end
                REG_TIMEOUT: begin
                    timeout_reg <= reg_wdata;
                    timeout_is_nonzero <= reg_wdata != 0;
                end
                REG_QUERY_ARG: query_arg_reg <= reg_wdata;
                REG_COMMAND: begin
                    if (reg_wdata == CMD_SAFE_IDLE) begin
                        status_reg <= ST_IDLE;
                        error_reg <= ERR_NONE;
                        fifo_count <= 0;
                        fifo_read_index <= 0;
                        watchdog_counter <= 0;
                        discarding_first_block <= 0;
                    end else if (reg_wdata == CMD_READ) begin
                        if (!lifecycle_ok || !read_shape_valid) begin
                            status_reg <= ST_IDLE | ST_ERROR;
                            error_reg <= lifecycle_ok ? ERR_BAD_COMMAND : ERR_LIFECYCLE;
                        end else begin
                            banked_read <= (MAPPER_AUTO_PROFILE != 0 &&
                                            mapper_auto_space) ?
                                           mapper_auto_banked :
                                           ((MAPPER_AUTO_PROFILE == 0 || dump_space) &&
                                            address_reg >= 32'h0000_4000);
                            state <= ((MAPPER_AUTO_PROFILE != 0 &&
                                       mapper_auto_space) ?
                                      mapper_auto_banked :
                                      ((MAPPER_AUTO_PROFILE == 0 || dump_space) &&
                                       address_reg >= 32'h0000_4000)) ?
                                     S_BANK_ADDRESS : S_READ_ADDRESS;
                            status_reg <= ST_BUSY;
                            error_reg <= ERR_NONE;
                            target_bank <= (MAPPER_AUTO_PROFILE != 0 &&
                                            mapper_auto_space) ?
                                           8'h00 :
                                           {1'b0, address_reg[20:14]};
                            byte_address <= (MAPPER_AUTO_PROFILE != 0 && !dump_space) ?
                                            address_reg[15:0] :
                                            ((address_reg < 32'h0000_4000) ?
                                             address_reg[15:0] :
                                             (16'h4000 | {2'b00, address_reg[13:0]}));
                            bytes_remaining <= length_reg;
                            phase_counter <= 0;
                            watchdog_counter <= 0;
                            fifo_count <= 0;
                            fifo_read_index <= 0;
                            if (MAPPER_AUTO_PROFILE == 0 || mapper_auto_space || dump_space || general_space)
                                read_command_count <= read_command_count + 1'b1;
                            last_byte <= 0;
                            discarding_first_block <= DISCARD_FIRST_BLOCK != 0 &&
                                                       (MAPPER_AUTO_PROFILE == 0 || dump_space || general_space ||
                                                        mapper_auto_space) &&
                                                       read_command_count == 0 &&
                                                       address_reg == 0;
                            general_read<=general_space;mapper_step<=0;
                            if(general_space)begin
                                mapper_kind<=space_reg[2:0];
                                mapper_last<=space_reg[2:0]==1?2:space_reg[2:0]==5?1:0;
                                target_bank<=address_reg[22:14];
                                banked_read<=space_reg[2:0]!=0;
                                state<=space_reg[2:0]==0?S_READ_ADDRESS:S_BANK_ADDRESS;
                                byte_address<=space_reg[2:0]==0?address_reg[15:0]:
                                  (address_reg[22:14]==0||(space_reg[2:0]==1&&address_reg[18:14]==0))?
                                  {2'b00,address_reg[13:0]}:{2'b01,address_reg[13:0]};
                            end
                        end
                    end else if (reg_wdata == CMD_QUERY_FW_INFO) begin
                        error_reg <= ERR_NONE;
                        status_reg <= ST_IDLE | ST_DONE;
                        case (query_arg_reg)
                            0: query_result_reg <= QUERY_SIGNATURE;
                            1: query_result_reg <= QUERY_VERSION;
                            2: query_result_reg <= QUERY_FEATURES;
                            default: begin
                                query_result_reg <= 0;
                                error_reg <= ERR_BAD_COMMAND;
                                status_reg <= ST_IDLE | ST_ERROR;
                            end
                        endcase
                    end else if (reg_wdata == CMD_PING) begin
                        error_reg <= ERR_NONE;
                        status_reg <= ST_IDLE | ST_DONE;
                        query_result_reg <= ~query_arg_reg;
                    end else if (reg_wdata != CMD_ABORT) begin
                        status_reg <= ST_IDLE | ST_ERROR;
                        error_reg <= ERR_BAD_COMMAND;
                    end
                end
                default: begin end
            endcase
        end

        case (state)
            S_BANK_ADDRESS: begin
                if (phase_counter + 1 >= WRITE_SETUP_CYCLES) begin
                    state <= S_BANK_WRITE; phase_counter <= 0;
                end else phase_counter <= phase_counter + 1'b1;
            end
            S_BANK_WRITE: begin
                if (phase_counter + 1 >= WRITE_LOW_CYCLES) begin
                    state <= S_BANK_RELEASE; phase_counter <= 0;
                end else phase_counter <= phase_counter + 1'b1;
            end
            S_BANK_RELEASE: begin
                if (phase_counter + 1 >= WRITE_RELEASE_CYCLES) begin
                    state <= S_READ_ADDRESS; phase_counter <= 0;
                    if(general_read&&mapper_step<mapper_last)begin mapper_step<=mapper_step+1'b1;state<=S_BANK_ADDRESS;end
                    else mapper_step<=0;
                end else phase_counter <= phase_counter + 1'b1;
            end
            S_READ_ADDRESS: begin
                if (phase_counter + 1 >= ADDRESS_SETUP_CYCLES) begin
                    state <= S_READ_SELECT; phase_counter <= 0;
                end else phase_counter <= phase_counter + 1'b1;
            end
            S_READ_SELECT: begin
                if (phase_counter + 1 >= SELECT_SETUP_CYCLES) begin
                    state <= S_READ_DATA; phase_counter <= 0;
                end else phase_counter <= phase_counter + 1'b1;
            end
            S_READ_DATA: begin
                if (phase_counter + 1 >= DATA_WAIT_CYCLES) begin
                    if (discarding_first_block) begin
                        fifo_count <= 0;
                    end else begin
                        rx_fifo[fifo_count] <= bank3_in;
                        fifo_count <= fifo_count + 1'b1;
                    end
                    bytes_remaining <= bytes_remaining - 1'b1;
                    last_byte <= bytes_remaining == 1;
                    state <= S_READ_RELEASE;
                    phase_counter <= 0;
                end else phase_counter <= phase_counter + 1'b1;
            end
            S_READ_RELEASE: begin
                if (phase_counter + 1 >= RELEASE_CYCLES) begin
                    phase_counter <= 0;
                    if (last_byte) begin
                        if (discarding_first_block) begin
                            // S_READ_RELEASE has already held /RD high for a
                            // full release phase.  Restart the same first host
                            // block; only this second pass reaches the FIFO.
                            discarding_first_block <= 0;
                            byte_address <= address_reg[15:0];
                            bytes_remaining <= length_reg;
                            fifo_count <= 0;
                            fifo_read_index <= 0;
                            last_byte <= 0;
                            state <= S_READ_ADDRESS;
                        end else if (banked_read)
                            state <= S_RESTORE_ADDRESS;
                        else begin
                            state <= S_IDLE;
                            status_reg <= ST_IDLE | ST_DONE;
                            watchdog_counter <= 0;
                        end
                    end else begin
                        byte_address <= byte_address + 1'b1;
                        state <= S_READ_ADDRESS;
                    end
                end else phase_counter <= phase_counter + 1'b1;
            end
            S_RESTORE_ADDRESS: begin
                if (phase_counter + 1 >= WRITE_SETUP_CYCLES) begin
                    state <= S_RESTORE_WRITE; phase_counter <= 0;
                end else phase_counter <= phase_counter + 1'b1;
            end
            S_RESTORE_WRITE: begin
                if (phase_counter + 1 >= WRITE_LOW_CYCLES) begin
                    state <= S_RESTORE_RELEASE; phase_counter <= 0;
                end else phase_counter <= phase_counter + 1'b1;
            end
            S_RESTORE_RELEASE: begin
                if (phase_counter + 1 >= WRITE_RELEASE_CYCLES) begin
                    state <= S_IDLE;
                    status_reg <= ST_IDLE | ST_DONE;
                    phase_counter <= 0;
                    watchdog_counter <= 0;
                    if(general_read&&mapper_step<mapper_last)begin
                        mapper_step<=mapper_step+1'b1;state<=S_RESTORE_ADDRESS;status_reg<=ST_BUSY;
                    end else mapper_step<=0;
                end else phase_counter <= phase_counter + 1'b1;
            end
            default: begin end
        endcase
    end
end

assign bank3_out = gb_address[7:0];
assign bank3_dir = gb_address_dir;
assign bank2_out = gb_address[15:8];
assign bank2_dir = gb_address_dir;
assign bank1_out = gb_write_data;
assign bank1_dir = gb_data_dir;
assign bank0_out = {1'b1, gb_wr_n, gb_rd_n, gb_cs_n};
assign bank0_dir = 1'b1;
assign pin30_out = 1'b0;
assign pin30_dir = 1'b0;
assign pin30_pwroff_reset = 1'b1;
assign pin31_out = 1'b0;
assign pin31_dir = 1'b0;
assign writes_enabled = 1'b0;

wire unused_bank2 = ^bank2_in;

endmodule

`default_nettype wire
