//------------------------------------------------------------------------------
// SPDX-License-Identifier: Apache-2.0
// SPDX-FileType: SOURCE
// SPDX-FileCopyrightText: (c) 2026, ThinkElastic <Think@Elastic.com>
//------------------------------------------------------------------------------

//
// Simulation testbench for axi_periph_slave.v — exercises the BRAM
// fetch path (192KB region) and peripheral polling reads that the
// boot stub uses on real HW.  tb_system.v uses `bram_model.v` for
// LOCAL, so `axi_periph_slave` itself has never been tested in
// simulation.
//

`default_nettype none

module tb_axi_periph #(
    parameter INCLUDE_CARTRIDGE = 0
) (
    input wire clk,
    input wire reset_n,

    // AXI slave ports — driven by the C++ harness
    input  wire        s_axi_arvalid,
    output wire        s_axi_arready,
    input  wire [31:0] s_axi_araddr,
    input  wire [7:0]  s_axi_arlen,

    output wire        s_axi_rvalid,
    input  wire        s_axi_rready,
    output wire [31:0] s_axi_rdata,
    output wire [1:0]  s_axi_rresp,
    output wire        s_axi_rlast,

    input  wire        s_axi_awvalid,
    output wire        s_axi_awready,
    input  wire [31:0] s_axi_awaddr,
    input  wire [7:0]  s_axi_awlen,
    input  wire [1:0]  s_axi_awburst,

    input  wire        s_axi_wvalid,
    output wire        s_axi_wready,
    input  wire [31:0] s_axi_wdata,
    input  wire [3:0]  s_axi_wstrb,
    input  wire        s_axi_wlast,

    output wire        s_axi_bvalid,
    input  wire        s_axi_bready,
    output wire [1:0]  s_axi_bresp,

    // Observability / debug
    output wire [3:0]  dbg_state,
    output wire        dbg_bram_hold,
    output wire        dbg_can_push_beat,

    // GPU MMIO outputs exposed so the harness can count pulses vs
    // AXI writes submitted — the gpudemo freeze-after-N-frames
    // investigation needs this to prove / disprove whether the AXI
    // periph slave is dropping MMIO writes on the way to the GPU.
    output wire        dbg_gpu_reg_wr,
    output wire [3:0]  dbg_gpu_reg_addr,
    output wire [31:0] dbg_gpu_reg_wdata,
    output wire [3:0]  dbg_save_dt_slot,
    output wire [31:0] dbg_save_dt_size,
    output wire        dbg_save_dt_commit,
    output wire [2:0]  dbg_analogizer_wr_toggle,
    output wire [31:0] dbg_analogizer_wr_settings,
    output wire [31:0] dbg_analogizer_wr_hoffset,
    output wire [31:0] dbg_analogizer_wr_voffset,
    output wire [7:0]  dbg_snac_pin_out,
    output wire [7:0]  dbg_snac_pin_dir,
    output wire        dbg_snac_enable,
    output reg  [7:0]  dbg_cart_wr_count,
    output reg  [7:0]  dbg_cart_rd_count,
    output reg  [4:0]  dbg_cart_last_addr,
    output reg  [31:0] dbg_cart_last_wdata,
    output wire        dbg_pal_commit,
    // Analog (Analogizer) framebuffer source mux — must mirror the LCD
    // selection (terminal incl.) unless TERM_FB_CTRL bit 1 (analog_keep_app)
    // pins it to the app FB + ANALOG_FB_* geometry.
    output wire [24:0] dbg_analog_fb_addr,
    output wire [2:0]  dbg_analog_color_mode,
    output wire [9:0]  dbg_analog_fb_width,
    output wire [9:0]  dbg_analog_fb_height,
    output wire [15:0] dbg_analog_fb_stride,
    output reg  [7:0]  dbg_pal_commit_count,
    input  wire        pal_busy_in,
    input  wire [31:0] analogizer_settings_in,

    // CMD_FLIP side-port (from gpu_core in production).  Tests can
    // pulse this and read FB_SWAP_CTRL via the AXI slave to verify
    // fb_swap_pending toggles.  vsync is also exposed so tests can
    // synthesize the rising edge that consumes the pending swap.
    input  wire        gpu_swap_req_in,
    input  wire [1:0]  gpu_swap_idx_in,
    input  wire        vsync_in,
    input  wire        early_vblank_in,
    input  wire        os_inmenu_in,
    input  wire        target_dataslot_ack_in,
    input  wire        target_dataslot_done_in,
    input  wire [2:0]  target_dataslot_err_in,
    input  wire        bridge_wr_idle_in,
`ifdef CHISBURNER_STORAGE_TRACE
    input  wire [15:0] storage_trace_in,
