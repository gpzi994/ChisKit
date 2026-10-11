// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// Hardware counterpart of core/nor/execute.c poll(). Uses the raw transaction
// adapter, never writes the cartridge. Owner must drain it after cancellation.
module chisburner_nor_poll131(
 input wire clk,reset_n,start,cancel,lifecycle_ok,
 input wire intel_mode,msp_wide,swapped,
 input wire [24:0] address,
 input wire [15:0] expected,
 input wire [31:0] timeout_cycles,
 output reg busy,done,
 output reg [3:0] error,
 output reg [15:0] last_status,
 output wire req,
 output wire [24:0] read_address,
 input wire ready,response,response_error,
 input wire [15:0] data
);
 localparam [3:0] ISSUE=0,WAIT=1,EVAL=2,SECOND=3,WAIT_SECOND=4,EVAL_SECOND=5,FALLBACK=6;
 reg [3:0] state;
 reg intel,wide,swap_bits,checking_failure;
 reg [24:0] addr;
 reg [15:0] target,first,second,failed_mask;
 reg [31:0] ticks,limit;
 wire [15:0] normalized=swap_bits?{first[15:2],first[0],first[1]}:first;
 wire [15:0] pending_bits=(first^target)&16'h8080;
 wire [15:0] failure_bits=((first&16'h2020)<<2)&pending_bits;
 assign req=busy && !cancel && lifecycle_ok && (state==ISSUE || state==SECOND);
 assign read_address=addr;
 task stop;
  input [3:0] code;
  begin busy<=0;done<=1;error<=code;end
 endtask
 always @(posedge clk or negedge reset_n)begin
  if(!reset_n)begin busy<=0;done<=0;error<=0;last_status<=0;state<=ISSUE;
   intel<=0;wide<=0;swap_bits<=0;checking_failure<=0;addr<=0;target<=0;
   first<=0;second<=0;failed_mask<=0;ticks<=0;limit<=0;end
  else if(busy)begin
   ticks<=ticks+1'b1;
   if(cancel || !lifecycle_ok)stop(4'd2);
   else if(ticks>=limit-1)stop(4'd5);
   else case(state)
    ISSUE:if(ready)state<=WAIT;
    WAIT:if(response)begin
     if(response_error)stop(4'd4);
     else begin first<=data;last_status<=data;state<=EVAL;end
    end
    EVAL:begin
     if(intel)begin
      if(normalized[7])stop((normalized&16'h003a)!=0?4'd7:4'd0);
      else state<=ISSUE;
     end else if(wide?first==target:first[7]==target[7])begin
      checking_failure<=0;state<=SECOND;
     end else state<=FALLBACK;
    end
    FALLBACK:begin
     if(wide?failure_bits!=0:first[5])begin
      failed_mask<=failure_bits;checking_failure<=1;state<=SECOND;
     end else state<=ISSUE;
    end
    SECOND:if(ready)state<=WAIT_SECOND;
    WAIT_SECOND:if(response)begin
     if(response_error)stop(4'd4);
     else begin second<=data;last_status<=data;state<=EVAL_SECOND;end
    end
    EVAL_SECOND:begin
     if(!checking_failure)begin
      if(wide?second==target:second[7]==target[7])stop(4'd0);
      else state<=FALLBACK;
     end else if(wide)begin
      if(((second^target)&failed_mask)!=0)stop(4'd7);else state<=ISSUE;
     end else stop(second[7]==target[7]?4'd0:4'd7);
    end
    default:stop(4'd1);
   endcase
  end else if(start)begin
   done<=0;error<=0;last_status<=0;ticks<=0;limit<=timeout_cycles;
   intel<=intel_mode;wide<=msp_wide;swap_bits<=swapped;addr<=address;target<=expected;state<=ISSUE;
   if(cancel || !lifecycle_ok)begin done<=1;error<=2;end
   else if(!timeout_cycles || (intel_mode && msp_wide))begin done<=1;error<=1;end
   else busy<=1;
  end
 end
endmodule
`default_nettype wire
