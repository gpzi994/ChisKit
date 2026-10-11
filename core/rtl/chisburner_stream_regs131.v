// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// Private submission port. Keys are an interlock, NOT fresh-ID authority.
// The parent must supply OS-token authority, exclusive bus ownership and
// idle_adapters covering both the memory CDC and the raw transaction drain.
// Reset must also reset those adapters. No production top-level connection yet.
module chisburner_stream_regs131(
 input wire clk,reset_n,lifecycle_ok,write_allowed,idle_adapters,
 input wire wr,input wire [4:0] address,read_address,input wire [31:0] data,
 output reg [31:0] read_data,
 output reg busy,done,output reg [3:0] error,output reg rejected,
 output wire mem_req,output wire [22:0] mem_address,
 input wire mem_ready,mem_done,mem_error,input wire [31:0] mem_data,
 output wire tx_req,tx_write,output wire [24:0] tx_address,output wire [15:0] tx_data,
 input wire tx_ready,tx_done,tx_error,input wire [15:0] tx_read_data
);
 // 20 index, 21 descriptor data, 22 control (1 submit, 2 cancel),
 // 23 status: busy/done/armed/rejected bits 0..3, error bits 7..4,
 // 24 key: 0x43425331 followed immediately by 0x43425332.
 reg [31:0] words[0:10];
 reg [15:0] written;
 reg [4:0] reserved_nonzero;
 reg [3:0] index;
 reg key1,armed,start,cancel;
 reg [1:0] phase;
 wire engine_busy,engine_done;
 wire [3:0] engine_error;
 wire compare_job=words[0][7];
 wire gb_compare_job=words[0]==32'h80;
 wire pb,pd,cb,cd,pmr,ptr,ptw,cmr,ctr;
 wire [3:0] pe,ce;
 wire [22:0] pma,cma;wire [24:0] pta,cta;wire [15:0] ptd;
 wire [25:0] compared,fault;wire [15:0] expected,actual;
 wire gb,gd,gmr,gtr;wire [3:0] ge;
 wire [22:0] gma;wire [24:0] gta;wire [25:0] gcompared,gfault;
 wire [15:0] gexpected,gactual;
 assign engine_busy=gb_compare_job?gb:compare_job?cb:pb;
 assign engine_done=gb_compare_job?gd:compare_job?cd:pd;
 assign engine_error=gb_compare_job?ge:compare_job?ce:pe;
 assign mem_req=gb_compare_job?gmr:compare_job?cmr:pmr;
 assign mem_address=gb_compare_job?gma:compare_job?cma:pma;
 assign tx_req=gb_compare_job?gtr:compare_job?ctr:ptr;
 assign tx_write=compare_job?1'b0:ptw;
 assign tx_address=gb_compare_job?gta:compare_job?cta:pta;
 assign tx_data=compare_job?16'd0:ptd;
 integer n;
 wire program_shape=(words[0]&32'hffffff80)==0 &&
  words[1][31:25]==0 && words[2][31:25]==0 &&
  words[3][31:17]==0 && words[4][31:17]==0 &&
  words[5][31:25]==0 && words[6][31:25]==0 && reserved_nonzero==0;
 // Compare descriptor: flags=0x81, base,length,image size,NOR size in 1..4.
 // Linear GBA only; the engine validates bounds/alignment without truncation.
 wire compare_shape=(words[0]==32'h81 || gb_compare_job) && words[1][31:26]==0 &&
  words[2][31:26]==0 && words[3][31:26]==0 && words[4][31:26]==0 &&
  words[5]==0 && words[6]==0 && words[7]==0 && words[8]==0 &&
  words[9]==0 && words[10]==0 && reserved_nonzero==0;
 wire shape_ok=compare_job?compare_shape:program_shape;
 wire cancel_now=cancel || (wr && address==22 && data==2);
 always @* begin
  read_data=0;
  case(read_address)
   20:read_data={28'd0,index};
   21:begin
    read_data=index<=10?words[index]:32'd0;
    if(compare_job)case(index)
     12:read_data={6'd0,gb_compare_job?gcompared:compared};
     13:read_data={6'd0,gb_compare_job?gfault:fault};
     14:read_data=gb_compare_job?{gactual,gexpected}:{actual,expected};
    endcase
    if(index==15)read_data=32'h47423230; // GB20 capability; writes still require zero.
   end
   23:read_data={24'd0,error,rejected,armed,done,busy};
   24:read_data={16'd0,written};
  endcase
 end
 always @(posedge clk or negedge reset_n) begin
  if(!reset_n)begin
   for(n=0;n<11;n=n+1)words[n]<=0;
   written<=0;reserved_nonzero<=0;index<=0;key1<=0;armed<=0;start<=0;cancel<=0;
   busy<=0;done<=0;error<=0;rejected<=0;phase<=0;
  end else begin
   start<=0;
   if(!lifecycle_ok || !write_allowed)begin key1<=0;armed<=0;end
   if(busy)begin
    key1<=0;armed<=0;
    if(!lifecycle_ok || !write_allowed || cancel_now)cancel<=1;
    // Ignore stale sticky done from the preceding command until start is consumed.
    if(phase==0)phase<=1;
    else if(phase==1)phase<=2;
    else if(engine_done && !engine_busy && idle_adapters)begin
     busy<=0;done<=1;error<=engine_error;cancel<=0;
    end
    if(wr && address>=20 && address<=24)begin
     if(!(address==22 && data==2))rejected<=1;
    end
   end else if(wr && address>=20 && address<=24)begin
    key1<=0;
    case(address)
     20:begin
      armed<=0;
      if(data<16)index<=data[3:0];else rejected<=1;
     end
     21:begin
      if(index<=10)words[index]<=data;
      else reserved_nonzero[index-11]<=|data;
      written[index]<=1;armed<=0;
     end
     22:begin
      armed<=0;
      if(data==1)begin
       done<=0;error<=0;rejected<=0;
       // Consume every submission attempt, including rejected submissions.
       written<=0;
       if(!lifecycle_ok || !write_allowed || !armed)begin done<=1;error<=3;end
       else if(!idle_adapters)begin done<=1;error<=8;end
       else if(written!=16'hffff || !shape_ok)begin done<=1;error<=1;end
       else begin busy<=1;start<=1;cancel<=0;phase<=0;end
      end else if(data!=2)rejected<=1;
     end
     24:begin
      armed<=0;
      if(lifecycle_ok && write_allowed)begin
       if(data==32'h43425331)key1<=1;
       else if(data==32'h43425332 && key1)armed<=1;
       else rejected<=1;
      end
     end
     default:begin armed<=0;rejected<=1;end
    endcase
   end
  end
 end
 chisburner_nor_program131 engine(
  .clk(clk),.reset_n(reset_n),.start(start && !compare_job),.cancel(cancel_now),
  .lifecycle_ok(lifecycle_ok),.write_allowed(write_allowed),.idle_adapters(idle_adapters),
  .unit16(words[0][0]),.intel_mode(words[0][1]),.dual_intel(words[0][2]),
  .msp_wide(words[0][3]),.swapped(words[0][4]),.raw_count(words[0][5]),
  .disable_buffer_program(words[0][6]),.source_address(words[1][24:0]),
  .physical_address(words[2][24:0]),.length_bytes(words[3][16:0]),
  .buffer_bytes(words[4][16:0]),.unlock0(words[5][24:0]),.unlock1(words[6][24:0]),
  .aa(words[7][15:0]),.bb(words[7][31:16]),.buffer_command(words[8][15:0]),
  .confirm_command(words[8][31:16]),.reset_command(words[9][15:0]),
  .word_command(words[9][31:16]),.timeout_cycles(words[10]),
  .busy(pb),.done(pd),.error(pe),
  .mem_req(pmr),.mem_address(pma),.mem_ready(mem_ready),.mem_done(mem_done && !compare_job),
  .mem_error(mem_error),.mem_data(mem_data),.tx_req(ptr),.tx_write(ptw),
  .tx_address(pta),.tx_data(ptd),.tx_ready(tx_ready),.tx_done(tx_done && !compare_job),
  .tx_error(tx_error),.tx_read_data(tx_read_data));
 chisburner_compare_engine131 comparison(
  .clk(clk),.reset_n(reset_n),.start(start && compare_job && !gb_compare_job),
  .lifecycle_ok(lifecycle_ok && write_allowed),.abort(cancel_now),.base_bytes(words[1][25:0]),
  .length_bytes(words[2][25:0]),.image_bytes(words[3][25:0]),.nor_bytes(words[4][25:0]),
  .busy(cb),.done(cd),.error(ce),.completed_bytes(compared),.fault_address(fault),
  .fault_expected(expected),.fault_actual(actual),.mem_req(cmr),.mem_word_address(cma),
  .mem_ready(mem_ready),.mem_done(mem_done && compare_job && !gb_compare_job),.mem_error(mem_error),.mem_data(mem_data),
  .nor_req(ctr),.nor_address(cta),.nor_ready(tx_ready),
  .nor_done(tx_done && compare_job && !gb_compare_job),.nor_error(tx_error),.nor_read_data(tx_read_data));
 chisburner_gb_compare207 gb_comparison(
  .clk(clk),.reset_n(reset_n),.start(start && gb_compare_job),.adapters_idle(idle_adapters),
  .lifecycle_ok(lifecycle_ok && write_allowed),.abort(cancel_now),.base_bytes(words[1][25:0]),
  .length_bytes(words[2][25:0]),.image_bytes(words[3][25:0]),.nor_bytes(words[4][25:0]),
  .busy(gb),.done(gd),.error(ge),.completed_bytes(gcompared),.fault_address(gfault),
  .fault_expected(gexpected),.fault_actual(gactual),.mem_req(gmr),.mem_word_address(gma),
  .mem_ready(mem_ready),.mem_done(mem_done && gb_compare_job),.mem_error(mem_error),.mem_data(mem_data),
  .nor_req(gtr),.nor_address(gta),.nor_ready(tx_ready),
  .nor_done(tx_done && gb_compare_job),.nor_error(tx_error),.nor_read_data(tx_read_data));
endmodule
`default_nettype wire
