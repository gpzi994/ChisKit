//------------------------------------------------------------------------------
// ChisBurner GBA ROM read-only cartridge peripheral.
//
// Applications never access this block directly.  The openfpgaOS kernel owns
// the register interface.  READS_ENABLED defaults to zero so adding the RTL to
// a profile does not, by itself, make a cartridge transaction reachable.
// There is deliberately no generic cartridge write-data path or write command.
// IDENTIFY_ENABLED adds one fixed, bounded JEDEC/CFI probe.  Its addresses and
// data are constants in this RTL; program and erase opcodes are unreachable.
//------------------------------------------------------------------------------

`default_nettype none

module chisburner_cart_gba_readonly #(
    parameter integer READS_ENABLED         = 0,
    parameter integer ADDRESS_SETUP_CYCLES = 0,
    parameter integer SELECT_SETUP_CYCLES  = 0,
    parameter integer TURNAROUND_CYCLES    = 0,
    parameter integer DATA_WAIT_CYCLES     = 0,
    parameter integer RELEASE_CYCLES       = 0,
    parameter integer MAX_READ_BYTES       = 32,
    parameter [31:0]  READ_WINDOW_BASE     = 32'h0000_0000,
    parameter [31:0]  READ_WINDOW_BYTES    = 32'h0200_0000,
    parameter integer EXACT_READ_WINDOW    = 0,
    parameter integer REQUIRED_READ_BYTES  = 0,
    parameter [31:0]  READ_ADDRESS_MASK    = 32'h0000_0000,
    parameter integer FIXED_ADDRESS_SET    = 0,
    parameter integer MAX_READ_COMMANDS    = 0,
    parameter integer IDENTIFY_ENABLED     = 0,
    parameter integer MAX_IDENTIFY_COMMANDS = 0
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
    input  wire [31:0] reg_wdata,
    output reg  [31:0] reg_rdata,

    input  wire [7:0]  bank3_in,
    input  wire [7:0]  bank2_in,

    output reg  [7:0]  bank3_out,
    output reg         bank3_dir,
    output reg  [7:0]  bank2_out,
    output reg         bank2_dir,
    output reg  [7:0]  bank1_out,
    output reg         bank1_dir,
    output reg  [3:0]  bank0_out,
    output reg         bank0_dir,
    output reg         pin30_out,
    output reg         pin30_dir,
    output wire        pin30_pwroff_reset,
    output wire        pin31_out,
    output wire        pin31_dir,
    output wire        writes_enabled,
    output wire        busy,
    output reg         profile_verified
);

// Register word offsets from CART_BASE.
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

localparam [31:0] CART_ID = 32'h3142_4746; // legacy wire value retained for compatibility

localparam [31:0] CMD_SAFE_IDLE = 32'd1;
localparam [31:0] CMD_READ      = 32'd2;
localparam [31:0] CMD_ABORT     = 32'd3;
// Match the harmless discovery commands in the LK command vocabulary used
// by ChisBurner firmware.  No other LK command is accepted by this block.
localparam [31:0] CMD_QUERY_FW_INFO = 32'h0000_00A1;
localparam [31:0] CMD_PING          = 32'h0000_00FE;
localparam [31:0] BUS_GBA       = 32'd2;
localparam [31:0] SPACE_ROM     = 32'd0;
localparam [31:0] SPACE_IDENTIFY = 32'd4;

localparam [31:0] QUERY_SIGNATURE = 32'h5042_4746; // legacy wire value retained for compatibility
localparam [31:0] QUERY_VERSION   = 32'h0001_0000; // protocol 1.0
localparam [31:0] QUERY_FEATURES  = 32'h0000_0003; // query-info + ping

localparam [31:0] ST_IDLE    = 32'h0000_0001;
localparam [31:0] ST_BUSY    = 32'h0000_0002;
localparam [31:0] ST_DONE    = 32'h0000_0004;
localparam [31:0] ST_ERROR   = 32'h0000_0008;
localparam [31:0] ST_ABORTED = 32'h0000_0010;
localparam [31:0] ST_TIMEOUT = 32'h0000_0020;

localparam [31:0] ERR_NONE          = 32'd0;
localparam [31:0] ERR_BAD_COMMAND   = 32'd1;
localparam [31:0] ERR_NOT_SUPPORTED = 32'd2;
localparam [31:0] ERR_LIFECYCLE     = 32'd3;
localparam [31:0] ERR_TIMEOUT       = 32'd4;

localparam [3:0] S_IDLE      = 4'd0;
localparam [3:0] S_ADDRESS   = 4'd1;
localparam [3:0] S_SELECT    = 4'd2;
localparam [3:0] S_TURN      = 4'd3;
localparam [3:0] S_READ      = 4'd4;
localparam [3:0] S_RELEASE   = 4'd5;
localparam [3:0] S_W_ADDRESS = 4'd6;
localparam [3:0] S_W_SELECT  = 4'd7;
localparam [3:0] S_W_DATA    = 4'd8;
localparam [3:0] S_W_PULSE   = 4'd9;
localparam [3:0] S_W_RELEASE = 4'd10;

localparam [3:0] P_NONE        = 4'd0;
localparam [3:0] P_RESET_BEGIN = 4'd1;
localparam [3:0] P_BASE_READ   = 4'd2;
localparam [3:0] P_ID_AA       = 4'd3;
localparam [3:0] P_ID_55       = 4'd4;
localparam [3:0] P_ID_90       = 4'd5;
localparam [3:0] P_ID_READ     = 4'd6;
localparam [3:0] P_RESET_ID    = 4'd7;
localparam [3:0] P_CFI_98      = 4'd8;
localparam [3:0] P_QRY_READ    = 4'd9;
localparam [3:0] P_SIZE_READ   = 4'd10;
localparam [3:0] P_RESET_END   = 4'd11;
localparam [3:0] P_RECOVERY    = 4'd12;
localparam [3:0] P_GEOMETRY_READ = 4'd13;

// CAPS: bus bits [1:0], operation bits [10:8].  Writes have no bit and are
// structurally absent.  Safe-idle and abort exist even while reads are gated.
localparam [31:0] CART_CAPS =
    ((READS_ENABLED != 0) ? 32'h0020_0102 : 32'h0000_0000) |
    ((IDENTIFY_ENABLED != 0) ? 32'h0200_0002 : 32'h0000_0000) |
    32'h0000_0E00;

reg [3:0]  state;
reg [31:0] status_reg;
reg [31:0] error_reg;
reg [31:0] bus_reg;
reg [31:0] space_reg;
reg [31:0] address_reg;
reg [31:0] length_reg;
reg [31:0] timeout_reg;
reg [31:0] query_arg_reg;
reg [31:0] query_result_reg;
reg [23:0] word_address;
reg [31:0] words_remaining;
// 135: opt-in width-only experiment; stage thresholds and watchdog stay unchanged.
`ifdef CHISBURNER_TIMERS135
function integer timer_max135;
 input integer a,b;
 begin timer_max135=(a>b)?a:b; end
