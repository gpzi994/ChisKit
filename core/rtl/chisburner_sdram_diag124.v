// SPDX-License-Identifier: Apache-2.0
// Passive first-event recorder. No outputs control the transport or locks.
// Frozen bundled metadata crosses only after a three-flop valid synchronizer.
// The 8 ns bundled-data routing bound is required in the diagnostic build.
`default_nettype none
module chisburner_sdram_diag124 (
 input wire cpu_clk,bridge_clk,reset_n,
 input wire select_write,input wire [31:0] select_data,
 input wire ingress_fault,read_fault,owned,host_busy,legacy_busy,
 input wire read_pulse,read_valid,writes_drained,
 input wire [31:0] bridge_address,
 output wire [31:0] data
);
 reg [31:0] last_read,read_count;
 reg [95:0] first_b;
 reg valid_b;
 (* preserve, altera_attribute = "-name SYNCHRONIZER_IDENTIFICATION FORCED_IF_ASYNCHRONOUS" *) reg [2:0] valid_c;
 reg [95:0] first_c;
 reg captured;
 reg [1:0] page;
 wire [5:0] cause={owned&&!writes_drained,owned&&(read_pulse||read_valid),
                    owned&&legacy_busy,owned&&host_busy,read_fault,ingress_fault};
 always @(posedge bridge_clk or negedge reset_n) begin
  if(!reset_n) begin last_read<=0;read_count<=0;first_b<=0;valid_b<=0;end
  else begin
   if(read_pulse) begin last_read<=bridge_address;read_count<=read_count+1;end
   if(!valid_b&&|cause) begin
    valid_b<=1;
    // read_fault is reported one cycle after the offending request.
    first_b<={read_count+(read_pulse?32'd1:32'd0),
              read_fault?last_read:bridge_address,26'd0,cause};
   end
  end
 end
 always @(posedge cpu_clk or negedge reset_n) begin
  if(!reset_n) begin valid_c<=0;first_c<=0;captured<=0;page<=0;end
  else begin
   valid_c<={valid_c[1:0],valid_b};
   if(valid_c[2]&&!captured) begin first_c<=first_b;captured<=1;end
   if(select_write) page<=select_data[1:0];
  end
 end
 assign data=page==3 ? 32'h53443124 : !captured ? 32'd0 :
             page==0 ? (32'h80000000|first_c[31:0]) :
             page==1 ? first_c[63:32] : first_c[95:64];
endmodule
`default_nettype wire
