// SPDX-License-Identifier: Apache-2.0
// Pocket development wiring. 0x10..13 bridge writes -> SDRAM; 0x21 reads
// -> prepared snapshot. MMIO +14 reservation command, +18 reservation status.
// PLL/power reset only: CPU warm reset must NOT discard an AXI transaction.
`default_nettype none
module chisburner_sdram_pocket120 (
 input wire cpu_clk,bridge_clk,power_reset_n,
 input wire selected,reg_write,
 input wire [31:0] reg_offset,reg_wdata,
 input wire [3:0] reg_wstrb,
 output wire [31:0] reg_rdata,
 input wire [31:0] bridge_addr,bridge_wdata,
 input wire bridge_wr,bridge_rd,host_busy,legacy_busy,
 output wire admission_blocked,bridge_drained,
 output wire [31:0] bridge_rdata,
 output wire bridge_rvalid,
 output wire arvalid,input wire arready,output wire [31:0] araddr,
 output wire [7:0] arlen,input wire rvalid,output wire rready,
 input wire [31:0] rdata,input wire [1:0] rresp,input wire rlast,
 output wire awvalid,input wire awready,output wire [31:0] awaddr,
 output wire [7:0] awlen,output wire wvalid,input wire wready,
 output wire [31:0] wdata,output wire [3:0] wstrb,output wire wlast,
 input wire bvalid,input wire [1:0] bresp
);
 (* preserve, altera_attribute = "-name SYNCHRONIZER_IDENTIFICATION FORCED_IF_ASYNCHRONOUS" *) reg [1:0] crst,brst;
 always @(posedge cpu_clk or negedge power_reset_n)
  if(!power_reset_n) crst<=0;else crst<={crst[0],1'b1};
 always @(posedge bridge_clk or negedge power_reset_n)
  if(!power_reset_n) brst<=0;else brst<={brst[0],1'b1};
 wire write_range=bridge_addr[31:26]==6'b000100;
 wire read_range=bridge_addr[31:24]==8'h21;
 reg rd_prev;
 always @(posedge bridge_clk or negedge power_reset_n)
  if(!power_reset_n) rd_prev<=0;else rd_prev<=bridge_rd&&read_range;
 wire read_pulse=bridge_rd&&read_range&&!rd_prev;
 wire in_fault,in_ready,in_drained,unused_bready;
 wire lock_idle,lock_owned,lock_fault,lock_rejected,lock_timeout;
 wire ctl_idle,ctl_fault,snapshot_ready,read_fault,read_ready;
 wire owned_bridge;
 wire [31:0] diag_data;
 wire [31:0] ctl_rdata;
 reg read_pending;
 always @(posedge cpu_clk or negedge power_reset_n)
  if(!power_reset_n) read_pending<=0;
  else begin
   if(arvalid&&arready) read_pending<=1;
   if(rvalid&&rready&&rlast) read_pending<=0;
  end
 wire axi_quiet=!read_pending&&!arvalid&&!awvalid&&!wvalid;
 // A gap between snapshot AXI words is not permission to release ownership.
 wire operation_quiet=axi_quiet&&(ctl_idle||snapshot_ready);
 wire command=selected&&reg_write&&reg_offset==32'h14&&reg_wstrb==4'hf;
 chisburner_sdram_interlock119 reservation (
  .cpu_clk(cpu_clk),.bridge_clk(bridge_clk),.reset_n(power_reset_n),
  .cpu_acquire(command&&reg_wdata==1),.cpu_release(command&&reg_wdata==2),
  .cpu_axi_idle(operation_quiet),.cpu_idle(lock_idle),.cpu_owned(lock_owned),
  .cpu_fault(lock_fault),.cpu_rejected(lock_rejected),.cpu_timeout(lock_timeout),
  .bridge_new_job(1'b0),.bridge_host_busy(host_busy),
  .bridge_read_pending(legacy_busy||read_pulse||bridge_rvalid),
  .bridge_writes_drained(in_drained&&!legacy_busy),.bridge_error(in_fault||read_fault),
  .bridge_accept(),.bridge_blocked(admission_blocked),.bridge_owned(owned_bridge),.bridge_rejected()
 );
 chisburner_sdram_diag124 diagnostic (
  .cpu_clk(cpu_clk),.bridge_clk(bridge_clk),.reset_n(power_reset_n),
  .select_write(selected&&reg_write&&reg_offset==32'h1c&&reg_wstrb==4'hf),
  .select_data(reg_wdata),.ingress_fault(in_fault),.read_fault(read_fault),
  .owned(owned_bridge),.host_busy(host_busy),.legacy_busy(legacy_busy),
  .read_pulse(read_pulse),.read_valid(bridge_rvalid),.writes_drained(in_drained),
  .bridge_address(bridge_addr),.data(diag_data)
 );
 assign reg_rdata=!selected ? 32'd0 : reg_offset==32'h1c ? diag_data : reg_offset==32'h18 ? {27'd0,lock_timeout,lock_rejected,lock_fault,lock_owned,lock_idle} : ctl_rdata;
 chisburner_sdram_read117 #(.APF_TAIL_PREFETCH(1)) reader (
  .cpu_clk(cpu_clk),.cpu_reset_n(crst[1]),.bridge_clk(bridge_clk),.bridge_reset_n(brst[1]),
  .selected(selected&&reg_offset<32'h14),.reg_write(reg_write),
  .reg_offset(reg_offset),.reg_wdata(reg_wdata),.reg_wstrb(reg_wstrb),.reg_rdata(ctl_rdata),
  .host_quiet(lock_owned),.axi_idle(axi_quiet),.ingress_drained(lock_owned),
  .ingress_fault(lock_fault),.locked(ctl_fault),.idle(ctl_idle),.snapshot_ready(snapshot_ready),
  .m_arvalid(arvalid),.m_arready(arready),.m_araddr(araddr),.m_arlen(arlen),
  .m_rvalid(rvalid),.m_rready(rready),.m_rdata(rdata),.m_rresp(rresp),.m_rlast(rlast),
  .bridge_read(read_pulse),.bridge_offset(bridge_addr-32'h21000000),
  .bridge_data(bridge_rdata),.bridge_valid(bridge_rvalid),.bridge_ready(read_ready),.bridge_fault(read_fault)
 );
 chisburner_sdram_ingress113 writer (
  .src_clk(bridge_clk),.src_reset_n(brst[1]),.dst_clk(cpu_clk),.dst_reset_n(crst[1]),
  .src_valid(bridge_wr&&write_range),.src_address(bridge_addr),.src_data(bridge_wdata),
  .src_ready(in_ready),.src_drained(in_drained),.src_fault(in_fault),
  .m_awvalid(awvalid),.m_awready(awready),.m_awaddr(awaddr),.m_awlen(awlen),
  .m_wvalid(wvalid),.m_wready(wready),.m_wdata(wdata),.m_wstrb(wstrb),.m_wlast(wlast),
  .m_bvalid(bvalid),.m_bresp(bresp),.m_bready(unused_bready)
 );
 assign bridge_drained=in_drained&&!in_fault;
endmodule
`default_nettype wire