endfunction
localparam integer TIMER_MAX135=timer_max135(timer_max135(timer_max135(timer_max135(timer_max135(1,ADDRESS_SETUP_CYCLES),SELECT_SETUP_CYCLES),TURNAROUND_CYCLES),DATA_WAIT_CYCLES),RELEASE_CYCLES);
// Counter holds 0..MAX-1. Keep at least one bit for zero/one-cycle parameters.
localparam integer TIMER_BITS135=(TIMER_MAX135<=1)?1:$clog2(TIMER_MAX135);
reg [TIMER_BITS135-1:0] phase_counter;
`else
reg [31:0] phase_counter;
`endif
reg [31:0] watchdog_counter;
reg [31:0] fifo_count;
reg [31:0] fifo_read_index;
reg [31:0] read_command_count;
reg [31:0] identify_command_count;
reg        last_word;
reg        identify_active;
reg [3:0]  probe_phase;
reg [15:0] write_data;
reg [31:0] recovery_status;
reg [31:0] recovery_error;
reg        probe_id0_ok;
reg        probe_id1_ok;
reg        probe_q_ok;
reg        probe_r_ok;
reg        probe_y_ok;
reg        probe_cmdset_ok;
reg        probe_size_ok;
reg        probe_buffer_ok;
reg        probe_regions_ok;
reg        probe_sector_count_ok;
reg        probe_sector_size_ok;
reg [7:0]  rx_fifo [0:MAX_READ_BYTES-1];

wire active = (state != S_IDLE);
wire timing_valid = ADDRESS_SETUP_CYCLES > 0 &&
                    SELECT_SETUP_CYCLES  > 0 &&
                    TURNAROUND_CYCLES    > 0 &&
                    DATA_WAIT_CYCLES     > 0 &&
                    RELEASE_CYCLES       > 0;
