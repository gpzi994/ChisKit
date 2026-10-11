// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// Read-only full-range comparison. Parent owns the raw NOR adapter and both
// PSRAM ports until hold_owner drops, including cancellation drain.
// NOR must already be in array-read mode; addresses here are linear GBA bytes.
module chisburner_compare_path131(
 input wire clk,reset_n,memory_clk,memory_reset_n,
 input wire lifecycle_ok,psram_granted,start,cancel,
 input wire [25:0] base_bytes,length_bytes,image_bytes,nor_bytes,
 output wire busy,done,hold_owner,output wire [3:0] error,
 output wire [25:0] completed_bytes,fault_address,
 output wire [15:0] fault_expected,fault_actual,
 output wire p0_rd,p1_rd,output wire [21:0] p0_address,p1_address,
 input wire [31:0] p0_data,p1_data,input wire p0_valid,p1_valid,
 output wire nor_req,output wire [24:0] nor_address,
 input wire nor_ready,nor_done,nor_error,nor_busy,
 input wire [15:0] nor_data
);
 wire mr,mready,mdone,merror,mbusy,engine_busy,engine_done;
 wire [22:0] ma;wire [31:0] md;
 reg draining;
 wire live=lifecycle_ok && psram_granted;
 wire stop=cancel || !live;
 assign hold_owner=engine_busy || mbusy || nor_busy || draining;
 assign busy=hold_owner;
 assign done=engine_done && !hold_owner;
 // Prevent a second request from inheriting an outstanding CDC response.
 always @(posedge clk or negedge reset_n)
  if(!reset_n)draining<=0;
  else if(engine_busy && stop)draining<=1;
  else if(!engine_busy && !mbusy && !nor_busy)draining<=0;
 chisburner_compare_engine131 engine(
  .clk(clk),.reset_n(reset_n),.lifecycle_ok(live),
  .abort(stop),.start(start && !hold_owner && live),
  .base_bytes(base_bytes),.length_bytes(length_bytes),.image_bytes(image_bytes),.nor_bytes(nor_bytes),
  .busy(engine_busy),.done(engine_done),.error(error),.completed_bytes(completed_bytes),
  .fault_address(fault_address),.fault_expected(fault_expected),.fault_actual(fault_actual),
  .mem_req(mr),.mem_word_address(ma),.mem_ready(mready),.mem_done(mdone),.mem_error(merror),.mem_data(md),
  .nor_req(nor_req),.nor_address(nor_address),
  .nor_ready(nor_ready),.nor_done(nor_done),.nor_error(nor_error),.nor_read_data(nor_data));
 chisburner_psram_stream131 memory_path(
  .s_clk(clk),.s_reset_n(reset_n),.cancel(stop),.req(mr),.address(ma),
  .ready(mready),.busy(mbusy),.done(mdone),.error(merror),.data(md),
  .m_clk(memory_clk),.m_reset_n(memory_reset_n),.p0_rd(p0_rd),.p1_rd(p1_rd),
  .p0_address(p0_address),.p1_address(p1_address),.p0_data(p0_data),.p1_data(p1_data),
  .p0_valid(p0_valid),.p1_valid(p1_valid));
endmodule
`default_nettype wire
