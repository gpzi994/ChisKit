`timescale 1ns/1ps
module tb_shared_lease131;
 parameter S=5,M=7;
 reg sc=0,mc=0;always #S sc=~sc;always #M mc=~mc;
 reg sr=0,mr=0,req=0,cancel=0,hold=0,rd=0,rb=0,pending=0;
 wire grant,busy,lease,mg,la,co,po,ab,rs;
 integer t;
 chisburner_lease_cdc131 #(.EXTERNAL_OWNER(1)) endpoint(
  .s_clk(sc),.s_reset_n(sr),.request(req),.cancel(cancel),.lifecycle_ok(1'b1),.hold(hold),
  .granted(grant),.busy(busy),.m_clk(mc),.m_reset_n(mr),
  .legacy_pending(1'b0),.stream_pending(1'b0),.p0_busy(1'b0),.p1_busy(1'b0),.p0_valid(1'b0),.p1_valid(1'b0),
  .m_lease(lease),.m_granted(mg),.m_returned(!po && !ab));
 chisburner_psram_arbiter131 arbiter(
  .clk(mc),.reset_n(mr),.read_request(rd),.scan_request(1'b0),.request_address(23'd0),
  .legacy_pending(1'b0),.p0_busy(1'b0),.p1_busy(1'b0),.p0_valid(1'b0),.p1_valid(1'b0),
  .reader_busy(rb),.program_lease(lease),.program_pending(pending),
  .read_start(rs),.legacy_accept(la),.cache_owned(co),.program_owned(po),.program_granted(mg),.busy(ab));
 task released;begin t=0;while(busy)begin @(negedge sc);t=t+1;if(t>200)$fatal(1,"release timeout");end end endtask
 initial begin
  repeat(3)@(negedge sc);sr=1;@(negedge mc);mr=1;rd=1;
  @(negedge mc);rd=0;wait(rs);@(negedge mc);rb=1;
  @(negedge sc);req=1;repeat(20)@(negedge sc);
  if(grant || !co || po)$fatal(1,"cache was preempted");
  @(negedge mc);rb=0;t=0;
  while(!grant)begin @(negedge sc);t=t+1;if(t>200)$fatal(1,"grant timeout");end
  hold=1;req=0;repeat(20)@(negedge sc);
  if(grant || !po || !busy)$fatal(1,"job ownership lost");
  pending=1;hold=0;repeat(20)@(negedge sc);
  if(!po || !busy || la)$fatal(1,"CDC pending ownership lost");
  @(negedge mc);pending=0;released;
  req=1;repeat(2)@(negedge sc);cancel=1;req=0;released;
  if(grant)$fatal(1,"cancelled grant");cancel=0;
  req=1;wait(grant);@(negedge sc);req=0;released;
  $display("PASS shared lease + arbiter S=%0d M=%0d",S,M);$finish;
 end
 initial begin #100000;$fatal(1,"watchdog");end
endmodule