wire fixed_address_valid =
                        address_reg == 32'h0000_00A0 ||
                        address_reg == 32'h0001_00A0 ||
                        address_reg == 32'h0002_00A0 ||
                        address_reg == 32'h0004_00A0 ||
                        address_reg == 32'h0008_00A0 ||
                        address_reg == 32'h0010_00A0 ||
                        address_reg == 32'h0020_00A0 ||
                        address_reg == 32'h0040_00A0 ||
                        address_reg == 32'h0080_00A0 ||
                        address_reg == 32'h0100_00A0 ||
                        address_reg == 32'h01FF_FFE0;

wire read_shape_valid = bus_reg == BUS_GBA &&
                        space_reg == SPACE_ROM &&
                        address_reg[0] == 1'b0 &&
                        length_reg != 0 &&
                        length_reg[0] == 1'b0 &&
                        length_reg <= MAX_READ_BYTES &&
                        ({1'b0, address_reg} + {1'b0, length_reg} - 1) <=
                            33'h001FF_FFFF &&
                        READ_WINDOW_BYTES != 0 &&
                        length_reg <= READ_WINDOW_BYTES &&
                        (address_reg - READ_WINDOW_BASE) <=
                            (READ_WINDOW_BYTES - length_reg) &&
                        (EXACT_READ_WINDOW == 0 ||
                         (address_reg == READ_WINDOW_BASE &&
                          length_reg == READ_WINDOW_BYTES)) &&
                        (REQUIRED_READ_BYTES == 0 ||
                         length_reg == REQUIRED_READ_BYTES) &&
                        ((address_reg & READ_ADDRESS_MASK) == 0) &&
                        (FIXED_ADDRESS_SET == 0 || fixed_address_valid) &&
                        (MAX_READ_COMMANDS == 0 ||
                         read_command_count < MAX_READ_COMMANDS) &&
                        timeout_reg != 0;

wire identify_shape_valid = bus_reg == BUS_GBA &&
                            space_reg == SPACE_IDENTIFY &&
                            address_reg == 0 &&
                            length_reg == 32 &&
                            timeout_reg != 0 &&
                            (MAX_IDENTIFY_COMMANDS == 0 ||
                             identify_command_count < MAX_IDENTIFY_COMMANDS);

assign pin30_pwroff_reset = 1'b0;
assign pin31_out          = 1'b0;
assign pin31_dir          = 1'b0;
assign writes_enabled     = 1'b0;
assign busy               = active;

always @(*) begin
    reg_rdata = 32'd0;
    case (reg_addr)
        REG_ID:       reg_rdata = CART_ID;
        REG_CAPS:     reg_rdata = CART_CAPS;
        REG_STATUS:   reg_rdata = status_reg;
        REG_BUS:      reg_rdata = bus_reg;
        REG_SPACE:    reg_rdata = space_reg;
        REG_ADDRESS:  reg_rdata = address_reg;
        REG_LENGTH:   reg_rdata = length_reg;
        REG_TIMEOUT:  reg_rdata = timeout_reg;
        REG_RX_DATA: begin
            if (fifo_count > 0) reg_rdata[7:0]   = rx_fifo[fifo_read_index];
            if (fifo_count > 1) reg_rdata[15:8]  = rx_fifo[fifo_read_index + 1];
            if (fifo_count > 2) reg_rdata[23:16] = rx_fifo[fifo_read_index + 2];
            if (fifo_count > 3) reg_rdata[31:24] = rx_fifo[fifo_read_index + 3];
        end
        REG_RX_LEVEL: reg_rdata = fifo_count;
        REG_ERROR:    reg_rdata = error_reg;
        REG_LIFECYCLE: reg_rdata = lifecycle_status;
        REG_HOST_B1_RAW: reg_rdata = host_b1_raw;
        REG_HOST_DIAG: reg_rdata = host_diag;
        REG_QUERY_ARG: reg_rdata = query_arg_reg;
        REG_QUERY_RESULT: reg_rdata = query_result_reg;
        default:      reg_rdata = 32'd0;
    endcase
end