`endif
`ifdef CHISBURNER_STORAGE_EVENTS
    input wire [31:0] events_status_in, events_count_in, events_data_in,
    output reg [31:0] events_freezes = 0, events_pops = 0,
`endif
`ifdef CHISBURNER_STORAGE_AUDIT
    output reg [31:0] events_arms = 0,
`endif

    input  wire [31:0] cont1_key_in,
    input  wire [31:0] cont1_joy_in,
    input  wire [15:0] cont1_trig_in,
    input  wire [31:0] cont2_key_in,
    input  wire [31:0] cont2_joy_in,
    input  wire [15:0] cont2_trig_in,
    input  wire [31:0] cont3_key_in,
    input  wire [31:0] cont3_joy_in,
    input  wire [15:0] cont3_trig_in,
    input  wire [31:0] cont4_key_in,
    input  wire [31:0] cont4_joy_in,
    input  wire [15:0] cont4_trig_in,
    input  wire [31:0] dt_query_data_in,
    input  wire        dt_query_valid_in,
    input  wire [7:0]  snac_pin_in_in,
    output wire        ext_irq_out
);

// Tie-offs / harness-driven peripheral inputs.
`ifdef CHISBURNER_STORAGE_EVENTS
wire events_freeze, events_pop;
`ifdef CHISBURNER_STORAGE_AUDIT
wire events_arm;
always @(posedge clk) begin
    if (!reset_n) events_arms <= 0;
    else if (events_arm) events_arms <= events_arms + 1;
end
`endif
always @(posedge clk) begin
    if (!reset_n) begin events_freezes <= 0; events_pops <= 0; end
    else begin
        if (events_freeze) events_freezes <= events_freezes + 1;
        if (events_pop) events_pops <= events_pops + 1;
    end
end
`endif
wire target_dataslot_ack   = target_dataslot_ack_in;
wire target_dataslot_done  = target_dataslot_done_in;
wire [2:0] target_dataslot_err = target_dataslot_err_in;
wire [31:0] link_reg_rdata = 32'b0;
wire [8:0] audio_fifo_level = 9'b0;
wire audio_fifo_full = 1'b0;
wire uart_tx_active = 1'b0;
wire uart_rx_dv = 1'b0;
wire [7:0] uart_rx_byte = 8'b0;
wire shutdown_pending = 1'b0;
wire [31:0] dt_query_data = dt_query_data_in;
wire dt_query_valid = dt_query_valid_in;
wire [7:0] snac_pin_in = snac_pin_in_in;
// Mock GPU register-read mux: returns a unique sentinel per address so
// the harness can verify that each AR returns the data of the requested
// register (catches the "read returns previous read's value" bug from
// the registered gpu_reg_rdata_r boundary that was removed in core_top).
`ifdef EXCLUDE_GPU
wire [31:0] gpu_reg_rdata = 32'b0;
`else
reg  [31:0] gpu_reg_rdata;
always @(*) begin
    case (dbg_gpu_reg_addr)
        4'd1:  gpu_reg_rdata = 32'h1111_1111;
        4'd4:  gpu_reg_rdata = 32'h4444_4444;
        4'd5:  gpu_reg_rdata = 32'h5555_5555;
        4'd6:  gpu_reg_rdata = 32'h6666_6666;
        default: gpu_reg_rdata = 32'h0;
    endcase
end
`endif
wire link_irq = 1'b0;
wire bridge_wr_idle = bridge_wr_idle_in;
wire dataslot_allcomplete = 1'b1;
wire vsync = vsync_in;
wire early_vblank = early_vblank_in;
wire os_inmenu = os_inmenu_in;
wire [31:0] cont1_key = cont1_key_in;
wire [31:0] cont1_joy = cont1_joy_in;
wire [15:0] cont1_trig = cont1_trig_in;
wire [31:0] cont2_key = cont2_key_in;
wire [31:0] cont2_joy = cont2_joy_in;
wire [15:0] cont2_trig = cont2_trig_in;
wire [31:0] cont3_key = cont3_key_in;
wire [31:0] cont3_joy = cont3_joy_in;
wire [15:0] cont3_trig = cont3_trig_in;
wire [31:0] cont4_key = cont4_key_in;
wire [31:0] cont4_joy = cont4_joy_in;
wire [15:0] cont4_trig = cont4_trig_in;
wire [31:0] app_id = 32'b0;
wire pal_commit_w;

