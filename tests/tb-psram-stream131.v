`timescale 1ns/1ps
`default_nettype none
module tb_psram_stream131 #(
 parameter integer SOURCE_HALF=5, MEMORY_HALF=7
);
 reg sc=0,mc=0;always #(SOURCE_HALF) sc=~sc;always #(MEMORY_HALF) mc=~mc;
 reg sr=0,mr=0,cancel=0,req=0;reg [22:0] address=0;
 wire ready,busy,done,error;wire [31:0] data;
 wire p0,p1;wire [21:0] a0,a1;
 reg [31:0] d0=0,d1=0;reg v0=0,v1=0;
 reg stall=0,chip=0;reg [22:0] pending_addr=0;
 integer pending=0,accepted=0,responses=0,checks=0,j,delay_cycles=4;
 reg [31:0] wanted;
 chisburner_psram_stream131 dut(
  .s_clk(sc),.s_reset_n(sr),.cancel(cancel),.req(req),.address(address),
  .ready(ready),.busy(busy),.done(done),.error(error),.data(data),
  .m_clk(mc),.m_reset_n(mr),.p0_rd(p0),.p1_rd(p1),
  .p0_address(a0),.p1_address(a1),.p0_data(d0),.p1_data(d1),.p0_valid(v0),.p1_valid(v1));
 function [31:0] contents;
  input [22:0] a;
  begin contents={a,9'h15a}^32'h369c5a78;end
 endfunction
 always @(posedge mc or negedge mr)begin
  if(!mr)begin pending<=0;v0<=0;v1<=0;end
  else begin
   v0<=0;v1<=0;
   if(p0 || p1)begin
    if(p0 && p1)$fatal(1,"both chips requested");
    if(pending!=0)$fatal(1,"duplicate physical request");
    chip<=p1;pending_addr<=p1?{1'b1,a1}:{1'b0,a0};pending<=delay_cycles;accepted<=accepted+1;
   end
   if(pending>0 && !stall)begin
    pending<=pending-1;
    if(pending==1)begin
     if(chip)begin v1<=1;d1<=contents(pending_addr);end
     else begin v0<=1;d0<=contents(pending_addr);end
    end
   end
  end
 end
 always @(posedge sc)if(done)responses<=responses+1;
 task submit;
  input [22:0] a;
  begin
   while(!ready)@(negedge sc);
   address=a;req=1;wanted=contents(a);
   @(negedge sc);req=0;address=~a; // external address need not stay stable
  end
 endtask
 task drain;
  integer t;
  begin t=0;while(busy && t<1000)begin @(negedge sc);t=t+1;end
   if(busy || !ready)$fatal(1,"failed drain");end
 endtask
 task good;
  input [22:0] a;
  integer t;
  begin
   submit(a);t=0;
   while(!done && t<1000)begin @(negedge sc);t=t+1;end
   if(!done || error || data!=wanted)$fatal(1,"read response %h wanted %h",data,wanted);
   drain;checks=checks+1;
  end
 endtask
 task cancelled_read;
  input integer after_cycles;
  integer before_response;
  begin
   before_response=responses;stall=1;
   submit(23'h400123);
   repeat(after_cycles)@(negedge sc);
   cancel=1;@(negedge sc);cancel=0;
   // Attempt a new request before the outstanding response has drained.
   req=1;address=23'h7fffff;repeat(12)@(negedge sc);req=0;
   if(!busy || ready)$fatal(1,"cancel released outstanding request");
   stall=0;drain;repeat(2)@(negedge sc);
   if(responses!=before_response)$fatal(1,"cancelled response escaped");
   checks=checks+1;good(23'h3fffff);
  end
 endtask
 initial begin
  repeat(3)@(negedge sc);sr=1;
  @(negedge mc);mr=1;
  @(negedge sc);
  good(0);good(23'h3fffff);good(23'h400000);good(23'h7fffff);
  for(j=0;j<100;j=j+1)begin delay_cycles=1+(j%11);good((j*7919)^(j[0]?23'h400000:23'd0));end
  cancelled_read(1);cancelled_read(5);cancelled_read(14);
  // Common reset also resets the physical response model. Never reset only CDC.
  stall=1;submit(15);repeat(15)@(negedge sc);sr=0;mr=0;
  repeat(3)@(negedge sc);sr=1;@(negedge mc);mr=1;
  stall=0;@(negedge sc);good(23'h400055);
  $display("PASS PSRAM stream CDC: %0d cases, %0d physical reads",checks,accepted);$finish;
 end
 initial begin #1000000;$fatal(1,"global timeout");end
endmodule
`default_nettype wire
