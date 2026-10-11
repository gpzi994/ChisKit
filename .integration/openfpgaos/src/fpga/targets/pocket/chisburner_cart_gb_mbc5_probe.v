//------------------------------------------------------------------------------
// ChisBurner constrained MBC5 ROM-bank probe.
//
// The public register interface remains read/query/safe/abort only.  A read is
// accepted only when it matches the exact 16-command diagnostic sequence below.
// Internally the engine may write one byte to 0x2000 to select the low eight
// MBC5 ROM-bank bits, then it restores bank 1 before reporting DONE.
//
// Structural safety boundary:
//   * /WR can be asserted only while A == 0x2000.
//   * Write data is only one of {1,2,31,32,63,64,126,127}.
//   * 0x0000 RAM-enable, 0x3000 bank-high, 0x4000 RAM-bank, and every NOR
//     unlock/erase/program address are unreachable.
//   * No erase, program, generic write, save-space, or arbitrary read command
//     exists in this profile.
//------------------------------------------------------------------------------

`default_nettype none

module chisburner_cart_gb_mbc5_probe #(
    parameter integer ADDRESS_SETUP_CYCLES = 18,
    parameter integer SELECT_SETUP_CYCLES  = 9,
    parameter integer DATA_WAIT_CYCLES     = 36,
    parameter integer RELEASE_CYCLES       = 9,
    parameter integer WRITE_SETUP_CYCLES   = 18,
    parameter integer WRITE_LOW_CYCLES     = 36,
    parameter integer WRITE_RELEASE_CYCLES = 18,
    parameter integer MAX_READ_BYTES       = 32
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

    // bank3_in carries physical Pocket bank1, the GB/GBC data bus.
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

localparam [31:0] CART_ID          = 32'h3142_4746;
localparam [31:0] CART_CAPS        = 32'h0001_0F01;
localparam [31:0] CMD_SAFE_IDLE    = 32'd1;
localparam [31:0] CMD_READ         = 32'd2;
localparam [31:0] CMD_ABORT        = 32'd3;
localparam [31:0] CMD_QUERY_FW_INFO = 32'h0000_00A1;
localparam [31:0] CMD_PING         = 32'h0000_00FE;
localparam [31:0] QUERY_SIGNATURE  = 32'h5042_4746;
localparam [31:0] QUERY_VERSION    = 32'h0001_0000;
localparam [31:0] QUERY_FEATURES   = 32'h0000_0003;

localparam [31:0] ST_IDLE    = 32'h0000_0001;
localparam [31:0] ST_BUSY    = 32'h0000_0002;
localparam [31:0] ST_DONE    = 32'h0000_0004;
localparam [31:0] ST_ERROR   = 32'h0000_0008;
localparam [31:0] ST_ABORTED = 32'h0000_0010;
localparam [31:0] ST_TIMEOUT = 32'h0000_0020;

localparam [31:0] ERR_NONE          = 32'd0;
localparam [31:0] ERR_BAD_COMMAND   = 32'd1;
localparam [31:0] ERR_LIFECYCLE     = 32'd3;
localparam [31:0] ERR_TIMEOUT       = 32'd4;

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
reg [31:0] query_arg_reg;
reg [31:0] query_result_reg;
reg [15:0] byte_address;
reg [7:0]  target_bank;
reg [31:0] bytes_remaining;
reg [31:0] phase_counter;
reg [31:0] watchdog_counter;
reg [31:0] fifo_count;
reg [31:0] fifo_read_index;
reg [4:0]  read_command_count;
reg        last_byte;
reg [7:0]  rx_fifo [0:MAX_READ_BYTES-1];

wire active = state != S_IDLE;
wire timing_valid = ADDRESS_SETUP_CYCLES > 0 && SELECT_SETUP_CYCLES > 0 &&
                    DATA_WAIT_CYCLES > 0 && RELEASE_CYCLES > 0 &&
                    WRITE_SETUP_CYCLES > 0 && WRITE_LOW_CYCLES > 0 &&
                    WRITE_RELEASE_CYCLES > 0;

function [31:0] expected_address;
    input [2:0] index;
    begin
        case (index)
            3'd0: expected_address = 32'h0000_4000; // bank 1, first 32 B
            3'd1: expected_address = 32'h0000_8000; // bank 2
            3'd2: expected_address = 32'h0007_C000; // bank 31
            3'd3: expected_address = 32'h0008_0000; // bank 32
            3'd4: expected_address = 32'h000F_C000; // bank 63
            3'd5: expected_address = 32'h0010_0000; // bank 64
            3'd6: expected_address = 32'h001F_C000; // bank 127
            default: expected_address = 32'h001F_FFE0; // final 32 B
        endcase
    end
endfunction

wire read_shape_valid = timing_valid && bus_reg == 32'd1 &&
                        space_reg == 32'd0 && length_reg == 32'd32 &&
                        read_command_count < 16 &&
                        address_reg == expected_address(read_command_count[2:0]) &&
                        address_reg[31:21] == 0 && timeout_reg != 0;

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
            gb_address     = 16'h2000;
            gb_address_dir = 1'b1;
            gb_write_data  = target_bank;
            gb_data_dir    = 1'b1;
            gb_wr_n        = (state == S_BANK_WRITE) ? 1'b0 : 1'b1;
        end
        S_READ_ADDRESS, S_READ_SELECT, S_READ_DATA, S_READ_RELEASE: begin
            gb_address     = byte_address;
            gb_address_dir = 1'b1;
            gb_rd_n        = (state == S_READ_DATA) ? 1'b0 : 1'b1;
        end
        S_RESTORE_ADDRESS, S_RESTORE_WRITE, S_RESTORE_RELEASE: begin
            gb_address     = 16'h2000;
            gb_address_dir = 1'b1;
            gb_write_data  = 8'h01;
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
        query_arg_reg <= 0;
        query_result_reg <= 0;
        byte_address <= 0;
        target_bank <= 1;
        bytes_remaining <= 0;
        phase_counter <= 0;
        watchdog_counter <= 0;
        fifo_count <= 0;
        fifo_read_index <= 0;
        read_command_count <= 0;
        last_byte <= 0;
    end else if (reg_wr && reg_addr == REG_COMMAND && reg_wdata == CMD_ABORT) begin
        state <= S_IDLE;
        status_reg <= ST_IDLE | ST_ABORTED;
        error_reg <= ERR_NONE;
        phase_counter <= 0;
        watchdog_counter <= 0;
    end else if (active && !lifecycle_ok) begin
        state <= S_IDLE;
        status_reg <= ST_IDLE | ST_ERROR;
        error_reg <= ERR_LIFECYCLE;
        phase_counter <= 0;
        watchdog_counter <= 0;
    end else if (active && watchdog_counter + 1 >= timeout_reg) begin
        state <= S_IDLE;
        status_reg <= ST_IDLE | ST_ERROR | ST_TIMEOUT;
        error_reg <= ERR_TIMEOUT;
        phase_counter <= 0;
        watchdog_counter <= 0;
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
            case (reg_addr)
                REG_BUS:       bus_reg <= reg_wdata;
                REG_SPACE:     space_reg <= reg_wdata;
                REG_ADDRESS:   address_reg <= reg_wdata;
                REG_LENGTH:    length_reg <= reg_wdata;
                REG_TIMEOUT:   timeout_reg <= reg_wdata;
                REG_QUERY_ARG: query_arg_reg <= reg_wdata;
                REG_COMMAND: begin
                    if (reg_wdata == CMD_SAFE_IDLE) begin
                        status_reg <= ST_IDLE;
                        error_reg <= ERR_NONE;
                        fifo_count <= 0;
                        fifo_read_index <= 0;
                        watchdog_counter <= 0;
                    end else if (reg_wdata == CMD_READ) begin
                        if (!lifecycle_ok || !read_shape_valid) begin
                            status_reg <= ST_IDLE | ST_ERROR;
                            error_reg <= lifecycle_ok ? ERR_BAD_COMMAND : ERR_LIFECYCLE;
                        end else begin
                            state <= S_BANK_ADDRESS;
                            status_reg <= ST_BUSY;
                            error_reg <= ERR_NONE;
                            target_bank <= {1'b0, address_reg[20:14]};
                            byte_address <= 16'h4000 | {2'b00, address_reg[13:0]};
                            bytes_remaining <= length_reg;
                            phase_counter <= 0;
                            watchdog_counter <= 0;
                            fifo_count <= 0;
                            fifo_read_index <= 0;
                            read_command_count <= read_command_count + 1'b1;
                            last_byte <= 0;
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
                    rx_fifo[fifo_count] <= bank3_in;
                    fifo_count <= fifo_count + 1'b1;
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
                        state <= S_RESTORE_ADDRESS;
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

// This flag advertises erase/program capability to the OS, not the private
// fixed MBC5 mapper-select pulse.  Destructive cartridge writes remain absent.
assign writes_enabled = 1'b0;

wire unused_bank2 = ^bank2_in;

endmodule

`default_nettype wire
