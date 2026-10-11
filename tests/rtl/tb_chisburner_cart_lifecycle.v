`timescale 1ns/1ps
`default_nettype none

module tb_chisburner_cart_lifecycle;
reg clk = 1'b0;
reg por_n = 1'b0;
reg host_b1_strobe = 1'b0;
reg [31:0] host_b1_value = 32'd0;
reg host_b1_valid = 1'b0;
reg host_reset_enter_strobe = 1'b0;
reg host_reset_exit_strobe = 1'b0;
reg host_reset_exited = 1'b0;
reg abort = 1'b0;
wire [2:0] state;
wire play_selected;
wire power_after_reset_exit;
wire [7:0] adapter_id;
wire read_ready;
wire [31:0] status_word;

always #5 clk = ~clk;

chisburner_cart_lifecycle #(
    .SETTLE_CYCLES(4)
) dut (
    .clk(clk),
    .por_n(por_n),
    .host_b1_strobe(host_b1_strobe),
    .host_b1_value(host_b1_value),
    .host_b1_valid(host_b1_valid),
    .host_reset_enter_strobe(host_reset_enter_strobe),
    .host_reset_exit_strobe(host_reset_exit_strobe),
    .host_reset_exited(host_reset_exited),
    .abort(abort),
    .state(state),
    .play_selected(play_selected),
    .power_after_reset_exit(power_after_reset_exit),
    .adapter_id(adapter_id),
    .read_ready(read_ready),
    .status_word(status_word)
);

task pulse_b1;
    input [31:0] value;
    begin
        @(negedge clk);
        host_b1_value = value;
        host_b1_valid = 1'b1;
        host_b1_strobe = 1'b1;
        @(negedge clk);
        host_b1_strobe = 1'b0;
    end
endtask

task pulse_reset_exit;
    begin
        @(negedge clk);
        host_reset_exited = 1'b1;
        host_reset_exit_strobe = 1'b1;
        @(negedge clk);
        host_reset_exit_strobe = 1'b0;
    end
endtask

initial begin
    repeat (3) @(posedge clk);
    por_n = 1'b1;
    @(posedge clk);
    #1;
    if (state != 3'd0 || status_word != 32'd0)
        $fatal(1, "guard did not start locked");

    pulse_b1(32'h0100005A);
    #1;
    if (state != 3'd0 || !play_selected || power_after_reset_exit ||
        adapter_id != 8'h5A || read_ready)
        $fatal(1, "invalid lifecycle was not held locked");

    pulse_b1(32'h010100A5);
    #1;
    if (state != 3'd1 || !play_selected || !power_after_reset_exit ||
        adapter_id != 8'hA5 || read_ready)
        $fatal(1, "valid B1 was not latched");

    pulse_reset_exit();
    repeat (5) @(posedge clk);
    #1;
    if (state != 3'd3 || !read_ready || !status_word[31] ||
        status_word[23:16] != 8'hA5 || !status_word[9] ||
        !status_word[8])
        $fatal(1, "settle path did not reach ready");

    @(negedge clk);
    abort = 1'b1;
    @(negedge clk);
    abort = 1'b0;
    @(posedge clk);
    #1;
    if (state != 3'd0 || status_word != 32'd0)
        $fatal(1, "abort did not relock and clear lifecycle");

    repeat (3) @(posedge clk);
    #1;
    if (state != 3'd0 || status_word != 32'd0)
        $fatal(1, "abort reused stale B1 level");

    @(negedge clk);
    host_reset_enter_strobe = 1'b1;
    host_reset_exited = 1'b0;
    host_b1_valid = 1'b0;
    @(negedge clk);
    host_reset_enter_strobe = 1'b0;

    // Persistent-level recovery: model both event toggles being missed.
    @(negedge clk);
    host_b1_value = 32'h0101003C;
    host_b1_valid = 1'b1;
    repeat (2) @(posedge clk);
    host_reset_exited = 1'b1;
    repeat (6) @(posedge clk);
    #1;
    if (state != 3'd3 || !read_ready || adapter_id != 8'h3C)
        $fatal(1, "persistent B1/reset levels did not recover missed events");

    $display("PASS tb_chisburner_cart_lifecycle");
    $finish;
end

endmodule

`default_nettype wire
