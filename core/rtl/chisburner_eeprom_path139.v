// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// Isolated complete EEPROM register-to-pin path. The outer router must route
// competing submissions through other_accept and mux pins on hold_owner.
// other_busy must include raw/program/adapter drain. Not in production top.
module chisburner_eeprom_path139 #(
 parameter integer HALF_CYCLES=18, TIMEOUT_CYCLES=20000
)(
 input wire clk,reset_n,lifecycle_ok,selected,other_busy,other_request,
 input wire wr,input wire [2:0] address,read_address,input wire [31:0] data,
 output wire [31:0] read_data,output wire hold_owner,other_accept,
 input wire [7:0] bank3_in,
 output wire [7:0] bank3_out,bank2_out,bank1_out,
 output wire bank3_dir,bank2_dir,bank1_dir,
 output wire [3:0] bank0_out,output wire bank0_dir,
 output wire pin30_out,pin30_dir,pin30_pwroff_reset,pin31_out,pin31_dir
);
 wire allow_other,start,cancel,owner,mode_large,write_mode,busy,done,error;
 wire [13:0] block_address;wire [63:0] result,write_data;
 assign other_accept=other_request&&allow_other&&!other_busy;
 chisburner_eeprom_regs139 regs(
  .clk(clk),.reset_n(reset_n),.lifecycle_ok(lifecycle_ok),.selected(selected),
  .other_busy(other_busy),.other_request(other_request),
  .wr(wr),.address(address),.read_address(read_address),.data(data),.read_data(read_data),
  .hold_owner(hold_owner),.allow_other(allow_other),.engine_start(start),
  .engine_cancel(cancel),.engine_owner(owner),.mode_large(mode_large),.write_mode(write_mode),
  .block_address(block_address),.write_data(write_data),.engine_busy(busy),.engine_done(done),
  .engine_error(error),.engine_data(result));
 chisburner_eeprom_pins139 #(.HALF_CYCLES(HALF_CYCLES),.TIMEOUT_CYCLES(TIMEOUT_CYCLES)) pins(
  .clk(clk),.reset_n(reset_n),.lifecycle_ok(lifecycle_ok&&selected),
  .granted(owner),.start(start),.cancel(cancel),.mode_large(mode_large),.write_mode(write_mode),
  .block_address(block_address),.write_data(write_data),.bank3_in(bank3_in),
  .busy(busy),.done(done),.error(error),.data(result),
  .bank3_out(bank3_out),.bank2_out(bank2_out),.bank1_out(bank1_out),
  .bank3_dir(bank3_dir),.bank2_dir(bank2_dir),.bank1_dir(bank1_dir),
  .bank0_out(bank0_out),.bank0_dir(bank0_dir),
  .pin30_out(pin30_out),.pin30_dir(pin30_dir),.pin30_pwroff_reset(pin30_pwroff_reset),
  .pin31_out(pin31_out),.pin31_dir(pin31_dir));
endmodule
`default_nettype wire
