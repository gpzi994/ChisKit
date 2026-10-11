`timescale 1ns/1ps
`default_nettype none

module tb_pct_cart_guard;
    logic clk = 1'b0;
    logic por_n = 1'b0;
    logic host_b1_strobe = 1'b0;
    logic [31:0] host_b1_value = 32'h00000000;
    logic host_reset_enter_strobe = 1'b0;
    logic host_reset_exit_strobe = 1'b0;
    logic abort = 1'b0;
    logic protocol_agb = 1'b1;

    logic [2:0] state;
    logic play_selected;
    logic power_after_reset_exit;
    logic [7:0] adapter_id;
    logic read_ready;
    logic force_safe;
    logic writes_enabled;
    logic [7:0] bank3_out;
    logic bank3_dir;
    logic [7:0] bank2_out;
    logic bank2_dir;
    logic [7:0] bank1_out;
    logic bank1_dir;
    logic [3:0] bank0_out;
    logic bank0_dir;
    logic pin30_out;
    logic pin30_dir;
    logic pin30_pwroff_reset;
    logic pin31_out;
    logic pin31_dir;

    always #5 clk = ~clk;

    pct_cart_guard #(
        .SETTLE_CYCLES (4)
    ) dut (
        .clk                     (clk),
        .por_n                   (por_n),
        .host_b1_strobe          (host_b1_strobe),
        .host_b1_value           (host_b1_value),
        .host_reset_enter_strobe (host_reset_enter_strobe),
        .host_reset_exit_strobe  (host_reset_exit_strobe),
        .abort                   (abort),
        .state                   (state),
        .play_selected           (play_selected),
        .power_after_reset_exit  (power_after_reset_exit),
        .adapter_id              (adapter_id),
        .read_ready              (read_ready),
        .force_safe              (force_safe),
        .writes_enabled          (writes_enabled)
    );

    pct_cart_safe_pins safe_pins (
        .read_ready          (read_ready),
        .protocol_agb        (protocol_agb),
        .bank3_out           (bank3_out),
        .bank3_dir           (bank3_dir),
        .bank2_out           (bank2_out),
        .bank2_dir           (bank2_dir),
        .bank1_out           (bank1_out),
        .bank1_dir           (bank1_dir),
        .bank0_out           (bank0_out),
        .bank0_dir           (bank0_dir),
        .pin30_out           (pin30_out),
        .pin30_dir           (pin30_dir),
        .pin30_pwroff_reset  (pin30_pwroff_reset),
        .pin31_out           (pin31_out),
        .pin31_dir           (pin31_dir)
    );

    task automatic pulse_b1(input logic [31:0] value);
        begin
            @(negedge clk);
            host_b1_value  = value;
            host_b1_strobe = 1'b1;
            @(negedge clk);
            host_b1_strobe = 1'b0;
        end
    endtask

    task automatic pulse_reset_exit;
        begin
            @(negedge clk);
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

        if (state != 3'd0 || read_ready || !force_safe) begin
            $fatal(1, "guard did not start locked");
        end
        if (writes_enabled) $fatal(1, "cartridge writes must be permanently disabled");
        if (bank3_dir || bank2_dir || bank1_dir) $fatal(1, "wide banks must start as inputs");
        if (!bank0_dir || bank0_out != 4'hF) $fatal(1, "bank0 inactive controls are unsafe");

        pulse_b1(32'h01000001);
        #1;
        if (state != 3'd0 || read_ready) begin
            $fatal(1, "Play Cartridge without bit16 must remain locked");
        end

        pulse_b1(32'h010100A5);
        #1;
        if (state != 3'd1 || !play_selected || !power_after_reset_exit || adapter_id != 8'hA5) begin
            $fatal(1, "valid B1 lifecycle was not latched");
        end
        if (read_ready || writes_enabled) $fatal(1, "B1 alone must not authorize a transaction");

        pulse_reset_exit();
        #1;
        if (state != 3'd2) $fatal(1, "reset exit did not start settling");
        repeat (4) @(posedge clk);
        #1;
        if (!read_ready || state != 3'd3) $fatal(1, "settle timer did not authorize reads");
        if (!pin30_dir || !pin30_out) $fatal(1, "GBA CS2# must be actively held inactive");
        if (writes_enabled) $fatal(1, "write gate changed during read authorization");

        @(negedge clk);
        abort = 1'b1;
        @(posedge clk);
        #1;
        if (state != 3'd0 || read_ready || !force_safe) $fatal(1, "abort did not relock the guard");
        if (bank3_dir || bank2_dir || bank1_dir) $fatal(1, "abort did not release wide banks");
        if (!bank0_dir || bank0_out != 4'hF) $fatal(1, "abort did not restore inactive controls");

        $display("PASS tb_pct_cart_guard");
        $finish;
    end
endmodule

`default_nettype wire
