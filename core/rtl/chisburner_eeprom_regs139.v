// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// Dedicated OS-private register window, NOT a public application interface.
// Parent routes ALL other cartridge submissions through allow_other, supplies
// other_busy including adapter drain, and uses hold_owner for the pin mux.
// A simultaneous other request wins. No production router connection yet.
module chisburner_eeprom_regs139(
 input wire clk,reset_n,lifecycle_ok,selected,
 input wire other_busy,other_request,
 input wire wr,input wire [2:0] address,read_address,input wire [31:0] data,
 output reg [31:0] read_data,
 output reg hold_owner,output wire allow_other,
 output reg engine_start,output wire engine_cancel,engine_owner,
 output reg mode_large,write_mode,output reg [13:0] block_address,
 output reg [63:0] write_data,
 input wire engine_busy,engine_done,engine_error,input wire [63:0] engine_data
);
 reg [31:0] descriptor;
 reg done,fault,rejected;
 reg [63:0] result,payload;
 wire live=reset_n&&lifecycle_ok&&selected;
 wire cancel_now=wr&&address==2&&data==2;
 wire shape=descriptor[31:15]==0 &&
  (descriptor[14]?descriptor[13:0]<1024:descriptor[13:0]<64);
 assign allow_other=live&&!hold_owner&&!engine_busy;
 assign engine_owner=live&&hold_owner;
 assign engine_cancel=!live||cancel_now;
 always @* begin
  read_data=0;
  case(read_address)
   0:read_data=32'h45453139;
   1:read_data=descriptor;
   3:read_data={28'd0,rejected,fault,done,hold_owner};
   4:read_data=done?result[63:32]:32'd0; // first four bytes, MSB first
   5:read_data=done?result[31:0]:32'd0;
   default:begin end
  endcase
 end
 always @(posedge clk or negedge reset_n) begin
  if(!reset_n)begin
   descriptor<=0;done<=0;fault<=0;rejected<=0;result<=0;
   hold_owner<=0;engine_start<=0;mode_large<=0;write_mode<=0;block_address<=0;write_data<=0;payload<=0;
  end else if(!live)begin
   descriptor<=0;done<=0;fault<=0;rejected<=0;result<=0;
   hold_owner<=0;engine_start<=0;mode_large<=0;write_mode<=0;block_address<=0;write_data<=0;payload<=0;
  end else begin
   engine_start<=0;
   // Keep ownership during the registered launch cycle: stale DONE from the
   // preceding block must not complete the new request before engine START.
   if(hold_owner&&!engine_start&&!engine_busy&&(engine_done||engine_error))begin
    hold_owner<=0;done<=engine_done&&!engine_error;fault<=engine_error;
    result<=engine_error?64'd0:engine_data;
   end
   if(cancel_now)begin
    engine_start<=0;done<=0;fault<=1;result<=0;
    // Hold through the abort edge; release only after the engine drains.
    if(!engine_busy)hold_owner<=0;
   end else if(wr)begin
    if(address==1&&!hold_owner&&!engine_busy)descriptor<=data;
    else if(address==4&&!hold_owner&&!engine_busy)payload[63:32]<=data;
    else if(address==5&&!hold_owner&&!engine_busy)payload[31:0]<=data;
    else if(address==2&&(data==1||data==32'h57524954)&&!hold_owner&&!engine_busy&&
            !other_busy&&!other_request&&shape)begin
     mode_large<=descriptor[14];block_address<=descriptor[13:0];
     write_mode<=data==32'h57524954;write_data<=payload;
     hold_owner<=1;engine_start<=1;done<=0;fault<=0;rejected<=0;result<=0;
    end else rejected<=1;
   end
  end
 end
endmodule
`default_nettype wire
