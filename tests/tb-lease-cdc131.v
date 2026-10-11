`timescale 1ns/1ps
module tb_lease_cdc131;
 parameter S=5,M=7;
 reg sc=0,mc=0;always #S sc=~sc;always #M mc=~mc;
 reg sr=0,mr=0,req=0,cancel=0,life=1,hold=0,lp=0,sp=0,p0=0,p1=0,v0=0,v1=0;
 wire grant,busy,la,lo,so,sa;integer t;
 chisburner_lease_cdc131 dut(
  .s_clk(sc),.s_reset_n(sr),.request(req),.cancel(cancel),.lifecycle_ok(life),.hold(hold),
  .granted(grant),.busy(busy),.m_clk(mc),.m_reset_n(mr),
  .legacy_pending(lp),.stream_pending(sp),.p0_busy(p0),.p1_busy(p1),.p0_valid(v0),.p1_valid(v1),
  .legacy_accept(la),.legacy_owner(lo),.stream_owner(so),.stream_accept(sa));
 task acquired;begin t=0;while(!grant)begin @(negedge sc);t=t+1;if(t>100)$fatal(1,"grant timeout");end end endtask
 task released;begin t=0;while(busy)begin @(negedge sc);t=t+1;if(t>100)$fatal(1,"release timeout");end
  if(!la || so)$fatal(1,"premature return");end endtask
 always @(posedge mc)if(mr && lo && so)$fatal(1,"dual owner");
 initial begin
  repeat(3)@(negedge sc);sr=1;@(negedge mc);mr=1;
  @(negedge sc);lp=1;req=1;repeat(20)@(negedge sc);
  if(grant || la || !lo)$fatal(1,"did not drain legacy");
  lp=0;acquired;hold=1;req=0;repeat(20)@(negedge sc);
  if(grant || !so || !busy)$fatal(1,"released running job");
  sp=1;hold=0;repeat(20)@(negedge sc);
  if(!so || !busy || la)$fatal(1,"released pending memory");
  sp=0;released;
  req=1;acquired;hold=1;life=0;repeat(10)@(negedge sc);
  if(grant || !so)$fatal(1,"lifecycle safety");hold=0;req=0;released;life=1;
  // Cancel before acquisition: complete handshake without exposing a grant.
  lp=1;req=1;repeat(5)@(negedge sc);cancel=1;req=0;
  repeat(10)@(negedge sc);lp=0;
  while(busy)begin @(negedge sc);if(grant)$fatal(1,"cancelled grant escaped");end
  cancel=0;req=1;acquired;
  req=0;p1=1;repeat(20)@(negedge sc);if(!busy || !so)$fatal(1,"physical drain lost");
  p1=0;released;
  $display("PASS lease CDC: legacy drain, job hold, memory drain, revoke and reacquire S=%0d M=%0d",S,M);$finish;
 end
 initial begin #100000;$fatal(1,"watchdog");end
endmodule
