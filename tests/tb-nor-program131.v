`timescale 1ns/1ps
module tb_nor_program131;
 reg clk=0;always #5 clk=~clk;
 reg reset_n=0,start=0,cancel=0,life=1,allow=1;
 reg intel=0,dual=0,wide=0,swap=0,rawcount=0;
 reg disable_buffer=0,unit16=1;
 reg [24:0] source=0,physical=25'h1000;
 reg [31:0] memory_value=32'h34123412;
 reg [15:0] wc=16'ha0;
 reg [15:0] aa=16'haa,bb=16'h55,bc=16'h25,cc=16'h29,rc=16'hf0;
 reg [16:0] length=12,bufsize=16;reg [31:0] timeout=10000;
 wire busy,done,mreq,treq,twrite;wire [3:0] error;
 wire [22:0] ma;wire [24:0] ta;wire [15:0] td;
 reg md=0,tf=0,te=0,tr=1;reg [15:0] read_value=16'h3412;
 integer mp=0,tp=0,ng=0,at=0,cases=0,fail_at=-1,k,dies,round;
 integer memory_reads=0;
 reg gw[0:2047];reg [24:0] ga[0:2047];reg [15:0] gd[0:2047];
 chisburner_nor_program131 dut(
  .clk(clk),.reset_n(reset_n),.start(start),.cancel(cancel),.lifecycle_ok(life),.write_allowed(allow),
  .idle_adapters(mp==0 && tp==0 && !md && !tf),.unit16(unit16),.intel_mode(intel),
  .dual_intel(dual),.msp_wide(wide),.swapped(swap),.raw_count(rawcount),
  .disable_buffer_program(disable_buffer),.word_command(wc),
  .source_address(source),.physical_address(physical),.unlock0(25'haaa),.unlock1(25'h554),
  .length_bytes(length),.buffer_bytes(bufsize),.aa(aa),.bb(bb),.buffer_command(bc),
  .confirm_command(cc),.reset_command(rc),.timeout_cycles(timeout),.busy(busy),.done(done),.error(error),
  .mem_req(mreq),.mem_address(ma),.mem_ready(1'b1),.mem_done(md),.mem_error(1'b0),.mem_data(memory_value),
  .tx_req(treq),.tx_write(twrite),.tx_address(ta),.tx_data(td),.tx_ready(tr),.tx_done(tf),.tx_error(te),
  .tx_read_data(read_value));
 task push;
  input wr;input [24:0] a;input [15:0] v;
  begin gw[ng]=wr;ga[ng]=a;gd[ng]=v;ng=ng+1;end
 endtask
 task resets;
  input [24:0] a;
  integer d;
  begin for(d=0;d<dies;d=d+1)begin
   if(intel)push(1,a+d*2,16'h50);push(1,a+d*2,rc);end end
 endtask
 task golden;
  reg [15:0] count;
  integer d;
  begin
   ng=0;dies=dual?2:1;resets(0);
   if(!unit16 && bufsize==0)begin
    for(d=0;d<length;d=d+1)begin
     push(1,25'haaa,aa);push(1,25'h554,bb);push(1,25'haaa,wc);
     push(1,physical+d,(memory_value >> (((source+d)%4)*8)) & 16'hff);
     push(0,physical+d,0);push(0,physical+d,0);
    end
   end else if(!unit16)begin
    push(1,25'haaa,aa);push(1,25'h554,bb);push(1,physical,bc);
    push(1,physical,length-1);
    for(d=0;d<length;d=d+1)push(1,physical+d,(memory_value >> (((source+d)%4)*8)) & 16'hff);
    push(1,physical,cc);
   end else if(bufsize==0 || disable_buffer)begin
    if(disable_buffer)begin push(1,0,16'h70);push(0,0,0);push(1,0,16'h10);end
    else if(intel)push(1,25'h1000,wc);
    else begin push(1,25'haaa,aa);push(1,25'h554,bb);push(1,25'haaa,wc);end
    push(1,25'h1000,16'h3412);
   end else begin
    if(intel)begin for(d=0;d<dies;d=d+1)begin push(1,25'h1000+d*2,bc);push(0,25'h1000+d*2,0);end end
    else begin push(1,25'haaa,aa);push(1,25'h554,bb);push(1,25'h1000,bc);end
    count=length/2/dies-1;if(swap && !rawcount)count={count[15:2],count[0],count[1]};
    for(d=0;d<dies;d=d+1)push(1,25'h1000+d*2,count);
    for(d=0;d<length/2;d=d+1)push(1,25'h1000+d*2,16'h3412);
    for(d=0;d<dies;d=d+1)push(1,25'h1000+d*2,cc);
   end
   if(!unit16 && bufsize==0)begin end
   else if(!unit16)begin push(0,physical+length-1,0);push(0,physical+length-1,0);end
   else if(intel)begin for(d=0;d<dies;d=d+1)push(0,25'h1000+d*2,0);end
   else begin push(0,25'h1000+length-2,0);push(0,25'h1000+length-2,0);end
   resets(!unit16?(bufsize==0?physical+length-1:physical):25'h1000);resets(0);
  end
 endtask
 always @(posedge clk)begin
  md<=0;tf<=0;
  if(mreq)begin
   if(mp!=0)$fatal(1,"duplicate mem");
   if(!unit16 && ma!=(source>>2)+memory_reads)$fatal(1,"GB PSRAM word address/cache mismatch");
   memory_reads<=memory_reads+1;mp<=3;
  end
  if(mp>0)begin mp<=mp-1;if(mp==1)md<=1;end
  if(treq && tr)begin
   if(tp!=0)$fatal(1,"duplicate tx");
   if(at>=ng || twrite!=gw[at] || ta!=ga[at] || (twrite && td!=gd[at]))
    $fatal(1,"trace %0d wr%0d a%h d%h expected wr%0d a%h d%h",at,twrite,ta,td,gw[at],ga[at],gd[at]);
   tp<=2;te<=at==fail_at;at<=at+1;
  end
  if(tp>0)begin tp<=tp-1;if(tp==1)tf<=1;end
 end
 task run;
  input [3:0] want;
  integer t;
  begin
   golden;
   @(negedge clk);reset_n=0;mp=0;tp=0;md=0;tf=0;at=0;memory_reads=0;
   @(negedge clk);reset_n=1;start=1;
   @(negedge clk);start=0;t=0;
   while(!done && t<200000)begin @(negedge clk);t=t+1;end
   if(!done || busy || error!=want || (want==0 && at!=ng))$fatal(1,"result %d count%0d/%0d",error,at,ng);
   if(want==1 && at!=0)$fatal(1,"invalid job issued write");cases=cases+1;
   if(want==0 && !unit16 && memory_reads!=((source%4+length+3)/4))$fatal(1,"GB reread PSRAM per byte");
  end
 endtask
 initial begin
  run(0);
  wide=1;rawcount=1;aa=16'haaa9;bb=16'h5556;bc=16'h2526;cc=16'h292a;rc=16'hf0f0;run(0);
  wide=0;rawcount=0;intel=1;aa=0;bb=0;bc=16'he8;cc=16'hd0;rc=16'hff;read_value=16'h80;run(0);
  dual=1;run(0);swap=1;run(0);swap=0;dual=0;
  fail_at=0;run(4);if(at!=1)$fatal(1,"writes after error");fail_at=-1;
  read_value=16'ha0;run(7);if(at!=4)$fatal(1,"payload after failed Intel readiness");
  read_value=16'h80;run(0);
  bufsize=0;run(1);bufsize=16;
  allow=0;run(2);allow=1;
  tr=0;timeout=20;run(5);tr=1;timeout=10000;
  fork
   run(2);
   begin wait(busy);repeat(8)@(negedge clk);cancel=1;end
  join
  if(treq || mreq)$fatal(1,"requests after cancel");cancel=0;
  bufsize=0;length=2;wc=16'h40;run(0); // Intel / Sharp standard word
  disable_buffer=1;run(0); // M36L0R806 status70 / word10 at zero
  bufsize=1024;run(0); // disabled buffer takes priority over reported buffer size
  disable_buffer=0;bufsize=0;intel=0;aa=16'haa;bb=16'h55;wc=16'ha0;rc=16'hf0;read_value=16'h3412;run(0);
  wide=1;rawcount=1;aa=16'haaa9;bb=16'h5556;wc=16'ha0a0;rc=16'hf0f0;run(0);
  wide=0;rawcount=0;disable_buffer=1;run(1); // cannot apply Intel special sequence to AMD
  disable_buffer=0;length=4;run(1); // single-unit only
  length=2;intel=1;dual=1;run(1);dual=0;
  // Byte batch: distinct byte lanes, unaligned PSRAM, upper PSRAM chip,
  // last bank byte, no cross-window writes, and mid-batch failure/cancel.
  unit16=0;intel=0;bufsize=0;aa=16'haa;bb=16'h55;wc=16'ha0;rc=16'hf0;
  memory_value=32'h44332211;read_value=0;timeout=200000;
  length=1;run(0);length=3;source=1;run(0);
  length=256;source=25'h1000000;run(0);
  length=257;run(1);
  length=2;physical=25'h7fff;run(1);length=1;run(0);
  physical=25'h4000;length=16;fail_at=10;run(4);fail_at=-1;
  fork
   run(2);
   begin wait(busy);wait(at>=15);@(negedge clk);cancel=1;end
  join
  cancel=0;run(0);
  bufsize=32;length=32;source=0;physical=25'h4000;run(0);
  length=7;source=1;physical=25'h4010;run(0);
  length=32;physical=25'h4010;run(1); // Cannot cross buffer boundary.
  $display("PASS buffered program supervisor: %0d command traces/guards",cases);$finish;
 end
 initial begin #3000000;$fatal(1,"timeout");end
endmodule
