// Real serial controller, with resolved input pads. This test observes TX
// state taps, not the simulator's unsupported procedural inout-reg waveform.
module tb_storage_tx_tap (
    input wire clk, endian_little, host_oe, host_clk, host_mosi, host_miso, host_ss,
    input wire [31:0] rd_data,
    output wire queued, done, aborted, paired, pmp_rd,
    output wire pad_clk, pad_mosi, pad_miso,
    output wire [31:0] address, current_address, data
);
wire sck,mosi,miso;
assign pad_clk=sck;
assign pad_mosi=mosi;
assign pad_miso=miso;
assign sck=host_oe?host_clk:1'bz;
assign mosi=host_oe?host_mosi:1'bz;
assign miso=host_oe?host_miso:1'bz;
io_bridge_peripheral dut (
    .clk(clk),.reset_n(1'b1),.endian_little(endian_little),
    .pmp_addr(),.pmp_addr_valid(),.pmp_rd(pmp_rd),.pmp_rd_data(rd_data),.pmp_wr(),.pmp_wr_data(),
    .trace_tx_valid(queued),.trace_tx_paired(paired),.trace_tx_address(address),
    .trace_tx_current_address(current_address),.trace_tx_data(data),
    .trace_tx_done(done),.trace_tx_abort(aborted),
    .phy_spiclk(sck),.phy_spimosi(mosi),.phy_spimiso(miso),.phy_spiss(host_ss)
);
endmodule
