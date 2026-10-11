// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// Autonomous register master for nor_bus108. Owner must exclusively route
// these signals to the raw engine while busy, including SAFE on failure.
// CPU configures bus/timeout before ownership transfer. No chip recipe here.
module chisburner_raw_adapter131 #(
 parameter integer TIMEOUT_CYCLES=9000000
)(
 input wire clk,reset_n,lifecycle_ok,write_allowed,abort,
 input wire req,write,
 input wire [24:0] address,
 input wire [15:0] data,
 output wire ready,
 output reg busy,done,fault,
 output reg [15:0] read_data,
 output reg reg_wr,reg_rd,
 output reg [4:0] reg_address,
 output reg [31:0] reg_data,
 input wire [31:0] reg_read_data,
 input wire raw_busy
);
 localparam [3:0] IDLE=0, ADDR=1, DATA=2, KEY1=3,KEY2=4,CMD=5,
                  ACCEPT=6,WAIT_BUSY=7,STATUS=8,RX=9,SAFE=10,DRAIN=11;
 reg [3:0] state;
 reg writing;
 reg [24:0] addr;
 reg [15:0] value;
 reg [31:0] watchdog;
 assign ready=!busy && !raw_busy && lifecycle_ok && !abort;
 // Combinational strobes are consumed by raw on the same rising edge.
 always @* begin
  reg_wr=0;reg_rd=0;reg_address=2;reg_data=0;
  if(busy) begin
   case(state)
    ADDR:begin reg_wr=1;reg_address=5;reg_data={7'd0,addr};end
    DATA:begin reg_wr=1;reg_address=17;reg_data={16'd0,value};end
    KEY1:begin reg_wr=1;reg_address=19;reg_data=32'h43424e31;end
    KEY2:begin reg_wr=1;reg_address=19;reg_data=32'h43424e32;end
    CMD:begin reg_wr=1;reg_address=3;reg_data=writing?7:2;end
    RX:begin reg_rd=1;reg_address=8;end
    SAFE:begin reg_wr=1;reg_address=3;reg_data=6;end
    default:begin end
   endcase
   // A revoke must suppress a write on the very same clock edge.
   if(state!=SAFE && (abort || !lifecycle_ok || (writing && !write_allowed)))begin
    reg_wr=0;reg_rd=0;
   end
  end
 end
 always @(posedge clk or negedge reset_n)begin
  if(!reset_n)begin state<=IDLE;busy<=0;done<=0;fault<=0;
   writing<=0;addr<=0;value<=0;watchdog<=0;read_data<=0;end
  else begin
   done<=0;
   if(busy)begin
    watchdog<=watchdog+1'b1;
    if(state!=SAFE && state!=DRAIN &&
       (abort || !lifecycle_ok || (writing && !write_allowed) || watchdog>=TIMEOUT_CYCLES-1))begin
     fault<=1;state<=SAFE;
    end else case(state)
     ADDR:state<=writing?DATA:CMD;
     DATA:state<=KEY1;
     KEY1:state<=KEY2;
     KEY2:state<=CMD;
     CMD:state<=ACCEPT;
     ACCEPT:state<=WAIT_BUSY;
     WAIT_BUSY:if(!raw_busy)state<=STATUS;
     STATUS:begin
      if(reg_read_data!=32'd5)begin fault<=1;state<=SAFE;end
      else if(writing)begin busy<=0;done<=1;state<=IDLE;end
      else state<=RX;
     end
     RX:begin read_data<=reg_read_data[15:0];busy<=0;done<=1;state<=IDLE;end
     SAFE:state<=DRAIN;
     DRAIN:if(!raw_busy)begin busy<=0;done<=1;state<=IDLE;end
     default:begin fault<=1;state<=SAFE;end
    endcase
   end else if(req && ready)begin
    busy<=1;fault<=0;writing<=write;addr<=address;value<=data;watchdog<=0;
    if(write && !write_allowed)begin fault<=1;state<=SAFE;end
    else state<=ADDR;
   end
  end
 end
endmodule
`default_nettype wire
