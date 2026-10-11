`timescale 1ns/1ps
module tb_nor_poll131;
 reg clk=0;always #5 clk=~clk;
 reg reset_n=0,start=0,cancel=0,life=1,intel=0,wide=0,swapped=0;
 reg [15:0] expected=0;reg [31:0] limit=1000;
 wire busy,done,req;wire [3:0] error;wire [15:0] last_status;wire [24:0] addr;
 reg ready=1,response=0,io_error=0;reg [15:0] data=0;
 reg [15:0] values[0:15];integer length=0,index=0,delay=0,cases=0;
 chisburner_nor_poll131 dut(
  .clk(clk),.reset_n(reset_n),.start(start),.cancel(cancel),.lifecycle_ok(life),
  .intel_mode(intel),.msp_wide(wide),.swapped(swapped),.address(25'h123456),
  .expected(expected),.timeout_cycles(limit),.busy(busy),.done(done),.error(error),
  .last_status(last_status),.req(req),.read_address(addr),.ready(ready),.response(response),
  .response_error(io_error),.data(data));
 always @(posedge clk)begin
  response<=0;
  if(req && ready)begin
   if(delay!=0)$fatal(1,"duplicate request");
   if(addr!=25'h123456)$fatal(1,"address changed");delay<=2;
  end
  if(delay>0)begin delay<=delay-1;
   if(delay==1)begin response<=1;data<=values[index<length?index:length-1];index<=index+1;end
  end
 end
 task run;
  input im,wm,sm;input [15:0] target;input [3:0] want;input integer reads;
  integer t;
  begin
   @(negedge clk);reset_n=0;delay=0;response=0;index=0;
   @(negedge clk);reset_n=1;intel=im;wide=wm;swapped=sm;expected=target;start=1;
   @(negedge clk);start=0;t=0;
   while(!done && t<2000)begin @(negedge clk);t=t+1;end
   if(!done || busy || error!=want || (reads>=0 && index!=reads))
    $fatal(1,"case %0d error=%0d want=%0d reads=%0d want=%0d",cases,error,want,index,reads);
   cases=cases+1;
  end
 endtask
 initial begin
  length=2;values[0]=16'h0080;values[1]=16'h0080;run(0,0,0,16'h0080,0,2);
  values[0]=0;values[1]=0;run(0,0,0,0,0,2);
  length=3;values[0]=0;values[1]=16'h0080;values[2]=16'h0080;run(0,0,0,16'h0080,0,3);
  length=2;values[0]=16'h0020;values[1]=16'h0080;run(0,0,0,16'h0080,0,2);
  values[1]=0;run(0,0,0,16'h0080,7,2);
  // Confirm read fails; use original first sample's DQ5 for final reread.
  length=3;values[0]=16'ha0;values[1]=0;values[2]=16'h80;run(0,0,0,16'h80,0,3);
  length=2;values[0]=16'h8080;values[1]=16'h8080;run(0,1,0,16'h8080,0,2);
  // Completed low lane contains DQ5 data; high lane still busy, not failure.
  length=3;values[0]=16'h00a0;values[1]=16'h80a0;values[2]=16'h80a0;run(0,1,0,16'h80a0,0,3);
  length=2;values[0]=16'h2080;values[1]=16'h0080;run(0,1,0,16'h8080,7,2);
  length=4;values[0]=16'h2080;values[1]=16'h8080;values[2]=16'h8080;values[3]=16'h8080;
  run(0,1,0,16'h8080,0,4);
  length=1;values[0]=16'h0080;run(1,0,0,0,0,1);
  values[0]=16'h0082;run(1,0,0,0,7,1);
  values[0]=16'h0081;run(1,0,0,0,0,1);run(1,0,1,0,7,1);
  values[0]=16'h0082;run(1,0,1,0,0,1);
  length=2;values[0]=0;values[1]=16'h0080;run(1,0,0,0,0,2);
  io_error=1;run(0,0,0,0,4,1);io_error=0;
  length=1;values[0]=0;limit=25;run(1,0,0,0,5,-1);limit=1000;
  ready=0;limit=20;run(1,0,0,0,5,0);ready=1;limit=1000;
  cancel=1;run(0,0,0,0,2,0);cancel=0;
  life=0;run(0,0,0,0,2,0);life=1;
  limit=0;run(0,0,0,0,1,0);limit=1000;run(1,1,0,0,1,0);
  values[0]=0;length=1;
  fork
   run(1,0,0,0,2,-1);
   begin wait(busy);repeat(4)@(negedge clk);cancel=1;end
  join
  if(req)$fatal(1,"request escaped cancellation");cancel=0;
  fork
   run(1,0,0,0,2,-1);
   begin wait(busy);repeat(4)@(negedge clk);life=0;end
  join
  if(req)$fatal(1,"request escaped lifecycle loss");life=1;
  $display("PASS NOR hardware poll: %0d cases",cases);$finish;
 end
 initial begin #1000000;$fatal(1,"global timeout");end
endmodule