// Pin owner.  Safe idle is the default for every state not explicitly listed.
// bank0 is {PHI#, WR#, RD#, CS#}.  WR# can pulse only in the internally
// sequenced identification states; no register controls write_data/address.
always @(*) begin
    bank3_out = 8'h00;
    bank3_dir = 1'b0;
    bank2_out = 8'h00;
    bank2_dir = 1'b0;
    bank1_out = 8'h00;
    bank1_dir = 1'b0;
    bank0_out = 4'hF;
    bank0_dir = 1'b1;
    pin30_out = 1'b1;
    pin30_dir = 1'b0;

    case (state)
        S_ADDRESS: begin
            bank3_out = word_address[7:0];
            bank3_dir = 1'b1;
            bank2_out = word_address[15:8];
            bank2_dir = 1'b1;
            bank1_out = word_address[23:16];
            bank1_dir = 1'b1;
            bank0_out = 4'b1111;
            pin30_dir = 1'b1;
        end
        S_SELECT: begin
            bank3_out = word_address[7:0];
            bank3_dir = 1'b1;
            bank2_out = word_address[15:8];
            bank2_dir = 1'b1;
            bank1_out = word_address[23:16];
            bank1_dir = 1'b1;
            bank0_out = 4'b1110;
            pin30_dir = 1'b1;
        end
        S_TURN: begin
            bank1_out = word_address[23:16];
            bank1_dir = 1'b1;
            bank0_out = 4'b1110;
            pin30_dir = 1'b1;
        end
        S_READ: begin
            bank1_out = word_address[23:16];
            bank1_dir = 1'b1;
            bank0_out = 4'b1100;
            pin30_dir = 1'b1;
        end
        S_RELEASE: begin
            bank1_out = word_address[23:16];
            bank1_dir = 1'b1;
            bank0_out = 4'b1110;
            pin30_dir = 1'b1;
        end
`ifndef CHISBURNER_GBA_IDENTIFY133
        S_W_ADDRESS: begin
            bank3_out = word_address[7:0];
            bank3_dir = 1'b1;
            bank2_out = word_address[15:8];
            bank2_dir = 1'b1;
            bank1_out = word_address[23:16];
            bank1_dir = 1'b1;
            bank0_out = 4'b1111;
            pin30_dir = 1'b1;
        end
        S_W_SELECT: begin
            bank3_out = word_address[7:0];
            bank3_dir = 1'b1;
            bank2_out = word_address[15:8];
            bank2_dir = 1'b1;
            bank1_out = word_address[23:16];
            bank1_dir = 1'b1;
            bank0_out = 4'b1110;
            pin30_dir = 1'b1;
        end
        S_W_DATA, S_W_PULSE, S_W_RELEASE: begin
            bank3_out = write_data[7:0];
            bank3_dir = 1'b1;
            bank2_out = write_data[15:8];
            bank2_dir = 1'b1;
            bank1_out = word_address[23:16];
            bank1_dir = 1'b1;
            bank0_out = (state == S_W_PULSE) ? 4'b1010 : 4'b1110;
            pin30_dir = 1'b1;
        end
`endif
        default: begin end
    endcase
end

always @(posedge clk or negedge reset_n) begin
    if (!reset_n) begin
        state            <= S_IDLE;
        status_reg       <= ST_IDLE;
        error_reg        <= ERR_NONE;
        bus_reg          <= 32'd0;
        space_reg        <= SPACE_ROM;
        address_reg      <= 32'd0;
        length_reg       <= 32'd0;
        timeout_reg      <= 32'd0;
        query_arg_reg    <= 32'd0;
        query_result_reg <= 32'd0;
        word_address     <= 24'd0;
        words_remaining  <= 32'd0;
        phase_counter    <= 32'd0;
        watchdog_counter <= 32'd0;
        fifo_count       <= 32'd0;
        fifo_read_index  <= 32'd0;
        read_command_count <= 32'd0;
        identify_command_count <= 32'd0;
        last_word        <= 1'b0;
        identify_active  <= 1'b0;
        probe_phase      <= P_NONE;
        write_data       <= 16'd0;
        recovery_status  <= ST_IDLE;
        recovery_error   <= ERR_NONE;
        probe_id0_ok     <= 1'b0;
        probe_id1_ok     <= 1'b0;
        probe_q_ok       <= 1'b0;
        probe_r_ok       <= 1'b0;
        probe_y_ok       <= 1'b0;
        probe_cmdset_ok  <= 1'b0;
        probe_size_ok    <= 1'b0;
        probe_buffer_ok  <= 1'b0;
        probe_regions_ok <= 1'b0;
        probe_sector_count_ok <= 1'b0;
        probe_sector_size_ok  <= 1'b0;
        profile_verified <= 1'b0;
    end else if (reg_wr && reg_addr == REG_COMMAND &&
                 reg_wdata == CMD_ABORT) begin
