// SPDX-License-Identifier: Apache-2.0
// Combined control and snapshot. No live Pocket MMIO decode yet.
`default_nettype none
module chisburner_sdram_read117 #(parameter APF_TAIL_PREFETCH = 0) (
 input wire cpu_clk,cpu_reset_n,bridge_clk,bridge_reset_n,
 input wire selected,reg_write,
 input wire [31:0] reg_offset,reg_wdata,
 input wire [3:0] reg_wstrb,
 output wire [31:0] reg_rdata,
 input wire host_quiet,axi_idle,ingress_drained,ingress_fault,
 output wire locked,idle,snapshot_ready,
 output wire m_arvalid,
 input wire m_arready,
 output wire [31:0] m_araddr,
 output wire [7:0] m_arlen,
 input wire m_rvalid,
 output wire m_rready,
 input wire [31:0] m_rdata,
 input wire [1:0] m_rresp,
 input wire m_rlast,
 input wire bridge_read,
 input wire [31:0] bridge_offset,
 output wire [31:0] bridge_data,
 output wire bridge_valid,bridge_ready,bridge_fault
);
 wire prepare,release_snapshot,ready,complete,fault,rejected;
 wire [31:0] source_address,byte_count;
 assign snapshot_ready=complete&&!locked;
 chisburner_sdram_control116 control (
  .clk(cpu_clk),.reset_n(cpu_reset_n),.selected(selected),.reg_write(reg_write),
  .reg_offset(reg_offset),.reg_wdata(reg_wdata),.reg_wstrb(reg_wstrb),
  .reg_rdata(reg_rdata),.transport_ready(ready),.transport_complete(complete),
  .transport_fault(fault),.transport_rejected(rejected),
  .ingress_drained(ingress_drained),.ingress_fault(ingress_fault),
  .host_quiet(host_quiet),.axi_idle(axi_idle),.prepare(prepare),
  .release_snapshot(release_snapshot),.source_address(source_address),
  .byte_count(byte_count),.locked(locked),.idle(idle)
 );
 chisburner_sdram_egress114 #(.APF_TAIL_PREFETCH(APF_TAIL_PREFETCH)) transport (
  .cpu_clk(cpu_clk),.cpu_reset_n(cpu_reset_n),.bridge_clk(bridge_clk),.bridge_reset_n(bridge_reset_n),
  .cmd_prepare(prepare),.cmd_release(release_snapshot),.cmd_address(source_address),.cmd_bytes(byte_count),
  .cmd_ready(ready),.cmd_complete(complete),.cmd_fault(fault),.cmd_rejected(rejected),
  .m_arvalid(m_arvalid),.m_arready(m_arready),.m_araddr(m_araddr),.m_arlen(m_arlen),
  .m_rvalid(m_rvalid),.m_rready(m_rready),.m_rdata(m_rdata),.m_rresp(m_rresp),.m_rlast(m_rlast),
  .bridge_read(bridge_read),.bridge_offset(bridge_offset),.bridge_data(bridge_data),
  .bridge_valid(bridge_valid),.bridge_ready(bridge_ready),.bridge_fault(bridge_fault)
 );
endmodule
`default_nettype wire
