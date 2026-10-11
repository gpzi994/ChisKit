//------------------------------------------------------------------------------
// ChisBurner physical GB/GBC ROM read-only adapter.
//
// Pocket translator mapping is cross-checked against the pinned gamebub and
// openfpga-GBC implementations:
//   bank3 = A[7:0], bank2 = A[15:8], bank1 = D[7:0]
//   bank0 = {PHI, /WR, /RD, /CS}, pin30 sideband releases GB /RESET
//
// The protocol engine has no cartridge write command or write-data output.
// bank1 is permanently input and /WR is structurally tied inactive.
//------------------------------------------------------------------------------

`default_nettype none

module chisburner_cart_gb_readonly #(
    parameter integer READS_ENABLED        = 0,
    parameter integer ADDRESS_SETUP_CYCLES = 0,
    parameter integer SELECT_SETUP_CYCLES  = 0,
    parameter integer DATA_WAIT_CYCLES     = 0,
    parameter integer RELEASE_CYCLES       = 0,
    parameter integer MAX_READ_BYTES       = 32,
    parameter [31:0]  READ_WINDOW_BASE     = 32'h0000_0000,
    parameter [31:0]  READ_WINDOW_BYTES    = 32'h0000_8000,
    parameter integer FIXED_HEADER_CHUNKS  = 0,
    parameter integer MAX_READ_COMMANDS    = 0
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
    output wire [31:0] reg_rdata,

    // For interface compatibility bank3_in carries the physical GB data bus.
    // bank2_in is deliberately ignored because GB reads use bank1 as data.
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

wire [15:0] gb_address;
wire        gb_address_dir;
wire        gb_cs_n;
wire        gb_rd_n;
wire        gb_wr_n;
wire        unused_gb_data_dir;
wire        unused_engine_writes_enabled;
wire        unused_bank2 = ^bank2_in;

chisburner_cart_gb_readonly_abstract #(
    .READS_ENABLED(READS_ENABLED),
    .ADDRESS_SETUP_CYCLES(ADDRESS_SETUP_CYCLES),
    .SELECT_SETUP_CYCLES(SELECT_SETUP_CYCLES),
    .DATA_WAIT_CYCLES(DATA_WAIT_CYCLES),
    .RELEASE_CYCLES(RELEASE_CYCLES),
    .MAX_READ_BYTES(MAX_READ_BYTES),
    .READ_WINDOW_BASE(READ_WINDOW_BASE),
    .READ_WINDOW_BYTES(READ_WINDOW_BYTES),
    .FIXED_HEADER_CHUNKS(FIXED_HEADER_CHUNKS),
    .MAX_READ_COMMANDS(MAX_READ_COMMANDS),
    .SAVE_READS_ENABLED(0)
) engine (
    .clk(clk),
    .reset_n(reset_n),
    .lifecycle_ok(lifecycle_ok),
    .lifecycle_status(lifecycle_status),
    .host_b1_raw(host_b1_raw),
    .host_diag(host_diag),
    .reg_wr(reg_wr),
    .reg_rd(reg_rd),
    .reg_addr(reg_addr),
    .reg_wdata(reg_wdata),
    .reg_rdata(reg_rdata),
    .gb_address(gb_address),
    .gb_address_dir(gb_address_dir),
    .gb_data_in(bank3_in),
    .gb_data_dir(unused_gb_data_dir),
    .gb_cs_n(gb_cs_n),
    .gb_rd_n(gb_rd_n),
    .gb_wr_n(gb_wr_n),
    .writes_enabled(unused_engine_writes_enabled)
);

assign bank3_out = gb_address[7:0];
assign bank3_dir = gb_address_dir;
assign bank2_out = gb_address[15:8];
assign bank2_dir = gb_address_dir;
assign bank1_out = 8'h00;
assign bank1_dir = 1'b0;

assign bank0_out = {1'b1, gb_wr_n, gb_rd_n, gb_cs_n};
assign bank0_dir = 1'b1;

// In GB/GBC mode this framework sideband must be high to release the special
// Pocket reset/translator circuit.  The cartridge /RESET pin itself floats.
assign pin30_out = 1'b0;
assign pin30_dir = 1'b0;
assign pin30_pwroff_reset = 1'b1;
assign pin31_out = 1'b0;
assign pin31_dir = 1'b0;

// Keep the top-level interlock structurally constant as well.  The reduction
// above only consumes otherwise-unused inputs; it cannot influence this pin.
assign writes_enabled = 1'b0;

endmodule

`default_nettype wire
