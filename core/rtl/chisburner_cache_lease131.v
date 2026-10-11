// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// Serializes existing CACHE129 commands against the TF write queues.
// A second command is rejected visibly (sticky conflict), never substituted.
module chisburner_cache_lease131(
 input wire clk,reset_n,read_request,scan_request,
 input wire [22:0] request_address,
 input wire legacy_pending,p0_busy,p1_busy,p0_valid,p1_valid,reader_busy,
 output reg read_start,scan_start,
 output reg [22:0] reader_address,
 output wire legacy_accept,owned,busy,
 output reg conflict
);
 localparam [2:0] IDLE=0,ACQUIRE=1,STARTED=2,RUNNING=3,DRAIN=4;
 reg [2:0] state;
 reg lease,scan;
 wire admit;
 assign busy=state!=IDLE || owned;
 chisburner_psram_owner131 owner(
  .clk(clk),.reset_n(reset_n),.lease(lease),.legacy_pending(legacy_pending),
  .stream_pending(reader_busy || state==STARTED || state==RUNNING),
  .p0_busy(p0_busy),.p1_busy(p1_busy),.p0_valid(p0_valid),.p1_valid(p1_valid),
  .legacy_accept(legacy_accept),.stream_accept(admit),.stream_owner(owned));
 always @(posedge clk or negedge reset_n)begin
  if(!reset_n)begin state<=IDLE;lease<=0;scan<=0;read_start<=0;scan_start<=0;
   reader_address<=0;conflict<=0;end
  else begin
   read_start<=0;scan_start<=0;
   if((read_request || scan_request) && (busy || (read_request && scan_request)))conflict<=1;
   case(state)
    IDLE:if((read_request ^ scan_request) && !owned)begin
     scan<=scan_request;reader_address<=request_address;lease<=1;state<=ACQUIRE;
    end
    ACQUIRE:if(admit)begin read_start<=!scan;scan_start<=scan;state<=STARTED;end
    STARTED:if(reader_busy)state<=RUNNING;
    RUNNING:if(!reader_busy)begin lease<=0;state<=DRAIN;end
    DRAIN:if(!owned)state<=IDLE;
    default:begin lease<=0;state<=DRAIN;end
   endcase
  end
 end
endmodule
`default_nettype wire
