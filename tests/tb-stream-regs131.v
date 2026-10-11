`timescale 1ns/1ps
module tb_stream_regs131;
 reg clk=0;always #5 clk=~clk;
 reg reset_n=0,life=1,allow=1,drain=1,wr=0;
 reg [4:0] address=0;reg [31:0] data=0;
 wire [31:0] rd;wire busy,done,rejected,mreq,treq,tw;
 wire [3:0] error;wire [22:0] ma;wire [24:0] ta;wire [15:0] td;
 reg md=0,txd=0;integer mp=0,tp=0,count=0,cases=0,before_count,k;
 reg [31:0] desc[0:15];
 chisburner_stream_regs131 dut(
  .clk(clk),.reset_n(reset_n),.lifecycle_ok(life),.write_allowed(allow),
  .idle_adapters(drain && mp==0 && tp==0 && !md && !txd),
  .wr(wr),.address(address),.read_address(address),.data(data),.read_data(rd),.busy(busy),.done(done),
  .error(error),.rejected(rejected),.mem_req(mreq),.mem_address(ma),
  .mem_ready(mp==0 && !md),.mem_done(md),.mem_error(1'b0),.mem_data(32'h34123412),
  .tx_req(treq),.tx_write(tw),.tx_address(ta),.tx_data(td),
  .tx_ready(tp==0 && !txd),.tx_done(txd),.tx_error(1'b0),.tx_read_data(16'h3412));
 always @(posedge clk)begin
  if(dut.compare_job && treq && tw)$fatal(1,"compare emitted write");
  md<=0;txd<=0;
  if(mreq && mp==0 && !md)mp<=3;
  if(mp>0)begin mp<=mp-1;if(mp==1)md<=1;end
  if(treq && tp==0 && !txd)begin tp<=3;count<=count+1;end
  if(tp>0)begin tp<=tp-1;if(tp==1)txd<=1;end
 end
 task put;input [4:0] a;input [31:0] d;
  begin @(negedge clk);address=a;data=d;wr=1;@(negedge clk);wr=0;end
 endtask
 task load;
  integer j;begin for(j=0;j<16;j=j+1)begin put(20,j);put(21,desc[j]);end end
 endtask
 task arm;begin put(24,32'h43425331);put(24,32'h43425332);end endtask
 task finish;input [3:0] want;integer ticks;
  begin ticks=0;while(!done || busy)begin @(negedge clk);ticks=ticks+1;
   if(ticks>2000)$fatal(1,"completion timeout");end
   if(error!==want)$fatal(1,"error %d expected %d",error,want);cases=cases+1;
  end
 endtask
 task reject_submit;input [3:0] want;
  begin before_count=count;put(22,1);finish(want);
   if(count!=before_count)$fatal(1,"rejected submission emitted command");end
 endtask
 initial begin
  for(k=0;k<16;k=k+1)desc[k]=0;
  desc[0]=1;desc[2]='h1000;desc[3]=2;desc[5]='haaa;desc[6]='h554;
  desc[7]='h005500aa;desc[8]='h00290025;desc[9]='h00a000f0;desc[10]=10000;
  repeat(3)@(negedge clk);reset_n=1;
  load;reject_submit(3); // No arm.
  arm;reject_submit(1); // Prior attempt consumed descriptor completeness.
  load;arm;put(20,0);reject_submit(3); // Config edit revokes arm.
  load;arm;put(22,1);finish(0);
  if(count!=9)$fatal(1,"single-word engine did not execute expected 9 transactions: %d",count);
  reject_submit(3); // No replay.
  desc[1]='h02000000;load;arm;reject_submit(1);desc[1]=0; // No truncation.
  desc[15]=1;load;arm;reject_submit(1);desc[15]=0;
  desc[0]='h81;load;arm;reject_submit(1);desc[0]=1;
  load;arm;drain=0;reject_submit(8);drain=1;
  load;arm;allow=0;repeat(2)@(negedge clk);allow=1;reject_submit(3);
  load;arm;put(22,1);
  put(20,2);put(21,'h1); // Frozen descriptor, observable rejection.
  if(!rejected || dut.words[2]!='h1000)$fatal(1,"busy descriptor changed");
  finish(0);
  // Cancel while a command is outstanding; keep ownership until drain.
  load;arm;put(22,1);wait(tp!=0);@(negedge clk);drain=0;put(22,2);
  before_count=count;repeat(20)@(negedge clk);
  if(!busy || count!=before_count)$fatal(1,"cancel did not drain safely");
  drain=1;finish(2);
  // Lifecycle loss has the same fail-closed behavior.
  load;arm;put(22,1);wait(tp!=0);@(negedge clk);life=0;drain=0;
  before_count=count;repeat(20)@(negedge clk);
  if(!busy || count!=before_count)$fatal(1,"lifecycle loss did not drain");
  drain=1;finish(2);life=1;
  // Child validation failure must not mistake the preceding sticky done.
  desc[3]=3;load;arm;reject_submit(1);desc[3]=2;
  load;arm;put(22,1);finish(0);
  desc[3]=12;desc[4]=16; // Submit a real buffered chunk through registers.
  load;arm;put(22,1);finish(0);
  for(k=0;k<16;k=k+1)desc[k]=0;
  desc[0]='h81;desc[2]=8;desc[3]=8;desc[4]=8;
  load;arm;put(22,1);finish(0);
  put(20,12);address=21;#1;
  if(rd!=8)$fatal(1,"compare progress");
  desc[3]=4; // Model does not return FF: first erased-tail mismatch at byte 4.
  load;arm;put(22,1);finish(6);
  put(20,13);address=21;#1;if(rd!=4)$fatal(1,"compare fault address");
  put(20,14);address=21;#1;if(rd!=32'h3412ffff)$fatal(1,"compare fault data");
  $display("stream regs131 PASS %0d cases",cases);$finish;
 end
 initial begin #1000000;$fatal(1,"watchdog");end
endmodule
