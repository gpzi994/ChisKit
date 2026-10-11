`timescale 1ns/1ps
module tb_gb_compare208;
 parameter S=5,M=7;
 reg clk=0,mc=0;always #S clk=~clk;always #M mc=~mc;
 reg reset_n=0,mreset=0,allow=1,wr=0,stall=0;
 reg [4:0] ra=0;reg [31:0] wd=0;
 wire hold_owner,busy,done,wa;wire [3:0] error;wire [31:0] rd;
 wire p0,p1;wire [21:0] a0,a1;
 reg v0=0,v1=0,chip=0;reg [31:0] memdata=0;
 integer pending=0,reads=0,cases=0,j,t,bank=0,before_reads;
 reg [31:0] d[0:15];reg [25:0] image_size=32771;
 reg [25:0] corrupt=26'h3ffffff;
 wire [25:0] logical_address=bank*16384+(dut.raw.active_addr&16383);
 function [7:0] pattern;input [25:0] a;begin pattern=a[7:0]^a[15:8]^a[23:16]^8'ha7;end endfunction
 wire [7:0] gbdata=(logical_address>=image_size?8'hff:pattern(logical_address))^(logical_address==corrupt?8'h01:8'h00);
 chisburner_program_path131 dut(
  .clk(clk),.reset_n(reset_n),.memory_clk(mc),.memory_reset_n(mreset),
  .lifecycle_ok(1'b1),.selected(1'b1),.write_allowed(allow),.psram_granted(1'b1),
  .reg_wr(wr),.reg_rd(1'b0),.reg_address(ra),.reg_read_address(ra),.reg_data(wd),.reg_read_data(rd),
  .hold_owner(hold_owner),.busy(busy),.done(done),.error(error),
  .p0_rd(p0),.p1_rd(p1),.p0_address(a0),.p1_address(a1),
  .p0_data(memdata),.p1_data(memdata),.p0_valid(v0),.p1_valid(v1),
  .gb_data_in(gbdata),.bank3_in(8'ha7),.bank2_in(8'ha6),.write_active(wa));
 reg [25:0] physical;
 always @(posedge mc)begin
  v0<=0;v1<=0;
  if(p0 || p1)begin
   if(pending || (p0&&p1))$fatal(1,"PSRAM overlap");
   physical=p1?{1'b1,a1,2'b00}:{1'b0,a0,2'b00};
   memdata<={pattern(physical+3),pattern(physical+2),pattern(physical+1),pattern(physical)};
   pending<=5;chip<=p1;reads<=reads+1;
  end
  if(pending>0 && !stall)begin pending<=pending-1;if(pending==1)begin v0<=!chip;v1<=chip;end end
 end
 always @(posedge clk)begin
  if(wa)$fatal(1,"compare emitted cartridge write");
  if(dut.raw.busy && d[0]=='h80 && (dut.raw.active_addr>=32768 ||
    (bank==0 && dut.raw.active_addr>=16384) || (bank!=0 && dut.raw.active_addr<16384)))
    $fatal(1,"wrong bank window");
 end
 task put;input [4:0] a;input [31:0] v;
 begin @(negedge clk);ra=a;wd=v;wr=1;@(negedge clk);wr=0;end endtask
 task submit;integer i;begin
  for(i=0;i<16;i=i+1)begin put(20,i);put(21,d[i]);end
  put(24,32'h43425331);put(24,32'h43425332);put(22,1);
 end endtask
 task finish;input [3:0] want;begin
  t=0;while(!done || hold_owner)begin @(negedge clk);t=t+1;if(t>5000000)$fatal(1,"drain timeout");end
  if(error!==want)$fatal(1,"result %d expected %d",error,want);cases=cases+1;
 end endtask
 task job;input integer b;input [25:0] len;begin
  bank=b;d[0]='h80;d[1]=b*16384;d[2]=len;d[3]=image_size;d[4]='h800000;
  submit;finish(0);
 end endtask
 initial begin
  for(j=0;j<16;j=j+1)d[j]=0;
  repeat(3)@(negedge clk);reset_n=1;@(negedge mc);mreset=1;
  put(4,1);put(7,2000);put(20,15);ra=21;#1;
  if(rd!=32'h47423230)$fatal(1,"missing capability");
  job(0,16384);job(1,16384);job(2,16384);
  before_reads=reads;job(511,16384);if(reads!=before_reads)$fatal(1,"FF tail fetched PSRAM");
  image_size='h800000;job(256,16384);job(511,16384);
  corrupt='h4003;bank=1;d[1]='h4000;d[2]=16;d[3]=image_size;submit;finish(6);
  put(20,13);ra=21;#1;if(rd!='h4003)$fatal(1,"fault address");
  put(20,14);ra=21;#1;if(rd[7:0]!=pattern(corrupt)||rd[23:16]!=(pattern(corrupt)^8'h01))$fatal(1,"fault bytes");
  corrupt='h3ffffff;stall=1;submit;wait(pending>0);put(22,2);
  repeat(40)@(negedge clk);if(!hold_owner)$fatal(1,"released pending memory");stall=0;finish(2);
  submit;finish(0);
  submit;wait(dut.raw.busy);put(22,2);finish(2);submit;finish(0);
  put(4,2);d[0]='h81;d[1]=0;d[2]=2;d[3]=2;d[4]=2;submit;finish(0);
  put(4,1);job(0,16);
  $display("PASS GB208 integrated NOR/PSRAM compare: %0d cases S=%0d M=%0d",cases,S,M);$finish;
 end
 initial begin #1000000000;$fatal(1,"watchdog");end
endmodule
