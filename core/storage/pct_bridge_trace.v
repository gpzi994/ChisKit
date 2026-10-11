// Observation-only APF recorder. Never feeds a command, ACK, or bridge mux.
// Capture first 256 distinct events after FLUSH. Repeated identical requests
// and replies are suppressed independently. Overflow/collision are explicit.
// Record words: kind[31:28]+ticks[27:0], address, data, current TX address.
// kind 1=request read, 2=TX reply, 3=host write, 4=unpaired TX, 15=arm.
module pct_bridge_trace #(
    parameter ADDR_BITS = 8
) (
    input wire clk_bridge,
    input wire arm,
    input wire bridge_rd,
    input wire bridge_wr,
    input wire [31:0] bridge_addr,
    input wire [31:0] bridge_wr_data_wire,
    input wire tx_valid,
    input wire tx_paired,
    input wire [31:0] tx_address,
    input wire [31:0] tx_current_address,
    input wire [31:0] tx_data_wire,
    input wire clk_cpu,
    input wire reset_cpu,
    input wire freeze_read,
    input wire data_pop,
    output wire [31:0] status_cpu,
    output wire [31:0] count_cpu,
    output wire [31:0] data_cpu
);
localparam DEPTH = (1 << ADDR_BITS);
(* ramstyle = "M10K, no_rw_check" *) reg [127:0] events [0:DEPTH-1];
reg armed = 0, frozen = 0, full = 0, collision = 0, arm_prev = 0;
reg [ADDR_BITS:0] used = 0;
reg [27:0] elapsed = 0;
reg freeze_request = 0;
(* async_reg = "true" *) reg freeze_s1 = 0, freeze_s2 = 0;
reg rd_seen = 0, tx_seen = 0;
reg [31:0] last_rd_addr = 0, last_tx_addr = 0, last_tx_data = 0;
wire rd_event = bridge_rd && bridge_addr[31:24] == 8'hf8 &&
                (!rd_seen || bridge_addr != last_rd_addr);
wire tx_relevant = tx_valid && (tx_address[31:24] == 8'hf8 ||
                                tx_current_address[31:24] == 8'hf8);
wire tx_event = tx_relevant && (!tx_seen || tx_address != last_tx_addr ||
                               tx_data_wire != last_tx_data);
wire wr_event = bridge_wr && bridge_addr[31:24] == 8'hf8;
reg [127:0] next_event;
always @(*) begin
    if (wr_event)
        next_event = {32'd0, bridge_wr_data_wire, bridge_addr, 4'd3, elapsed};
    else if (tx_event)
        next_event = {tx_current_address, tx_data_wire, tx_address,
                      (tx_paired ? 4'd2 : 4'd4), elapsed};
    else
        next_event = {64'd0, bridge_addr, 4'd1, elapsed};
end
always @(posedge clk_bridge) begin
    freeze_s1 <= freeze_request;
    freeze_s2 <= freeze_s1;
    arm_prev <= arm;
    if (arm && !arm_prev && !armed && !freeze_s2) begin
        armed <= 1;
        elapsed <= 0;
        used <= 1;
        events[0] <= {96'd0, 4'd15, 28'd0};
        rd_seen <= 0;
        tx_seen <= 0;
    end else if (freeze_s2) begin
        frozen <= 1;
    end else if (armed && !frozen) begin
        elapsed <= elapsed + 1'b1;
        if (bridge_rd && bridge_addr[31:24] == 8'hf8) begin
            rd_seen <= 1;
            last_rd_addr <= bridge_addr;
        end
        if (tx_relevant) begin
            tx_seen <= 1;
            last_tx_addr <= tx_address;
            last_tx_data <= tx_data_wire;
        end
        if ((wr_event && (tx_event || rd_event)) || (tx_event && rd_event))
            collision <= 1;
        if (wr_event || tx_event || rd_event) begin
            if (used < DEPTH) begin
                events[used[ADDR_BITS-1:0]] <= next_event;
                used <= used + 1'b1;
            end else begin
                full <= 1;
            end
        end
    end
end

// Once frozen, the write port and metadata are immutable. Synchronize ACK
// and then allow another two CPU clocks before publishing the snapshot.
(* async_reg = "true" *) reg [3:0] frozen_sync = 0;
reg [ADDR_BITS:0] used_s1 = 0, used_s2 = 0;
reg [2:0] flags_s1 = 0, flags_s2 = 0;
reg [ADDR_BITS+2:0] read_word = 0;
reg [127:0] read_entry;
wire ready = frozen_sync[3];
wire [ADDR_BITS+2:0] total_words = {used_s2, 2'b00};
always @(posedge clk_cpu) begin
    read_entry <= events[read_word[ADDR_BITS+1:2]];
    if (reset_cpu) begin
        freeze_request <= 0;
        frozen_sync <= 0;
        read_word <= 0;
        used_s1 <= 0;
        used_s2 <= 0;
        flags_s1 <= 0;
        flags_s2 <= 0;
    end else begin
        if (freeze_read) freeze_request <= 1;
        frozen_sync <= {frozen_sync[2:0], frozen};
        used_s1 <= used;
        used_s2 <= used_s1;
        flags_s1 <= {armed, collision, full};
        flags_s2 <= flags_s1;
        if (data_pop && ready && read_word < total_words)
            read_word <= read_word + 1'b1;
    end
end
assign status_cpu = {31'd0, ready};
assign count_cpu = ready ? ({13'd0, flags_s2, 16'd0} | used_s2) : 32'd0;
assign data_cpu = (!ready || read_word >= total_words) ? 32'd0 :
    read_word[1:0] == 0 ? read_entry[31:0] :
    read_word[1:0] == 1 ? read_entry[63:32] :
    read_word[1:0] == 2 ? read_entry[95:64] : read_entry[127:96];
endmodule