`ifndef CHISBURNER_GBA_IDENTIFY133
        if (active && identify_active && lifecycle_ok) begin
            state             <= S_W_ADDRESS;
            status_reg        <= ST_BUSY;
            error_reg         <= ERR_NONE;
            word_address      <= 24'd0;
            write_data        <= 16'h00F0;
            probe_phase       <= P_RECOVERY;
            recovery_status   <= ST_IDLE | ST_ABORTED;
            recovery_error    <= ERR_NONE;
            phase_counter     <= 32'd0;
            watchdog_counter  <= 32'd0;
        end else
`endif
        begin
            state            <= S_IDLE;
            status_reg       <= ST_IDLE | ST_ABORTED;
            error_reg        <= ERR_NONE;
            phase_counter    <= 32'd0;
            watchdog_counter <= 32'd0;
            identify_active  <= 1'b0;
            probe_phase      <= P_NONE;
            profile_verified <= 1'b0;
        end
    end else if (active && !lifecycle_ok) begin
        state            <= S_IDLE;
        status_reg       <= ST_IDLE | ST_ERROR;
        error_reg        <= ERR_LIFECYCLE;
        phase_counter    <= 32'd0;
        watchdog_counter <= 32'd0;
        identify_active  <= 1'b0;
        probe_phase      <= P_NONE;
        profile_verified <= 1'b0;
    end else if (active && watchdog_counter + 1 >= timeout_reg &&
                 probe_phase != P_RECOVERY) begin
`ifndef CHISBURNER_GBA_IDENTIFY133
        if (identify_active && lifecycle_ok) begin
            state             <= S_W_ADDRESS;
            status_reg        <= ST_BUSY;
            error_reg         <= ERR_NONE;
            word_address      <= 24'd0;
            write_data        <= 16'h00F0;
            probe_phase       <= P_RECOVERY;
            recovery_status   <= ST_IDLE | ST_ERROR | ST_TIMEOUT;
            recovery_error    <= ERR_TIMEOUT;
            phase_counter     <= 32'd0;
            watchdog_counter  <= 32'd0;
        end else
`endif
        begin
            state            <= S_IDLE;
            status_reg       <= ST_IDLE | ST_ERROR | ST_TIMEOUT;
            error_reg        <= ERR_TIMEOUT;
            phase_counter    <= 32'd0;
            watchdog_counter <= 32'd0;
        end
    end else begin
        // A cartridge removal/lifecycle drop invalidates the last hardware
        // identification even when no transaction is currently active.
        if (!lifecycle_ok)
            profile_verified <= 1'b0;
        if (active)
            watchdog_counter <= watchdog_counter + 1;

        // RX_DATA pops up to four bytes only after the transaction is idle.
        if (reg_rd && reg_addr == REG_RX_DATA && !active && fifo_count != 0) begin
            if (fifo_count > 4) begin
                fifo_read_index <= fifo_read_index + 4;
                fifo_count      <= fifo_count - 4;
            end else begin
                fifo_read_index <= 32'd0;
                fifo_count      <= 32'd0;
            end
        end

        if (reg_wr && !active) begin
            case (reg_addr)
                REG_BUS:     bus_reg     <= reg_wdata;
                REG_SPACE:   space_reg   <= reg_wdata;
                REG_ADDRESS: address_reg <= reg_wdata;
                REG_LENGTH:  length_reg  <= reg_wdata;
                REG_TIMEOUT: timeout_reg <= reg_wdata;
                REG_QUERY_ARG: query_arg_reg <= reg_wdata;
                REG_COMMAND: begin
                    if (reg_wdata == CMD_SAFE_IDLE) begin
                        state            <= S_IDLE;
                        status_reg       <= ST_IDLE;
                        error_reg        <= ERR_NONE;
                        fifo_count       <= 32'd0;
                        fifo_read_index  <= 32'd0;
                        watchdog_counter <= 32'd0;
                    end else if (reg_wdata == CMD_READ) begin
                        if (!timing_valid) begin
                            status_reg <= ST_IDLE | ST_ERROR;
                            error_reg  <= ERR_NOT_SUPPORTED;
                        end else if (!lifecycle_ok ||
                                     !((READS_ENABLED != 0 && read_shape_valid) ||
                                       (IDENTIFY_ENABLED != 0 && identify_shape_valid))) begin
                            status_reg <= ST_IDLE | ST_ERROR;
                            error_reg  <= lifecycle_ok ? ERR_BAD_COMMAND : ERR_LIFECYCLE;
                        end
