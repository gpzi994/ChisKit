`timescale 1ns/1ps
module tb_gb_compare207;
 reg clk=0;always #5 clk=~clk;
 reg reset_n=0,live=1,abort=0,start=0,idle=1;
 reg [25:0] base=0,len=0,img=0,capacity=0;
 wire busy,done,mreq,nreq;wire [3:0] error;
 wire [25:0] completed,fault;wire [15:0] expected,actual;
 wire [22:0] ma;wire [24:0] na;
 reg mr=1,md=0,me=0,nr=1,nd=0,ne=0;
 reg [31:0] mdata=0;reg [15:0] ndata=0;
 integer cases=0,checks=0,reads=0,fetches=0,mt=0,nt=0,cycles=0;
 integer bad=-1,memfail=-1,norfail=-1;
 reg [25:0] job_base,job_img;reg [22:0] saved_ma;reg [25:0] saved_a;
 reg delay_ready=0;
 chisburner_gb_compare207 dut(.clk(clk),.reset_n(reset_n),.lifecycle_ok(live),.abort(abort),
  .start(start),.adapters_idle(idle),.base_bytes(base),.length_bytes(len),.image_bytes(img),.nor_bytes(capacity),
  .busy(busy),.done(done),.error(error),.completed_bytes(completed),.fault_address(fault),
  .fault_expected(expected),.fault_actual(actual),.mem_req(mreq),.mem_word_address(ma),
  .mem_ready(mr),.mem_done(md),.mem_error(me),.mem_data(mdata),
  .nor_req(nreq),.nor_address(na),.nor_ready(nr),.nor_done(nd),.nor_error(ne),.nor_read_data(ndata));
 function [7:0] pattern(input [25:0] a);
  pattern=(a*13)^(a>>8)^(a>>16)^8'h96;
 endfunction
 task check(input bit ok,input [511:0] label_text);begin
  checks=checks+1;if(!ok)$fatal(1,"FAIL %0s case=%0d state=%0d",label_text,cases,dut.state);
 end endtask
 // Responses are delayed independently. NOR model maps the selected bank's
 // physical window back to its linear ROM address, independently of DUT internals.
 always @(posedge clk)if(reset_n)begin
  cycles=cycles+1;
  if(mreq && mr)begin
   check(mt==0,"one memory request outstanding");
   check(ma*4>=job_base && ma*4<job_img,"no PSRAM fetch outside image word range");
   saved_ma=ma;mt=2+(fetches%4);fetches=fetches+1;
  end
  if(nreq && nr)begin
   check(nt==0,"one NOR request outstanding");
   check(na==((job_base==0?0:26'h4000)+reads),"sequential physical bank address");
   saved_a=job_base+reads;nt=2+(reads%3);reads=reads+1;
  end
 end
 always @(negedge clk)begin
  md=0;nd=0;me=0;ne=0;
  mr=!delay_ready || cycles%3!=0;nr=!delay_ready || cycles%4!=0;
  if(mt>0)begin
   mt=mt-1;
   if(mt==0)begin md=1;me=(saved_ma*4==memfail);
    mdata={pattern(saved_ma*4+3),pattern(saved_ma*4+2),pattern(saved_ma*4+1),pattern(saved_ma*4)};
   end
  end
  if(nt>0)begin
   nt=nt-1;
   if(nt==0)begin nd=1;ne=(saved_a==norfail);
    ndata={8'ha5,(saved_a<job_img?pattern(saved_a):8'hff)};
    if(saved_a==bad)ndata=ndata^16'h0001;
   end
  end
 end
 task launch(input [25:0] b,l,i,c);begin
  @(negedge clk);#1;cases=cases+1;base=b;len=l;img=i;capacity=c;
  job_base=b;job_img=i;reads=0;fetches=0;start=1;
  @(negedge clk);#1;start=0;
 end endtask
 task finish_case(input [3:0] wanted);integer timeout;begin
  timeout=0;
  while(!done && timeout<400000)begin @(negedge clk);#1;timeout=timeout+1;end
  check(done===1'b1,"bounded completion");check(error===wanted,"result code");
  check(!busy && !mreq && !nreq,"no requests after completion");
  repeat(8)begin @(negedge clk);#1;end
 end endtask
 task good(input [25:0] b,l,i,c);integer wanted_fetch;begin
  launch(b,l,i,c);finish_case(0);check(completed===l,"completed byte count");
  check(reads==l,"one NOR read per byte");
  wanted_fetch=i<=b?0:((i-b<l?i-b:l)+3)/4;
  check(fetches==wanted_fetch,"one PSRAM fetch per four source bytes");
 end endtask
 integer k;
 initial begin
  repeat(4)@(negedge clk);#1;reset_n=1;
  good(0,16384,32768,32768);
  delay_ready=1;good(16384,16384,32768,32768);delay_ready=0;
  good(26'h400000,16384,26'h800000,26'h800000); // MBC5 bank 256
  good(26'h7fc000,16384,26'h800000,26'h800000); // bank 511
  for(k=0;k<4;k=k+1)good(0,19,8+k,32); // image end in every PSRAM byte lane
  good(16384,16384,0,32768); // all-FF NOR tail must never fetch PSRAM
  good(0,1,1,1);
  for(k=0;k<4;k=k+1)begin
   bad=16384+k;launch(16384,16,32768,32768);finish_case(6);
   check(fault===26'(bad) && completed===26'(k),"first difference linear address");
   check(expected=={8'd0,pattern(bad)} && actual==(expected^16'd1),"byte lanes and zero-extended diagnostic");
  end
  bad=16399;launch(16384,16,16387,32768);finish_case(6);
  check(fault==16399 && expected==255 && actual==254,"FF tail difference");bad=-1;
  launch(1,16384,32768,32768);finish_case(1);check(reads==0&&fetches==0,"unaligned bank denied");
  launch(0,16385,32768,32768);finish_case(1);
  launch(0,0,32768,32768);finish_case(1);
  launch(16384,16384,32768,20000);finish_case(1);
  launch(0,1,33,32);finish_case(1);
  launch(26'h3ffc000,16384,0,26'h2000000);finish_case(1);
  launch(0,1,0,26'h2000001);finish_case(1);
  memfail=0;launch(0,16,16,16);finish_case(3);check(fault==0&&reads==0,"PSRAM error stops NOR");memfail=-1;
  norfail=7;launch(0,16,16,16);finish_case(4);check(fault==7&&completed==7,"NOR error location");norfail=-1;
  // Abort in each active state, including responses in flight. Hold the outer
  // adapter lease until late responses drain; reject restart during that drain.
  for(k=1;k<=5;k=k+1)begin
   launch(0,64,64,64);
   while(dut.state!=k)begin @(negedge clk);#1;end
   abort=1;#1;check(!mreq&&!nreq,"same-cycle abort suppresses requests");
   @(negedge clk);#1;check(done&&error==2,"abort completion");abort=0;idle=0;
   launch(0,4,4,4);finish_case(8);idle=1;
   good(16384,4,32768,32768); // no stale cache/response after external drain
  end
  launch(0,64,64,64);live=0;#1;check(!mreq&&!nreq,"lifecycle suppresses requests");finish_case(2);live=1;
  good(0,4,4,4);
  $display("PASS GB207: %0d cases, %0d assertions; no production connection",cases,checks);$finish;
 end
 initial begin #100000000;$fatal(1,"global simulation timeout");end
endmodule
