`timescale 1ns/1ps
`default_nettype none
module tb_rom_stream131;
 reg clk=0;always #5 clk=~clk;
 reg reset_n=0,life=1,allowed=1,abort=0,start=0,cmp=1,wide=1;
 reg [25:0] base=0,length=0,image=0,capacity=0;
 wire busy,done;wire [3:0] error;wire [25:0] count,badaddr;
 wire [15:0] expected,actual;
 wire mr,nr,nw;wire [22:0] ma;wire [24:0] na;wire [15:0] nd;
 reg md=0,me=0,ndone=0,ne=0;
 reg [31:0] mdata=0;reg [15:0] ndata=0;
 reg mready=1,nready=1,hangmem=0,hangnor=0,injectmem=0,injectnor=0;
 reg mismatch=0;reg [24:0] mismatch_at=0;
 integer mp=0,np=0,reads=0,transactions=0,checks=0;
 reg [22:0] pending_ma;reg [24:0] pending_na;
 chisburner_rom_stream131 #(.TIMEOUT_CYCLES(40)) dut(
  .clk(clk),.reset_n(reset_n),.lifecycle_ok(life),.write_allowed(allowed),.abort(abort),
  .start(start),.compare_mode(cmp),.unit16(wide),.base_bytes(base),.length_bytes(length),
  .image_bytes(image),.nor_bytes(capacity),.busy(busy),.done(done),.error(error),
  .completed_bytes(count),.fault_address(badaddr),.fault_expected(expected),.fault_actual(actual),
  .mem_req(mr),.mem_word_address(ma),.mem_ready(mready),.mem_done(md),.mem_error(me),.mem_data(mdata),
  .nor_req(nr),.nor_write(nw),.nor_address(na),.nor_data(nd),.nor_ready(nready),
  .nor_done(ndone),.nor_error(ne),.nor_read_data(ndata));
 function [7:0] byte_at;
  input [25:0] a;
  begin byte_at=a[7:0]^a[15:8]^a[23:16]^{7'd0,a[24]}^8'h5a;end
 endfunction
 function [7:0] nor_byte;
  input [25:0] a;
  begin nor_byte=a<image?byte_at(a):8'hff;end
 endfunction
 always @(posedge clk) begin
  md<=0;ndone<=0;
  if(mr && mready) begin
   if(mp!=0)$fatal(1,"duplicate memory request");
   mp<=3;pending_ma<=ma;reads<=reads+1;
  end
  if(mp>0 && !hangmem) begin
   mp<=mp-1;
   if(mp==1) begin md<=1;me<=injectmem;
    mdata<={byte_at({1'b0,pending_ma,2'b00}+3),byte_at({1'b0,pending_ma,2'b00}+2),
            byte_at({1'b0,pending_ma,2'b00}+1),byte_at({1'b0,pending_ma,2'b00})};
   end
  end
  if(nr && nready) begin
   if(np!=0)$fatal(1,"duplicate NOR request");
   if(na!=base+transactions*(wide?2:1))$fatal(1,"address sequence");
   if(nw && nd!=(wide?{byte_at({1'b0,na}+1),byte_at({1'b0,na})}:{8'd0,byte_at({1'b0,na})}))
    $fatal(1,"program payload mismatch at %h",na);
   np<=5;pending_na<=na;transactions<=transactions+1;
  end
  if(np>0 && !hangnor) begin
   np<=np-1;
   if(np==1)begin ndone<=1;ne<=injectnor;
    ndata<={nor_byte({1'b0,pending_na}+1),nor_byte({1'b0,pending_na})}^
      ((mismatch && pending_na==mismatch_at)?16'h0001:16'h0000);
   end
  end
 end
 task launch;
  input c,w;input [25:0] b,l,i,n;
  begin
   // Draining/resetting adapters on failed jobs is the integration contract.
   @(negedge clk);reset_n=0;mp=0;np=0;md=0;ndone=0;
   @(negedge clk);reset_n=1;cmp=c;wide=w;base=b;length=l;image=i;capacity=n;
   reads=0;transactions=0;start=1;
   @(negedge clk);start=0;
  end
 endtask
 task finish;
  input [3:0] want_error;input [25:0] want_count;
  integer cycles;
  begin
   cycles=0;
   while(!done && cycles<10000)begin @(negedge clk);cycles=cycles+1;end
   if(!done || busy || error!=want_error || count!=want_count)
    $fatal(1,"result e=%0d want=%0d count=%0d want=%0d",error,want_error,count,want_count);
   checks=checks+1;
  end
 endtask
 initial begin
  launch(1,1,0,32,20,32);finish(0,32);
  if(reads!=5 || transactions!=16)$fatal(1,"word reuse/tail read failed");
  launch(0,1,26'hfffff8,16,26'h1000008,26'h2000000);finish(0,16);
  if(reads!=4)$fatal(1,"chip boundary reuse");
  launch(1,1,26'h1fffff8,8,26'h2000000,26'h2000000);finish(0,8);
  launch(1,0,1,11,7,16);finish(0,11);
  launch(0,0,1,7,9,16);finish(0,7);
  mismatch=1;mismatch_at=6;
  launch(1,1,0,16,16,16);finish(6,6);
  if(badaddr!=6 || (expected^actual)!=1)$fatal(1,"mismatch diagnostics");
  mismatch_at=12;launch(1,1,0,16,8,16);finish(6,12);
  if(expected!=16'hffff)$fatal(1,"tail expected FF");mismatch=0;
  injectmem=1;launch(1,1,0,8,8,8);finish(3,0);injectmem=0;
  injectnor=1;launch(0,1,0,8,8,8);finish(4,0);injectnor=0;
  hangmem=1;launch(1,1,0,8,8,8);finish(5,0);hangmem=0;
  hangnor=1;launch(0,1,0,8,8,8);finish(5,0);hangnor=0;
  mready=0;launch(1,1,0,8,8,8);finish(5,0);mready=1;
  nready=0;launch(0,1,0,8,8,8);finish(5,0);nready=1;
  launch(0,1,0,8,8,8);repeat(3)@(negedge clk);allowed=0;
  finish(2,0);if(mr || nr)$fatal(1,"requests after revoke");allowed=1;
  launch(1,1,0,8,8,8);repeat(3)@(negedge clk);life=0;
  finish(2,0);life=1;
  launch(1,1,0,8,8,8);repeat(3)@(negedge clk);abort=1;
  finish(2,0);abort=0;
  allowed=0;launch(0,1,0,8,8,8);finish(2,0);
  launch(1,1,0,8,8,8);finish(0,8);allowed=1;
  launch(1,1,0,0,8,8);finish(1,0);
  launch(1,1,1,8,16,16);finish(1,0);
  launch(1,1,0,7,16,16);finish(1,0);
  launch(1,1,0,8,7,16);finish(1,0);
  launch(0,1,0,16,8,16);finish(1,0);
  launch(1,1,26'h1fffffe,4,26'h2000000,26'h2000000);finish(1,0);
  launch(1,1,26'h3fffffe,4,26'h2000000,26'h2000000);finish(1,0);
  launch(1,1,0,8,8,26'h2000002);finish(1,0);
  $display("PASS rom-stream131: %0d cases",checks);$finish;
 end
 initial begin #2000000;$fatal(1,"global timeout");end
endmodule
`default_nettype wire
