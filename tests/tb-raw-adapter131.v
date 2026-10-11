`timescale 1ns/1ps
`default_nettype none
module tb_raw_adapter131;
 reg clk=0;always #5 clk=~clk;
 reg reset_n=0,life=1,allowed=1,abort=0,req=0,writing=0;
 reg [24:0] address=0;reg [15:0] data=0;
 wire ready,busy,done,fault;wire [15:0] result;
 wire wr,rd;wire [4:0] ra;wire [31:0] wd,rdata;
 reg configure=1,cwr=0;reg [4:0] ca=0;reg [31:0] cd=0;
 wire rawbusy,wa;wire [7:0] b3,b2,b1;wire [3:0] b0;
 wire d3,d2,d1,d0,p30,p30d,p30p,p31,p31d;
 integer strobes=0,checks=0;reg prev_strobe=0;
 chisburner_raw_adapter131 #(.TIMEOUT_CYCLES(100)) dut(
  .clk(clk),.reset_n(reset_n),.lifecycle_ok(life),.write_allowed(allowed),.abort(abort),
  .req(req),.write(writing),.address(address),.data(data),.ready(ready),
  .busy(busy),.done(done),.fault(fault),.read_data(result),
  .reg_wr(wr),.reg_rd(rd),.reg_address(ra),.reg_data(wd),.reg_read_data(rdata),.raw_busy(rawbusy));
 chisburner_nor_bus108 #(.ADDRESS_CYCLES(2),.SELECT_CYCLES(2),.DATA_SETUP_CYCLES(2),
  .STROBE_CYCLES(3),.RELEASE_CYCLES(2)) raw(
  .clk(clk),.reset_n(reset_n),.lifecycle_ok(life),.selected(1'b1),
  .reg_wr(configure?cwr:wr),.reg_rd(configure?1'b0:rd),
  .reg_addr(configure?ca:ra),.reg_wr_addr(configure?ca:ra),
  .reg_wdata(configure?cd:wd),.reg_rdata(rdata),
  .gb_data_in(8'h78),.bank3_in(8'h34),.bank2_in(8'h12),
  .bank3_out(b3),.bank2_out(b2),.bank1_out(b1),.bank0_out(b0),
  .bank3_dir(d3),.bank2_dir(d2),.bank1_dir(d1),.bank0_dir(d0),
  .pin30_out(p30),.pin30_dir(p30d),.pin30_pwroff_reset(p30p),.pin31_out(p31),.pin31_dir(p31d),
  .busy(rawbusy),.write_active(wa));
 always @(posedge clk) begin
  prev_strobe<=!b0[2];
  if(!b0[2] && !prev_strobe)begin
   strobes<=strobes+1;
   if(!allowed || !life)$fatal(1,"unauthorized strobe");
   if({b2,b3}!=data)$fatal(1,"write data lanes");
  end
 end
 task config_write;
  input [4:0] a;input [31:0] d;
  begin @(negedge clk);configure=1;cwr=1;ca=a;cd=d;
   @(negedge clk);cwr=0;end
 endtask
 task launch;
  input w;input [24:0] a;
  begin @(negedge clk);configure=0;writing=w;address=a;data=16'habc5;req=1;
   @(negedge clk);req=0;end
 endtask
 task finish;
  input f;
  integer t;
  begin t=0;while(!done && t<150)begin @(negedge clk);t=t+1;end
   if(!done || fault!=f || busy || rawbusy)$fatal(1,"adapter completion fault=%b want=%b",fault,f);
   checks=checks+1;
  end
 endtask
 initial begin
  repeat(2)@(negedge clk);reset_n=1;
  config_write(4,2);config_write(7,100);
  launch(0,0);finish(0);if(result!=16'h1234)$fatal(1,"read lanes");
  launch(1,2);finish(0);if(strobes!=1)$fatal(1,"no write");
  launch(0,3);finish(1); // Raw rejects odd GBA address, no silent success.
  allowed=0;launch(1,2);finish(1);allowed=1;
  if(strobes!=1)$fatal(1,"write without grant");
  launch(1,2);@(negedge clk);abort=1;finish(1);abort=0;
  if(strobes!=1)$fatal(1,"write after cancel");
  launch(0,4);finish(0);
  launch(1,2);@(negedge clk);life=0;finish(1);life=1;
  config_write(4,2);config_write(7,100);
  launch(0,6);finish(0);
  $display("PASS raw-adapter131: %0d cases",checks);$finish;
 end
 initial begin #100000;$fatal(1,"timeout");end
endmodule
`default_nettype wire
