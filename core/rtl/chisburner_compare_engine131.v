// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// Compact GBA-only 16-bit comparator. Descriptor inputs remain stable while
// busy. Timeout/cancellation ownership is enforced by the outer submitter.
module chisburner_compare_engine131(
 input wire clk,reset_n,lifecycle_ok,abort,start,
 input wire [25:0] base_bytes,length_bytes,image_bytes,nor_bytes,
 output reg busy,done,output reg [3:0] error,
 output wire [25:0] completed_bytes,output reg [25:0] fault_address,
 output reg [15:0] fault_expected,fault_actual,
 output wire mem_req,output wire [22:0] mem_word_address,
 input wire mem_ready,mem_done,mem_error,input wire [31:0] mem_data,
 output wire nor_req,output wire [24:0] nor_address,
 input wire nor_ready,nor_done,nor_error,input wire [15:0] nor_read_data
);
 localparam [2:0] IDLE=0,FETCH=1,MREQ=2,MWAIT=3,NREQ=4,NWAIT=5;
 localparam [3:0] E_RANGE=1,E_ABORT=2,E_MEMORY=3,E_NOR=4,E_COMPARE=6;
 reg [2:0] state;
 reg [25:0] address,origin;
 reg cached;
 reg [22:0] cached_address;
 reg [31:0] cached_data;
 reg [15:0] expected;
 wire [26:0] requested_end={1'b0,base_bytes}+{1'b0,length_bytes};
 wire [31:0] shifted=cached_data>>{address[1:0],3'b000};
 assign completed_bytes=address-origin;
 assign mem_req=busy && lifecycle_ok && !abort && state==MREQ;
 assign mem_word_address=address[24:2];
 assign nor_req=busy && lifecycle_ok && !abort && state==NREQ;
 assign nor_address=address[24:0];
 task stop;input [3:0] why;begin
  busy<=0;done<=1;error<=why;state<=IDLE;fault_address<=address;cached<=0;
 end endtask
 always @(posedge clk or negedge reset_n)begin
  if(!reset_n)begin
   busy<=0;done<=0;error<=0;state<=IDLE;address<=0;origin<=0;cached<=0;
   cached_address<=0;cached_data<=0;expected<=0;fault_address<=0;
   fault_expected<=0;fault_actual<=0;
  end else if(busy)begin
   if(!lifecycle_ok || abort)stop(E_ABORT);
   else case(state)
    FETCH:begin
     if(address>=requested_end[25:0])begin busy<=0;done<=1;state<=IDLE;cached<=0;end
     else if(address>=image_bytes)begin expected<=16'hffff;state<=NREQ;end
     else if(cached && cached_address==address[24:2])begin
      expected<=shifted[15:0];state<=NREQ;
     end else state<=MREQ;
    end
    MREQ:if(mem_ready)state<=MWAIT;
    MWAIT:if(mem_done)begin
     if(mem_error)stop(E_MEMORY);
     else begin cached<=1;cached_address<=address[24:2];cached_data<=mem_data;state<=FETCH;end
    end
    NREQ:if(nor_ready)state<=NWAIT;
    NWAIT:if(nor_done)begin
     if(nor_error)stop(E_NOR);
     else if(nor_read_data!=expected)begin
      fault_expected<=expected;fault_actual<=nor_read_data;stop(E_COMPARE);
     end else begin address<=address+2'd2;state<=FETCH;end
    end
    default:stop(E_ABORT);
   endcase
  end else if(start)begin
   done<=0;error<=0;fault_address<=0;fault_expected<=0;fault_actual<=0;
   address<=base_bytes;origin<=base_bytes;cached<=0;
   if(!lifecycle_ok || abort)begin done<=1;error<=E_ABORT;end
   else if(!length_bytes || base_bytes[0] || length_bytes[0] ||
    requested_end[26] || requested_end>nor_bytes || image_bytes>nor_bytes || nor_bytes>26'h2000000)
    begin done<=1;error<=E_RANGE;end
   else begin busy<=1;state<=FETCH;end
  end
 end
endmodule
`default_nettype wire