`ifndef CHISBURNER_GBA_IDENTIFY133
                        else if (IDENTIFY_ENABLED != 0 && space_reg == SPACE_IDENTIFY) begin
                            state             <= S_W_ADDRESS;
                            status_reg        <= ST_BUSY;
                            error_reg         <= ERR_NONE;
                            word_address      <= 24'd0;
                            write_data        <= 16'h00F0;
                            phase_counter     <= 32'd0;
                            watchdog_counter  <= 32'd0;
                            fifo_count        <= 32'd0;
                            fifo_read_index   <= 32'd0;
                            identify_command_count <= identify_command_count + 1'b1;
                            identify_active   <= 1'b1;
                            probe_phase       <= P_RESET_BEGIN;
                            last_word         <= 1'b0;
                        end
`endif
                        else begin
                            state            <= S_ADDRESS;
                            status_reg       <= ST_BUSY;
                            error_reg        <= ERR_NONE;
                            word_address     <= address_reg[24:1];
                            words_remaining  <= length_reg >> 1;
                            phase_counter    <= 32'd0;
                            watchdog_counter <= 32'd0;
                            fifo_count       <= 32'd0;
                            fifo_read_index  <= 32'd0;
                            read_command_count <= read_command_count + 1'b1;
                            last_word        <= 1'b0;
                            identify_active  <= 1'b0;
                            probe_phase      <= P_NONE;
                        end
                    end else if (reg_wdata == CMD_QUERY_FW_INFO) begin
                        state      <= S_IDLE;
                        error_reg  <= ERR_NONE;
                        status_reg <= ST_IDLE | ST_DONE;
                        case (query_arg_reg)
                            32'd0: query_result_reg <= QUERY_SIGNATURE;
                            32'd1: query_result_reg <= QUERY_VERSION;
                            32'd2: query_result_reg <= QUERY_FEATURES;
                            default: begin
                                query_result_reg <= 32'd0;
                                error_reg        <= ERR_BAD_COMMAND;
                                status_reg       <= ST_IDLE | ST_ERROR;
                            end
                        endcase
                    end else if (reg_wdata == CMD_PING) begin
                        state            <= S_IDLE;
                        error_reg        <= ERR_NONE;
                        status_reg       <= ST_IDLE | ST_DONE;
                        query_result_reg <= ~query_arg_reg;
                    end else if (reg_wdata != CMD_ABORT) begin
                        status_reg <= ST_IDLE | ST_ERROR;
                        error_reg  <= ERR_BAD_COMMAND;
                    end
                end
                default: begin end
            endcase
        end

        case (state)
            S_ADDRESS: begin
                if (phase_counter + 1 >= ADDRESS_SETUP_CYCLES) begin
                    state         <= S_SELECT;
                    phase_counter <= 32'd0;
                end else begin
                    phase_counter <= phase_counter + 1;
                end
            end
            S_SELECT: begin
                if (phase_counter + 1 >= SELECT_SETUP_CYCLES) begin
                    state         <= S_TURN;
                    phase_counter <= 32'd0;
                end else begin
                    phase_counter <= phase_counter + 1;
                end
            end
            S_TURN: begin
                if (phase_counter + 1 >= TURNAROUND_CYCLES) begin
                    state         <= S_READ;
                    phase_counter <= 32'd0;
                end else begin
                    phase_counter <= phase_counter + 1;
                end
            end
            S_READ: begin
                if (phase_counter + 1 >= DATA_WAIT_CYCLES) begin
                    // Keep the public identification snapshot exactly 32
                    // bytes.  The additional geometry words are consumed
                    // only by this fail-closed hardware profile check.
`ifdef CHISBURNER_GBA_IDENTIFY133
                    begin
`else
                    if (!(identify_active && probe_phase == P_GEOMETRY_READ)) begin
