`timescale 1ns/1ps
`default_nettype none

module tb_pct_core_storage;
    logic clk = 1'b0;
    logic por_n = 1'b0;
    logic bridge_endian_little = 1'b0;
    logic [31:0] bridge_addr = 32'h00000000;
    logic bridge_rd = 1'b0;
    logic bridge_wr = 1'b0;
    logic [31:0] bridge_wr_data = 32'h00000000;
    logic bridge_claimed;
    logic [31:0] bridge_rd_data;
    logic status_boot_done = 1'b1;
    logic status_setup_done = 1'b1;
    logic protocol_agb = 1'b1;
    logic cart_abort = 1'b0;
    logic storage_test_start = 1'b0;
    logic storage_test_cancel = 1'b0;

    logic core_reset_n;
    logic [2:0] cart_guard_state;
    logic cart_read_ready;
    logic cart_writes_enabled;
    logic storage_start_blocked;
    logic storage_busy;
    logic storage_done;
    logic storage_success;
    logic [15:0] storage_error_code;
    logic [31:0] storage_bytes_committed;
    logic [7:0] cart_bank3_out;
    logic cart_bank3_dir;
    logic [7:0] cart_bank2_out;
    logic cart_bank2_dir;
    logic [7:0] cart_bank1_out;
    logic cart_bank1_dir;
    logic [3:0] cart_bank0_out;
    logic cart_bank0_dir;
    logic cart_pin30_out;
    logic cart_pin30_dir;
    logic cart_pin30_pwroff_reset;
    logic cart_pin31_out;
    logic cart_pin31_dir;

    integer write_index;
    logic [31:0] rd_value;

    always #5 clk = ~clk;

    pct_core_skeleton #(
        .CART_SETTLE_CYCLES     (4),
        .STORAGE_PAYLOAD_BYTES  (20),
        .STORAGE_CHUNK_BYTES    (8),
        .STORAGE_TIMEOUT_CYCLES (20)
    ) dut (
        .clk                       (clk),
        .por_n                     (por_n),
        .bridge_endian_little      (bridge_endian_little),
        .bridge_addr               (bridge_addr),
        .bridge_rd                 (bridge_rd),
        .bridge_wr                 (bridge_wr),
        .bridge_wr_data            (bridge_wr_data),
        .bridge_claimed            (bridge_claimed),
        .bridge_rd_data            (bridge_rd_data),
        .status_boot_done          (status_boot_done),
        .status_setup_done         (status_setup_done),
        .protocol_agb              (protocol_agb),
        .cart_abort                (cart_abort),
        .storage_test_start        (storage_test_start),
        .storage_test_cancel       (storage_test_cancel),
        .core_reset_n              (core_reset_n),
        .cart_guard_state          (cart_guard_state),
        .cart_read_ready           (cart_read_ready),
        .cart_writes_enabled       (cart_writes_enabled),
        .storage_start_blocked     (storage_start_blocked),
        .storage_busy              (storage_busy),
        .storage_done              (storage_done),
        .storage_success           (storage_success),
        .storage_error_code        (storage_error_code),
        .storage_bytes_committed   (storage_bytes_committed),
        .cart_bank3_out            (cart_bank3_out),
        .cart_bank3_dir            (cart_bank3_dir),
        .cart_bank2_out            (cart_bank2_out),
        .cart_bank2_dir            (cart_bank2_dir),
        .cart_bank1_out            (cart_bank1_out),
        .cart_bank1_dir            (cart_bank1_dir),
        .cart_bank0_out            (cart_bank0_out),
        .cart_bank0_dir            (cart_bank0_dir),
        .cart_pin30_out            (cart_pin30_out),
        .cart_pin30_dir            (cart_pin30_dir),
        .cart_pin30_pwroff_reset   (cart_pin30_pwroff_reset),
        .cart_pin31_out            (cart_pin31_out),
        .cart_pin31_dir            (cart_pin31_dir)
    );

    task automatic bridge_read(input logic [31:0] address, output logic [31:0] value);
        begin
            @(negedge clk);
            bridge_addr = address;
            bridge_rd   = 1'b1;
            #1;
            if (!bridge_claimed) $fatal(1, "bridge read was not claimed at %h", address);
            value = bridge_rd_data;
            @(negedge clk);
            bridge_rd = 1'b0;
        end
    endtask

    task automatic bridge_write(input logic [31:0] address, input logic [31:0] value);
        begin
            @(negedge clk);
            bridge_addr    = address;
            bridge_wr_data = value;
            bridge_wr      = 1'b1;
            @(negedge clk);
            bridge_wr      = 1'b0;
        end
    endtask

    task automatic pulse_storage_start;
        begin
            @(negedge clk);
            storage_test_start = 1'b1;
            @(negedge clk);
            storage_test_start = 1'b0;
        end
    endtask

    task automatic wait_command(input logic [15:0] code);
        integer attempts;
        logic [31:0] status;
        begin
            status = 32'h00000000;
            for (attempts = 0; attempts < 100; attempts = attempts + 1) begin
                bridge_read(32'hF8001000, status);
                if (status == {16'h636D, code}) attempts = 100;
            end
            if (status != {16'h636D, code}) begin
                $fatal(1, "target command %h was not issued; status=%h", code, status);
            end
        end
    endtask

    task automatic complete_command(input logic [15:0] result_code);
        begin
            bridge_write(32'hF8001000, {16'h6F6B, result_code});
        end
    endtask

    task automatic host_command(input logic [15:0] code, input logic [31:0] param0);
        begin
            bridge_write(32'hF8000020, param0);
            bridge_write(32'hF8000000, {16'h434D, code});
            repeat (4) @(posedge clk);
        end
    endtask

    initial begin
        repeat (3) @(posedge clk);
        por_n = 1'b1;
        repeat (2) @(posedge clk);

        if (cart_writes_enabled) $fatal(1, "cartridge write gate is not hard-disabled");
        if (cart_bank3_dir || cart_bank2_dir || cart_bank1_dir) begin
            $fatal(1, "wide cartridge banks are driven during no-card storage test");
        end
        if (!cart_bank0_dir || cart_bank0_out != 4'hF) begin
            $fatal(1, "inactive cartridge controls are not held high");
        end

        bridge_read(32'hF8341004, rd_value);
        if (rd_value != 32'h00000020) $fatal(1, "F8xx10xx target decoding is too narrow");
        bridge_read(32'hF8120004, rd_value);
        if (rd_value != 32'h00000020) $fatal(1, "F8xx00xx host decoding is too narrow");

        pulse_storage_start();
        wait_command(16'h0192);
        bridge_read(32'hF8001020, rd_value);
        if (rd_value != 32'h00000100) $fatal(1, "wrong storage slot id");
        bridge_read(32'hF8001024, rd_value);
        if (rd_value != 32'hF8004000) $fatal(1, "wrong open-parameter pointer");
        bridge_read(32'hF8004000, rd_value);
        if (rd_value != 32'h7661532F) $fatal(1, "open path prefix is wrong: %h", rd_value);
        bridge_read(32'hF8004100, rd_value);
        if (rd_value != 32'h00000003) $fatal(1, "create/truncate flags are wrong");
        bridge_read(32'hF8004104, rd_value);
        if (rd_value != 32'd20) $fatal(1, "desired file size is wrong");
        complete_command(16'h0001);

        for (write_index = 0; write_index < 3; write_index = write_index + 1) begin
            wait_command(16'h0184);
            bridge_read(32'hF8001020, rd_value);
            if (rd_value != 32'h00000100) $fatal(1, "wrong slot id on write");
            bridge_read(32'hF8001024, rd_value);
            if (rd_value != write_index * 8) $fatal(1, "wrong write offset: %h", rd_value);
            bridge_read(32'hF8001028, rd_value);
            if (rd_value != (32'hF8004200 + write_index * 8)) begin
                $fatal(1, "wrong payload pointer: %h", rd_value);
            end
            bridge_read(32'hF800102C, rd_value);
            if ((write_index < 2 && rd_value != 8) || (write_index == 2 && rd_value != 4)) begin
                $fatal(1, "wrong chunk length: %h", rd_value);
            end
            if (write_index == 0) begin
                bridge_read(32'hF8004200, rd_value);
                if (rd_value != 32'hEEA55C13) $fatal(1, "payload pattern is wrong: %h", rd_value);
            end
            complete_command(16'h0000);
        end

        wait_command(16'h0188);
        bridge_read(32'hF8001020, rd_value);
        if (rd_value != 32'h00000100) $fatal(1, "wrong slot id on flush");
        complete_command(16'h0000);
        wait (storage_done);
        #1;
        if (!storage_success || storage_error_code != 16'h0000 || storage_bytes_committed != 20) begin
            $fatal(1, "storage sequence did not complete cleanly");
        end

        // A deliberately unanswered command must time out and release the
        // target port so the design cannot remain stuck busy.
        pulse_storage_start();
        wait_command(16'h0192);
        wait (storage_done);
        #1;
        if (storage_success || storage_error_code != 16'hFFFD) begin
            $fatal(1, "storage timeout was not reported");
        end
        repeat (3) @(posedge clk);
        bridge_read(32'hF8001000, rd_value);
        if (rd_value != 32'h00000000) $fatal(1, "target port did not abort after timeout");

        // Once Pocket announces Play Cartridge, the no-card storage test is
        // rejected. This keeps the two bring-up paths mutually exclusive.
        host_command(16'h00B1, 32'h01010000);
        @(negedge clk);
        storage_test_start = 1'b1;
        @(posedge clk);
        #1;
        if (!storage_start_blocked || storage_busy) begin
            $fatal(1, "storage start was not blocked in Play Cartridge mode");
        end
        storage_test_start = 1'b0;

        if (cart_writes_enabled) $fatal(1, "cartridge write gate changed during test");
        if (cart_bank3_dir || cart_bank2_dir || cart_bank1_dir) begin
            $fatal(1, "wide cartridge banks were driven without read authorization");
        end

        $display("PASS tb_pct_core_storage");
        $finish;
    end
endmodule

`default_nettype wire
