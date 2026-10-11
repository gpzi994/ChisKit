// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// Dedicated streaming owner using a one-shot physical PSRAM word reader.
// External arbiter must grant exclusive controller ownership for the whole job,
// including drain after cancellation. This wrapper never writes either PSRAM.
// Source/memory reset must follow read_cdc131's common-reset contract.
module chisburner_psram_stream131(
 input wire s_clk,s_reset_n,cancel,req,
 input wire [22:0] address,
 output wire ready,busy,done,error,
 output wire [31:0] data,
 input wire m_clk,m_reset_n,
 output wire p0_rd,p1_rd,
 output wire [21:0] p0_address,p1_address,
 input wire [31:0] p0_data,p1_data,
 input wire p0_valid,p1_valid
);
 wire request,reader_busy,reader_fault,toggle;
 wire [22:0] word_address;
 wire [31:0] reader_data;
 reg previous_toggle;
 always @(posedge m_clk or negedge m_reset_n)
  if(!m_reset_n)previous_toggle<=0;else previous_toggle<=toggle;
 chisburner_read_cdc131 crossing(
  .s_clk(s_clk),.s_reset_n(s_reset_n),.s_cancel(cancel),.s_req(req),.s_address(address),
  .s_ready(ready),.s_busy(busy),.s_done(done),.s_error(error),.s_data(data),
  .m_clk(m_clk),.m_reset_n(m_reset_n),.m_req(request),.m_address(word_address),
  .m_ready(!reader_busy),.m_done(toggle^previous_toggle),.m_error(reader_fault),.m_data(reader_data));
 chisburner_psram_word_reader131 reader(
  .clk(m_clk),.reset_n(m_reset_n),.start(request && !reader_busy),.address(word_address),
  .p0_rd(p0_rd),.p0_address(p0_address),.p0_data(p0_data),.p0_valid(p0_valid),
  .p1_rd(p1_rd),.p1_address(p1_address),.p1_data(p1_data),.p1_valid(p1_valid),
  .busy(reader_busy),.done_toggle(toggle),.fault(reader_fault),.data(reader_data),.half());
endmodule
`default_nettype wire
