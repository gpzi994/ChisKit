`default_nettype none

// Pocket framework adapter for the safety skeleton. Cartridge authorization is
// deliberately aborted in this revision: the wrapper can exercise Host bridge
// handling, but cannot leave the locked cartridge state or start the SD test.
module core_top (
    input  wire         clk_74a,
    input  wire         clk_74b,

    inout  wire [7:0]   cart_tran_bank2,
    output wire         cart_tran_bank2_dir,
    inout  wire [7:0]   cart_tran_bank3,
    output wire         cart_tran_bank3_dir,
    inout  wire [7:0]   cart_tran_bank1,
    output wire         cart_tran_bank1_dir,
    inout  wire [7:4]   cart_tran_bank0,
    output wire         cart_tran_bank0_dir,
    inout  wire         cart_tran_pin30,
    output wire         cart_tran_pin30_dir,
    output wire         cart_pin30_pwroff_reset,
    inout  wire         cart_tran_pin31,
    output wire         cart_tran_pin31_dir,

    input  wire         port_ir_rx,
    output wire         port_ir_tx,
    output wire         port_ir_rx_disable,
    inout  wire         port_tran_si,
    output wire         port_tran_si_dir,
    inout  wire         port_tran_so,
    output wire         port_tran_so_dir,
    inout  wire         port_tran_sck,
    output wire         port_tran_sck_dir,
    inout  wire         port_tran_sd,
    output wire         port_tran_sd_dir,

    output wire [21:16] cram0_a,
    inout  wire [15:0]  cram0_dq,
    input  wire         cram0_wait,
    output wire         cram0_clk,
    output wire         cram0_adv_n,
    output wire         cram0_cre,
    output wire         cram0_ce0_n,
    output wire         cram0_ce1_n,
    output wire         cram0_oe_n,
    output wire         cram0_we_n,
    output wire         cram0_ub_n,
    output wire         cram0_lb_n,

    output wire [21:16] cram1_a,
    inout  wire [15:0]  cram1_dq,
    input  wire         cram1_wait,
    output wire         cram1_clk,
    output wire         cram1_adv_n,
    output wire         cram1_cre,
    output wire         cram1_ce0_n,
    output wire         cram1_ce1_n,
    output wire         cram1_oe_n,
    output wire         cram1_we_n,
    output wire         cram1_ub_n,
    output wire         cram1_lb_n,

    output wire [12:0]  dram_a,
    output wire [1:0]   dram_ba,
    inout  wire [15:0]  dram_dq,
    output wire [1:0]   dram_dqm,
    output wire         dram_clk,
    output wire         dram_cke,
    output wire         dram_ras_n,
    output wire         dram_cas_n,
    output wire         dram_we_n,

    output wire [16:0]  sram_a,
    inout  wire [15:0]  sram_dq,
    output wire         sram_oe_n,
    output wire         sram_we_n,
    output wire         sram_ub_n,
    output wire         sram_lb_n,

    input  wire         vblank,
    output wire         dbg_tx,
    input  wire         dbg_rx,
    output wire         user1,
    input  wire         user2,
    inout  wire         aux_sda,
    output wire         aux_scl,
    output wire         vpll_feed,

    output wire [23:0]  video_rgb,
    output wire         video_rgb_clock,
    output wire         video_rgb_clock_90,
    output wire         video_de,
    output wire         video_skip,
    output wire         video_vs,
    output wire         video_hs,
    output wire         audio_mclk,
    input  wire         audio_adc,
    output wire         audio_dac,
    output wire         audio_lrck,

    output wire         bridge_endian_little,
    input  wire [31:0]  bridge_addr,
    input  wire         bridge_rd,
    output wire [31:0]  bridge_rd_data,
    input  wire         bridge_wr,
    input  wire [31:0]  bridge_wr_data,

    input  wire [31:0]  cont1_key,
    input  wire [31:0]  cont2_key,
    input  wire [31:0]  cont3_key,
    input  wire [31:0]  cont4_key,
    input  wire [31:0]  cont1_joy,
    input  wire [31:0]  cont2_joy,
    input  wire [31:0]  cont3_joy,
    input  wire [31:0]  cont4_joy,
    input  wire [15:0]  cont1_trig,
    input  wire [15:0]  cont2_trig,
    input  wire [15:0]  cont3_trig,
    input  wire [15:0]  cont4_trig
);

    logic [3:0] power_on_count = 4'h0;
    wire por_n = &power_on_count;

    always_ff @(posedge clk_74a) begin
        if (!por_n) begin
            power_on_count <= power_on_count + 1'b1;
        end
    end

    wire        bridge_claimed;
    wire        core_reset_n;
    wire [2:0]  cart_guard_state;
    wire        cart_read_ready;
    wire        cart_writes_enabled;
    wire        storage_start_blocked;
    wire        storage_busy;
    wire        storage_done;
    wire        storage_success;
    wire [15:0] storage_error_code;
    wire [31:0] storage_bytes_committed;
    wire [7:0]  cart_bank3_out;
    wire        cart_bank3_dir;
    wire [7:0]  cart_bank2_out;
    wire        cart_bank2_dir;
    wire [7:0]  cart_bank1_out;
    wire        cart_bank1_dir;
    wire [3:0]  cart_bank0_out;
    wire        cart_bank0_dir;
    wire        cart_pin30_out;
    wire        cart_pin30_dir;
    wire        cart_pin31_out;
    wire        cart_pin31_dir;

    assign bridge_endian_little = 1'b0;

    pct_core_skeleton core (
        .clk                       (clk_74a),
        .por_n                     (por_n),
        .bridge_endian_little      (bridge_endian_little),
        .bridge_addr               (bridge_addr),
        .bridge_rd                 (bridge_rd),
        .bridge_wr                 (bridge_wr),
        .bridge_wr_data            (bridge_wr_data),
        .bridge_claimed            (bridge_claimed),
        .bridge_rd_data            (bridge_rd_data),
        .status_boot_done          (por_n),
        .status_setup_done         (por_n),
        .protocol_agb              (1'b1),
        .cart_abort                (1'b1),
        .storage_test_start        (1'b0),
        .storage_test_cancel       (1'b1),
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

    assign cart_tran_bank3     = cart_bank3_dir ? cart_bank3_out : 8'hZZ;
    assign cart_tran_bank3_dir = cart_bank3_dir;
    assign cart_tran_bank2     = cart_bank2_dir ? cart_bank2_out : 8'hZZ;
    assign cart_tran_bank2_dir = cart_bank2_dir;
    assign cart_tran_bank1     = cart_bank1_dir ? cart_bank1_out : 8'hZZ;
    assign cart_tran_bank1_dir = cart_bank1_dir;
    assign cart_tran_bank0     = cart_bank0_dir ? cart_bank0_out : 4'hZ;
    assign cart_tran_bank0_dir = cart_bank0_dir;
    assign cart_tran_pin30     = cart_pin30_dir ? cart_pin30_out : 1'bZ;
    assign cart_tran_pin30_dir = cart_pin30_dir;
    assign cart_tran_pin31     = cart_pin31_dir ? cart_pin31_out : 1'bZ;
    assign cart_tran_pin31_dir = cart_pin31_dir;

    assign port_ir_tx         = 1'b0;
    assign port_ir_rx_disable = 1'b1;
    assign port_tran_si       = 1'bZ;
    assign port_tran_si_dir   = 1'b0;
    assign port_tran_so       = 1'bZ;
    assign port_tran_so_dir   = 1'b0;
    assign port_tran_sck      = 1'bZ;
    assign port_tran_sck_dir  = 1'b0;
    assign port_tran_sd       = 1'bZ;
    assign port_tran_sd_dir   = 1'b0;

    assign cram0_a     = '0;
    assign cram0_dq    = 16'hZZZZ;
    assign cram0_clk   = 1'b0;
    assign cram0_adv_n = 1'b1;
    assign cram0_cre   = 1'b0;
    assign cram0_ce0_n = 1'b1;
    assign cram0_ce1_n = 1'b1;
    assign cram0_oe_n  = 1'b1;
    assign cram0_we_n  = 1'b1;
    assign cram0_ub_n  = 1'b1;
    assign cram0_lb_n  = 1'b1;

    assign cram1_a     = '0;
    assign cram1_dq    = 16'hZZZZ;
    assign cram1_clk   = 1'b0;
    assign cram1_adv_n = 1'b1;
    assign cram1_cre   = 1'b0;
    assign cram1_ce0_n = 1'b1;
    assign cram1_ce1_n = 1'b1;
    assign cram1_oe_n  = 1'b1;
    assign cram1_we_n  = 1'b1;
    assign cram1_ub_n  = 1'b1;
    assign cram1_lb_n  = 1'b1;

    assign dram_a     = '0;
    assign dram_ba    = '0;
    assign dram_dq    = 16'hZZZZ;
    assign dram_dqm   = '0;
    assign dram_clk   = 1'b0;
    assign dram_cke   = 1'b0;
    assign dram_ras_n = 1'b1;
    assign dram_cas_n = 1'b1;
    assign dram_we_n  = 1'b1;

    assign sram_a    = '0;
    assign sram_dq   = 16'hZZZZ;
    assign sram_oe_n = 1'b1;
    assign sram_we_n = 1'b1;
    assign sram_ub_n = 1'b1;
    assign sram_lb_n = 1'b1;

    assign video_rgb          = 24'h000000;
    assign video_rgb_clock    = clk_74a;
    assign video_rgb_clock_90 = clk_74a;
    assign video_de           = 1'b0;
    assign video_skip         = 1'b0;
    assign video_vs           = 1'b0;
    assign video_hs           = 1'b0;
    assign audio_mclk         = 1'b0;
    assign audio_dac          = 1'b0;
    assign audio_lrck         = 1'b0;

    assign dbg_tx    = 1'bZ;
    assign user1     = 1'bZ;
    assign aux_sda   = 1'bZ;
    assign aux_scl   = 1'bZ;
    assign vpll_feed = 1'bZ;

endmodule

`default_nettype wire
