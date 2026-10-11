//------------------------------------------------------------------------------
// ChisBurner abstract GB/GBC ROM read-only cartridge peripheral.
//
// This module owns the common CBR1 command contract and protocol-level GB/GBC
// read sequence.  Its pins are deliberately protocol-level signals; production
// builds may reach it only through chisburner_cart_gb_readonly, whose translator
// mapping is separately synthesis-locked.  There is deliberately no cartridge
// write-data path or write command.
//------------------------------------------------------------------------------

`default_nettype none

module chisburner_cart_gb_readonly_abstract #(
    parameter integer READS_ENABLED        = 0,
    parameter integer ADDRESS_SETUP_CYCLES = 0,
    parameter integer SELECT_SETUP_CYCLES  = 0,
    parameter integer DATA_WAIT_CYCLES     = 0,
    parameter integer RELEASE_CYCLES       = 0,
    parameter integer MAX_READ_BYTES       = 32,
    parameter [31:0]  READ_WINDOW_BASE     = 32'h0000_0000,
    parameter [31:0]  READ_WINDOW_BYTES    = 32'h0000_8000,
    parameter integer FIXED_HEADER_CHUNKS  = 0,
    parameter integer MAX_READ_COMMANDS    = 0,
    parameter integer SAVE_READS_ENABLED   = 0
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

    output reg  [15:0] gb_address,
    output reg         gb_address_dir,
    input  wire [7:0]  gb_data_in,
    output wire        gb_data_dir,
    output reg         gb_cs_n,
    output reg         gb_rd_n,
    output wire        gb_wr_n,
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

localparam [31:0] CART_ID = 32'h3142_4746; // legacy wire value retained for compatibility

localparam [31:0] CMD_SAFE_IDLE    = 32'd1;
localparam [31:0] CMD_READ         = 32'd2;
localparam [31:0] CMD_ABORT        = 32'd3;
localparam [31:0] CMD_QUERY_FW_INFO = 32'h0000_00A1;
localparam [31:0] CMD_PING          = 32'h0000_00FE;
localparam [31:0] BUS_GB_GBC        = 32'd1;
localparam [31:0] SPACE_ROM          = 32'd0;
localparam [31:0] SPACE_SAVE_PARALLEL = 32'd1;

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

localparam [2:0] S_IDLE    = 3'd0;
localparam [2:0] S_ADDRESS = 3'd1;
localparam [2:0] S_SELECT  = 3'd2;
localparam [2:0] S_READ    = 3'd3;
localparam [2:0] S_RELEASE = 3'd4;

// CAPS: bus bits [1:0], operation bits [10:8], GB/GBC spaces [19:16].
// Writes are absent.  SAVE_READS_ENABLED defaults closed.
localparam [31:0] CART_CAPS =
    ((READS_ENABLED != 0) ?
        (32'h0001_0101 |
         ((SAVE_READS_ENABLED != 0) ? 32'h0002_0000 : 32'h0000_0000)) :
        32'h0000_0000) |
    32'h0000_0E00;

reg [2:0]  state;
reg [31:0] status_reg;
reg [31:0] error_reg;
reg [31:0] bus_reg;
reg [31:0] space_reg;
reg [31:0] address_reg;
reg [31:0] length_reg;
reg [31:0] timeout_reg;
reg [31:0] query_arg_reg;
reg [31:0] query_result_reg;
reg [15:0] byte_address;
reg [31:0] bytes_remaining;
reg [31:0] phase_counter;
reg [31:0] watchdog_counter;
reg [31:0] fifo_count;
reg [31:0] fifo_read_index;
reg [31:0] read_command_count;
reg        last_byte;
reg        save_select;
reg [7:0]  rx_fifo [0:MAX_READ_BYTES-1];

wire active = state != S_IDLE;
wire timing_valid = ADDRESS_SETUP_CYCLES > 0 &&
                    SELECT_SETUP_CYCLES  > 0 &&
                    DATA_WAIT_CYCLES     > 0 &&
                    RELEASE_CYCLES       > 0;
wire [32:0] read_last_address =
    {1'b0, address_reg} + {1'b0, length_reg} - 1;
wire read_rom_range_valid = address_reg[31:16] == 16'd0 &&
                            address_reg[15:0] <= 16'h7FFF &&
                            read_last_address <= 33'h00000_7FFF &&
                            READ_WINDOW_BYTES != 0 &&
                            length_reg <= READ_WINDOW_BYTES &&
                            address_reg >= READ_WINDOW_BASE &&
                            (address_reg - READ_WINDOW_BASE) <=
                                (READ_WINDOW_BYTES - length_reg);
wire read_save_range_valid = address_reg[31:16] == 16'd0 &&
                             address_reg[15:0] >= 16'hA000 &&
                             address_reg[15:0] <= 16'hBFFF &&
                             read_last_address <= 33'h00000_BFFF;
wire read_shape_valid = bus_reg == BUS_GB_GBC &&
                        length_reg != 0 &&
                        length_reg <= MAX_READ_BYTES &&
                        ((space_reg == SPACE_ROM && read_rom_range_valid) ||
                         (SAVE_READS_ENABLED != 0 &&
                          space_reg == SPACE_SAVE_PARALLEL &&
                          read_save_range_valid)) &&
                        (FIXED_HEADER_CHUNKS == 0 ||
                         (space_reg == SPACE_ROM &&
                          ((address_reg == 32'h0000_0100 &&
                            length_reg == 32) ||
                           (address_reg == 32'h0000_0120 &&
                            length_reg == 32) ||
                           (address_reg == 32'h0000_0140 &&
                            length_reg == 16)))) &&
                        (MAX_READ_COMMANDS == 0 ||
                         read_command_count < MAX_READ_COMMANDS) &&
                        timeout_reg != 0;

assign gb_data_dir    = 1'b0;
assign gb_wr_n        = 1'b1;
assign writes_enabled = 1'b0;

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
        REG_RX_LEVEL:     reg_rdata = fifo_count;
        REG_ERROR:        reg_rdata = error_reg;
        REG_LIFECYCLE:    reg_rdata = lifecycle_status;
        REG_HOST_B1_RAW:  reg_rdata = host_b1_raw;
        REG_HOST_DIAG:    reg_rdata = host_diag;
        REG_QUERY_ARG:    reg_rdata = query_arg_reg;
        REG_QUERY_RESULT: reg_rdata = query_result_reg;
        default:          reg_rdata = 32'd0;
    endcase
end

// Abstract GB/GBC pin owner.  The data bus is input-only and /WR is a
// constant one.  Every inactive or terminal state returns to this safe idle.
always @(*) begin
    gb_address     = 16'd0;
    gb_address_dir = 1'b0;
    gb_cs_n        = 1'b1;
    gb_rd_n        = 1'b1;

    case (state)
        S_ADDRESS: begin
            gb_address     = byte_address;
            gb_address_dir = 1'b1;
        end
        S_SELECT: begin
            gb_address     = byte_address;
            gb_address_dir = 1'b1;
            gb_cs_n        = save_select ? 1'b0 : 1'b1;
        end
        S_READ: begin
            gb_address     = byte_address;
            gb_address_dir = 1'b1;
            gb_cs_n        = save_select ? 1'b0 : 1'b1;
            gb_rd_n        = 1'b0;
        end
        S_RELEASE: begin
            gb_address     = byte_address;
            gb_address_dir = 1'b1;
            gb_cs_n        = save_select ? 1'b0 : 1'b1;
        end
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
        byte_address     <= 16'd0;
        bytes_remaining  <= 32'd0;
        phase_counter    <= 32'd0;
        watchdog_counter <= 32'd0;
        fifo_count       <= 32'd0;
        fifo_read_index  <= 32'd0;
        read_command_count <= 32'd0;
        last_byte        <= 1'b0;
        save_select      <= 1'b0;
    end else if (reg_wr && reg_addr == REG_COMMAND &&
                 reg_wdata == CMD_ABORT) begin
        state            <= S_IDLE;
        status_reg       <= ST_IDLE | ST_ABORTED;
        error_reg        <= ERR_NONE;
        phase_counter    <= 32'd0;
        watchdog_counter <= 32'd0;
    end else if (active && !lifecycle_ok) begin
        state            <= S_IDLE;
        status_reg       <= ST_IDLE | ST_ERROR;
        error_reg        <= ERR_LIFECYCLE;
        phase_counter    <= 32'd0;
        watchdog_counter <= 32'd0;
    end else if (active && watchdog_counter + 1 >= timeout_reg) begin
        state            <= S_IDLE;
        status_reg       <= ST_IDLE | ST_ERROR | ST_TIMEOUT;
        error_reg        <= ERR_TIMEOUT;
        phase_counter    <= 32'd0;
        watchdog_counter <= 32'd0;
    end else begin
        if (active)
            watchdog_counter <= watchdog_counter + 1;

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
                REG_BUS:       bus_reg       <= reg_wdata;
                REG_SPACE:     space_reg     <= reg_wdata;
                REG_ADDRESS:   address_reg   <= reg_wdata;
                REG_LENGTH:    length_reg    <= reg_wdata;
                REG_TIMEOUT:   timeout_reg   <= reg_wdata;
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
                        if (READS_ENABLED == 0 || !timing_valid) begin
                            status_reg <= ST_IDLE | ST_ERROR;
                            error_reg  <= ERR_NOT_SUPPORTED;
                        end else if (!lifecycle_ok || !read_shape_valid) begin
                            status_reg <= ST_IDLE | ST_ERROR;
                            error_reg  <= lifecycle_ok ? ERR_BAD_COMMAND : ERR_LIFECYCLE;
                        end else begin
                            state            <= S_ADDRESS;
                            status_reg       <= ST_BUSY;
                            error_reg        <= ERR_NONE;
                            byte_address     <= address_reg[15:0];
                            bytes_remaining  <= length_reg;
                            phase_counter    <= 32'd0;
                            watchdog_counter <= 32'd0;
                            fifo_count       <= 32'd0;
                            fifo_read_index  <= 32'd0;
                            read_command_count <= read_command_count + 1'b1;
                            last_byte        <= 1'b0;
                            save_select      <= space_reg ==
                                                SPACE_SAVE_PARALLEL;
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
                    state         <= S_READ;
                    phase_counter <= 32'd0;
                end else begin
                    phase_counter <= phase_counter + 1;
                end
            end
            S_READ: begin
                if (phase_counter + 1 >= DATA_WAIT_CYCLES) begin
                    rx_fifo[fifo_count] <= gb_data_in;
                    fifo_count          <= fifo_count + 1;
                    bytes_remaining     <= bytes_remaining - 1;
                    last_byte           <= bytes_remaining == 1;
                    state               <= S_RELEASE;
                    phase_counter       <= 32'd0;
                end else begin
                    phase_counter <= phase_counter + 1;
                end
            end
            S_RELEASE: begin
                if (phase_counter + 1 >= RELEASE_CYCLES) begin
                    phase_counter <= 32'd0;
                    if (last_byte) begin
                        state            <= S_IDLE;
                        status_reg       <= ST_IDLE | ST_DONE;
                        watchdog_counter <= 32'd0;
                    end else begin
                        byte_address <= byte_address + 1'b1;
                        state        <= S_ADDRESS;
                    end
                end else begin
                    phase_counter <= phase_counter + 1;
                end
            end
            default: begin end
        endcase
    end
end

endmodule

`default_nettype wire
