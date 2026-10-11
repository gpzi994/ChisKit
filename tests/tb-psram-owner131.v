`timescale 1ns/1ps
`default_nettype none
module tb_psram_owner131;
 reg clk=0;always #5 clk=~clk;
 reg reset_n=0,lease=0,lp=0,sp=0,b0=0,b1=0,v0=0,v1=0;
 reg lr0=0,lw0=0,lr1=0,lw1=0,sr0=0,sr1=0;
 wire la,sa,so,pr0,pw0,pr1,pw1,lv0,lv1,sv0,sv1;
 wire [21:0] a0,a1;wire [31:0] d0,d1;wire [3:0] st0,st1;
 integer checks=0;
 chisburner_psram_ports131 dut(
  .clk(clk),.reset_n(reset_n),.lease(lease),.legacy_pending(lp),.stream_pending(sp),
  .legacy_accept(la),.stream_accept(sa),.stream_owner(so),
  .l0_rd(lr0),.l0_wr(lw0),.l1_rd(lr1),.l1_wr(lw1),.s0_rd(sr0),.s1_rd(sr1),
  .l0_addr(22'd17),.l1_addr(22'd23),.s0_addr(22'd31),.s1_addr(22'd43),
  .l0_data(32'h12345678),.l1_data(32'habcdef01),.l0_strb(4'hf),.l1_strb(4'h3),
  .p0_rd(pr0),.p0_wr(pw0),.p1_rd(pr1),.p1_wr(pw1),.p0_addr(a0),.p1_addr(a1),
  .p0_data(d0),.p1_data(d1),.p0_strb(st0),.p1_strb(st1),
  .p0_busy(b0),.p1_busy(b1),.p0_valid(v0),.p1_valid(v1),
  .l0_valid(lv0),.l1_valid(lv1),.s0_valid(sv0),.s1_valid(sv1));
 task tick;begin @(negedge clk);end endtask
 task check;
  input condition;
  begin if(!condition)$fatal(1,"check %0d failed",checks+1);checks=checks+1;end
 endtask
 task grant;
  integer t;
  begin t=0;while(!sa && t<10)begin tick;t=t+1;end check(sa && so && !la);end
 endtask
 always @(posedge clk)begin
  if(so && (pw0 || pw1 || lv0 || lv1))$fatal(1,"stream owner leaked legacy transaction");
  if(!so && (sv0 || sv1))$fatal(1,"legacy response leaked into stream");
  if(la && sa)$fatal(1,"double admission");
 end
 initial begin
  tick;check(!la && !sa && !pr0 && !pw0);reset_n=1;tick;
  lw0=1;lr1=1;tick;check(la && pw0 && pr1 && a0==17 && a1==23 && st1==3);
  lp=1;lease=1;tick;check(!la && !so && pw0 && pr1); // stop new, drain old
  repeat(4)tick;check(!so);
  lp=0;lw0=0;lr1=0;b1=1;repeat(4)tick;check(!so);
  b1=0;v1=1;tick;check(lv1 && !sv1 && !so);
  v1=0;grant;
  // Accidental legacy strobes cannot touch either controller while streaming.
  lw0=1;lw1=1;sr0=1;sr1=1;tick;
  check(pr0 && pr1 && !pw0 && !pw1 && a0==31 && a1==43 && st0==0 && st1==0);
  sr0=0;sr1=0;v0=1;v1=1;tick;check(sv0 && sv1 && !lv0 && !lv1);
  v0=0;v1=0;sp=1;lease=0;tick;check(so && !sa && !la);
  // Already accepted stream work can still issue during cancellation drain.
  sr1=1;tick;check(pr1 && so);sr1=0;
  repeat(4)tick;check(so && !la);
  sp=0;b0=1;repeat(3)tick;check(so);
  b0=0;v0=1;tick;check(so && sv0 && !lv0);
  lw0=0;lw1=0;v0=0;repeat(3)tick;check(la && !so);
  // Abandon acquisition while legacy work is still in flight.
  lp=1;lease=1;tick;tick;lease=0;tick;check(la && !so);lp=0;
  lease=1;grant;
  sp=1;lease=0;tick;lease=1;repeat(3)tick;check(so && !sa);
  sp=0;repeat(3)tick;check(!so && !sa);grant;
  // Common reset must reset controllers too; here the physical model is idle.
  reset_n=0;lease=0;tick;check(!so && !la && !sa);
  reset_n=1;tick;check(la && !so);
  $display("PASS PSRAM ownership/mux: %0d checks",checks);$finish;
 end
 initial begin #10000;$fatal(1,"timeout");end
endmodule
`default_nettype wire
