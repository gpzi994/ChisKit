`timescale 1ns/1ps
`default_nettype none

module tb_core_top_safe;
    logic clk_74a = 1'b0;
    logic clk_74b = 1'b0;
    wire [7:0] cart_tran_bank2;
    wire cart_tran_bank2_dir;
    wire [7:0] cart_tran_bank3;
    wire cart_tran_bank3_dir;
    wire [7:0] cart_tran_bank1;
    wire cart_tran_bank1_dir;
    wire [7:4] cart_tran_bank0;
    wire cart_tran_bank0_dir;
    wire cart_tran_pin30;
    wire cart_tran_pin30_dir;
    wire cart_pin30_pwroff_reset;
    wire cart_tran_pin31;
    wire cart_tran_pin31_dir;
    logic [31:0] bridge_addr = 32'h00000000;
    logic bridge_rd = 1'b0;
    logic bridge_wr = 1'b0;
    logic [31:0] bridge_wr_data = 32'h00000000;
    wire [31:0] bridge_rd_data;
    wire bridge_endian_little;

    always #5 clk_74a = ~clk_74a;
    always #7 clk_74b = ~clk_74b;

    core_top dut (
        .clk_74a                 (clk_74a),
        .clk_74b                 (clk_74b),
        .cart_tran_bank2         (cart_tran_bank2),
        .cart_tran_bank2_dir     (cart_tran_bank2_dir),
        .cart_tran_bank3         (cart_tran_bank3),
        .cart_tran_bank3_dir     (cart_tran_bank3_dir),
        .cart_tran_bank1         (cart_tran_bank1),
        .cart_tran_bank1_dir     (cart_tran_bank1_dir),
        .cart_tran_bank0         (cart_tran_bank0),
        .cart_tran_bank0_dir     (cart_tran_bank0_dir),
        .cart_tran_pin30         (cart_tran_pin30),
        .cart_tran_pin30_dir     (cart_tran_pin30_dir),
        .cart_pin30_pwroff_reset (cart_pin30_pwroff_reset),
        .cart_tran_pin31         (cart_tran_pin31),
        .cart_tran_pin31_dir     (cart_tran_pin31_dir),
        .port_ir_rx              (1'b0),
        .port_ir_tx              (),
        .port_ir_rx_disable      (),
        .port_tran_si            (),
        .port_tran_si_dir        (),
        .port_tran_so            (),
        .port_tran_so_dir        (),
        .port_tran_sck           (),
        .port_tran_sck_dir       (),
        .port_tran_sd            (),
        .port_tran_sd_dir        (),
        .cram0_a                 (),
        .cram0_dq                (),
        .cram0_wait              (1'b0),
        .cram0_clk               (),
        .cram0_adv_n             (),
        .cram0_cre               (),
        .cram0_ce0_n             (),
        .cram0_ce1_n             (),
        .cram0_oe_n              (),
        .cram0_we_n              (),
        .cram0_ub_n              (),
        .cram0_lb_n              (),
        .cram1_a                 (),
        .cram1_dq                (),
        .cram1_wait              (1'b0),
        .cram1_clk               (),
        .cram1_adv_n             (),
        .cram1_cre               (),
        .cram1_ce0_n             (),
        .cram1_ce1_n             (),
        .cram1_oe_n              (),
        .cram1_we_n              (),
        .cram1_ub_n              (),
        .cram1_lb_n              (),
        .dram_a                  (),
        .dram_ba                 (),
        .dram_dq                 (),
        .dram_dqm                (),
        .dram_clk                (),
        .dram_cke                (),
        .dram_ras_n              (),
        .dram_cas_n              (),
        .dram_we_n               (),
        .sram_a                  (),
        .sram_dq                 (),
        .sram_oe_n               (),
        .sram_we_n               (),
        .sram_ub_n               (),
        .sram_lb_n               (),
        .vblank                  (1'b0),
        .dbg_tx                  (),
        .dbg_rx                  (1'b0),
        .user1                   (),
        .user2                   (1'b0),
        .aux_sda                 (),
        .aux_scl                 (),
        .vpll_feed               (),
        .video_rgb               (),
        .video_rgb_clock         (),
        .video_rgb_clock_90      (),
        .video_de                (),
        .video_skip              (),
        .video_vs                (),
        .video_hs                (),
        .audio_mclk              (),
        .audio_adc               (1'b0),
        .audio_dac               (),
        .audio_lrck              (),
        .bridge_endian_little    (bridge_endian_little),
        .bridge_addr             (bridge_addr),
        .bridge_rd               (bridge_rd),
        .bridge_rd_data          (bridge_rd_data),
        .bridge_wr               (bridge_wr),
        .bridge_wr_data          (bridge_wr_data),
        .cont1_key               (32'h00000000),
        .cont2_key               (32'h00000000),
        .cont3_key               (32'h00000000),
        .cont4_key               (32'h00000000),
        .cont1_joy               (32'h00000000),
        .cont2_joy               (32'h00000000),
        .cont3_joy               (32'h00000000),
        .cont4_joy               (32'h00000000),
        .cont1_trig              (16'h0000),
        .cont2_trig              (16'h0000),
        .cont3_trig              (16'h0000),
        .cont4_trig              (16'h0000)
    );

    initial begin
        repeat (20) @(posedge clk_74a);
        #1;
        if (bridge_endian_little !== 1'b0) $fatal(1, "unexpected bridge endianness");
        if (cart_tran_bank3_dir || cart_tran_bank2_dir || cart_tran_bank1_dir) begin
            $fatal(1, "framework adapter drove a wide cartridge bank");
        end
        if (cart_tran_bank3 !== 8'hZZ || cart_tran_bank2 !== 8'hZZ ||
            cart_tran_bank1 !== 8'hZZ) begin
            $fatal(1, "framework adapter did not tri-state wide cartridge banks");
        end
        if (!cart_tran_bank0_dir || cart_tran_bank0 !== 4'hF) begin
            $fatal(1, "framework adapter did not hold inactive controls high");
        end
        if (cart_tran_pin30_dir || cart_tran_pin30 !== 1'bZ ||
            cart_pin30_pwroff_reset) begin
            $fatal(1, "pin30 did not remain under safe hardware control");
        end
        if (cart_tran_pin31_dir || cart_tran_pin31 !== 1'bZ) begin
            $fatal(1, "pin31 did not remain an input");
        end
        if (dut.cart_read_ready || dut.cart_writes_enabled) begin
            $fatal(1, "framework adapter escaped the locked cartridge state");
        end

        $display("PASS tb_core_top_safe");
        $finish;
    end
endmodule

`default_nettype wire
