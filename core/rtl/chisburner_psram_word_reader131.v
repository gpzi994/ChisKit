// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// One-shot read-only PSRAM word reader for the Stream131 programming path.
// The external owner guarantees that both physical controllers are idle before
// this reader receives a request and keeps the lease until the CDC drains.
module chisburner_psram_word_reader131(
 input wire clk,reset_n,start,input wire [22:0] address,
 output reg p0_rd,p1_rd,output reg [21:0] p0_address,p1_address,
 input wire [31:0] p0_data,p1_data,input wire p0_valid,p1_valid,
 output reg busy,output reg done_toggle,output reg fault,output reg [31:0] data,
 output reg half
);
 localparam [1:0] IDLE=2'd0,ISSUE=2'd1,WAIT=2'd2;
 reg [1:0] state;
 reg [23:0] watchdog;
 wire selected_valid=half?p1_valid:p0_valid;
 wire [31:0] selected_data=half?p1_data:p0_data;
 always @(posedge clk or negedge reset_n) begin
  if(!reset_n)begin
   state<=IDLE;half<=0;watchdog<=0;p0_rd<=0;p1_rd<=0;
   p0_address<=0;p1_address<=0;busy<=0;done_toggle<=0;fault<=0;data<=0;
  end else begin
   p0_rd<=0;p1_rd<=0;
   case(state)
    IDLE:if(start && !busy)begin
     half<=address[22];p0_address<=address[21:0];p1_address<=address[21:0];
     busy<=1;fault<=0;watchdog<=0;state<=ISSUE;
    end
    ISSUE:begin
     if(half)p1_rd<=1;else p0_rd<=1;
     watchdog<=0;state<=WAIT;
    end
    WAIT:begin
     if(selected_valid)begin
      data<=selected_data;busy<=0;done_toggle<=~done_toggle;state<=IDLE;
     end else if(&watchdog)begin
      busy<=0;fault<=1;done_toggle<=~done_toggle;state<=IDLE;
     end else watchdog<=watchdog+24'd1;
    end
    default:begin busy<=0;fault<=1;state<=IDLE;end
   endcase
  end
 end
endmodule
`default_nettype wire
