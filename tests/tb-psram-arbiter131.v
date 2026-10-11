`timescale 1ns/1ps
module tb_psram_arbiter131;
 reg clk=0;always #5 clk=~clk;
 reg reset_n=0,rd=0,scan=0,lp=0,rb=0,lease=0,pending=0,p0=0;
 wire rs,ss,la,co,po,grant,busy,conflict;wire [22:0] a;
 integer t;
 chisburner_psram_arbiter131 dut(
  .clk(clk),.reset_n(reset_n),.read_request(rd),.scan_request(scan),.request_address(23'h123),
  .legacy_pending(lp),.p0_busy(p0),.p1_busy(1'b0),.p0_valid(1'b0),.p1_valid(1'b0),
  .reader_busy(rb),.program_lease(lease),.program_pending(pending),
  .read_start(rs),.scan_start(ss),.reader_address(a),.legacy_accept(la),
  .cache_owned(co),.program_owned(po),.program_granted(grant),.busy(busy),.conflict(conflict));
 task tick;begin @(negedge clk);end endtask
 task waitgrant;begin t=0;while(!grant)begin tick;t=t+1;if(t>50)$fatal(1,"grant timeout");end end endtask
 always @(posedge clk)begin
  if(co && po)$fatal(1,"two physical owners");
  if((rs || ss) && !co)$fatal(1,"cache started without ownership");
 end
 initial begin
  tick;reset_n=1;tick;
  // Cache already accepted: finish it before granting a later program request.
  rd=1;tick;rd=0;while(!rs)tick;rb=1;lease=1;
  repeat(6)tick;if(grant || po || !co || a!='h123)$fatal(1,"preempted cache");
  rb=0;waitgrant;
  scan=1;tick;scan=0;if(!conflict || ss)$fatal(1,"cache conflict not rejected");
  pending=1;lease=0;repeat(12)tick;
  if(!po || la || grant)$fatal(1,"program drain released early");
  pending=0;p0=1;repeat(8)tick;if(!po)$fatal(1,"physical drain lost");
  p0=0;while(busy)tick;if(!la)$fatal(1,"legacy not restored");
  // Simultaneous arrival: explicit program priority, cache visibly rejected.
  lp=1;lease=1;rd=1;tick;rd=0;repeat(8)tick;
  if(grant || la || rs)$fatal(1,"legacy queue bypassed");
  lp=0;waitgrant;lease=0;while(busy)tick;
  scan=1;tick;scan=0;while(!ss)tick;rb=1;repeat(4)tick;rb=0;
  while(busy)tick;if(!la)$fatal(1,"cache return");
  $display("PASS unified PSRAM arbiter: cache/program ordering, conflicts and drain");$finish;
 end
 initial begin #10000;$fatal(1,"watchdog");end
endmodule
