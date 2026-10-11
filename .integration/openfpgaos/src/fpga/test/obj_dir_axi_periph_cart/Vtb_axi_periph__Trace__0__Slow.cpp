// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vtb_axi_periph__Syms.h"


VL_ATTR_COLD void Vtb_axi_periph___024root__trace_init_sub__TOP__0(Vtb_axi_periph___024root* vlSelf, VerilatedVcd* tracep) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root__trace_init_sub__TOP__0\n"); );
    // Init
    const int c = vlSymsp->__Vm_baseCode;
    // Body
    tracep->declBit(c+340,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+341,0,"reset_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+342,0,"s_axi_arvalid",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+343,0,"s_axi_arready",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+344,0,"s_axi_araddr",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+345,0,"s_axi_arlen",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+346,0,"s_axi_rvalid",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+347,0,"s_axi_rready",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+348,0,"s_axi_rdata",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+349,0,"s_axi_rresp",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+350,0,"s_axi_rlast",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+351,0,"s_axi_awvalid",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+352,0,"s_axi_awready",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+353,0,"s_axi_awaddr",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+354,0,"s_axi_awlen",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+355,0,"s_axi_awburst",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+356,0,"s_axi_wvalid",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+357,0,"s_axi_wready",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+358,0,"s_axi_wdata",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+359,0,"s_axi_wstrb",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+360,0,"s_axi_wlast",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+361,0,"s_axi_bvalid",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+362,0,"s_axi_bready",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+363,0,"s_axi_bresp",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+364,0,"dbg_state",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+365,0,"dbg_bram_hold",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+366,0,"dbg_can_push_beat",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+367,0,"dbg_gpu_reg_wr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+368,0,"dbg_gpu_reg_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+369,0,"dbg_gpu_reg_wdata",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+370,0,"dbg_save_dt_slot",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+371,0,"dbg_save_dt_size",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+372,0,"dbg_save_dt_commit",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+373,0,"dbg_analogizer_wr_toggle",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+374,0,"dbg_analogizer_wr_settings",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+375,0,"dbg_analogizer_wr_hoffset",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+376,0,"dbg_analogizer_wr_voffset",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+377,0,"dbg_snac_pin_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+378,0,"dbg_snac_pin_dir",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+379,0,"dbg_snac_enable",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+380,0,"dbg_cart_wr_count",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+381,0,"dbg_cart_rd_count",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+382,0,"dbg_cart_last_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+383,0,"dbg_cart_last_wdata",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+384,0,"dbg_pal_commit",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+385,0,"dbg_analog_fb_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 24,0);
    tracep->declBus(c+386,0,"dbg_analog_color_mode",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+387,0,"dbg_analog_fb_width",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBus(c+388,0,"dbg_analog_fb_height",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBus(c+389,0,"dbg_analog_fb_stride",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+390,0,"dbg_pal_commit_count",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+391,0,"pal_busy_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+392,0,"analogizer_settings_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+393,0,"gpu_swap_req_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+394,0,"gpu_swap_idx_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+395,0,"vsync_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+396,0,"early_vblank_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+397,0,"os_inmenu_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+398,0,"target_dataslot_ack_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+399,0,"target_dataslot_done_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+400,0,"target_dataslot_err_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBit(c+401,0,"bridge_wr_idle_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+402,0,"cont1_key_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+403,0,"cont1_joy_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+404,0,"cont1_trig_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+405,0,"cont2_key_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+406,0,"cont2_joy_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+407,0,"cont2_trig_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+408,0,"cont3_key_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+409,0,"cont3_joy_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+410,0,"cont3_trig_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+411,0,"cont4_key_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+412,0,"cont4_joy_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+413,0,"cont4_trig_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+414,0,"dt_query_data_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+415,0,"dt_query_valid_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+416,0,"snac_pin_in_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+417,0,"ext_irq_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("tb_axi_periph", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+439,0,"INCLUDE_CARTRIDGE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+340,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+341,0,"reset_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+342,0,"s_axi_arvalid",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+343,0,"s_axi_arready",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+344,0,"s_axi_araddr",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+345,0,"s_axi_arlen",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+346,0,"s_axi_rvalid",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+347,0,"s_axi_rready",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+348,0,"s_axi_rdata",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+349,0,"s_axi_rresp",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+350,0,"s_axi_rlast",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+351,0,"s_axi_awvalid",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+352,0,"s_axi_awready",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+353,0,"s_axi_awaddr",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+354,0,"s_axi_awlen",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+355,0,"s_axi_awburst",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+356,0,"s_axi_wvalid",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+357,0,"s_axi_wready",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+358,0,"s_axi_wdata",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+359,0,"s_axi_wstrb",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+360,0,"s_axi_wlast",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+361,0,"s_axi_bvalid",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+362,0,"s_axi_bready",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+363,0,"s_axi_bresp",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+364,0,"dbg_state",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+365,0,"dbg_bram_hold",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+366,0,"dbg_can_push_beat",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+367,0,"dbg_gpu_reg_wr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+368,0,"dbg_gpu_reg_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+369,0,"dbg_gpu_reg_wdata",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+370,0,"dbg_save_dt_slot",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+371,0,"dbg_save_dt_size",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+372,0,"dbg_save_dt_commit",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+373,0,"dbg_analogizer_wr_toggle",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+374,0,"dbg_analogizer_wr_settings",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+375,0,"dbg_analogizer_wr_hoffset",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+376,0,"dbg_analogizer_wr_voffset",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+377,0,"dbg_snac_pin_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+378,0,"dbg_snac_pin_dir",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+379,0,"dbg_snac_enable",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+380,0,"dbg_cart_wr_count",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+381,0,"dbg_cart_rd_count",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+382,0,"dbg_cart_last_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+383,0,"dbg_cart_last_wdata",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+384,0,"dbg_pal_commit",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+385,0,"dbg_analog_fb_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 24,0);
    tracep->declBus(c+386,0,"dbg_analog_color_mode",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+387,0,"dbg_analog_fb_width",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBus(c+388,0,"dbg_analog_fb_height",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBus(c+389,0,"dbg_analog_fb_stride",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+390,0,"dbg_pal_commit_count",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+391,0,"pal_busy_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+392,0,"analogizer_settings_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+393,0,"gpu_swap_req_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+394,0,"gpu_swap_idx_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+395,0,"vsync_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+396,0,"early_vblank_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+397,0,"os_inmenu_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+398,0,"target_dataslot_ack_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+399,0,"target_dataslot_done_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+400,0,"target_dataslot_err_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBit(c+401,0,"bridge_wr_idle_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+402,0,"cont1_key_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+403,0,"cont1_joy_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+404,0,"cont1_trig_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+405,0,"cont2_key_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+406,0,"cont2_joy_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+407,0,"cont2_trig_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+408,0,"cont3_key_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+409,0,"cont3_joy_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+410,0,"cont3_trig_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+411,0,"cont4_key_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+412,0,"cont4_joy_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+413,0,"cont4_trig_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+414,0,"dt_query_data_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+415,0,"dt_query_valid_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+416,0,"snac_pin_in_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+417,0,"ext_irq_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+398,0,"target_dataslot_ack",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+399,0,"target_dataslot_done",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+400,0,"target_dataslot_err",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+440,0,"link_reg_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+441,0,"audio_fifo_level",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBit(c+442,0,"audio_fifo_full",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+442,0,"uart_tx_active",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+442,0,"uart_rx_dv",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+443,0,"uart_rx_byte",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+442,0,"shutdown_pending",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+414,0,"dt_query_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+415,0,"dt_query_valid",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+416,0,"snac_pin_in",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+266,0,"gpu_reg_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+442,0,"link_irq",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+401,0,"bridge_wr_idle",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+444,0,"dataslot_allcomplete",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+395,0,"vsync",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+396,0,"early_vblank",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+397,0,"os_inmenu",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+402,0,"cont1_key",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+403,0,"cont1_joy",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+404,0,"cont1_trig",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+405,0,"cont2_key",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+406,0,"cont2_joy",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+407,0,"cont2_trig",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+408,0,"cont3_key",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+409,0,"cont3_joy",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+410,0,"cont3_trig",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+411,0,"cont4_key",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+412,0,"cont4_joy",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+413,0,"cont4_trig",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+440,0,"app_id",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+259,0,"pal_commit_w",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+267,0,"cart_reg_wr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+418,0,"cart_reg_rd",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+268,0,"cart_reg_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+269,0,"cart_reg_wdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+270,0,"cart_reg_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+271,0,"mix_voice_sel_rd",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+240,0,"mix_pos_readback_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 21,0);
    tracep->pushPrefix("dut", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+445,0,"CLK_HZ",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+439,0,"INCLUDE_ANALOGIZER",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+439,0,"INCLUDE_LINK",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+439,0,"INCLUDE_CARTRIDGE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+439,0,"INCLUDE_HW_MIXER",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+439,0,"INCLUDE_PARAM_TRI",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+439,0,"INCLUDE_VERT_TRI",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+439,0,"INCLUDE_PARAM_TRI_RECS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+439,0,"INCLUDE_COLUMN_LIST",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+439,0,"INCLUDE_COMPACT_SPAN",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+439,0,"INCLUDE_TEX_MEM",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+446,0,"INCLUDE_DIRECT_COLOR",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+446,0,"INCLUDE_XFORM_RGB",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+446,0,"INCLUDE_GPU_CLIP_LOAD",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+446,0,"INCLUDE_GPU_LIGHT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+439,0,"INCLUDE_COMBINE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+439,0,"INCLUDE_PARAM_SPAN_Q29",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+340,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+341,0,"reset_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+342,0,"s_axi_arvalid",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+343,0,"s_axi_arready",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+344,0,"s_axi_araddr",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+345,0,"s_axi_arlen",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+346,0,"s_axi_rvalid",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+347,0,"s_axi_rready",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+348,0,"s_axi_rdata",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+349,0,"s_axi_rresp",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+350,0,"s_axi_rlast",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+351,0,"s_axi_awvalid",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+352,0,"s_axi_awready",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+353,0,"s_axi_awaddr",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+354,0,"s_axi_awlen",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+355,0,"s_axi_awburst",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+356,0,"s_axi_wvalid",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+357,0,"s_axi_wready",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+358,0,"s_axi_wdata",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+359,0,"s_axi_wstrb",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+360,0,"s_axi_wlast",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+361,0,"s_axi_bvalid",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+362,0,"s_axi_bready",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+363,0,"s_axi_bresp",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+444,0,"dataslot_allcomplete",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+395,0,"vsync",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+396,0,"early_vblank",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+397,0,"os_inmenu",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+402,0,"cont1_key",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+403,0,"cont1_joy",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+404,0,"cont1_trig",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+405,0,"cont2_key",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+406,0,"cont2_joy",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+407,0,"cont2_trig",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+408,0,"cont3_key",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+409,0,"cont3_joy",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+410,0,"cont3_trig",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+411,0,"cont4_key",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+412,0,"cont4_joy",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+413,0,"cont4_trig",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+440,0,"rtc_epoch_seconds",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+442,0,"rtc_valid",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+398,0,"target_dataslot_ack",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+399,0,"target_dataslot_done",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+400,0,"target_dataslot_err",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+447,0,"hps_status",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declQuad(c+448,0,"hps_img_size",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declBus(c+450,0,"hps_boot_len",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declQuad(c+451,0,"hps_img1_size",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declQuad(c+453,0,"hps_img2_size",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declBus(c+455,0,"hps_ini_len",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+456,0,"hps_elf_len",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+401,0,"bridge_wr_idle",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+440,0,"bridge_dbg_wcnt",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+4,0,"color_mode",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+5,0,"fb_display_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 24,0);
    tracep->declBus(c+6,0,"fb_width",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBus(c+7,0,"fb_height",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBus(c+8,0,"fb_stride",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+385,0,"analog_fb_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 24,0);
    tracep->declBus(c+386,0,"analog_color_mode",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+387,0,"analog_fb_width",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBus(c+388,0,"analog_fb_height",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBus(c+389,0,"analog_fb_stride",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+9,0,"video_scaler_slot",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBit(c+10,0,"pal_wr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+11,0,"pal_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+12,0,"pal_data",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 23,0);
    tracep->declBit(c+259,0,"pal_commit",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+391,0,"pal_busy",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+13,0,"target_dataslot_read",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+14,0,"target_dataslot_write",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+15,0,"target_dataslot_openfile",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+16,0,"target_dataslot_getfile",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+17,0,"target_dataslot_id",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+18,0,"target_dataslot_slotoffset",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+19,0,"target_dataslot_bridgeaddr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+20,0,"target_dataslot_length",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+21,0,"target_buffer_param_struct",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+22,0,"target_buffer_resp_struct",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+457,0,"audio_fifo_level",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBit(c+442,0,"audio_fifo_full",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+272,0,"audio_sample_wr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+273,0,"audio_sample_data",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+274,0,"cram0_mode",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+275,0,"mix_enable",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+276,0,"mix_voice_wr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+277,0,"mix_voice_sel",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+278,0,"mix_voice_field",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+279,0,"mix_voice_wdata",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+280,0,"mix_irq_clear_wr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+281,0,"mix_irq_clear",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+282,0,"mix_master_vol",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+283,0,"mix_group_vol_0",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+284,0,"mix_group_vol_1",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+285,0,"mix_group_vol_2",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+286,0,"mix_group_vol_3",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declQuad(c+287,0,"mix_voice_group_packed",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declBus(c+271,0,"mix_voice_sel_rd",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+458,0,"mix_active_mask",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+240,0,"mix_pos_readback",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 21,0);
    tracep->declBus(c+459,0,"mix_voice_end_pending",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+289,0,"link_reg_wr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+290,0,"link_reg_rd",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+291,0,"link_reg_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+292,0,"link_reg_wdata",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+440,0,"link_reg_rdata",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+370,0,"save_dt_slot",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+371,0,"save_dt_size",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+372,0,"save_dt_commit",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+23,0,"save_dt_word",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBit(c+24,0,"save_dt_word_mode",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+25,0,"uart_tx_dv",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+26,0,"uart_tx_byte",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+442,0,"uart_tx_active",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+442,0,"uart_rx_dv",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+443,0,"uart_rx_byte",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+440,0,"app_id",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+392,0,"analogizer_settings",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+460,0,"analogizer_hoffset",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+461,0,"analogizer_voffset",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+462,0,"mouse_speed_pct",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+463,0,"sdram_wlast_err",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+464,0,"scanout_fetch_diag",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+373,0,"analogizer_cpu_wr_toggle",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+374,0,"analogizer_cpu_wr_settings",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+375,0,"analogizer_cpu_wr_hoffset",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+376,0,"analogizer_cpu_wr_voffset",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+442,0,"shutdown_pending",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+27,0,"shutdown_ack",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+28,0,"timer_irq",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+29,0,"uart_rx_irq",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+442,0,"link_irq",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+417,0,"ext_irq",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+30,0,"dt_query_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBit(c+31,0,"dt_query_toggle",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+414,0,"dt_query_data",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+415,0,"dt_query_valid",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+419,0,"vrr_v_total",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBit(c+442,0,"analogizer_enabled",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+377,0,"snac_pin_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+378,0,"snac_pin_dir",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+416,0,"snac_pin_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+379,0,"snac_enable",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+267,0,"cart_reg_wr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+418,0,"cart_reg_rd",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+268,0,"cart_reg_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+269,0,"cart_reg_wdata",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+270,0,"cart_reg_rdata",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+367,0,"gpu_reg_wr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+368,0,"gpu_reg_addr",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+369,0,"gpu_reg_wdata",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+266,0,"gpu_reg_rdata",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+393,0,"gpu_swap_req",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+394,0,"gpu_swap_idx",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+32,0,"fb_swap_pending_o",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+420,0,"reset",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+344,0,"ar_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+353,0,"aw_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+33,0,"snac_en_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+260,0,"snac_mode_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+261,0,"snac_div_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+262,0,"snac_tx_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+34,0,"snac_gpio_out_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+35,0,"snac_gpio_dir_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+263,0,"snac_start_pulse",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+36,0,"snac_hw_enable_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+37,0,"snac_hw_fast_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+38,0,"snac_hw_clear_edges_pulse",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+293,0,"snac_busy",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+241,0,"snac_done",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+242,0,"snac_rx_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+243,0,"snac_shift_clk",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+244,0,"snac_shift_mosi",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+245,0,"snac_shift_latch",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+39,0,"snac_hw_pin_out",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+465,0,"snac_hw_pin_dir",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+36,0,"snac_hw_valid",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+442,0,"snac_hw_irq_pending",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+40,0,"snac_hw_buttons",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+41,0,"snac_hw_pressed",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+42,0,"snac_hw_released",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+40,0,"snac_hw_raw_buttons",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+43,0,"snac_hw_lx",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+44,0,"snac_hw_ly",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+45,0,"snac_hw_rx",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+46,0,"snac_hw_ry",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+443,0,"snac_hw_debug_status",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+47,0,"snac_pin_in_meta",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+48,0,"snac_pin_in_sync",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+49,0,"snac_miso_a",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+50,0,"snac_miso_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+264,0,"snac_bit_count_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+265,0,"snac_latch_en_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+51,0,"psx_stb_cnt",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBit(c+52,0,"psx_stb",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+53,0,"psx_stb_top",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+40,0,"psx_key1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+54,0,"psx_key2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+55,0,"psx_joy1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+56,0,"psx_joy2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+57,0,"psx_clk_o",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+58,0,"psx_cmd_o",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+59,0,"psx_att1_o",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+60,0,"psx_att2_o",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+61,0,"psx_prev_key1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+41,0,"psx_pressed_acc",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+42,0,"psx_released_acc",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declQuad(c+62,0,"psx_p2_unused",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 47,0);
    tracep->declBus(c+333,0,"snac_pin_out_mux",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+334,0,"snac_pin_dir_mux",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+421,0,"ram_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+422,0,"ram_addr_mux",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 12,0);
    tracep->declBit(c+294,0,"ram_wren",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+423,0,"sysreg_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declQuad(c+64,0,"cycle_counter",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declBus(c+4,0,"color_mode_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+6,0,"fb_width_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBus(c+7,0,"fb_height_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBus(c+8,0,"fb_stride_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+66,0,"analog_color_mode_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+67,0,"analog_fb_width_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBus(c+68,0,"analog_fb_height_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBus(c+69,0,"analog_fb_stride_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+9,0,"video_scaler_slot_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+70,0,"ds_slot_id_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+71,0,"ds_slot_offset_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+72,0,"ds_bridge_addr_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+73,0,"ds_length_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+74,0,"ds_param_addr_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+75,0,"ds_resp_addr_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+76,0,"pal_index_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+77,0,"timer_period",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+78,0,"timer_counter",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+79,0,"timer_enable",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+80,0,"timer_irq_pending",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+81,0,"irq_mask",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->declBit(c+82,0,"vsync_irq_pending",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+83,0,"dataslot_irq_pending",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+84,0,"input_irq_pending",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+466,0,"FEAT_LINK",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+467,0,"HW_CONTRACT_REV",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+468,0,"HW_FEATURES_RESOLVED",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+469,0,"FB_ADDR_0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 24,0);
    tracep->declBus(c+470,0,"FB_ADDR_1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 24,0);
    tracep->declBus(c+471,0,"FB_ADDR_2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 24,0);
    tracep->declBus(c+472,0,"TERM_FB_ADDR",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 24,0);
    tracep->declBus(c+85,0,"fb_display_idx",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+86,0,"fb_ready_idx",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+32,0,"fb_swap_pending",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+87,0,"fb_swap_consumed_this_frame",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+88,0,"term_fb_active",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+89,0,"analog_keep_app",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+90,0,"vrr_v_total_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBit(c+424,0,"analogizer_video_enabled",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+425,0,"analogizer_video_mode",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+426,0,"analogizer_pal_mode",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+427,0,"snac_configured",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+428,0,"vrr_fixed_rate_mode",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+429,0,"vrr_fixed_v_total",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBus(c+91,0,"fb_app_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 24,0);
    tracep->declBus(c+5,0,"fb_display_addr_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 24,0);
    tracep->declBit(c+92,0,"analog_show_term",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+93,0,"dataslot_allcomplete_sync",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBit(c+94,0,"dataslot_allcomplete_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+95,0,"bridge_dbg_wcnt_meta",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+96,0,"bridge_dbg_wcnt_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+97,0,"vsync_sync",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBit(c+98,0,"vsync_rising",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+99,0,"early_vblank_sync",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBit(c+100,0,"early_vblank_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+101,0,"os_inmenu_sync",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBit(c+102,0,"os_inmenu_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+103,0,"clamp_fb_stride__Vstatic__even_stride",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+246,0,"target_ack_sync",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+247,0,"target_done_sync",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->pushPrefix("target_err_sync", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 3; ++i) {
        tracep->declBus(c+248+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 2,0);
    }
    tracep->popPrefix();
    tracep->declBit(c+251,0,"target_ack_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+252,0,"target_done_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+253,0,"target_err_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBit(c+104,0,"rtc_valid_meta",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+105,0,"rtc_valid_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+106,0,"cont1_key_meta",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+107,0,"cont1_key_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+108,0,"cont1_joy_meta",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+109,0,"cont1_joy_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+110,0,"cont1_trig_meta",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+111,0,"cont1_trig_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+112,0,"cont2_key_meta",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+113,0,"cont2_key_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+114,0,"cont2_joy_meta",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+115,0,"cont2_joy_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+116,0,"cont2_trig_meta",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+117,0,"cont2_trig_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+118,0,"cont3_key_meta",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+119,0,"cont3_key_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+120,0,"cont3_joy_meta",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+121,0,"cont3_joy_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+122,0,"cont3_trig_meta",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+123,0,"cont3_trig_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+124,0,"cont4_key_meta",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+125,0,"cont4_key_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+126,0,"cont4_joy_meta",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+127,0,"cont4_joy_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+128,0,"cont4_trig_meta",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+129,0,"cont4_trig_s",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+473,0,"INPUT_EVENT_SLOT_CHANGE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+130,0,"input_irq_mask",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+131,0,"input_overflow",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+132,0,"input_seq",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+133,0,"input_prev_key0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+134,0,"input_prev_key1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+135,0,"input_prev_key2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+136,0,"input_prev_key3",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+137,0,"input_prev_joy0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+138,0,"input_prev_joy1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+139,0,"input_prev_joy2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+140,0,"input_prev_joy3",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+141,0,"input_prev_trig0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+142,0,"input_prev_trig1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+143,0,"input_prev_trig2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+144,0,"input_prev_trig3",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+145,0,"input_scan_slot",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+146,0,"input_chg_slot0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+147,0,"input_chg_slot1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+148,0,"input_chg_slot2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+149,0,"input_chg_slot3",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+150,0,"input_event_fields",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+145,0,"input_event_slot",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+151,0,"input_event_valid",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+152,0,"input_fifo_empty",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+153,0,"input_fifo_full",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+154,0,"input_fifo_count",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declQuad(c+155,0,"input_fifo_dout",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declQuad(c+157,0,"input_fifo_din",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declBit(c+430,0,"input_fifo_pop",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+159,0,"input_fifo_push",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+160,0,"input_fifo_clear",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+295,0,"sysreg_wr_fire",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+161,0,"ds_cmd_active",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+162,0,"ds_ack_latched",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+163,0,"ds_done_latched",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+164,0,"ds_ack_seen_low",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+165,0,"ds_done_seen_low",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+166,0,"ds_err_latched",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBit(c+296,0,"sysreg_input_page",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+297,0,"sysreg_word",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->declBit(c+298,0,"sysreg_cart_window",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+299,0,"input_slot_read",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+335,0,"input_slot_key_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+336,0,"input_slot_joy_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+337,0,"input_slot_trig_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBit(c+338,0,"input_slot_irq_en_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+339,0,"input_slot_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+300,0,"low_page_cont_read",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+301,0,"input_slot_sel",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+302,0,"input_field_sel",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+167,0,"uart_rx_wr_ptr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBus(c+168,0,"uart_rx_rd_ptr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBus(c+169,0,"uart_rx_count",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 9,0);
    tracep->declBit(c+170,0,"uart_rx_empty",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+171,0,"uart_rx_full",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+172,0,"uart_rx_data",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+29,0,"uart_rx_valid",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+303,0,"uart_tx_fifo_we",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+304,0,"uart_tx_fifo_din",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+444,0,"uart_tx_idle",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+444,0,"uart_tx_rdy",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+431,0,"uart_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+440,0,"audio_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+305,0,"cram0_mode_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+306,0,"mixer_per_voice_rd",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+307,0,"mixer_ctrl_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+308,0,"mixer_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+309,0,"periph_rd_data_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+474,0,"S_IDLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+475,0,"S_BRAM_RD",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+476,0,"S_PERIPH_RD",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+477,0,"S_PERIPH_WR",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+478,0,"S_PERIPH_RD_LATCH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+479,0,"S_WR_NEXT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+480,0,"S_BRAM_WR",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+481,0,"S_PERIPH_RD_WAIT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+310,0,"state",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+311,0,"req_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+269,0,"req_wdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+312,0,"req_wstrb",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+313,0,"burst_len",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+314,0,"burst_count",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+315,0,"awburst_latched",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBit(c+316,0,"burst_is_fixed",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+482,0,"REGION_NONE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+483,0,"REGION_RAM",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+484,0,"REGION_SYSREG",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+485,0,"REGION_AUDIO",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+486,0,"REGION_CRAM0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+487,0,"REGION_LINK",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+488,0,"REGION_UART",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+489,0,"REGION_GPU",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+490,0,"REGION_MIXER",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+491,0,"REGION_HPS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+317,0,"req_region",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+318,0,"req_is_ram",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+319,0,"req_is_sysreg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+320,0,"req_is_audio",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+321,0,"req_is_cram0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+322,0,"req_is_link",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+323,0,"req_is_uart",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+324,0,"req_is_gpu",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+325,0,"req_is_mixer",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+326,0,"req_is_hps",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+327,0,"hps_rdata",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+328,0,"beat_is_last",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+329,0,"req_addr_plus4",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+330,0,"periph_next_addr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+331,0,"bram_next_word",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 12,0);
    tracep->declBit(c+432,0,"can_push_beat",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+433,0,"bram_hold",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+434,0,"ar_region",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+435,0,"aw_region",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->pushPrefix("input_event_fifo", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+492,0,"WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+493,0,"DEPTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+494,0,"ADDR_WIDTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declArray(c+495,0,"RAMSTYLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 135,0);
    tracep->declBit(c+340,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+420,0,"reset",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+160,0,"clear",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+159,0,"push",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declQuad(c+157,0,"din",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declBit(c+430,0,"pop",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declQuad(c+155,0,"dout",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declBit(c+152,0,"empty",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+153,0,"full",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+154,0,"count",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->pushPrefix("mem", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 16; ++i) {
        tracep->declQuad(c+173+i*2,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 63,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+205,0,"wr_ptr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+206,0,"rd_ptr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+500,0,"COUNT_ZERO",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+501,0,"COUNT_ONE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+502,0,"COUNT_DEPTH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+436,0,"do_pop",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+437,0,"do_push",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->pushPrefix("ram", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declArray(c+503,0,"operation_mode",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 87,0);
    tracep->declBus(c+506,0,"width_a",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+507,0,"widthad_a",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+508,0,"numwords_a",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+494,0,"width_byteena_a",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+506,0,"width_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+507,0,"widthad_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+508,0,"numwords_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+494,0,"width_byteena_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declArray(c+509,0,"lpm_type",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 79,0);
    tracep->declArray(c+512,0,"outdata_reg_a",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 95,0);
    tracep->declArray(c+512,0,"outdata_reg_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 95,0);
    tracep->declArray(c+515,0,"init_file",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 95,0);
    tracep->declArray(c+518,0,"intended_device_family",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 71,0);
    tracep->declArray(c+521,0,"read_during_write_mode_port_a",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 159,0);
    tracep->declArray(c+521,0,"read_during_write_mode_port_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 159,0);
    tracep->declQuad(c+526,0,"read_during_write_mode_mixed_ports",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 63,0);
    tracep->declQuad(c+528,0,"clock_enable_input_a",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 47,0);
    tracep->declQuad(c+528,0,"clock_enable_input_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 47,0);
    tracep->declQuad(c+528,0,"clock_enable_output_a",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 47,0);
    tracep->declQuad(c+528,0,"clock_enable_output_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 47,0);
    tracep->declQuad(c+530,0,"power_up_uninitialized",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 39,0);
    tracep->declQuad(c+532,0,"address_reg_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 47,0);
    tracep->declQuad(c+532,0,"rdcontrol_reg_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 47,0);
    tracep->declBus(c+534,0,"address_aclr_a",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+534,0,"address_aclr_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+534,0,"outdata_aclr_a",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+534,0,"outdata_aclr_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+534,0,"byteena_aclr_a",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+534,0,"byteena_aclr_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+535,0,"byte_size",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+534,0,"wrcontrol_aclr_a",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+534,0,"wrcontrol_aclr_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declQuad(c+532,0,"wrcontrol_wraddress_reg_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 47,0);
    tracep->declBus(c+534,0,"indata_aclr_a",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+534,0,"indata_aclr_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declQuad(c+532,0,"indata_reg_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 47,0);
    tracep->declBus(c+536,0,"ram_block_type",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declQuad(c+537,0,"init_file_layout",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 47,0);
    tracep->declBus(c+446,0,"maximum_depth",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declQuad(c+530,0,"enable_ecc",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 39,0);
    tracep->declArray(c+539,0,"clock_enable_core_a",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 119,0);
    tracep->declArray(c+539,0,"clock_enable_core_b",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 119,0);
    tracep->declBit(c+340,0,"clock0",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+422,0,"address_a",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 12,0);
    tracep->declBus(c+269,0,"data_a",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+294,0,"wren_a",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+312,0,"byteena_a",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+421,0,"q_a",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+442,0,"aclr0",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+442,0,"aclr1",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+442,0,"addressstall_a",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+442,0,"addressstall_b",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+543,0,"address_b",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 12,0);
    tracep->declBus(c+440,0,"data_b",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+442,0,"wren_b",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+483,0,"byteena_b",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+444,0,"clock1",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+444,0,"clocken0",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+444,0,"clocken1",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+444,0,"clocken2",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+444,0,"clocken3",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+474,0,"eccstatus",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+1,0,"q_b",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+444,0,"rden_a",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+442,0,"rden_b",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+421,0,"q_a_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+1,0,"q_b_reg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+207,0,"q_a_reg2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+2,0,"q_b_reg2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+3,0,"i",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::INTEGER, false,-1, 31,0);
    tracep->popPrefix();
    tracep->pushPrefix("snac_hw_psx", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+544,0,"MASTER_CLK_FREQ",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+340,0,"i_clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+420,0,"i_rst",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+36,0,"i_ena",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+52,0,"i_stb",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+40,0,"key1",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+55,0,"joy1",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+54,0,"key2",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+56,0,"joy2",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+545,0,"i_VIB_SW1",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+443,0,"i_VIB_DAT1",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+545,0,"i_VIB_SW2",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+443,0,"i_VIB_DAT2",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+57,0,"PSX_CLK",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+50,0,"PSX_DAT",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+58,0,"PSX_CMD",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+59,0,"PSX_ATT1",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+60,0,"PSX_ATT2",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+208,0,"PSX_ACK",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+482,0,"DBG_TX",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+209,0,"rx0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+210,0,"rx1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+211,0,"rx2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+212,0,"rx3",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+213,0,"rx4",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+214,0,"rx5",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+215,0,"rx6",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+216,0,"RXD_ID",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+217,0,"att1r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+218,0,"att2r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+219,0,"no_gamepad",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("ds1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+544,0,"FREQ",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+340,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+341,0,"i_RSTn",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+36,0,"i_ena",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+52,0,"i_stb",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+442,0,"i_MULTITAP_ena",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+545,0,"i_VIB_SW1",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+443,0,"i_VIB_DAT1",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+545,0,"i_VIB_SW2",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+443,0,"i_VIB_DAT2",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+57,0,"o_psCLK",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+59,0,"o_ATT1",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+60,0,"o_ATT2",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+58,0,"o_psTXD",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+50,0,"i_psRXD",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+208,0,"i_psACK",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+216,0,"o_RXD_ID",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+209,0,"o_RXD_0",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+210,0,"o_RXD_1",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+211,0,"o_RXD_2",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+212,0,"o_RXD_3",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+213,0,"o_RXD_4",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+214,0,"o_RXD_5",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+215,0,"o_RXD_6",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+220,0,"i_CLK",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+221,0,"R_CE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+222,0,"F_CE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+223,0,"ack_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+224,0,"device_id_type",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+444,0,"W_type",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+225,0,"W_byte_cnt",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBit(c+226,0,"W_RXWT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+227,0,"W_TXWT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+228,0,"W_TXEN",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+546,0,"W_TXSET",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+229,0,"W_TXD_DAT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+230,0,"W_RXD_DAT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+231,0,"W_RXWT_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("pls", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+340,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+221,0,"R_CE",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+220,0,"i_CLK",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+341,0,"i_RSTn",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+224,0,"i_TYPE",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+226,0,"o_RXWT",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+227,0,"o_TXWT",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+232,0,"o_TXSET",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+228,0,"o_TXEN",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+57,0,"o_psCLK",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+59,0,"o_ATT1",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+60,0,"o_ATT2",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+225,0,"o_byte_cnt",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+547,0,"Timer_size",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+225,0,"o_byte_cnt_r",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+233,0,"Timer",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 10,0);
    tracep->declBit(c+234,0,"RXWT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+235,0,"TXWT",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+236,0,"TXSET",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+237,0,"psCLK_gate",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+59,0,"psATT1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+60,0,"psATT2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->pushPrefix("rxd", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+340,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+221,0,"R_CE",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+341,0,"i_RSTn",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+226,0,"i_WT",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+50,0,"i_psRXD",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+230,0,"o_RXD_DAT",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+238,0,"sp",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->pushPrefix("txd", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+340,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+222,0,"F_CE",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+341,0,"i_RSTn",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+227,0,"i_WT",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+228,0,"i_EN",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+229,0,"i_TXD_DAT",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBit(c+58,0,"o_psTXD",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+239,0,"ps",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->pushPrefix("snac_shift", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+340,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+341,0,"reset_n",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+263,0,"start",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+264,0,"bit_count",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+265,0,"latch_en",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+260,0,"mode",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+261,0,"clk_div",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+262,0,"tx_data",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBus(c+242,0,"rx_data",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+293,0,"busy",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+241,0,"done",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+243,0,"shift_clk",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+244,0,"shift_mosi",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+245,0,"shift_latch",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+49,0,"miso_a",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+50,0,"miso_b",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+474,0,"S_IDLE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+475,0,"S_LATCH_HIGH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+476,0,"S_LATCH_LOW",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+477,0,"S_CLK_LOW",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+478,0,"S_CLK_HIGH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+479,0,"S_DONE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+480,0,"S_SETUP",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+332,0,"state",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+254,0,"div_cnt",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+255,0,"bits_left",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+256,0,"total_bits",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBus(c+257,0,"tx_sr",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 31,0);
    tracep->declBit(c+258,0,"miso_sel",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+438,0,"miso",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+258,0,"clk_idle",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->popPrefix();
    tracep->popPrefix();
}

VL_ATTR_COLD void Vtb_axi_periph___024root__trace_init_top(Vtb_axi_periph___024root* vlSelf, VerilatedVcd* tracep) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root__trace_init_top\n"); );
    // Body
    Vtb_axi_periph___024root__trace_init_sub__TOP__0(vlSelf, tracep);
}

VL_ATTR_COLD void Vtb_axi_periph___024root__trace_const_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
VL_ATTR_COLD void Vtb_axi_periph___024root__trace_full_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
void Vtb_axi_periph___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
void Vtb_axi_periph___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/);

VL_ATTR_COLD void Vtb_axi_periph___024root__trace_register(Vtb_axi_periph___024root* vlSelf, VerilatedVcd* tracep) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root__trace_register\n"); );
    // Body
    tracep->addConstCb(&Vtb_axi_periph___024root__trace_const_0, 0U, vlSelf);
    tracep->addFullCb(&Vtb_axi_periph___024root__trace_full_0, 0U, vlSelf);
    tracep->addChgCb(&Vtb_axi_periph___024root__trace_chg_0, 0U, vlSelf);
    tracep->addCleanupCb(&Vtb_axi_periph___024root__trace_cleanup, vlSelf);
}

VL_ATTR_COLD void Vtb_axi_periph___024root__trace_const_0_sub_0(Vtb_axi_periph___024root* vlSelf, VerilatedVcd::Buffer* bufp);

VL_ATTR_COLD void Vtb_axi_periph___024root__trace_const_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root__trace_const_0\n"); );
    // Init
    Vtb_axi_periph___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vtb_axi_periph___024root*>(voidSelf);
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    Vtb_axi_periph___024root__trace_const_0_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vtb_axi_periph___024root__trace_const_0_sub_0(Vtb_axi_periph___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root__trace_const_0_sub_0\n"); );
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    VlWide<5>/*159:0*/ __Vtemp_1;
    VlWide<3>/*95:0*/ __Vtemp_2;
    VlWide<3>/*95:0*/ __Vtemp_3;
    VlWide<3>/*95:0*/ __Vtemp_4;
    VlWide<3>/*95:0*/ __Vtemp_5;
    VlWide<3>/*95:0*/ __Vtemp_6;
    VlWide<5>/*159:0*/ __Vtemp_7;
    VlWide<4>/*127:0*/ __Vtemp_8;
    // Body
    bufp->fullIData(oldp+439,(1U),32);
    bufp->fullIData(oldp+440,(0U),32);
    bufp->fullSData(oldp+441,(0U),9);
    bufp->fullBit(oldp+442,(0U));
    bufp->fullCData(oldp+443,(0U),8);
    bufp->fullBit(oldp+444,(1U));
    bufp->fullIData(oldp+445,(0x5f5e100U),32);
    bufp->fullIData(oldp+446,(0U),32);
    bufp->fullIData(oldp+447,(0x125U),32);
    bufp->fullQData(oldp+448,(0x1111222233334444ULL),64);
    bufp->fullIData(oldp+450,(0xabc00U),32);
    bufp->fullQData(oldp+451,(0x5555666677778888ULL),64);
    bufp->fullQData(oldp+453,(0x9999aaaabbbbccccULL),64);
    bufp->fullIData(oldp+455,(0xface1234U),32);
    bufp->fullIData(oldp+456,(0xe1f05678U),32);
    bufp->fullSData(oldp+457,(0U),10);
    bufp->fullIData(oldp+458,(0xa5a51234U),32);
    bufp->fullIData(oldp+459,(0xc0U),32);
    bufp->fullIData(oldp+460,(0xfffffff9U),32);
    bufp->fullIData(oldp+461,(5U),32);
    bufp->fullIData(oldp+462,(vlSelf->tb_axi_periph__DOT__dut__DOT__mouse_speed_pct),32);
    bufp->fullIData(oldp+463,(vlSelf->tb_axi_periph__DOT__dut__DOT__sdram_wlast_err),32);
    bufp->fullIData(oldp+464,(vlSelf->tb_axi_periph__DOT__dut__DOT__scanout_fetch_diag),32);
    bufp->fullCData(oldp+465,(0xc3U),8);
    bufp->fullIData(oldp+466,(4U),32);
    bufp->fullIData(oldp+467,(2U),32);
    bufp->fullIData(oldp+468,(0x3ffe37fU),32);
    bufp->fullIData(oldp+469,(0U),25);
    bufp->fullIData(oldp+470,(0x80000U),25);
    bufp->fullIData(oldp+471,(0x100000U),25);
    bufp->fullIData(oldp+472,(0x180000U),25);
    bufp->fullCData(oldp+473,(1U),8);
    bufp->fullCData(oldp+474,(0U),3);
    bufp->fullCData(oldp+475,(1U),3);
    bufp->fullCData(oldp+476,(2U),3);
    bufp->fullCData(oldp+477,(3U),3);
    bufp->fullCData(oldp+478,(4U),3);
    bufp->fullCData(oldp+479,(5U),3);
    bufp->fullCData(oldp+480,(6U),3);
    bufp->fullCData(oldp+481,(7U),3);
    bufp->fullCData(oldp+482,(0U),4);
    bufp->fullCData(oldp+483,(1U),4);
    bufp->fullCData(oldp+484,(2U),4);
    bufp->fullCData(oldp+485,(3U),4);
    bufp->fullCData(oldp+486,(4U),4);
    bufp->fullCData(oldp+487,(5U),4);
    bufp->fullCData(oldp+488,(6U),4);
    bufp->fullCData(oldp+489,(7U),4);
    bufp->fullCData(oldp+490,(8U),4);
    bufp->fullCData(oldp+491,(9U),4);
    bufp->fullIData(oldp+492,(0x40U),32);
    bufp->fullIData(oldp+493,(0x10U),32);
    bufp->fullIData(oldp+494,(4U),32);
    __Vtemp_1[0U] = 0x6865636bU;
    __Vtemp_1[1U] = 0x72775f63U;
    __Vtemp_1[2U] = 0x206e6f5fU;
    __Vtemp_1[3U] = 0x4c41422cU;
    __Vtemp_1[4U] = 0x4dU;
    bufp->fullWData(oldp+495,(__Vtemp_1),136);
    bufp->fullCData(oldp+500,(0U),5);
    bufp->fullCData(oldp+501,(1U),5);
    bufp->fullCData(oldp+502,(0x10U),5);
    __Vtemp_2[0U] = 0x504f5254U;
    __Vtemp_2[1U] = 0x474c455fU;
    __Vtemp_2[2U] = 0x53494eU;
    bufp->fullWData(oldp+503,(__Vtemp_2),88);
    bufp->fullIData(oldp+506,(0x20U),32);
    bufp->fullIData(oldp+507,(0xdU),32);
    bufp->fullIData(oldp+508,(0x2000U),32);
    __Vtemp_3[0U] = 0x6372616dU;
    __Vtemp_3[1U] = 0x7473796eU;
    __Vtemp_3[2U] = 0x616cU;
    bufp->fullWData(oldp+509,(__Vtemp_3),80);
    __Vtemp_4[0U] = 0x45524544U;
    __Vtemp_4[1U] = 0x47495354U;
    __Vtemp_4[2U] = 0x554e5245U;
    bufp->fullWData(oldp+512,(__Vtemp_4),96);
    __Vtemp_5[0U] = 0x2e6d6966U;
    __Vtemp_5[1U] = 0x77617265U;
    __Vtemp_5[2U] = 0x6669726dU;
    bufp->fullWData(oldp+515,(__Vtemp_5),96);
    __Vtemp_6[0U] = 0x6e652056U;
    __Vtemp_6[1U] = 0x79636c6fU;
    __Vtemp_6[2U] = 0x43U;
    bufp->fullWData(oldp+518,(__Vtemp_6),72);
    __Vtemp_7[0U] = 0x52454144U;
    __Vtemp_7[1U] = 0x4e42455fU;
    __Vtemp_7[2U] = 0x5f4e4f5fU;
    __Vtemp_7[3U] = 0x44415441U;
    __Vtemp_7[4U] = 0x4e45575fU;
    bufp->fullWData(oldp+521,(__Vtemp_7),160);
    bufp->fullQData(oldp+526,(0x4f4c445f44415441ULL),64);
    bufp->fullQData(oldp+528,(0x425950415353ULL),48);
    bufp->fullQData(oldp+530,(0x46414c5345ULL),40);
    bufp->fullQData(oldp+532,(0x434c4f434b31ULL),48);
    bufp->fullIData(oldp+534,(0x4e4f4e45U),32);
    bufp->fullIData(oldp+535,(8U),32);
    bufp->fullIData(oldp+536,(0x4155544fU),32);
    bufp->fullQData(oldp+537,(0x504f52545f41ULL),48);
    __Vtemp_8[0U] = 0x4c4b454eU;
    __Vtemp_8[1U] = 0x55545f43U;
    __Vtemp_8[2U] = 0x5f494e50U;
    __Vtemp_8[3U] = 0x555345U;
    bufp->fullWData(oldp+539,(__Vtemp_8),120);
    bufp->fullSData(oldp+543,(0U),13);
    bufp->fullIData(oldp+544,(0x5f5e100U),32);
    bufp->fullCData(oldp+545,(0U),2);
    bufp->fullBit(oldp+546,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_TXSET));
    bufp->fullIData(oldp+547,(0xbU),32);
}

VL_ATTR_COLD void Vtb_axi_periph___024root__trace_full_0_sub_0(Vtb_axi_periph___024root* vlSelf, VerilatedVcd::Buffer* bufp);

VL_ATTR_COLD void Vtb_axi_periph___024root__trace_full_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root__trace_full_0\n"); );
    // Init
    Vtb_axi_periph___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vtb_axi_periph___024root*>(voidSelf);
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    Vtb_axi_periph___024root__trace_full_0_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vtb_axi_periph___024root__trace_full_0_sub_0(Vtb_axi_periph___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root__trace_full_0_sub_0\n"); );
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    // Body
    bufp->fullIData(oldp+1,(vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_b_reg),32);
    bufp->fullIData(oldp+2,(vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_b_reg2),32);
    bufp->fullIData(oldp+3,(vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__i),32);
    bufp->fullCData(oldp+4,(vlSelf->tb_axi_periph__DOT__dut__DOT__color_mode_reg),3);
    bufp->fullIData(oldp+5,(((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__term_fb_active)
                              ? 0x180000U : vlSelf->tb_axi_periph__DOT__dut__DOT__fb_app_addr)),25);
    bufp->fullSData(oldp+6,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_width_reg),10);
    bufp->fullSData(oldp+7,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_height_reg),10);
    bufp->fullSData(oldp+8,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_stride_reg),16);
    bufp->fullCData(oldp+9,(vlSelf->tb_axi_periph__DOT__dut__DOT__video_scaler_slot_reg),3);
    bufp->fullBit(oldp+10,(vlSelf->tb_axi_periph__DOT__dut__DOT__pal_wr));
    bufp->fullCData(oldp+11,(vlSelf->tb_axi_periph__DOT__dut__DOT__pal_addr),8);
    bufp->fullIData(oldp+12,(vlSelf->tb_axi_periph__DOT__dut__DOT__pal_data),24);
    bufp->fullBit(oldp+13,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_read));
    bufp->fullBit(oldp+14,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_write));
    bufp->fullBit(oldp+15,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_openfile));
    bufp->fullBit(oldp+16,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_getfile));
    bufp->fullSData(oldp+17,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_id),16);
    bufp->fullIData(oldp+18,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_slotoffset),32);
    bufp->fullIData(oldp+19,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_bridgeaddr),32);
    bufp->fullIData(oldp+20,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_length),32);
    bufp->fullIData(oldp+21,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_buffer_param_struct),32);
    bufp->fullIData(oldp+22,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_buffer_resp_struct),32);
    bufp->fullSData(oldp+23,(vlSelf->tb_axi_periph__DOT__dut__DOT__save_dt_word),10);
    bufp->fullBit(oldp+24,(vlSelf->tb_axi_periph__DOT__dut__DOT__save_dt_word_mode));
    bufp->fullBit(oldp+25,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_dv));
    bufp->fullCData(oldp+26,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_byte),8);
    bufp->fullBit(oldp+27,(vlSelf->tb_axi_periph__DOT__dut__DOT__shutdown_ack));
    bufp->fullBit(oldp+28,(((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_enable) 
                            & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_irq_pending))));
    bufp->fullBit(oldp+29,((1U & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_empty)))));
    bufp->fullSData(oldp+30,(vlSelf->tb_axi_periph__DOT__dut__DOT__dt_query_addr),10);
    bufp->fullBit(oldp+31,(vlSelf->tb_axi_periph__DOT__dut__DOT__dt_query_toggle));
    bufp->fullBit(oldp+32,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_pending));
    bufp->fullBit(oldp+33,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_en_reg));
    bufp->fullCData(oldp+34,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_gpio_out_reg),8);
    bufp->fullCData(oldp+35,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_gpio_dir_reg),8);
    bufp->fullBit(oldp+36,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg));
    bufp->fullBit(oldp+37,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_fast_reg));
    bufp->fullBit(oldp+38,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_clear_edges_pulse));
    bufp->fullCData(oldp+39,((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_cmd_o) 
                               << 7U) | ((0x40U & (
                                                   ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psCLK_gate) 
                                                    | ((~ 
                                                        ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1) 
                                                         ^ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2))) 
                                                       | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK))) 
                                                   << 6U)) 
                                         | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2) 
                                             << 1U) 
                                            | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1))))),8);
    bufp->fullSData(oldp+40,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key1),16);
    bufp->fullSData(oldp+41,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_pressed_acc),16);
    bufp->fullSData(oldp+42,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_released_acc),16);
    bufp->fullSData(oldp+43,((0xff00U & ((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                          - (IData)(0x80U)) 
                                         << 8U))),16);
    bufp->fullSData(oldp+44,((0xff00U & (((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                           >> 8U) - (IData)(0x80U)) 
                                         << 8U))),16);
    bufp->fullSData(oldp+45,((0xff00U & (((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                           >> 0x10U) 
                                          - (IData)(0x80U)) 
                                         << 8U))),16);
    bufp->fullSData(oldp+46,((0xff00U & (((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                           >> 0x18U) 
                                          - (IData)(0x80U)) 
                                         << 8U))),16);
    bufp->fullCData(oldp+47,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_meta),8);
    bufp->fullCData(oldp+48,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync),8);
    bufp->fullBit(oldp+49,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                                  >> 2U))));
    bufp->fullBit(oldp+50,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                                  >> 5U))));
    bufp->fullSData(oldp+51,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_stb_cnt),9);
    bufp->fullBit(oldp+52,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_stb));
    bufp->fullSData(oldp+53,(((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_fast_reg)
                               ? 0xc7U : 0x18fU)),9);
    bufp->fullSData(oldp+54,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key2),16);
    bufp->fullIData(oldp+55,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1),32);
    bufp->fullIData(oldp+56,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy2),32);
    bufp->fullBit(oldp+57,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psCLK_gate) 
                                  | ((~ ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1) 
                                         ^ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2))) 
                                     | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK))))));
    bufp->fullBit(oldp+58,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_cmd_o));
    bufp->fullBit(oldp+59,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1));
    bufp->fullBit(oldp+60,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2));
    bufp->fullSData(oldp+61,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_prev_key1),16);
    bufp->fullQData(oldp+62,((((QData)((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key2)) 
                               << 0x20U) | (QData)((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy2)))),48);
    bufp->fullQData(oldp+64,(vlSelf->tb_axi_periph__DOT__dut__DOT__cycle_counter),64);
    bufp->fullCData(oldp+66,(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_color_mode_reg),3);
    bufp->fullSData(oldp+67,(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_width_reg),10);
    bufp->fullSData(oldp+68,(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_height_reg),10);
    bufp->fullSData(oldp+69,(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_stride_reg),16);
    bufp->fullSData(oldp+70,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_id_reg),16);
    bufp->fullIData(oldp+71,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_offset_reg),32);
    bufp->fullIData(oldp+72,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_bridge_addr_reg),32);
    bufp->fullIData(oldp+73,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_length_reg),32);
    bufp->fullIData(oldp+74,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_param_addr_reg),32);
    bufp->fullIData(oldp+75,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_resp_addr_reg),32);
    bufp->fullCData(oldp+76,(vlSelf->tb_axi_periph__DOT__dut__DOT__pal_index_reg),8);
    bufp->fullIData(oldp+77,(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_period),32);
    bufp->fullIData(oldp+78,(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_counter),32);
    bufp->fullBit(oldp+79,(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_enable));
    bufp->fullBit(oldp+80,(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_irq_pending));
    bufp->fullCData(oldp+81,(vlSelf->tb_axi_periph__DOT__dut__DOT__irq_mask),6);
    bufp->fullBit(oldp+82,(vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_irq_pending));
    bufp->fullBit(oldp+83,(vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_irq_pending));
    bufp->fullBit(oldp+84,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_pending));
    bufp->fullCData(oldp+85,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx),2);
    bufp->fullCData(oldp+86,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_ready_idx),2);
    bufp->fullBit(oldp+87,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame));
    bufp->fullBit(oldp+88,(vlSelf->tb_axi_periph__DOT__dut__DOT__term_fb_active));
    bufp->fullBit(oldp+89,(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_keep_app));
    bufp->fullSData(oldp+90,(vlSelf->tb_axi_periph__DOT__dut__DOT__vrr_v_total_reg),10);
    bufp->fullIData(oldp+91,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_app_addr),25);
    bufp->fullBit(oldp+92,(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_show_term));
    bufp->fullCData(oldp+93,(vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_allcomplete_sync),3);
    bufp->fullBit(oldp+94,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_allcomplete_sync) 
                                  >> 2U))));
    bufp->fullIData(oldp+95,(vlSelf->tb_axi_periph__DOT__dut__DOT__bridge_dbg_wcnt_meta),32);
    bufp->fullIData(oldp+96,(vlSelf->tb_axi_periph__DOT__dut__DOT__bridge_dbg_wcnt_s),32);
    bufp->fullCData(oldp+97,(vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_sync),3);
    bufp->fullBit(oldp+98,(vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_rising));
    bufp->fullCData(oldp+99,(vlSelf->tb_axi_periph__DOT__dut__DOT__early_vblank_sync),3);
    bufp->fullBit(oldp+100,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__early_vblank_sync) 
                                   >> 2U))));
    bufp->fullCData(oldp+101,(vlSelf->tb_axi_periph__DOT__dut__DOT__os_inmenu_sync),3);
    bufp->fullBit(oldp+102,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__os_inmenu_sync) 
                                   >> 2U))));
    bufp->fullSData(oldp+103,(vlSelf->tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__Vstatic__even_stride),16);
    bufp->fullBit(oldp+104,(vlSelf->tb_axi_periph__DOT__dut__DOT__rtc_valid_meta));
    bufp->fullBit(oldp+105,(vlSelf->tb_axi_periph__DOT__dut__DOT__rtc_valid_s));
    bufp->fullIData(oldp+106,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_meta),32);
    bufp->fullIData(oldp+107,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_s),32);
    bufp->fullIData(oldp+108,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_meta),32);
    bufp->fullIData(oldp+109,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_s),32);
    bufp->fullSData(oldp+110,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_meta),16);
    bufp->fullSData(oldp+111,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_s),16);
    bufp->fullIData(oldp+112,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_meta),32);
    bufp->fullIData(oldp+113,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_s),32);
    bufp->fullIData(oldp+114,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_meta),32);
    bufp->fullIData(oldp+115,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_s),32);
    bufp->fullSData(oldp+116,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_meta),16);
    bufp->fullSData(oldp+117,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_s),16);
    bufp->fullIData(oldp+118,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_meta),32);
    bufp->fullIData(oldp+119,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_s),32);
    bufp->fullIData(oldp+120,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_meta),32);
    bufp->fullIData(oldp+121,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_s),32);
    bufp->fullSData(oldp+122,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_meta),16);
    bufp->fullSData(oldp+123,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_s),16);
    bufp->fullIData(oldp+124,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_meta),32);
    bufp->fullIData(oldp+125,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_s),32);
    bufp->fullIData(oldp+126,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_meta),32);
    bufp->fullIData(oldp+127,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_s),32);
    bufp->fullSData(oldp+128,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_meta),16);
    bufp->fullSData(oldp+129,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_s),16);
    bufp->fullCData(oldp+130,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask),4);
    bufp->fullBit(oldp+131,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_overflow));
    bufp->fullSData(oldp+132,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_seq),16);
    bufp->fullIData(oldp+133,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key0),32);
    bufp->fullIData(oldp+134,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key1),32);
    bufp->fullIData(oldp+135,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key2),32);
    bufp->fullIData(oldp+136,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key3),32);
    bufp->fullIData(oldp+137,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy0),32);
    bufp->fullIData(oldp+138,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy1),32);
    bufp->fullIData(oldp+139,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy2),32);
    bufp->fullIData(oldp+140,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy3),32);
    bufp->fullSData(oldp+141,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig0),16);
    bufp->fullSData(oldp+142,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig1),16);
    bufp->fullSData(oldp+143,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig2),16);
    bufp->fullSData(oldp+144,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig3),16);
    bufp->fullCData(oldp+145,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot),2);
    bufp->fullCData(oldp+146,(((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_s) 
                                 != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig0)) 
                                << 2U) | (((vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_s 
                                            != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy0) 
                                           << 1U) | 
                                          (vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_s 
                                           != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key0)))),3);
    bufp->fullCData(oldp+147,(((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_s) 
                                 != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig1)) 
                                << 2U) | (((vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_s 
                                            != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy1) 
                                           << 1U) | 
                                          (vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_s 
                                           != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key1)))),3);
    bufp->fullCData(oldp+148,(((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_s) 
                                 != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig2)) 
                                << 2U) | (((vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_s 
                                            != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy2) 
                                           << 1U) | 
                                          (vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_s 
                                           != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key2)))),3);
    bufp->fullCData(oldp+149,(((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_s) 
                                 != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig3)) 
                                << 2U) | (((vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_s 
                                            != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy3) 
                                           << 1U) | 
                                          (vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_s 
                                           != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key3)))),3);
    bufp->fullCData(oldp+150,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fields),3);
    bufp->fullBit(oldp+151,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_valid));
    bufp->fullBit(oldp+152,((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count))));
    bufp->fullBit(oldp+153,((0x10U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count))));
    bufp->fullCData(oldp+154,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count),5);
    bufp->fullQData(oldp+155,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_dout),64);
    bufp->fullQData(oldp+157,((0x100000000000000ULL 
                               | (((QData)((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot)) 
                                   << 0x30U) | (((QData)((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fields)) 
                                                 << 0x28U) 
                                                | (QData)((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_seq)))))),64);
    bufp->fullBit(oldp+159,(((0x10U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
                             & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_valid))));
    bufp->fullBit(oldp+160,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_clear));
    bufp->fullBit(oldp+161,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_cmd_active));
    bufp->fullBit(oldp+162,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_latched));
    bufp->fullBit(oldp+163,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_latched));
    bufp->fullBit(oldp+164,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_seen_low));
    bufp->fullBit(oldp+165,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_seen_low));
    bufp->fullCData(oldp+166,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_err_latched),3);
    bufp->fullSData(oldp+167,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_wr_ptr),10);
    bufp->fullSData(oldp+168,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr),10);
    bufp->fullSData(oldp+169,((0x3ffU & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_wr_ptr) 
                                         - (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr)))),10);
    bufp->fullBit(oldp+170,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_empty));
    bufp->fullBit(oldp+171,((0x3ffU == (0x3ffU & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_wr_ptr) 
                                                  - (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr))))));
    bufp->fullCData(oldp+172,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_fifo
                              [vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr]),8);
    bufp->fullQData(oldp+173,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[0]),64);
    bufp->fullQData(oldp+175,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[1]),64);
    bufp->fullQData(oldp+177,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[2]),64);
    bufp->fullQData(oldp+179,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[3]),64);
    bufp->fullQData(oldp+181,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[4]),64);
    bufp->fullQData(oldp+183,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[5]),64);
    bufp->fullQData(oldp+185,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[6]),64);
    bufp->fullQData(oldp+187,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[7]),64);
    bufp->fullQData(oldp+189,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[8]),64);
    bufp->fullQData(oldp+191,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[9]),64);
    bufp->fullQData(oldp+193,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[10]),64);
    bufp->fullQData(oldp+195,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[11]),64);
    bufp->fullQData(oldp+197,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[12]),64);
    bufp->fullQData(oldp+199,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[13]),64);
    bufp->fullQData(oldp+201,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[14]),64);
    bufp->fullQData(oldp+203,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[15]),64);
    bufp->fullCData(oldp+205,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__wr_ptr),4);
    bufp->fullCData(oldp+206,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr),4);
    bufp->fullIData(oldp+207,(vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_a_reg2),32);
    bufp->fullBit(oldp+208,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                                   >> 4U))));
    bufp->fullCData(oldp+209,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx0),8);
    bufp->fullCData(oldp+210,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1),8);
    bufp->fullCData(oldp+211,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2),8);
    bufp->fullCData(oldp+212,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx3),8);
    bufp->fullCData(oldp+213,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx4),8);
    bufp->fullCData(oldp+214,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx5),8);
    bufp->fullCData(oldp+215,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx6),8);
    bufp->fullCData(oldp+216,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__RXD_ID),8);
    bufp->fullBit(oldp+217,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__att1r));
    bufp->fullBit(oldp+218,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__att2r));
    bufp->fullBit(oldp+219,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__no_gamepad));
    bufp->fullBit(oldp+220,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK));
    bufp->fullBit(oldp+221,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__R_CE));
    bufp->fullBit(oldp+222,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__F_CE));
    bufp->fullBit(oldp+223,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__ack_r));
    bufp->fullBit(oldp+224,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__device_id_type));
    bufp->fullCData(oldp+225,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r),4);
    bufp->fullBit(oldp+226,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXWT));
    bufp->fullBit(oldp+227,(((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT____VdfgTmp_h6d2737cc__0) 
                             & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXWT))));
    bufp->fullBit(oldp+228,(((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psCLK_gate)) 
                             & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT____VdfgTmp_h6d2737cc__0))));
    bufp->fullCData(oldp+229,(((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))
                                ? 1U : ((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))
                                         ? 0x42U : 0U))),8);
    bufp->fullCData(oldp+230,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXD_DAT),8);
    bufp->fullBit(oldp+231,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXWT_r));
    bufp->fullBit(oldp+232,(((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT____VdfgTmp_h6d2737cc__0) 
                             & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXSET))));
    bufp->fullSData(oldp+233,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer),11);
    bufp->fullBit(oldp+234,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__RXWT));
    bufp->fullBit(oldp+235,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXWT));
    bufp->fullBit(oldp+236,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXSET));
    bufp->fullBit(oldp+237,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psCLK_gate));
    bufp->fullCData(oldp+238,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__rxd__DOT__sp),8);
    bufp->fullCData(oldp+239,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps),8);
    bufp->fullIData(oldp+240,(vlSelf->tb_axi_periph__DOT__mix_pos_readback_r),22);
    bufp->fullBit(oldp+241,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_done));
    bufp->fullIData(oldp+242,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_rx_data),32);
    bufp->fullBit(oldp+243,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk));
    bufp->fullBit(oldp+244,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_mosi));
    bufp->fullBit(oldp+245,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_latch));
    bufp->fullCData(oldp+246,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_ack_sync),3);
    bufp->fullCData(oldp+247,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_done_sync),3);
    bufp->fullCData(oldp+248,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync[0]),3);
    bufp->fullCData(oldp+249,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync[1]),3);
    bufp->fullCData(oldp+250,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync[2]),3);
    bufp->fullBit(oldp+251,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_ack_sync) 
                                   >> 2U))));
    bufp->fullBit(oldp+252,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_done_sync) 
                                   >> 2U))));
    bufp->fullCData(oldp+253,(((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync
                                [2U]) | ((2U & (vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync
                                                [1U] 
                                                >> 1U)) 
                                         | (1U & (vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync
                                                  [0U] 
                                                  >> 2U))))),3);
    bufp->fullSData(oldp+254,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt),16);
    bufp->fullCData(oldp+255,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__bits_left),5);
    bufp->fullCData(oldp+256,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__total_bits),5);
    bufp->fullIData(oldp+257,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__tx_sr),32);
    bufp->fullBit(oldp+258,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__miso_sel));
    bufp->fullBit(oldp+259,(vlSelf->tb_axi_periph__DOT__pal_commit_w));
    bufp->fullCData(oldp+260,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_mode_reg),2);
    bufp->fullSData(oldp+261,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_div_reg),16);
    bufp->fullIData(oldp+262,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_tx_reg),32);
    bufp->fullBit(oldp+263,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_start_pulse));
    bufp->fullCData(oldp+264,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_bit_count_reg),5);
    bufp->fullBit(oldp+265,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_latch_en_reg));
    bufp->fullIData(oldp+266,(vlSelf->tb_axi_periph__DOT__gpu_reg_rdata),32);
    bufp->fullBit(oldp+267,(vlSelf->tb_axi_periph__DOT__cart_reg_wr));
    bufp->fullCData(oldp+268,((0x1fU & ((vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                         >> 2U) - (IData)(8U)))),5);
    bufp->fullIData(oldp+269,(vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata),32);
    bufp->fullIData(oldp+270,(vlSelf->tb_axi_periph__DOT__cart_reg_rdata),32);
    bufp->fullCData(oldp+271,((0x1fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                        >> 2U))),5);
    bufp->fullBit(oldp+272,(vlSelf->tb_axi_periph__DOT__dut__DOT__audio_sample_wr));
    bufp->fullIData(oldp+273,(vlSelf->tb_axi_periph__DOT__dut__DOT__audio_sample_data),32);
    bufp->fullBit(oldp+274,(vlSelf->tb_axi_periph__DOT__dut__DOT__cram0_mode));
    bufp->fullBit(oldp+275,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_enable));
    bufp->fullBit(oldp+276,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_wr));
    bufp->fullCData(oldp+277,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_sel),5);
    bufp->fullCData(oldp+278,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_field),4);
    bufp->fullIData(oldp+279,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_wdata),32);
    bufp->fullBit(oldp+280,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_irq_clear_wr));
    bufp->fullIData(oldp+281,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_irq_clear),32);
    bufp->fullCData(oldp+282,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_master_vol),8);
    bufp->fullCData(oldp+283,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_0),8);
    bufp->fullCData(oldp+284,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_1),8);
    bufp->fullCData(oldp+285,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_2),8);
    bufp->fullCData(oldp+286,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_3),8);
    bufp->fullQData(oldp+287,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_group_packed),64);
    bufp->fullBit(oldp+289,(vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_wr));
    bufp->fullBit(oldp+290,(vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_rd));
    bufp->fullCData(oldp+291,(vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_addr),5);
    bufp->fullIData(oldp+292,(vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_wdata),32);
    bufp->fullBit(oldp+293,((0U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state))));
    bufp->fullBit(oldp+294,(((6U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state)) 
                             & (0U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_wstrb)))));
    bufp->fullBit(oldp+295,(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire));
    bufp->fullBit(oldp+296,((1U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                   >> 8U))));
    bufp->fullCData(oldp+297,((0x3fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                        >> 2U))),6);
    bufp->fullBit(oldp+298,(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_cart_window));
    bufp->fullBit(oldp+299,(((vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 8U) & ((1U <= (0xfU 
                                                & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                   >> 4U))) 
                                        & (4U >= (0xfU 
                                                  & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                     >> 4U)))))));
    bufp->fullBit(oldp+300,(vlSelf->tb_axi_periph__DOT__dut__DOT__low_page_cont_read));
    bufp->fullCData(oldp+301,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_sel),4);
    bufp->fullCData(oldp+302,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_field_sel),2);
    bufp->fullBit(oldp+303,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_fifo_we));
    bufp->fullCData(oldp+304,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_fifo_din),8);
    bufp->fullIData(oldp+305,(((0U == (3U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                             >> 2U)))
                                ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cram0_mode)
                                : 0U)),32);
    bufp->fullBit(oldp+306,((IData)((0x880U == (0x880U 
                                                & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)))));
    bufp->fullIData(oldp+307,(((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? 0U : ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                         ? ((0x10U 
                                             & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                             ? ((8U 
                                                 & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                 ? 0U
                                                 : 
                                                ((4U 
                                                  & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                  ? 0U
                                                  : 0xa5a51234U))
                                             : ((8U 
                                                 & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                 ? 0U
                                                 : 
                                                ((4U 
                                                  & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                  ? 0xc0U
                                                  : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_enable))))
                                         : ((0x10U 
                                             & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                             ? ((8U 
                                                 & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                 ? 
                                                ((4U 
                                                  & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                  ? 0U
                                                  : (IData)(
                                                            (vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_group_packed 
                                                             >> 0x20U)))
                                                 : 
                                                ((4U 
                                                  & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                  ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_group_packed)
                                                  : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_3)))
                                             : ((8U 
                                                 & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                 ? 
                                                ((4U 
                                                  & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                  ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_2)
                                                  : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_1))
                                                 : 
                                                ((4U 
                                                  & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                  ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_0)
                                                  : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_master_vol))))))),32);
    bufp->fullIData(oldp+308,(vlSelf->tb_axi_periph__DOT__dut__DOT__mixer_rdata),32);
    bufp->fullIData(oldp+309,(vlSelf->tb_axi_periph__DOT__dut__DOT__periph_rd_data_r),32);
    bufp->fullCData(oldp+310,(vlSelf->tb_axi_periph__DOT__dut__DOT__state),3);
    bufp->fullIData(oldp+311,(vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr),32);
    bufp->fullCData(oldp+312,(vlSelf->tb_axi_periph__DOT__dut__DOT__req_wstrb),4);
    bufp->fullCData(oldp+313,(vlSelf->tb_axi_periph__DOT__dut__DOT__burst_len),8);
    bufp->fullCData(oldp+314,(vlSelf->tb_axi_periph__DOT__dut__DOT__burst_count),8);
    bufp->fullCData(oldp+315,(vlSelf->tb_axi_periph__DOT__dut__DOT__awburst_latched),2);
    bufp->fullBit(oldp+316,((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__awburst_latched))));
    bufp->fullCData(oldp+317,(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region),4);
    bufp->fullBit(oldp+318,((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
    bufp->fullBit(oldp+319,((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
    bufp->fullBit(oldp+320,((3U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
    bufp->fullBit(oldp+321,((4U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
    bufp->fullBit(oldp+322,((5U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
    bufp->fullBit(oldp+323,((6U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
    bufp->fullBit(oldp+324,((7U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
    bufp->fullBit(oldp+325,((8U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
    bufp->fullBit(oldp+326,((9U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
    bufp->fullIData(oldp+327,(((0U == (0xfU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                               >> 2U)))
                                ? 0x125U : ((1U == 
                                             (0xfU 
                                              & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                 >> 2U)))
                                             ? 0x33334444U
                                             : ((2U 
                                                 == 
                                                 (0xfU 
                                                  & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                     >> 2U)))
                                                 ? 0x11112222U
                                                 : 
                                                ((3U 
                                                  == 
                                                  (0xfU 
                                                   & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                      >> 2U)))
                                                  ? 0xabc00U
                                                  : 
                                                 ((4U 
                                                   == 
                                                   (0xfU 
                                                    & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                       >> 2U)))
                                                   ? 0x77778888U
                                                   : 
                                                  ((5U 
                                                    == 
                                                    (0xfU 
                                                     & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                        >> 2U)))
                                                    ? 0x55556666U
                                                    : 
                                                   ((6U 
                                                     == 
                                                     (0xfU 
                                                      & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                         >> 2U)))
                                                     ? 0xbbbbccccU
                                                     : 
                                                    ((7U 
                                                      == 
                                                      (0xfU 
                                                       & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                          >> 2U)))
                                                      ? 0x9999aaaaU
                                                      : 
                                                     ((8U 
                                                       == 
                                                       (0xfU 
                                                        & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                           >> 2U)))
                                                       ? 0xface1234U
                                                       : 
                                                      ((9U 
                                                        == 
                                                        (0xfU 
                                                         & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                            >> 2U)))
                                                        ? 0xe1f05678U
                                                        : 0U))))))))))),32);
    bufp->fullBit(oldp+328,(vlSelf->tb_axi_periph__DOT__dut__DOT__beat_is_last));
    bufp->fullIData(oldp+329,(vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr_plus4),32);
    bufp->fullIData(oldp+330,(vlSelf->tb_axi_periph__DOT__dut__DOT__periph_next_addr),32);
    bufp->fullSData(oldp+331,((0x1fffU & ((IData)(1U) 
                                          + (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                             >> 2U)))),13);
    bufp->fullCData(oldp+332,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state),3);
    bufp->fullCData(oldp+333,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_out_mux),8);
    bufp->fullCData(oldp+334,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_dir_mux),8);
    bufp->fullIData(oldp+335,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_key_rdata),32);
    bufp->fullIData(oldp+336,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_joy_rdata),32);
    bufp->fullSData(oldp+337,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_trig_rdata),16);
    bufp->fullBit(oldp+338,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_irq_en_rdata));
    bufp->fullIData(oldp+339,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_rdata),32);
    bufp->fullBit(oldp+340,(vlSelf->clk));
    bufp->fullBit(oldp+341,(vlSelf->reset_n));
    bufp->fullBit(oldp+342,(vlSelf->s_axi_arvalid));
    bufp->fullBit(oldp+343,(vlSelf->s_axi_arready));
    bufp->fullIData(oldp+344,(vlSelf->s_axi_araddr),32);
    bufp->fullCData(oldp+345,(vlSelf->s_axi_arlen),8);
    bufp->fullBit(oldp+346,(vlSelf->s_axi_rvalid));
    bufp->fullBit(oldp+347,(vlSelf->s_axi_rready));
    bufp->fullIData(oldp+348,(vlSelf->s_axi_rdata),32);
    bufp->fullCData(oldp+349,(vlSelf->s_axi_rresp),2);
    bufp->fullBit(oldp+350,(vlSelf->s_axi_rlast));
    bufp->fullBit(oldp+351,(vlSelf->s_axi_awvalid));
    bufp->fullBit(oldp+352,(vlSelf->s_axi_awready));
    bufp->fullIData(oldp+353,(vlSelf->s_axi_awaddr),32);
    bufp->fullCData(oldp+354,(vlSelf->s_axi_awlen),8);
    bufp->fullCData(oldp+355,(vlSelf->s_axi_awburst),2);
    bufp->fullBit(oldp+356,(vlSelf->s_axi_wvalid));
    bufp->fullBit(oldp+357,(vlSelf->s_axi_wready));
    bufp->fullIData(oldp+358,(vlSelf->s_axi_wdata),32);
    bufp->fullCData(oldp+359,(vlSelf->s_axi_wstrb),4);
    bufp->fullBit(oldp+360,(vlSelf->s_axi_wlast));
    bufp->fullBit(oldp+361,(vlSelf->s_axi_bvalid));
    bufp->fullBit(oldp+362,(vlSelf->s_axi_bready));
    bufp->fullCData(oldp+363,(vlSelf->s_axi_bresp),2);
    bufp->fullCData(oldp+364,(vlSelf->dbg_state),4);
    bufp->fullBit(oldp+365,(vlSelf->dbg_bram_hold));
    bufp->fullBit(oldp+366,(vlSelf->dbg_can_push_beat));
    bufp->fullBit(oldp+367,(vlSelf->dbg_gpu_reg_wr));
    bufp->fullCData(oldp+368,(vlSelf->dbg_gpu_reg_addr),4);
    bufp->fullIData(oldp+369,(vlSelf->dbg_gpu_reg_wdata),32);
    bufp->fullCData(oldp+370,(vlSelf->dbg_save_dt_slot),4);
    bufp->fullIData(oldp+371,(vlSelf->dbg_save_dt_size),32);
    bufp->fullBit(oldp+372,(vlSelf->dbg_save_dt_commit));
    bufp->fullCData(oldp+373,(vlSelf->dbg_analogizer_wr_toggle),3);
    bufp->fullIData(oldp+374,(vlSelf->dbg_analogizer_wr_settings),32);
    bufp->fullIData(oldp+375,(vlSelf->dbg_analogizer_wr_hoffset),32);
    bufp->fullIData(oldp+376,(vlSelf->dbg_analogizer_wr_voffset),32);
    bufp->fullCData(oldp+377,(vlSelf->dbg_snac_pin_out),8);
    bufp->fullCData(oldp+378,(vlSelf->dbg_snac_pin_dir),8);
    bufp->fullBit(oldp+379,(vlSelf->dbg_snac_enable));
    bufp->fullCData(oldp+380,(vlSelf->dbg_cart_wr_count),8);
    bufp->fullCData(oldp+381,(vlSelf->dbg_cart_rd_count),8);
    bufp->fullCData(oldp+382,(vlSelf->dbg_cart_last_addr),5);
    bufp->fullIData(oldp+383,(vlSelf->dbg_cart_last_wdata),32);
    bufp->fullBit(oldp+384,(vlSelf->dbg_pal_commit));
    bufp->fullIData(oldp+385,(vlSelf->dbg_analog_fb_addr),25);
    bufp->fullCData(oldp+386,(vlSelf->dbg_analog_color_mode),3);
    bufp->fullSData(oldp+387,(vlSelf->dbg_analog_fb_width),10);
    bufp->fullSData(oldp+388,(vlSelf->dbg_analog_fb_height),10);
    bufp->fullSData(oldp+389,(vlSelf->dbg_analog_fb_stride),16);
    bufp->fullCData(oldp+390,(vlSelf->dbg_pal_commit_count),8);
    bufp->fullBit(oldp+391,(vlSelf->pal_busy_in));
    bufp->fullIData(oldp+392,(vlSelf->analogizer_settings_in),32);
    bufp->fullBit(oldp+393,(vlSelf->gpu_swap_req_in));
    bufp->fullCData(oldp+394,(vlSelf->gpu_swap_idx_in),2);
    bufp->fullBit(oldp+395,(vlSelf->vsync_in));
    bufp->fullBit(oldp+396,(vlSelf->early_vblank_in));
    bufp->fullBit(oldp+397,(vlSelf->os_inmenu_in));
    bufp->fullBit(oldp+398,(vlSelf->target_dataslot_ack_in));
    bufp->fullBit(oldp+399,(vlSelf->target_dataslot_done_in));
    bufp->fullCData(oldp+400,(vlSelf->target_dataslot_err_in),3);
    bufp->fullBit(oldp+401,(vlSelf->bridge_wr_idle_in));
    bufp->fullIData(oldp+402,(vlSelf->cont1_key_in),32);
    bufp->fullIData(oldp+403,(vlSelf->cont1_joy_in),32);
    bufp->fullSData(oldp+404,(vlSelf->cont1_trig_in),16);
    bufp->fullIData(oldp+405,(vlSelf->cont2_key_in),32);
    bufp->fullIData(oldp+406,(vlSelf->cont2_joy_in),32);
    bufp->fullSData(oldp+407,(vlSelf->cont2_trig_in),16);
    bufp->fullIData(oldp+408,(vlSelf->cont3_key_in),32);
    bufp->fullIData(oldp+409,(vlSelf->cont3_joy_in),32);
    bufp->fullSData(oldp+410,(vlSelf->cont3_trig_in),16);
    bufp->fullIData(oldp+411,(vlSelf->cont4_key_in),32);
    bufp->fullIData(oldp+412,(vlSelf->cont4_joy_in),32);
    bufp->fullSData(oldp+413,(vlSelf->cont4_trig_in),16);
    bufp->fullIData(oldp+414,(vlSelf->dt_query_data_in),32);
    bufp->fullBit(oldp+415,(vlSelf->dt_query_valid_in));
    bufp->fullCData(oldp+416,(vlSelf->snac_pin_in_in),8);
    bufp->fullBit(oldp+417,(vlSelf->ext_irq_out));
    bufp->fullBit(oldp+418,(((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_cart_window) 
                             & ((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state)) 
                                & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat)))));
    bufp->fullSData(oldp+419,(((1U & (IData)(((0U != 
                                               (0x801fU 
                                                & vlSelf->analogizer_settings_in)) 
                                              | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_en_reg))))
                                ? (((vlSelf->analogizer_settings_in 
                                     >> 0xfU) & ((4U 
                                                  == 
                                                  (0xfU 
                                                   & (vlSelf->analogizer_settings_in 
                                                      >> 0xaU))) 
                                                 | (0xcU 
                                                    == 
                                                    (0xfU 
                                                     & (vlSelf->analogizer_settings_in 
                                                        >> 0xaU)))))
                                    ? 0x276U : 0x20dU)
                                : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__vrr_v_total_reg))),10);
    bufp->fullBit(oldp+420,((1U & (~ (IData)(vlSelf->reset_n)))));
    bufp->fullIData(oldp+421,(vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_a_reg),32);
    bufp->fullSData(oldp+422,(vlSelf->tb_axi_periph__DOT__dut__DOT__ram_addr_mux),13);
    bufp->fullIData(oldp+423,(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata),32);
    bufp->fullBit(oldp+424,((1U & (vlSelf->analogizer_settings_in 
                                   >> 0xfU))));
    bufp->fullCData(oldp+425,((0xfU & (vlSelf->analogizer_settings_in 
                                       >> 0xaU))),4);
    bufp->fullBit(oldp+426,(((4U == (0xfU & (vlSelf->analogizer_settings_in 
                                             >> 0xaU))) 
                             | (0xcU == (0xfU & (vlSelf->analogizer_settings_in 
                                                 >> 0xaU))))));
    bufp->fullBit(oldp+427,((0U != (0x1fU & vlSelf->analogizer_settings_in))));
    bufp->fullBit(oldp+428,((1U & (IData)(((0U != (0x801fU 
                                                   & vlSelf->analogizer_settings_in)) 
                                           | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_en_reg))))));
    bufp->fullSData(oldp+429,((((vlSelf->analogizer_settings_in 
                                 >> 0xfU) & ((4U == 
                                              (0xfU 
                                               & (vlSelf->analogizer_settings_in 
                                                  >> 0xaU))) 
                                             | (0xcU 
                                                == 
                                                (0xfU 
                                                 & (vlSelf->analogizer_settings_in 
                                                    >> 0xaU)))))
                                ? 0x276U : 0x20dU)),10);
    bufp->fullBit(oldp+430,(((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state)) 
                             & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat) 
                                & ((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region)) 
                                   & (IData)(((0x154U 
                                               == (0x1fcU 
                                                   & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) 
                                              & (0U 
                                                 != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)))))))));
    bufp->fullIData(oldp+431,(((0U == (3U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                             >> 2U)))
                                ? (0xbU | ((0x3ff00U 
                                            & (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_wr_ptr) 
                                                - (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr)) 
                                               << 8U)) 
                                           | (4U & 
                                              ((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_empty)) 
                                               << 2U))))
                                : ((2U == (3U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                 >> 2U)))
                                    ? vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_fifo
                                   [vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr]
                                    : 0U))),32);
    bufp->fullBit(oldp+432,(vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat));
    bufp->fullBit(oldp+433,((1U & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat)))));
    bufp->fullCData(oldp+434,(vlSelf->tb_axi_periph__DOT__dut__DOT__ar_region),4);
    bufp->fullCData(oldp+435,(vlSelf->tb_axi_periph__DOT__dut__DOT__aw_region),4);
    bufp->fullBit(oldp+436,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_pop));
    bufp->fullBit(oldp+437,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_push));
    bufp->fullBit(oldp+438,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__miso_sel)
                                    ? ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                                       >> 5U) : ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                                                 >> 2U)))));
}
