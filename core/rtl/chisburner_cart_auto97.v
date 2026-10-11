// SPDX-License-Identifier: Apache-2.0
// ChisBurner UI97: bounded dual-bus header probe. No NOR or save commands.
// GB ROM probe keeps /CS high, so a GBA cartridge stays deselected.
// Selection changes only between transactions. SAFE/ABORT reach both engines.
`default_nettype none
module chisburner_cart_auto97(
input wire clk, reset_n, lifecycle_ok,
input wire [31:0] lifecycle_status, host_b1_raw, host_diag,
input wire reg_wr, reg_rd,
input wire [4:0] reg_addr,
input wire [31:0] reg_wdata,
output wire [31:0] reg_rdata,
input wire [7:0] gb_data_in, bank3_in, bank2_in,
output wire [7:0] bank3_out,
output wire bank3_dir,
output wire [7:0] bank2_out,
output wire bank2_dir,
output wire [7:0] bank1_out,
output wire bank1_dir,
output wire [3:0] bank0_out,
output wire bank0_dir,
output wire pin30_out,
output wire pin30_dir,
output wire pin30_pwroff_reset,
output wire pin31_out,
output wire pin31_dir,
output wire writes_enabled
);
reg [31:0] selected;
wire [31:0] gb_rdata, gba_rdata;
wire gb_busy, gba_busy;
wire [31:0] active_rdata = selected == 1 ? gb_rdata : gba_rdata;
// reg_rdata status is only valid while reg_addr==2; latch command ownership
// with an explicit active bit until SAFE/ABORT, or consult each engine status.
wire selecting = reg_wr && reg_addr == 4 && (reg_wdata == 1 || reg_wdata == 2);
reg pending;
always @(posedge clk or negedge reset_n) begin
 if (!reset_n) begin selected <= 1; pending <= 0; end
 else if (!lifecycle_ok) begin selected <= 1; pending <= 0; end
 else begin
  if (reg_addr == 2 && !active_rdata[1]) pending <= 0;
  if (reg_wr && reg_addr == 3 && reg_wdata == 2) pending <= 1;
  if (reg_wr && reg_addr == 3 && (reg_wdata == 1 || reg_wdata == 3)) pending <= 0;
  if (selecting && !pending) selected <= reg_wdata;
 end
end
wire broadcast = (reg_addr != 3) ||
                 (reg_wdata == 1 || reg_wdata == 3);
wire accepted_wr = reg_wr && !(selecting && pending);
assign reg_rdata = reg_addr == 1 ? 32'h00210f03 : active_rdata;
assign writes_enabled = 0;
wire [7:0] gb_bank3_out, gba_bank3_out;
assign bank3_out = selected == 1 ? gb_bank3_out : gba_bank3_out;
wire gb_bank3_dir, gba_bank3_dir;
assign bank3_dir = selected == 1 ? gb_bank3_dir : gba_bank3_dir;
wire [7:0] gb_bank2_out, gba_bank2_out;
assign bank2_out = selected == 1 ? gb_bank2_out : gba_bank2_out;
wire gb_bank2_dir, gba_bank2_dir;
assign bank2_dir = selected == 1 ? gb_bank2_dir : gba_bank2_dir;
wire [7:0] gb_bank1_out, gba_bank1_out;
assign bank1_out = selected == 1 ? gb_bank1_out : gba_bank1_out;
wire gb_bank1_dir, gba_bank1_dir;
assign bank1_dir = selected == 1 ? gb_bank1_dir : gba_bank1_dir;
wire [3:0] gb_bank0_out, gba_bank0_out;
assign bank0_out = selected == 1 ? gb_bank0_out : gba_bank0_out;
wire gb_bank0_dir, gba_bank0_dir;
assign bank0_dir = selected == 1 ? gb_bank0_dir : gba_bank0_dir;
wire gb_pin30_out, gba_pin30_out;
assign pin30_out = selected == 1 ? gb_pin30_out : gba_pin30_out;
wire gb_pin30_dir, gba_pin30_dir;
assign pin30_dir = selected == 1 ? gb_pin30_dir : gba_pin30_dir;
wire gb_pin30_pwroff_reset, gba_pin30_pwroff_reset;
assign pin30_pwroff_reset = selected == 1 ? gb_pin30_pwroff_reset : gba_pin30_pwroff_reset;
wire gb_pin31_out, gba_pin31_out;
assign pin31_out = selected == 1 ? gb_pin31_out : gba_pin31_out;
wire gb_pin31_dir, gba_pin31_dir;
assign pin31_dir = selected == 1 ? gb_pin31_dir : gba_pin31_dir;
chisburner_cart_gb_readonly #(
 .READS_ENABLED(1), .ADDRESS_SETUP_CYCLES(18),
 .SELECT_SETUP_CYCLES(9), .DATA_WAIT_CYCLES(36), .RELEASE_CYCLES(9),
 .MAX_READ_BYTES(32), .READ_WINDOW_BASE(32'h100),
 .READ_WINDOW_BYTES(32'h60), .MAX_READ_COMMANDS(0)
) gb (
 .clk(clk), .reset_n(reset_n), .lifecycle_ok(lifecycle_ok),
 .lifecycle_status(lifecycle_status), .host_b1_raw(host_b1_raw), .host_diag(host_diag),
 .reg_wr(accepted_wr && (broadcast || selected == 1)),
 .reg_rd(reg_rd && selected == 1), .reg_addr(reg_addr), .reg_wdata(reg_wdata),
 .reg_rdata(gb_rdata), .bank3_in(gb_data_in), .bank2_in(8'd0),
 .bank3_out(gb_bank3_out),
 .bank3_dir(gb_bank3_dir),
 .bank2_out(gb_bank2_out),
 .bank2_dir(gb_bank2_dir),
 .bank1_out(gb_bank1_out),
 .bank1_dir(gb_bank1_dir),
 .bank0_out(gb_bank0_out),
 .bank0_dir(gb_bank0_dir),
 .pin30_out(gb_pin30_out),
 .pin30_dir(gb_pin30_dir),
 .pin30_pwroff_reset(gb_pin30_pwroff_reset),
 .pin31_out(gb_pin31_out),
 .pin31_dir(gb_pin31_dir),
 .writes_enabled()
);
chisburner_cart_gba_readonly #(
 .READS_ENABLED(1), .ADDRESS_SETUP_CYCLES(12), .SELECT_SETUP_CYCLES(6),
 .TURNAROUND_CYCLES(3), .DATA_WAIT_CYCLES(18), .RELEASE_CYCLES(6),
 .MAX_READ_BYTES(32), .READ_WINDOW_BYTES(32'hc0),
 .REQUIRED_READ_BYTES(32), .READ_ADDRESS_MASK(32'h1f),
 .IDENTIFY_ENABLED(0), .MAX_READ_COMMANDS(0)
) gba (
 .clk(clk), .reset_n(reset_n), .lifecycle_ok(lifecycle_ok),
 .lifecycle_status(lifecycle_status), .host_b1_raw(host_b1_raw), .host_diag(host_diag),
 .reg_wr(accepted_wr && (broadcast || selected == 2)),
 .reg_rd(reg_rd && selected == 2), .reg_addr(reg_addr), .reg_wdata(reg_wdata),
 .reg_rdata(gba_rdata), .bank3_in(bank3_in), .bank2_in(bank2_in),
 .bank3_out(gba_bank3_out),
 .bank3_dir(gba_bank3_dir),
 .bank2_out(gba_bank2_out),
 .bank2_dir(gba_bank2_dir),
 .bank1_out(gba_bank1_out),
 .bank1_dir(gba_bank1_dir),
 .bank0_out(gba_bank0_out),
 .bank0_dir(gba_bank0_dir),
 .pin30_out(gba_pin30_out),
 .pin30_dir(gba_pin30_dir),
 .pin30_pwroff_reset(gba_pin30_pwroff_reset),
 .pin31_out(gba_pin31_out),
 .pin31_dir(gba_pin31_dir),
 .writes_enabled(), .busy(gba_busy), .profile_verified()
);
endmodule
`default_nettype wire

