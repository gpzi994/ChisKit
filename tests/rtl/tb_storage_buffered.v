// Test-only composition of the real command/table module with the registered
// F8/default read-mux branches from core_top. Other memory regions are absent.
// This is NOT a serial PHY or Pocket host firmware simulation.
`default_nettype none
module tb_storage_buffered (
    input wire clk,
    input wire bridge_endian_little,
    input wire [31:0] bridge_addr,
    input wire bridge_rd,
    output reg [31:0] bridge_rd_data,
    input wire bridge_wr,
    input wire [31:0] bridge_wr_data,
    input wire target_dataslot_flush_in,
    output wire target_dataslot_ack_out,
    output wire target_dataslot_done_out,
    output wire [15:0] storage_trace_out
);
wire [31:0] cmd_bridge_rd_data;
always @(posedge clk) begin
    case (bridge_addr[31:24])
        8'hF8: bridge_rd_data <= cmd_bridge_rd_data;
        default: bridge_rd_data <= 32'd0;
    endcase
end
tb_core_bridge_cmd command_path (
    .clk(clk),
    .bridge_endian_little(bridge_endian_little),
    .bridge_addr(bridge_addr),
    .bridge_rd(bridge_rd),
    .bridge_rd_data(cmd_bridge_rd_data),
    .bridge_wr(bridge_wr),
    .bridge_wr_data(bridge_wr_data),
    .status_boot_done(1'b1),
    .status_setup_done(1'b0),
    .status_running(1'b1),
    .shutdown_ack_s(1'b1),
    .target_dataslot_flush_in(target_dataslot_flush_in),
    .target_dataslot_id_in(16'd4),
    .target_dataslot_ack_out(target_dataslot_ack_out),
    .target_dataslot_done_out(target_dataslot_done_out),
    .storage_trace_out(storage_trace_out),
    .reset_n(),
    .shutdown_pending()
);
endmodule
`default_nettype wire
