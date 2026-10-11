`timescale 1ns/1ps
module tb_cache_lease131;
 reg clk=0;always #5 clk=~clk;
 reg reset_n=0,rd=0,scan=0,pending=0,reader_busy=0;
 reg [22:0] address=0;
 wire rs,ss,accept,owned,busy,conflict;wire [22:0] selected;
 integer t,starts=0;
 chisburner_cache_lease131 dut(
 .clk(clk),.reset_n(reset_n),.read_request(rd),.scan_request(scan),.request_address(address),
 .legacy_pending(pending),.p0_busy(1'b0),.p1_busy(1'b0),.p0_valid(1'b0),.p1_valid(1'b0),
 .reader_busy(reader_busy),.read_start(rs),.scan_start(ss),.reader_address(selected),
 .legacy_accept(accept),.owned(owned),.busy(busy),.conflict(conflict));
 always @(posedge clk)if(rs || ss)begin
  starts<=starts+1;if(!owned || pending)$fatal(1,"start before drain");end
 task tick;begin @(negedge clk);end endtask
 initial begin
  tick;reset_n=1;tick;pending=1;rd=1;address=23'h456789;tick;rd=0;address=0;
  repeat(8)tick;if(rs || ss || !busy || accept)$fatal(1,"acquisition");
  pending=0;t=0;while(!rs && t<20)begin tick;t=t+1;end
  if(!rs || selected!=23'h456789)$fatal(1,"lost request address");
  reader_busy=1;tick;rd=1;tick;rd=0;
  if(!conflict)$fatal(1,"overlap not reported");
  repeat(4)tick;reader_busy=0;t=0;while(busy && t<20)begin tick;t=t+1;end
  if(busy || !accept || starts!=1)$fatal(1,"release/duplicate");
  scan=1;tick;scan=0;t=0;while(!ss && t<20)begin tick;t=t+1;end
  if(!ss || rs)$fatal(1,"scan dispatch");
  reader_busy=1;repeat(4)tick;reader_busy=0;repeat(10)tick;
  if(busy || !accept || starts!=2)$fatal(1,"scan release");
  $display("PASS cache lease: queue drain, address latch, conflict, read and scan release");$finish;
 end
 initial begin #10000;$fatal(1,"timeout");end
endmodule
