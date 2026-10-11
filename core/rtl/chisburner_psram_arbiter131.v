// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// One physical owner for legacy queues, CACHE129 and autonomous programming.
// All inputs are memory-clock-domain signals. program_lease is synchronized and
// held for the entire job; program_pending includes the complete data CDC drain.
module chisburner_psram_arbiter131(
 input wire clk,reset_n,read_request,scan_request,
 input wire [22:0] request_address,
 input wire legacy_pending,p0_busy,p1_busy,p0_valid,p1_valid,reader_busy,
 input wire program_lease,program_pending,
 output reg read_start,scan_start,output reg [22:0] reader_address,
 output wire legacy_accept,cache_owned,program_owned,program_granted,busy,
 output reg conflict
);
 localparam [2:0] IDLE=0,ACQUIRE=1,STARTED=2,RUNNING=3,DRAIN=4,PROGRAM=5;
 reg [2:0] state;
 reg lease,scan,program_selected;
 wire admit,owned,old_accept;
 wire request_any=read_request || scan_request;
 assign busy=state!=IDLE || owned;
 assign cache_owned=owned && !program_selected;
 assign program_owned=owned && program_selected;
 assign program_granted=admit && program_selected && program_lease && state==PROGRAM;
 // Close upstream admission in the very cycle a new lease is selected.
 assign legacy_accept=old_accept && !program_lease && !request_any;
 chisburner_psram_owner131 owner(
  .clk(clk),.reset_n(reset_n),.lease(lease),.legacy_pending(legacy_pending),
  .stream_pending(program_selected?program_pending:
    (reader_busy || state==STARTED || state==RUNNING)),
  .p0_busy(p0_busy),.p1_busy(p1_busy),.p0_valid(p0_valid),.p1_valid(p1_valid),
  .legacy_accept(old_accept),.stream_accept(admit),.stream_owner(owned));
 always @(posedge clk or negedge reset_n)begin
  if(!reset_n)begin
   state<=IDLE;lease<=0;scan<=0;program_selected<=0;
   read_start<=0;scan_start<=0;reader_address<=0;conflict<=0;
  end else begin
   read_start<=0;scan_start<=0;
   if(request_any && (busy || program_lease || (read_request && scan_request)))conflict<=1;
   case(state)
    IDLE:if(!owned)begin
     if(program_lease)begin program_selected<=1;lease<=1;state<=ACQUIRE;end
     else if(read_request ^ scan_request)begin
      program_selected<=0;scan<=scan_request;reader_address<=request_address;
      lease<=1;state<=ACQUIRE;
     end
    end
    ACQUIRE:if(admit)begin
     if(program_selected)state<=PROGRAM;
     else begin read_start<=!scan;scan_start<=scan;state<=STARTED;end
    end
    STARTED:if(reader_busy)state<=RUNNING;
    RUNNING:if(!reader_busy)begin lease<=0;state<=DRAIN;end
    PROGRAM:if(!program_lease)begin lease<=0;state<=DRAIN;end
    DRAIN:if(!owned)begin state<=IDLE;program_selected<=0;end
    default:begin lease<=0;state<=DRAIN;end
   endcase
  end
 end
endmodule
`default_nettype wire
