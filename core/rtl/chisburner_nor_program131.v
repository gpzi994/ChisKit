// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// One buffered chunk or single programming unit using a wire-encoded recipe.
// OS must validate sector bounds, mapper routing and fresh-ID job authority.
// Special Intel 70/status/10-at-zero flow is selected explicitly by the OS.
// idle_adapters must include the complete memory CDC and raw bus transaction.
// Recipe inputs stay immutable in stream_regs131 for the whole busy period;
// keeping one authoritative descriptor avoids a second register copy.
module chisburner_nor_program131(
 input wire clk,reset_n,start,cancel,lifecycle_ok,write_allowed,idle_adapters,
 input wire unit16,intel_mode,dual_intel,msp_wide,swapped,raw_count,
 input wire disable_buffer_program,
 input wire [24:0] source_address,physical_address,unlock0,unlock1,
 input wire [16:0] length_bytes,buffer_bytes,
 input wire [15:0] aa,bb,buffer_command,confirm_command,reset_command,word_command,
 input wire [31:0] timeout_cycles,
 output reg busy,done,
 output reg [3:0] error,
 output wire mem_req,output wire [22:0] mem_address,
 input wire mem_ready,mem_done,mem_error,input wire [31:0] mem_data,
 output wire tx_req,tx_write,output wire [24:0] tx_address,output wire [15:0] tx_data,
 input wire tx_ready,tx_done,tx_error,input wire [15:0] tx_read_data
);
 localparam [4:0] R_CLEAR=0,R_RESET=1,R_NEXT=2,U0=3,U1=4,ENTER=5,AFTER_ENTER=6,
  B_START=7,B_ARM=8,B_WAIT=9,P_START=10,P_ARM=11,P_WAIT=12,AFTER_POLL=13,
  SEND=14,SEND_WAIT=15,DRAIN=16,FINISH=17,WORD_MEM=18,WORD_WAIT=19,WORD_SEND=20,WORD_POLL=21;
 reg [4:0] state,continuation;
 reg [1:0] reset_round;
 reg die,poll_entry;
 reg [15:0] single_payload;
 reg [24:0] command_addr;
 reg [15:0] command_data;
 reg [31:0] ticks;
 reg [7:0] byte_offset;
 // One FPGA register cache, never a write to PSRAM or SDRAM. Four byte
 // programs share one PSRAM word; invalidated on every new descriptor.
 reg word_valid;
 reg [31:0] word_data;
 reg bstart,pstart;
 wire bdone,pdone,breq,preq;
 wire [3:0] berror,perror;
 wire [24:0] baddr,paddr;
 wire [15:0] bdata,last_payload;
 wire bmem;
 wire [22:0] bmem_addr;
 wire single_requested=disable_buffer_program || buffer_bytes==0;
 wire w=unit16,i=intel_mode,d=dual_intel,m=msp_wide,s=swapped,r=raw_count;
 wire single=single_requested,special=disable_buffer_program;
 wire byte_batch=single_requested && !unit16 && !disable_buffer_program;
 wire [24:0] src=source_address+byte_offset,pa=physical_address+byte_offset,u0=unlock0,u1=unlock1;
 wire [16:0] len=length_bytes,bufsize=buffer_bytes;
 wire [15:0] cmd_aa=aa,cmd_bb=bb,cmd_buffer=buffer_command;
 wire [15:0] cmd_confirm=confirm_command,cmd_reset=reset_command,cmd_word=word_command;
 wire [31:0] timeout=timeout_cycles;
 wire [31:0] single_shifted=mem_data >> {src[1:0],3'b000};
 wire [31:0] cached_shifted=word_data >> {src[1:0],3'b000};
 // A byte batch is strictly sequential.  Once a word has been fetched, the
 // remaining non-zero byte lanes are necessarily in that same word; lane 0
 // always starts the next word.  This avoids a wide tag comparator in the
 // already placement-constrained core while preserving unaligned starts.
 wire word_hit=byte_batch && word_valid && src[1:0]!=0;
 wire child_cancel=cancel || state==DRAIN || state==FINISH;
 wire gate=busy && lifecycle_ok && write_allowed && !cancel;
 wire buffer_selected=state==B_ARM || state==B_WAIT;
 wire poll_selected=state==P_ARM || state==P_WAIT;
 assign mem_req=gate && (single?(state==WORD_MEM && !word_hit):bmem);
 assign mem_address=single?src[24:2]:bmem_addr;
 assign tx_req=gate && (buffer_selected?breq:poll_selected?preq:state==SEND);
 assign tx_write=!poll_selected;
 assign tx_address=buffer_selected?baddr:poll_selected?paddr:command_addr;
 assign tx_data=buffer_selected?bdata:command_data;
 wire [25:0] source_end={1'b0,source_address}+length_bytes;
 wire [25:0] physical_end={1'b0,physical_address}+length_bytes;
 wire [17:0] extent={1'b0,physical_address[16:0]&(buffer_bytes-17'd1)}+length_bytes;
 wire [24:0] reset_addr=(reset_round==1?pa:25'd0)+(die?25'd2:25'd0);
 wire [24:0] poll_addr=poll_entry && special?25'd0:
  poll_entry || i?pa+(die?25'd2:25'd0):single?pa:pa+len-(w?25'd2:25'd1);
 chisburner_nor_buffer131 payload(
  .clk(clk),.reset_n(reset_n),.start(bstart),.cancel(child_cancel),
  .lifecycle_ok(lifecycle_ok),.write_allowed(write_allowed),.unit16(w),.dual_intel(d),
  .swapped(s),.raw_count(r),.source_address(src),.physical_address(pa),
  .length_bytes(len),.buffer_bytes(bufsize),.confirm_command(cmd_confirm),.timeout_cycles(timeout),
  .done(bdone),.error(berror),.last_payload(last_payload),
  .mem_req(bmem),.mem_address(bmem_addr),.mem_ready(mem_ready && !single),.mem_done(mem_done && !single),
  .mem_error(mem_error),.mem_data(mem_data),.tx_req(breq),.tx_address(baddr),.tx_data(bdata),
  .tx_ready(tx_ready && buffer_selected),.tx_done(tx_done && buffer_selected),.tx_error(tx_error));
 chisburner_nor_poll131 polling(
  .clk(clk),.reset_n(reset_n),.start(pstart),.cancel(child_cancel),.lifecycle_ok(lifecycle_ok),
  .intel_mode(i),.msp_wide(m),.swapped(s),.address(poll_addr),.expected(single?single_payload:last_payload),
  .timeout_cycles(timeout),.done(pdone),.error(perror),.req(preq),.read_address(paddr),
  .ready(tx_ready && poll_selected),.response(tx_done && poll_selected),.response_error(tx_error),.data(tx_read_data));
 task send;
  input [24:0] a;input [15:0] v;input [4:0] next;
  begin command_addr<=a;command_data<=v;continuation<=next;state<=SEND;end
 endtask
 task fail;
  input [3:0] code;
  begin if(error==0)error<=code;state<=DRAIN;end
 endtask
 always @(posedge clk or negedge reset_n)begin
  if(!reset_n)begin
   busy<=0;done<=0;error<=0;state<=FINISH;continuation<=FINISH;reset_round<=0;
   die<=0;poll_entry<=0;single_payload<=0;command_addr<=0;command_data<=0;
   ticks<=0;bstart<=0;pstart<=0;byte_offset<=0;word_valid<=0;word_data<=0;
  end else begin
   bstart<=0;pstart<=0;
   if(busy)begin
    ticks<=ticks+1'b1;
    if(state!=DRAIN && state!=FINISH && (!lifecycle_ok || !write_allowed || cancel))fail(4'd2);
    else if(state!=DRAIN && state!=FINISH && ticks>=timeout-1)fail(4'd5);
    else case(state)
     R_CLEAR:if(i)send(reset_addr,16'h50,R_RESET);else state<=R_RESET;
     R_RESET:send(reset_addr,cmd_reset,R_NEXT);
     R_NEXT:begin
      if(d && !die)begin die<=1;state<=R_CLEAR;end
      else begin die<=0;
       if(reset_round==0)state<=i?ENTER:U0;
       else if(reset_round==1)begin reset_round<=2;state<=R_CLEAR;end
       else state<=FINISH;
      end
     end
     U0:send(u0,cmd_aa,U1);
     U1:send(u1,cmd_bb,ENTER);
     ENTER:begin
      if(special)send(25'd0,16'h70,AFTER_ENTER);
      else if(single)send(i?pa:u0,cmd_word,WORD_MEM);
      else send(pa+(die?25'd2:25'd0),cmd_buffer,AFTER_ENTER);
     end
     AFTER_ENTER:if(i)begin poll_entry<=1;state<=P_START;end else state<=B_START;
     B_START:begin bstart<=1;state<=B_ARM;end
     B_ARM:state<=B_WAIT;
     B_WAIT:if(bdone)begin
      if(berror!=0)fail(berror);else begin die<=0;poll_entry<=0;state<=P_START;end
     end
     P_START:begin pstart<=1;state<=P_ARM;end
     P_ARM:state<=P_WAIT;
     P_WAIT:if(pdone)begin if(perror!=0)fail(perror);else state<=AFTER_POLL;end
     AFTER_POLL:begin
      if(poll_entry)begin
       if(special)send(25'd0,16'h10,WORD_MEM);
       else if(d && !die)begin die<=1;state<=ENTER;end else begin die<=0;state<=B_START;end
      end else if(d && !die)begin die<=1;state<=P_START;end
      else if(byte_batch && byte_offset+17'd1<len)begin
       // Like Mori's block executor: unlock/program/poll each byte, without
       // returning to the OS or resetting/reselecting the mapper per byte.
       byte_offset<=byte_offset+1'b1;state<=U0;
      end else begin die<=0;reset_round<=1;state<=R_CLEAR;end
     end
     WORD_MEM:if(word_hit)begin single_payload<={8'd0,cached_shifted[7:0]};state<=WORD_SEND;end
      else if(mem_ready)state<=WORD_WAIT;
     WORD_WAIT:if(mem_done)begin
      if(mem_error)fail(4'd3);
      else begin
       word_data<=mem_data;word_valid<=1;
       single_payload<=w?single_shifted[15:0]:{8'd0,single_shifted[7:0]};state<=WORD_SEND;
      end
     end
     WORD_SEND:send(pa,single_payload,WORD_POLL);
     WORD_POLL:begin die<=0;poll_entry<=0;state<=P_START;end
     SEND:if(tx_ready)state<=SEND_WAIT;
     SEND_WAIT:if(tx_done)begin if(tx_error)fail(4'd4);else state<=continuation;end
     // Failure is fail-closed: retain ownership until both adapters drain.
     // OS must re-identify/reset before authorizing another operation.
     DRAIN:if(idle_adapters)state<=FINISH;
     FINISH:if(idle_adapters)begin busy<=0;done<=1;end
     default:fail(4'd1);
    endcase
   end else if(start)begin
    done<=0;error<=0;ticks<=0;single_payload<=0;die<=0;reset_round<=0;byte_offset<=0;word_valid<=0;
    if(cancel || !lifecycle_ok || !write_allowed || !idle_adapters)begin done<=1;error<=2;end
    else if(!length_bytes || !timeout_cycles ||
     (single_requested?((unit16?length_bytes!=17'd2:length_bytes>17'd256) || dual_intel):
       ((buffer_bytes&(buffer_bytes-17'd1))!=0 || length_bytes>buffer_bytes || extent>{1'b0,buffer_bytes})) ||
     (disable_buffer_program && (!intel_mode || !unit16 || dual_intel || msp_wide)) ||
     source_end>26'h2000000 || physical_end>26'h2000000 ||
     (unit16 && (source_address[0] || physical_address[0] || length_bytes[0] || unlock0[0] || unlock1[0])) ||
     (dual_intel && (!intel_mode || !unit16 || physical_address[1:0]!=0 || length_bytes[1:0]!=0)) ||
     (intel_mode && (msp_wide || !unit16)) || (msp_wide && !unit16) ||
     (!unit16 && (physical_end>26'h8000 || unlock0>=25'h8000 || unlock1>=25'h8000 ||
      ({1'b0,physical_address[13:0]}+length_bytes)>17'h4000)))begin done<=1;error<=1;end
    else begin busy<=1;state<=R_CLEAR;end
   end
  end
 end
endmodule
`default_nettype wire