`endif
                        rx_fifo[fifo_count]     <= bank3_in;
                        rx_fifo[fifo_count + 1] <= bank2_in;
                        fifo_count              <= fifo_count + 2;
                    end
`ifndef CHISBURNER_GBA_IDENTIFY133
                    if (identify_active && probe_phase == P_ID_READ) begin
                        if (word_address == 24'h000000)
                            probe_id0_ok <= bank3_in == 8'h01 && bank2_in == 8'h00;
                        if (word_address == 24'h000001)
                            probe_id1_ok <= bank3_in == 8'h7E && bank2_in == 8'h22;
                    end
                    if (identify_active && probe_phase == P_QRY_READ) begin
                        if (word_address == 24'h000010)
                            probe_q_ok <= bank3_in == 8'h51 && bank2_in == 8'h00;
                        if (word_address == 24'h000011)
                            probe_r_ok <= bank3_in == 8'h52 && bank2_in == 8'h00;
                        if (word_address == 24'h000012)
                            probe_y_ok <= bank3_in == 8'h59 && bank2_in == 8'h00;
                        if (word_address == 24'h000013)
                            probe_cmdset_ok <= bank3_in == 8'h02 && bank2_in == 8'h00;
                    end
                    if (identify_active && probe_phase == P_SIZE_READ) begin
                        if (word_address == 24'h000027)
                            probe_size_ok <= bank3_in == 8'h19 && bank2_in == 8'h00;
                        // The target BOM is S29GL256S.  Its CFI word 2Ah is
                        // 09h (512-byte maximum write buffer).  The programmer
                        // deliberately uses a smaller, fixed 32-byte chunk;
                        // this check validates capability, not transfer size.
                        if (word_address == 24'h00002A)
                            probe_buffer_ok <= bank3_in == 8'h09 && bank2_in == 8'h00;
                    end
                    if (identify_active && probe_phase == P_GEOMETRY_READ) begin
                        if (word_address == 24'h00002C)
                            probe_regions_ok <= bank3_in == 8'h01 && bank2_in == 8'h00;
                        if (word_address == 24'h00002D)
                            probe_sector_count_ok <= bank3_in == 8'hFF && bank2_in == 8'h00;
                        if (word_address == 24'h00002E &&
                            !(bank3_in == 8'h00 && bank2_in == 8'h00))
                            probe_sector_count_ok <= 1'b0;
                        if (word_address == 24'h00002F)
                            probe_sector_size_ok <= bank3_in == 8'h00 && bank2_in == 8'h00;
                        if (word_address == 24'h000030 &&
                            !(bank3_in == 8'h02 && bank2_in == 8'h00))
                            probe_sector_size_ok <= 1'b0;
                    end
`endif
                    words_remaining         <= words_remaining - 1;
                    last_word               <= (words_remaining == 1);
                    state                   <= S_RELEASE;
                    phase_counter           <= 32'd0;
                end else begin
                    phase_counter <= phase_counter + 1;
                end
            end
            S_RELEASE: begin
                if (phase_counter + 1 >= RELEASE_CYCLES) begin
                    phase_counter <= 32'd0;
                    if (last_word) begin
`ifdef CHISBURNER_GBA_IDENTIFY133
                        begin
`else
                        if (!identify_active) begin
`endif
                            state            <= S_IDLE;
                            status_reg       <= ST_IDLE | ST_DONE;
                            watchdog_counter <= 32'd0;
                        end
`ifndef CHISBURNER_GBA_IDENTIFY133
                        else if (probe_phase == P_BASE_READ) begin
                            state         <= S_W_ADDRESS;
                            word_address  <= 24'h000555;
                            write_data    <= 16'h00AA;
                            probe_phase   <= P_ID_AA;
                        end else if (probe_phase == P_ID_READ) begin
                            state         <= S_W_ADDRESS;
                            word_address  <= 24'd0;
                            write_data    <= 16'h00F0;
                            probe_phase   <= P_RESET_ID;
                        end else if (probe_phase == P_QRY_READ) begin
                            state            <= S_ADDRESS;
                            word_address     <= 24'h000027;
                            words_remaining  <= 32'd4;
                            last_word        <= 1'b0;
                            probe_phase      <= P_SIZE_READ;
                        end else if (probe_phase == P_SIZE_READ) begin
                            state            <= S_ADDRESS;
                            word_address     <= 24'h00002C;
                            words_remaining  <= 32'd5;
                            last_word        <= 1'b0;
                            probe_phase      <= P_GEOMETRY_READ;
                        end else if (probe_phase == P_GEOMETRY_READ) begin
                            state         <= S_W_ADDRESS;
                            word_address  <= 24'd0;
                            write_data    <= 16'h00F0;
                            probe_phase   <= P_RESET_END;
                        end else begin
                            state            <= S_IDLE;
                            status_reg       <= ST_IDLE | ST_ERROR;
                            error_reg        <= ERR_BAD_COMMAND;
                            identify_active  <= 1'b0;
                            probe_phase      <= P_NONE;
                            watchdog_counter <= 32'd0;
                        end
`endif
                    end else begin
                        word_address <= word_address + 1'b1;
                        state        <= S_ADDRESS;
                    end
                end else begin
                    phase_counter <= phase_counter + 1;
                end
            end
`ifndef CHISBURNER_GBA_IDENTIFY133
            S_W_ADDRESS: begin
                // Clear the previous identification snapshot from an internal
                // state/phase decode, one cycle after the MMIO command.  This
                // keeps the peripheral request address off every CFI result
                // register's D path while still invalidating the profile well
                // before the first identification read can complete.
                if (identify_active && probe_phase == P_RESET_BEGIN &&
                    phase_counter == 0) begin
                    probe_id0_ok          <= 1'b0;
                    probe_id1_ok          <= 1'b0;
                    probe_q_ok            <= 1'b0;
                    probe_r_ok            <= 1'b0;
                    probe_y_ok            <= 1'b0;
                    probe_cmdset_ok       <= 1'b0;
                    probe_size_ok         <= 1'b0;
                    probe_buffer_ok       <= 1'b0;
                    probe_regions_ok      <= 1'b0;
                    probe_sector_count_ok <= 1'b0;
                    probe_sector_size_ok  <= 1'b0;
                    profile_verified      <= 1'b0;
                end
                if (phase_counter + 1 >= ADDRESS_SETUP_CYCLES) begin
                    state <= S_W_SELECT; phase_counter <= 32'd0;
                end else phase_counter <= phase_counter + 1;
            end
            S_W_SELECT: begin
                if (phase_counter + 1 >= SELECT_SETUP_CYCLES) begin
                    state <= S_W_DATA; phase_counter <= 32'd0;
                end else phase_counter <= phase_counter + 1;
            end
            S_W_DATA: begin
                if (phase_counter + 1 >= TURNAROUND_CYCLES) begin
                    state <= S_W_PULSE; phase_counter <= 32'd0;
                end else phase_counter <= phase_counter + 1;
            end
            S_W_PULSE: begin
                if (phase_counter + 1 >= DATA_WAIT_CYCLES) begin
                    state <= S_W_RELEASE; phase_counter <= 32'd0;
                end else phase_counter <= phase_counter + 1;
            end
            S_W_RELEASE: begin
                if (phase_counter + 1 >= RELEASE_CYCLES) begin
                    phase_counter <= 32'd0;
                    if (probe_phase == P_RESET_BEGIN) begin
                        state <= S_ADDRESS; word_address <= 24'd0;
                        words_remaining <= 32'd4; last_word <= 1'b0;
                        probe_phase <= P_BASE_READ;
                    end else if (probe_phase == P_ID_AA) begin
                        state <= S_W_ADDRESS; word_address <= 24'h0002AA;
                        write_data <= 16'h0055; probe_phase <= P_ID_55;
                    end else if (probe_phase == P_ID_55) begin
                        state <= S_W_ADDRESS; word_address <= 24'h000555;
                        write_data <= 16'h0090; probe_phase <= P_ID_90;
                    end else if (probe_phase == P_ID_90) begin
                        state <= S_ADDRESS; word_address <= 24'd0;
                        words_remaining <= 32'd4; last_word <= 1'b0;
                        probe_phase <= P_ID_READ;
                    end else if (probe_phase == P_RESET_ID) begin
                        state <= S_W_ADDRESS; word_address <= 24'h000055;
                        write_data <= 16'h0098; probe_phase <= P_CFI_98;
                    end else if (probe_phase == P_CFI_98) begin
                        state <= S_ADDRESS; word_address <= 24'h000010;
                        words_remaining <= 32'd4; last_word <= 1'b0;
                        probe_phase <= P_QRY_READ;
                    end else if (probe_phase == P_RESET_END) begin
                        state <= S_IDLE; status_reg <= ST_IDLE | ST_DONE;
                        error_reg <= ERR_NONE; identify_active <= 1'b0;
                        probe_phase <= P_NONE; watchdog_counter <= 32'd0;
                        profile_verified <= probe_id0_ok && probe_id1_ok &&
                                            probe_q_ok && probe_r_ok &&
                                            probe_y_ok && probe_cmdset_ok &&
                                            probe_size_ok && probe_buffer_ok &&
                                            probe_regions_ok &&
                                            probe_sector_count_ok &&
                                            probe_sector_size_ok;
                    end else if (probe_phase == P_RECOVERY) begin
                        state <= S_IDLE; status_reg <= recovery_status;
                        error_reg <= recovery_error; identify_active <= 1'b0;
                        probe_phase <= P_NONE; watchdog_counter <= 32'd0;
                    end else begin
                        state <= S_IDLE; status_reg <= ST_IDLE | ST_ERROR;
                        error_reg <= ERR_BAD_COMMAND; identify_active <= 1'b0;
                        probe_phase <= P_NONE; watchdog_counter <= 32'd0;
                    end
                end else phase_counter <= phase_counter + 1;
            end
`endif
            default: begin end
        endcase
    end
end

endmodule

`default_nettype wire
