// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// Word-port mux around two external PSRAM controllers. Read data may fan out,
// but valid is delivered ONLY to the retained owner. Stream is read-only.
// Legacy request signals here are drained/issued requests, not new admission.
module chisburner_psram_ports131(
 input wire clk,reset_n,lease,legacy_pending,stream_pending,
 output wire legacy_accept,stream_accept,stream_owner,
 input wire l0_rd,l0_wr,l1_rd,l1_wr,s0_rd,s1_rd,
 input wire [21:0] l0_addr,l1_addr,s0_addr,s1_addr,
 input wire [31:0] l0_data,l1_data,
 input wire [3:0] l0_strb,l1_strb,
 output wire p0_rd,p0_wr,p1_rd,p1_wr,
 output wire [21:0] p0_addr,p1_addr,
 output wire [31:0] p0_data,p1_data,
 output wire [3:0] p0_strb,p1_strb,
 input wire p0_busy,p1_busy,p0_valid,p1_valid,
 output wire l0_valid,l1_valid,s0_valid,s1_valid
);
 wire legacy_owner;
 chisburner_psram_owner131 owner(
  .clk(clk),.reset_n(reset_n),.lease(lease),.legacy_pending(legacy_pending),
  .stream_pending(stream_pending),.p0_busy(p0_busy),.p1_busy(p1_busy),
  .p0_valid(p0_valid),.p1_valid(p1_valid),.legacy_accept(legacy_accept),
  .legacy_owner(legacy_owner),.stream_accept(stream_accept),.stream_owner(stream_owner));
 assign p0_rd=legacy_owner?l0_rd:stream_owner?s0_rd:1'b0;
 assign p1_rd=legacy_owner?l1_rd:stream_owner?s1_rd:1'b0;
 assign p0_wr=legacy_owner && l0_wr;
 assign p1_wr=legacy_owner && l1_wr;
 assign p0_addr=stream_owner?s0_addr:l0_addr;
 assign p1_addr=stream_owner?s1_addr:l1_addr;
 assign p0_data=l0_data;
 assign p1_data=l1_data;
 assign p0_strb=legacy_owner?l0_strb:4'd0;
 assign p1_strb=legacy_owner?l1_strb:4'd0;
 assign l0_valid=legacy_owner && p0_valid;
 assign l1_valid=legacy_owner && p1_valid;
 assign s0_valid=stream_owner && p0_valid;
 assign s1_valid=stream_owner && p1_valid;
endmodule
`default_nettype wire
