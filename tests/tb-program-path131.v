`timescale 1ns/1ps
module tb_program_path131;
 reg clk=0,mc=0;always #5 clk=~clk;always #7 mc=~mc;
 reg reset_n=0,mreset=0,life=1,allow=1,grant=1,wr=0;
 reg [4:0] ra=0;reg [31:0] wd=0;
 wire hold_owner,busy,done,rejected,wa;wire [3:0] error;wire [31:0] rd;
 wire p0,p1;wire [21:0] a0,a1;
 reg v0=0,v1=0,chip=0,stall=0;integer pending=0,reads=0,cases=0,writes=0;
 reg [22:0] last_address;
 reg [31:0] d[0:15];integer j,t,before_w;
 reg was_write=0;
 chisburner_program_path131 dut(
  .clk(clk),.reset_n(reset_n),.memory_clk(mc),.memory_reset_n(mreset),
  .lifecycle_ok(life),.selected(1'b1),.write_allowed(allow),.psram_granted(grant),
  .reg_wr(wr),.reg_rd(1'b0),.reg_address(ra),.reg_read_address(ra),.reg_data(wd),.reg_read_data(rd),
  .hold_owner(hold_owner),.busy(busy),.done(done),.error(error),.rejected(rejected),
  .p0_rd(p0),.p1_rd(p1),.p0_address(a0),.p1_address(a1),
  .p0_data(32'h34123412),.p1_data(32'h34123412),.p0_valid(v0),.p1_valid(v1),
  .gb_data_in(8'h12),.bank3_in(8'h12),.bank2_in(8'h34),.write_active(wa));
 always @(posedge mc)begin
  v0<=0;v1<=0;
  if(p0 || p1)begin
   if(pending || (p0 && p1))$fatal(1,"PSRAM overlap");
   pending<=5;chip<=p1;last_address<=p1?{1'b1,a1}:{1'b0,a0};reads<=reads+1;
  end
  if(pending>0 && !stall)begin pending<=pending-1;
   if(pending==1)begin v0<=!chip;v1<=chip;end
  end
 end
 always @(posedge clk)begin
  was_write<=wa;
  if(wa && !was_write)writes<=writes+1;
 end
 task put;input [4:0] a;input [31:0] v;
  begin @(negedge clk);ra=a;wd=v;wr=1;@(negedge clk);wr=0;end
 endtask
 task submit;
  integer i;begin
   for(i=0;i<16;i=i+1)begin put(20,i);put(21,d[i]);end
   put(24,32'h43425331);put(24,32'h43425332);put(22,1);
  end
 endtask
 task finish;input [3:0] want;
  begin t=0;while(!done || hold_owner)begin @(negedge clk);t=t+1;
   if(t>20000)$fatal(1,"drain timeout");end
   if(error!=want)$fatal(1,"result %d expected %d",error,want);cases=cases+1;
  end
 endtask
 initial begin
  for(j=0;j<16;j=j+1)d[j]=0;
  d[0]=1;d[2]='h1000;d[3]=2;d[5]='haaa;d[6]='h554;
  d[7]='h005500aa;d[8]='h00290025;d[9]='h00a000f0;d[10]=100000;
  repeat(3)@(negedge clk);reset_n=1;@(negedge mc);mreset=1;
`ifdef CHISBURNER_EEPROM139
  ra=1;#1;
`ifdef CHISBURNER_SAVE139
  if(rd!=32'h0001007f)$fatal(1,"automatic save probe capabilities missing");
`else
  if((rd&32'h40)==0)$fatal(1,"EEPROM capability missing");
`endif
  // EEPROM works without a PSRAM lease or NOR write authority.
  allow=0;grant=0;put(27,32'h4000);put(28,1);
  if(!hold_owner)$fatal(1,"EEPROM did not reserve pins");
  put(4,1);put(22,1); // Neither raw nor stream may start while EEPROM owns.
  t=0;while(hold_owner)begin @(negedge clk);t=t+1;
   if(p0 || p1 || wa)$fatal(1,"EEPROM touched PSRAM/NOR write");
   if(t>10000)$fatal(1,"EEPROM timeout");end
  ra=28;#1;if((rd&7)!=2)$fatal(1,"EEPROM result");
  ra=29;#1;if(rd!=0)$fatal(1,"EEPROM payload high");
  ra=30;#1;if(rd!=0)$fatal(1,"EEPROM payload low");
  put(28,1);repeat(100)@(negedge clk);put(28,2);
  repeat(5)@(negedge clk);ra=28;#1;
  if(hold_owner || (rd&7)!=4)$fatal(1,"EEPROM abort");
  allow=1;grant=1;
`endif
  put(4,2);put(7,2000); // Actual production NOR timing engine setup.
  submit;finish(0);
  if(reads!=1 || writes!=7 || last_address!=0)$fatal(1,"single program path");
  d[1]='h1000000;submit;finish(0);
  if(last_address!='h400000)$fatal(1,"PSRAM1 not selected");
  d[1]='hfffffc;d[3]=12;d[4]=16;reads=0;submit;
  put(4,1); // CPU cannot change the active raw bus mode.
  finish(0);
  if(reads!=3 || last_address!='h400001 || !rejected || dut.raw.bus_reg!=2)
   $fatal(1,"buffered chip-boundary path or CPU exclusion");
  // Cancel with an outstanding physical read, no premature ownership release.
  stall=1;submit;wait(pending>0);put(22,2);before_w=writes;
  repeat(40)@(negedge clk);
  if(!hold_owner || writes!=before_w)$fatal(1,"cancel released pending PSRAM");
  stall=0;finish(2);
  // Recover with a new descriptor/authorization and no stale response.
  submit;finish(0);
  grant=0;before_w=writes;submit;finish(3);
  if(writes!=before_w)$fatal(1,"programmed without PSRAM grant");grant=1;
  // Revoke during an active raw cycle and confirm bus/CDC drain.
  submit;wait(wa);@(negedge clk);allow=0;finish(2);allow=1;
  submit;finish(0);
  $display("PASS integrated program path: %0d cases",cases);$finish;
 end
 initial begin #5000000;$fatal(1,"watchdog");end
endmodule
