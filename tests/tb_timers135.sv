`timescale 1ns/1ps
module tb_timers135;
 parameter P=36;
 reg clk=0, reset_n=0, reg_wr=0;
 always #5 clk=~clk;
 reg [4:0] addr=0;
 reg [31:0] data=0;
 wire busy;
 wire [3:0] pins;
 chisburner_nor_bus108 #(.ADDRESS_CYCLES(P),.SELECT_CYCLES(P),
 .DATA_SETUP_CYCLES(P),.STROBE_CYCLES(P),.RELEASE_CYCLES(P)) d(
 .clk(clk),.reset_n(reset_n),.lifecycle_ok(1'b1),.selected(1'b1),
 .reg_wr(reg_wr),.reg_rd(1'b0),.reg_addr(5'd0),.reg_wr_addr(addr),.reg_wdata(data),
 .gb_data_in(8'h69),.bank3_in(8'h5a),.bank2_in(8'ha5),.busy(busy),.bank0_out(pins));
 task wr(input [4:0] a,input [31:0] v);
 begin @(negedge clk);addr=a;data=v;reg_wr=1;@(negedge clk);reg_wr=0;end
 endtask
 integer cycles,pulse;
 initial begin
 repeat(2) @(negedge clk);reset_n=1;
 wr(4,2);wr(5,0);wr(7,10000);wr(17,16'h1234);
 wr(19,32'h43424e31);wr(19,32'h43424e32);wr(3,7);
 cycles=0;pulse=0;
 while(busy)begin
 if(!pins[2])pulse=pulse+1;
 cycles=cycles+1;
 @(negedge clk);
 end
 if(cycles!=5*P || pulse!=P || d.error_reg!=0)
 $fatal(1,"P=%0d cycles=%0d pulse=%0d error=%0d",P,cycles,pulse,d.error_reg);
 if($bits(d.ticks)!=((P<=1)?1:$clog2(P)))$fatal(1,"width mismatch");
 $display("PASS timer135 P=%0d cycles=%0d pulse=%0d width=%0d",P,cycles,pulse,$bits(d.ticks));
 $finish;
 end
 initial begin #200000;$fatal(1,"timeout");end
endmodule