wire        cart_reg_wr;
wire        cart_reg_rd;
wire [4:0]  cart_reg_addr;
wire [31:0] cart_reg_wdata;
wire [31:0] cart_reg_rdata;
chisburner_cart_auto98 cart(
 .clk(clk),.reset_n(reset_n),.lifecycle_ok(1'b1),
 .lifecycle_status(32'h80000303),.host_b1_raw(32'h01018000),.host_diag(32'h0100013b),
 .reg_wr(cart_reg_wr),.reg_rd(cart_reg_rd),.reg_addr(cart_reg_addr),
 .reg_wdata(cart_reg_wdata),.reg_rdata(cart_reg_rdata),
 .gb_data_in(8'h5a),.bank3_in(8'h5a),.bank2_in(8'ha5),
 .bank3_out(),.bank3_dir(),.bank2_out(),.bank2_dir(),
 .bank1_out(),.bank1_dir(),.bank0_out(),.bank0_dir(),
 .pin30_out(),.pin30_dir(),.pin30_pwroff_reset(),.pin31_out(),.pin31_dir(),.writes_enabled()
);
always @(posedge clk or negedge reset_n) begin
    if (!reset_n) begin
        dbg_cart_wr_count   <= 8'd0;
        dbg_cart_rd_count   <= 8'd0;
        dbg_cart_last_addr  <= 5'd0;
        dbg_cart_last_wdata <= 32'd0;
    end else begin
        if (cart_reg_wr) begin
            dbg_cart_wr_count   <= dbg_cart_wr_count + 8'd1;
            dbg_cart_last_addr  <= cart_reg_addr;
            dbg_cart_last_wdata <= cart_reg_wdata;
        end
        if (cart_reg_rd)
            dbg_cart_rd_count <= dbg_cart_rd_count + 8'd1;
    end
end

assign dbg_pal_commit = pal_commit_w;

always @(posedge clk or negedge reset_n) begin
    if (!reset_n)
        dbg_pal_commit_count <= 8'd0;
    else if (pal_commit_w)
        dbg_pal_commit_count <= dbg_pal_commit_count + 8'd1;
end

// Model core_top's one-cycle register boundary between audio_mixer's
// combinational position mux and axi_periph_slave's read data input.
wire [4:0]  mix_voice_sel_rd;
reg  [21:0] mix_pos_readback_r;
always @(posedge clk or negedge reset_n) begin
    if (!reset_n)
        mix_pos_readback_r <= 22'd0;
    else
        mix_pos_readback_r <= 22'h100 + {17'd0, mix_voice_sel_rd};
end

// DUT
`ifdef CHISBURNER_SDRAM118
wire sd118_selected,sd118_write;
wire [31:0] sd118_offset,sd118_wdata,sd118_rdata;
wire [3:0] sd118_wstrb;
chisburner_sdram_control116 sd118_control (
 .clk(clk),.reset_n(reset_n),.selected(sd118_selected),.reg_write(sd118_write),
 .reg_offset(sd118_offset),.reg_wdata(sd118_wdata),.reg_wstrb(sd118_wstrb),
 .reg_rdata(sd118_rdata),.transport_ready(1'b1),.transport_complete(1'b0),
 .transport_fault(1'b0),.transport_rejected(1'b0),.ingress_drained(1'b1),
 .ingress_fault(1'b0),.host_quiet(1'b1),.axi_idle(1'b1),
 .prepare(),.release_snapshot(),.source_address(),.byte_count(),.locked(),.idle()
);
`endif
axi_periph_slave #(
    .INCLUDE_CARTRIDGE(INCLUDE_CARTRIDGE)
) dut (
    .clk    (clk),
    .reset_n(reset_n),

    .s_axi_arvalid(s_axi_arvalid), .s_axi_arready(s_axi_arready),
    .s_axi_araddr (s_axi_araddr),  .s_axi_arlen  (s_axi_arlen),
    .s_axi_rvalid (s_axi_rvalid),  .s_axi_rready (s_axi_rready),
    .s_axi_rdata  (s_axi_rdata),   .s_axi_rresp  (s_axi_rresp),
    .s_axi_rlast  (s_axi_rlast),
    .s_axi_awvalid(s_axi_awvalid), .s_axi_awready(s_axi_awready),
    .s_axi_awaddr (s_axi_awaddr),  .s_axi_awlen  (s_axi_awlen),
    .s_axi_awburst(s_axi_awburst),
    .s_axi_wvalid (s_axi_wvalid),  .s_axi_wready (s_axi_wready),
    .s_axi_wdata  (s_axi_wdata),   .s_axi_wstrb  (s_axi_wstrb),
    .s_axi_wlast  (s_axi_wlast),
    .s_axi_bvalid (s_axi_bvalid),  .s_axi_bready (s_axi_bready),
    .s_axi_bresp  (s_axi_bresp),

    .dataslot_allcomplete(dataslot_allcomplete),
    .vsync               (vsync),
    .early_vblank        (early_vblank),
    .os_inmenu           (os_inmenu),
    .cont1_key           (cont1_key),
    .cont1_joy           (cont1_joy),
    .cont1_trig          (cont1_trig),
    .cont2_key           (cont2_key),
    .cont2_joy           (cont2_joy),
    .cont2_trig          (cont2_trig),
    .cont3_key           (cont3_key),
    .cont3_joy           (cont3_joy),
    .cont3_trig          (cont3_trig),
    .cont4_key           (cont4_key),
    .cont4_joy           (cont4_joy),
    .cont4_trig          (cont4_trig),
    .rtc_epoch_seconds   (32'd0),
    .rtc_valid           (1'b0),
    .target_dataslot_ack (target_dataslot_ack),
    .target_dataslot_done(target_dataslot_done),
    .target_dataslot_err (target_dataslot_err),
    // Distinct constants so the REGION_HPS word decode (req_addr[5:2])
    // can be verified by readback — see test_hps_region_decode() in
    // tb_axi_periph_main.cpp.
    .hps_status          (32'h0000_0125),
    .hps_img_size        (64'h1111_2222_3333_4444),
    .hps_boot_len        (32'h000A_BC00),
    .hps_img1_size       (64'h5555_6666_7777_8888),
    .hps_img2_size       (64'h9999_AAAA_BBBB_CCCC),
    .hps_ini_len         (32'hFACE_1234),
    .hps_elf_len         (32'hE1F0_5678),
    .bridge_wr_idle      (bridge_wr_idle),
    .bridge_dbg_wcnt     (32'd0),

    .color_mode     (),
    .fb_display_addr(),
    .fb_width       (),
    .fb_height      (),
    .fb_stride      (),

    .analog_fb_addr   (dbg_analog_fb_addr),
    .analog_color_mode(dbg_analog_color_mode),
    .analog_fb_width  (dbg_analog_fb_width),
    .analog_fb_height (dbg_analog_fb_height),
    .analog_fb_stride (dbg_analog_fb_stride),

    .pal_wr  (), .pal_addr(), .pal_data(), .pal_commit(pal_commit_w),
    .pal_busy(pal_busy_in),

    .target_dataslot_read      (),
    .target_dataslot_write     (),
    .target_dataslot_openfile  (),
    .target_dataslot_getfile   (),
    .target_dataslot_flush     (),
`ifdef CHISBURNER_STORAGE_TRACE
    .storage_trace_74a         (storage_trace_in),
`endif
`ifdef CHISBURNER_STORAGE_EVENTS
    .storage_events_status(events_status_in),
    .storage_events_count(events_count_in),
    .storage_events_data(events_data_in),
    .storage_events_freeze(events_freeze),
    .storage_events_pop(events_pop),
`endif
`ifdef CHISBURNER_STORAGE_AUDIT
    .storage_events_arm(events_arm),
`endif
    .target_dataslot_id        (),
    .target_dataslot_slotoffset(),
    .target_dataslot_bridgeaddr(),
    .target_dataslot_length    (),
    .target_buffer_param_struct(),
    .target_buffer_resp_struct (),

    .audio_fifo_level (audio_fifo_level),
    .audio_fifo_full  (audio_fifo_full),

    .cram0_mode(),
`ifdef CHISBURNER_SDRAM118
    .sd118_selected(sd118_selected),.sd118_write(sd118_write),
    .sd118_offset(sd118_offset),.sd118_wdata(sd118_wdata),
    .sd118_wstrb(sd118_wstrb),.sd118_rdata(sd118_rdata),
`endif

    .mix_enable(),
    .mix_voice_wr(),
    .mix_voice_sel(),
    .mix_voice_field(),
    .mix_voice_wdata(),
    .mix_irq_clear_wr(),
    .mix_irq_clear(),
    .mix_master_vol(),
    .mix_group_vol_0(),
    .mix_group_vol_1(),
    .mix_group_vol_2(),
    .mix_group_vol_3(),
    .mix_voice_group_packed(),
    .mix_voice_sel_rd(mix_voice_sel_rd),
    .mix_active_mask(32'hA5A5_1234),
    .mix_pos_readback(mix_pos_readback_r),
    .mix_voice_end_pending(32'h0000_00C0),

    .link_reg_wr   (),
    .link_reg_rd   (),
    .link_reg_addr (),
    .link_reg_wdata(),
    .link_reg_rdata(link_reg_rdata),

    .save_dt_slot   (dbg_save_dt_slot),
    .save_dt_size   (dbg_save_dt_size),
    .save_dt_commit (dbg_save_dt_commit),

    .uart_tx_dv    (),
    .uart_tx_byte  (),
    .uart_tx_active(uart_tx_active),
    .uart_rx_dv    (uart_rx_dv),
    .uart_rx_byte  (uart_rx_byte),

    .app_id         (app_id),
    .analogizer_settings(analogizer_settings_in),
    .analogizer_hoffset (32'hFFFF_FFF9),
    .analogizer_voffset (32'h0000_0005),
    .analogizer_cpu_wr_toggle(dbg_analogizer_wr_toggle),
    .analogizer_cpu_wr_settings(dbg_analogizer_wr_settings),
    .analogizer_cpu_wr_hoffset(dbg_analogizer_wr_hoffset),
    .analogizer_cpu_wr_voffset(dbg_analogizer_wr_voffset),
    .shutdown_pending(shutdown_pending),
    .shutdown_ack   (),

    .timer_irq  (),
    .uart_rx_irq(),

    .link_irq        (link_irq),
    .ext_irq         (ext_irq_out),

    .dt_query_addr  (),
    .dt_query_toggle(),
    .dt_query_data  (dt_query_data),
    .dt_query_valid (dt_query_valid),

    .vrr_v_total(),
    .analogizer_enabled(1'b0),

    .snac_pin_out(dbg_snac_pin_out),
    .snac_pin_dir(dbg_snac_pin_dir),
    .snac_pin_in (snac_pin_in),
    .snac_enable (dbg_snac_enable),

    .cart_reg_wr   (cart_reg_wr),
    .cart_reg_rd   (cart_reg_rd),
    .cart_reg_addr (cart_reg_addr),
    .cart_reg_wdata(cart_reg_wdata),
    .cart_reg_rdata(cart_reg_rdata),

    .gpu_reg_wr   (dbg_gpu_reg_wr),
    .gpu_reg_addr (dbg_gpu_reg_addr),
    .gpu_reg_wdata(dbg_gpu_reg_wdata),
    .gpu_reg_rdata(gpu_reg_rdata),
    // No gpu_core in this bench — tie the CMD_FLIP side-port low.
    .gpu_swap_req (gpu_swap_req_in),
    .gpu_swap_idx (gpu_swap_idx_in),
    .fb_swap_pending_o()
);

// Hoist FSM internals for debug
assign dbg_state         = dut.state;
assign dbg_bram_hold     = dut.bram_hold;
assign dbg_can_push_beat = dut.can_push_beat;

endmodule
