`timescale 1ns/1ps
module tb_compare_path131;
 reg clk=0,mclk=0;always #5 clk=~clk;always #7 mclk=~mclk;
 reg reset=0,start=0,cancel=0,grant=1,bad=0;
 wire busy,done,hold;wire [3:0] error;wire [25:0] completed,fault;
 wire [15:0] expected,actual;wire p0,p1,nreq;wire [21:0] a0,a1;wire [24:0] na;
 reg v0=0,v1=0,ndone=0;reg [15:0] data=0;
 reg [25:0] base=0,len=8,img=4,capacity=8;
 always @(posedge mclk)begin v0<=p0;v1<=p1;end
 always @(posedge clk)begin
  ndone<=nreq;
  if(nreq)data<=bad?16'h1234:(na<img?16'h5678:16'hffff);
 end
 chisburner_compare_path131 dut(
  .clk(clk),.reset_n(reset),.memory_clk(mclk),.memory_reset_n(reset),
  .lifecycle_ok(1'b1),.psram_granted(grant),.start(start),.cancel(cancel),
  .base_bytes(base),.length_bytes(len),.image_bytes(img),.nor_bytes(capacity),
  .busy(busy),.done(done),.hold_owner(hold),.error(error),.completed_bytes(completed),
  .fault_address(fault),.fault_expected(expected),.fault_actual(actual),
  .p0_rd(p0),.p1_rd(p1),.p0_address(a0),.p1_address(a1),
  .p0_data(32'h56785678),.p1_data(32'h56785678),.p0_valid(v0),.p1_valid(v1),
  .nor_req(nreq),.nor_address(na),.nor_ready(1'b1),.nor_done(ndone),
  .nor_error(1'b0),.nor_busy(ndone),.nor_data(data));
 task launch;begin @(negedge clk);start=1;@(negedge clk);start=0;end endtask
 task finish_job;integer i;begin
  i=0;while(!done && i<2000)begin @(negedge clk);i=i+1;end
  if(!done || hold)$fatal(1,"job did not drain");
 end endtask
 initial begin
  repeat(4)@(negedge clk);reset=1;
  launch;finish_job;
  if(error || completed!=8)$fatal(1,"image plus erased tail");
  bad=1;launch;finish_job;
  if(error!=6 || fault!=0 || expected!=16'h5678 || actual!=16'h1234)$fatal(1,"fault diagnostics");
  bad=0;base=26'hfffffc;len=8;img=26'h1000004;capacity=img;
  launch;finish_job;if(error || completed!=8)$fatal(1,"PSRAM chip boundary");
  base=0;len=8;img=8;capacity=8;
  launch;wait(p0);@(negedge clk);cancel=1;
  finish_job;if(error!=2)$fatal(1,"cancel");cancel=0;
  launch;finish_job;if(error || completed!=8)$fatal(1,"restart after drain");
  $display("PASS compare physical PSRAM path: tail, mismatch, chip boundary, cancel and restart");$finish;
 end
 initial begin #1000000;$fatal(1,"watchdog");end
endmodule
