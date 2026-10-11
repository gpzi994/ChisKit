`timescale 1ns/1ps
`default_nettype none
// End-to-end read-only simulation: PSRAM model -> stream comparator <- real
// NOR108 pin-timing engine, via autonomous adapter. No per-word CPU accesses.
module tb_stream_raw131;
 reg clk=0;always #5 clk=~clk;
 reg reset_n=0,start=0,setup=1,cwr=0;
 reg [4:0] ca=0;reg [31:0] cd=0;
 reg [25:0] base=0,length=0,image=0;
 wire busy,done;wire [3:0] error;wire [25:0] count,badaddr;
 wire [15:0] expected,actual;
 wire mr,nr,nw,nready,ndone,nfault,adapterbusy;
 wire [22:0] ma;wire [24:0] na;wire [15:0] nd,nread;
 reg md=0;reg [31:0] mdata=0;reg [22:0] saved_ma;
 integer pending=0,reads=0,cases=0;
 wire wr,rd;wire [4:0] ra;wire [31:0] wd,rdata;wire rawbusy,wa;
 reg corrupt=0;
 function [7:0] pattern;
  input [25:0] a;
  begin pattern=a[7:0]^a[15:8]^a[23:16]^{7'd0,a[24]}^8'h69;end
 endfunction
 wire [25:0] physical={1'b0,raw.active_addr};
 wire [7:0] lo=(physical<image?pattern(physical):8'hff)^(corrupt?8'h01:8'h00);
 wire [7:0] hi=physical+1<image?pattern(physical+1):8'hff;
 chisburner_rom_stream131 #(.TIMEOUT_CYCLES(200)) stream(
  .clk(clk),.reset_n(reset_n),.lifecycle_ok(1'b1),.write_allowed(1'b0),.abort(1'b0),
  .start(start),.compare_mode(1'b1),.unit16(1'b1),.base_bytes(base),.length_bytes(length),
  .image_bytes(image),.nor_bytes(26'h2000000),.busy(busy),.done(done),.error(error),
  .completed_bytes(count),.fault_address(badaddr),.fault_expected(expected),.fault_actual(actual),
  .mem_req(mr),.mem_word_address(ma),.mem_ready(1'b1),.mem_done(md),.mem_error(1'b0),.mem_data(mdata),
  .nor_req(nr),.nor_write(nw),.nor_address(na),.nor_data(nd),.nor_ready(nready),
  .nor_done(ndone),.nor_error(nfault),.nor_read_data(nread));
 chisburner_raw_adapter131 #(.TIMEOUT_CYCLES(150)) adapter(
  .clk(clk),.reset_n(reset_n),.lifecycle_ok(1'b1),.write_allowed(1'b0),.abort(1'b0),
  .req(nr),.write(nw),.address(na),.data(nd),.ready(nready),.busy(adapterbusy),
  .done(ndone),.fault(nfault),.read_data(nread),.reg_wr(wr),.reg_rd(rd),
  .reg_address(ra),.reg_data(wd),.reg_read_data(rdata),.raw_busy(rawbusy));
 // Use production cycle counts, not accelerated transaction timing.
 chisburner_nor_bus108 raw(
  .clk(clk),.reset_n(reset_n),.lifecycle_ok(1'b1),.selected(1'b1),
  .reg_wr(setup?cwr:wr),.reg_rd(setup?1'b0:rd),.reg_addr(setup?ca:ra),
  .reg_wr_addr(setup?ca:ra),.reg_wdata(setup?cd:wd),.reg_rdata(rdata),
  .gb_data_in(8'd0),.bank3_in(lo),.bank2_in(hi),.busy(rawbusy),.write_active(wa));
 always @(posedge clk)begin
  md<=0;
  if(wa)$fatal(1,"compare wrote cartridge");
  if(mr)begin
   if(pending!=0)$fatal(1,"memory request overlap");
   saved_ma<=ma;pending<=4;reads<=reads+1;
  end
  if(pending>0)begin
   pending<=pending-1;
   if(pending==1)begin md<=1;
    mdata<={pattern({1'b0,saved_ma,2'd0}+3),pattern({1'b0,saved_ma,2'd0}+2),
            pattern({1'b0,saved_ma,2'd0}+1),pattern({1'b0,saved_ma,2'd0})};
   end
  end
 end
 task run;
  input [25:0] b,l,i;input bad;
  integer cycles;
  begin
   @(negedge clk);base=b;length=l;image=i;corrupt=bad;start=1;reads=0;
   @(negedge clk);start=0;cycles=0;
   while(!done && cycles<20000)begin @(negedge clk);cycles=cycles+1;end
   if(!done || error!=(bad?6:0) || count!=(bad?0:l) || adapterbusy || rawbusy)
    $fatal(1,"integration result error=%0d count=%0d",error,count);
   if(bad && badaddr!=b)$fatal(1,"bad address");
   cases=cases+1;
  end
 endtask
 initial begin
  repeat(2)@(negedge clk);reset_n=1;
  @(negedge clk);cwr=1;ca=4;cd=2;
  @(negedge clk);ca=7;cd=200;
  @(negedge clk);cwr=0;setup=0;
  run(0,64,40,0);if(reads!=10)$fatal(1,"tail fetched from PSRAM");
  run(26'hfffff8,32,26'h1000010,0);
  run(26'h1fffff0,16,26'h1fffff8,0);
  run(12,32,40,1);
  run(0,8,40,0);
  $display("PASS stream + production NOR timing: %0d cases",cases);$finish;
 end
 initial begin #1000000;$fatal(1,"global timeout");end
endmodule
`default_nettype wire
