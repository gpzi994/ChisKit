`timescale 1ns/1ps
module tb_nor_buffer131;
 reg clk=0;always #5 clk=~clk;
 reg reset_n=0,start=0,cancel=0,life=1,allow=1,wide=1,dual=0,swap=0,raw=0;
 reg [24:0] source=0,physical=0;reg [16:0] length=0,buffer_size=32;
 reg [31:0] timeout=10000;
 wire busy,done,mreq,treq;wire [3:0] error;wire [16:0] sent;wire [15:0] last;
 wire [22:0] ma;wire [24:0] ta;wire [15:0] td;
 reg mr=1,tr=1,md=0,te=0,me=0,tfinish=0;
 reg [31:0] mdata=0;reg [22:0] saved_ma;
 integer mp=0,tp=0,txs=0,mems=0,cases=0,fail_at=-1;
 reg [15:0] wanted_count=0;
 integer dies,units,offset;
 chisburner_nor_buffer131 dut(
  .clk(clk),.reset_n(reset_n),.start(start),.cancel(cancel),.lifecycle_ok(life),.write_allowed(allow),
  .unit16(wide),.dual_intel(dual),.swapped(swap),.raw_count(raw),.source_address(source),
  .physical_address(physical),.length_bytes(length),.buffer_bytes(buffer_size),
  .confirm_command(16'h00d0),.timeout_cycles(timeout),.busy(busy),.done(done),.error(error),
  .sent_bytes(sent),.last_payload(last),.mem_req(mreq),.mem_address(ma),.mem_ready(mr),
  .mem_done(md),.mem_error(me),.mem_data(mdata),.tx_req(treq),.tx_address(ta),.tx_data(td),
  .tx_ready(tr),.tx_done(tfinish),.tx_error(te));
 function [7:0] octet;
  input [25:0] a;
  begin octet=a[7:0]^a[15:8]^a[23:16]^{7'd0,a[24]}^8'h37;end
 endfunction
 always @(posedge clk)begin
  md<=0;tfinish<=0;
  if(mreq && mr)begin
   if(mp!=0)$fatal(1,"duplicate memory read");mp<=3;saved_ma<=ma;mems<=mems+1;
  end
  if(mp>0)begin mp<=mp-1;if(mp==1)begin md<=1;
   mdata<={octet({1'b0,saved_ma,2'd0}+3),octet({1'b0,saved_ma,2'd0}+2),
           octet({1'b0,saved_ma,2'd0}+1),octet({1'b0,saved_ma,2'd0})};end end
  if(treq && tr)begin
   if(tp!=0)$fatal(1,"duplicate write");
   dies=dual?2:1;units=length/(wide?2:1);
   if(txs<dies)begin
    if(ta!=physical+txs*2 || td!=wanted_count)$fatal(1,"count trace %h want %h",td,wanted_count);
   end else if(txs<dies+units)begin
    offset=(txs-dies)*(wide?2:1);
    if(ta!=physical+offset || td!=(wide?{octet(source+offset+1),octet(source+offset)}:{8'd0,octet(source+offset)}))
     $fatal(1,"payload trace");
   end else if(txs<dies*2+units)begin
    if(ta!=physical+(txs-dies-units)*2 || td!=16'hd0)$fatal(1,"confirm trace");
   end else $fatal(1,"extra write");
   tp<=2;te<=txs==fail_at;txs<=txs+1;
  end
  if(tp>0)begin tp<=tp-1;if(tp==1)tfinish<=1;end
 end
 task run;
  input [3:0] want;
  integer t;
  begin
   @(negedge clk);reset_n=0;mp=0;tp=0;md=0;tfinish=0;txs=0;mems=0;
   @(negedge clk);reset_n=1;start=1;
   @(negedge clk);start=0;t=0;
   while(!done && t<20000)begin @(negedge clk);t=t+1;end
   if(!done || busy || error!=want)$fatal(1,"case%0d e=%0d want=%0d",cases,error,want);
   if(want==0 && (sent!=length || txs!=(dual?4:2)+length/(wide?2:1)))$fatal(1,"incomplete trace");
   if(want==0 && last!=(wide?{octet(source+length-1),octet(source+length-2)}:{8'd0,octet(source+length-1)}))
    $fatal(1,"wrong final payload for AMD polling");
   if(want==1 && txs!=0)$fatal(1,"invalid descriptor wrote bus");cases=cases+1;
  end
 endtask
 initial begin
  length=16;wanted_count=7;run(0);if(mems!=4)$fatal(1,"word reuse");
  dual=1;wanted_count=3;run(0);dual=0;
  length=1024;buffer_size=1024;wanted_count=511;run(0);
  if(mems!=256)$fatal(1,"1024-byte buffer word reuse");buffer_size=32;
  source=25'hfffffc;length=8;wanted_count=3;run(0); // PSRAM chip crossing
  source=25'h1fffff8;physical=25'h1fffff8;run(0); // last 8 bytes
  physical=0;source=0;length=6;swap=1;wanted_count=1;run(0);
  raw=1;wanted_count=2;run(0);swap=0;raw=0;
  wide=0;length=7;source=1;physical=17;wanted_count=6;run(0);wide=1;
  source=0;physical=0;length=8;wanted_count=3;
  fail_at=0;run(4);if(txs!=1)$fatal(1,"continued after count error");
  fail_at=2;run(4);if(txs!=3)$fatal(1,"continued after data error");fail_at=-1;
  me=1;run(3);if(txs!=1)$fatal(1,"confirm after memory error");me=0;
  mr=0;timeout=40;run(5);mr=1;timeout=10000;
  allow=0;run(2);allow=1;cancel=1;run(2);cancel=0;
  length=0;run(1);length=8;
  buffer_size=12;run(1);buffer_size=32;
  physical=30;run(1);physical=0;
  source=1;run(1);source=0;
  length=7;run(1);length=8;
  dual=1;physical=2;run(1);physical=0;dual=0;
  source=25'h1fffffc;run(1);source=0;
  wide=0;physical=25'h7ffc;run(1);wide=1;physical=0;
  fork
   run(2);
   begin wait(busy);repeat(8)@(negedge clk);allow=0;end
  join
  if(treq || mreq)$fatal(1,"request after revoke");allow=1;
  $display("PASS NOR buffer transfer: %0d cases",cases);$finish;
 end
 initial begin #3000000;$fatal(1,"global timeout");end
endmodule
