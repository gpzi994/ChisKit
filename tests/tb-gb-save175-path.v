`timescale 1ns/1ps
module tb_gb_save175_path;
 reg clk=0,mc=0;always #5 clk=~clk;always #7 mc=~mc;
 reg reset_n=0,life=1,wr=0;reg [4:0] ra=0;reg [31:0] wd=0;
 wire [31:0] rd;wire hold_owner,p0,p1,wa;wire [3:0] pins;
 wire [7:0] b1,b2,b3;wire d1,d2,d3,p30dir;
 integer n,ram_reads=0,rom_reads=0,controls=0;reg checking=0;
 integer ram_writes180=0;reg writing180=0;
 integer serial185=0;reg writing185=0;
 chisburner_program_path131 dut(
 .clk(clk),.reset_n(reset_n),.memory_clk(mc),.memory_reset_n(reset_n),
 .lifecycle_ok(life),.selected(1'b1),.write_allowed(1'b0),.psram_granted(1'b0),
 .reg_wr(wr),.reg_rd(1'b0),.reg_address(ra),.reg_read_address(ra),.reg_data(wd),.reg_read_data(rd),
 .hold_owner(hold_owner),.p0_rd(p0),.p1_rd(p1),
 .p0_data(32'b0),.p1_data(32'b0),.p0_valid(1'b0),.p1_valid(1'b0),
 .gb_data_in(8'h5a),.bank3_in(8'h78),.bank2_in(8'h56),
 .bank0_out(pins),.bank1_out(b1),.bank2_out(b2),.bank3_out(b3),
 .bank1_dir(d1),.bank2_dir(d2),.bank3_dir(d3),.pin30_dir(p30dir),.write_active(wa));
 always @(posedge clk)if(reset_n)begin
  if(p0||p1)$fatal(1,"GB save touched PSRAM");
  if(checking&&!pins[1])begin
   if(!d2||!d3||d1||p30dir)$fatal(1,"GB directions");
   if({b2,b3}>=16'ha000)begin
    if(pins[0])$fatal(1,"RAM CS not active");ram_reads=ram_reads+1;
   end else begin
    if(!pins[0])$fatal(1,"Header selected RAM");rom_reads=rom_reads+1;
   end
  end
  if(checking&&!pins[2])begin
   if(writing185)begin
    if(pins[0]||{b2,b3}!=16'ha080||b1!=8'hc2||!d1||p30dir)$fatal(1,"GB185 EEPROM pins");
    serial185=serial185+1;
   end else if(writing180)begin
    if(pins[0]||{b2,b3}!=16'hbfff||b1!=8'ha5||!d1||p30dir)$fatal(1,"GB180 RAM write pins");
    ram_writes180=ram_writes180+1;
   end else begin
   if(!pins[0]||({b2,b3}!=0&&{b2,b3}!=16'h4000))$fatal(1,"invalid control write");
   controls=controls+1;
   end
  end
 end
 task put;input [4:0] a;input [31:0] v;
 begin @(negedge clk);ra=a;wd=v;wr=1;@(negedge clk);wr=0;end endtask
 task expect_reg;input [4:0] a;input [31:0] v;
 begin @(negedge clk);ra=a;#1;if(rd!==v)$fatal(1,"register %d got %h want %h",a,rd,v);end endtask
 task drain;begin repeat(200)@(negedge clk);if(hold_owner)$fatal(1,"owner did not drain");end endtask
 initial begin
 repeat(5)@(negedge clk);reset_n=1;repeat(5)@(negedge clk);
 put(4,8);put(7,500);checking=1;
 put(5,32'h100);put(3,2);drain();expect_reg(8,32'h5a);expect_reg(9,1);
 put(5,32'ha000);put(3,2);drain();expect_reg(8,32'h5a);
 put(5,0);put(17,10);put(19,32'h43424e31);put(19,32'h43424e32);put(3,7);drain();expect_reg(10,0);
 put(5,32'ha000);put(19,32'h43424e31);put(19,32'h43424e32);put(3,7);drain();expect_reg(10,7);
 if(ram_reads!=36||rom_reads!=36||controls!=36)$fatal(1,"pulse count");
`ifdef CHISBURNER_GB_RESTORE180
 put(4,10);writing180=1;put(5,32'hbfff);put(17,32'ha5);
 put(19,32'h43424e31);put(19,32'h43424e32);put(3,7);drain();expect_reg(10,0);
 if(ram_writes180!=36)$fatal(1,"GB180 write pulse count");writing180=0;
 $display("PASS GB180 production wrapper: SRAM write/CS, no PSRAM grants or requests");
`endif
`ifdef CHISBURNER_GB_RESTORE185
 put(4,11);writing185=1;put(5,32'ha080);put(17,32'hc2);
 put(19,32'h43424e31);put(19,32'h43424e32);put(3,7);drain();expect_reg(10,0);
 if(serial185!=36)$fatal(1,"GB185 serial write pulse count");writing185=0;
 put(3,2);drain();expect_reg(8,32'h5a);
 put(5,32'ha081);put(19,32'h43424e31);put(19,32'h43424e32);put(3,7);drain();expect_reg(10,7);
 $display("PASS GB185 production wrapper: isolated serial write/read CS and address denial, zero PSRAM requests");
`endif
 checking=0;put(4,2);put(5,0);put(3,2);drain();expect_reg(8,32'h5678);expect_reg(9,2);
 if(p0||p1)$fatal(1,"PSRAM requested");
 $display("PASS GB175 production wrapper: header/RAM/control isolation, old GBA read, zero PSRAM requests");$finish;
 end
 initial begin #100000;$fatal(1,"timeout");end
endmodule
