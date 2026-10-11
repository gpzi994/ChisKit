// SPDX-License-Identifier: AGPL-3.0-only
// Isolated EEPROM pin adapter. Parent must grant exclusive cartridge
// ownership before start and keep it until busy falls. Not in production top.
`default_nettype none
module chisburner_eeprom_pins139 #(
 parameter integer HALF_CYCLES=18,
 parameter integer TIMEOUT_CYCLES=20000
)(
 input wire clk,reset_n,lifecycle_ok,granted,start,cancel,mode_large,write_mode,
 input wire [13:0] block_address,input wire [63:0] write_data,input wire [7:0] bank3_in,
 output wire busy,done,error,output wire [63:0] data,
 output wire [7:0] bank3_out,bank2_out,bank1_out,
 output wire bank3_dir,bank2_dir,bank1_dir,
 output wire [3:0] bank0_out,output wire bank0_dir,
 output wire pin30_out,pin30_dir,pin30_pwroff_reset,pin31_out,pin31_dir
);
 wire live=reset_n&&lifecycle_ok&&granted&&!cancel;
 wire cs_n,rd_n,wr_n,data_oe,data_out,cart_clk,address_hold;
 wire drive=live&&busy;
 chisburner_eeprom139 #(.HALF_CYCLES(HALF_CYCLES),.TIMEOUT_CYCLES(TIMEOUT_CYCLES)) engine(
  .clk(clk),.reset_n(reset_n),.owner_ok(lifecycle_ok&&granted),
  .start(start&&live),.cancel(cancel),.mode_large(mode_large),.write_mode(write_mode),
  .block_address(block_address),.write_data(write_data),.data_in(bank3_in[0]),
  .busy(busy),.done(done),.error(error),.data(data),
  .cs_n(cs_n),.rd_n(rd_n),.wr_n(wr_n),.data_oe(data_oe),
  .data_out(data_out),.cart_clk(cart_clk),.address_hold(address_hold));
 // Pocket translator direction is per bank, not per bit. AD[15:1] stay at
 // their address values during TX; both AD banks become inputs before RX.
 assign bank3_out=address_hold?8'h80:{7'b1000000,data_out};
 assign bank2_out=8'hff;
 assign bank1_out=8'hff;
 assign bank3_dir=drive&&(address_hold||data_oe);
 assign bank2_dir=drive&&(address_hold||data_oe);
 assign bank1_dir=drive;
 assign bank0_out=drive?{cart_clk,wr_n,rd_n,cs_n}:4'hf;
 assign bank0_dir=1'b1;
 assign pin30_out=1'b1; // CS2 never selected by serial EEPROM reads.
 assign pin30_dir=drive;
 assign pin30_pwroff_reset=1'b0;
 assign pin31_out=1'b0;
 assign pin31_dir=1'b0;
endmodule
`default_nettype wire
