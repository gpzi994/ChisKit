// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtb_core_bridge_cmd.h for the primary calling header

#include "Vtb_core_bridge_cmd__pch.h"
#include "Vtb_core_bridge_cmd___024root.h"

VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___eval_static__TOP(Vtb_core_bridge_cmd___024root* vlSelf);

VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___eval_static(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval_static\n"); );
    // Body
    Vtb_core_bridge_cmd___024root___eval_static__TOP(vlSelf);
    vlSelf->__Vm_traceActivity[1U] = 1U;
    vlSelf->__Vm_traceActivity[0U] = 1U;
}

VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___eval_static__TOP(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval_static__TOP\n"); );
    // Body
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_4 = 0x20U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_8 = 0x40U;
}

VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___eval_initial__TOP(Vtb_core_bridge_cmd___024root* vlSelf);

VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___eval_initial(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval_initial\n"); );
    // Body
    Vtb_core_bridge_cmd___024root___eval_initial__TOP(vlSelf);
    vlSelf->__Vm_traceActivity[1U] = 1U;
    vlSelf->__Vm_traceActivity[0U] = 1U;
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = vlSelf->clk;
}

VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___eval_initial__TOP(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval_initial__TOP\n"); );
    // Body
    vlSelf->reset_n = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestread = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestwrite = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dataslot_update = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dataslot_allcomplete = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__rtc_valid = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__savestate_start = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__savestate_load = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__osnotify_inmenu = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_toggle = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_value = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_valid = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_last_value = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_count = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_enter_toggle = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_exit_toggle = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_enter_count = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_exit_count = 0U;
    vlSelf->shutdown_pending = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_queue = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__warm_reset_ready_pending = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_queue = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_queue = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_queue = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_queue = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_queue = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_ack = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_done = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_err = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_addr = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__i = 0U;
    while (VL_GTS_III(32, 0x400U, vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__i)) {
        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem[(0x3ffU 
                                                                                & vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__i)] = 0x13U;
        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__i 
            = ((IData)(1U) + vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__i);
    }
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_a_reg = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_b_reg = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_a_reg2 = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_b_reg2 = 0U;
}

VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___eval_final(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval_final\n"); );
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___dump_triggers__stl(Vtb_core_bridge_cmd___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtb_core_bridge_cmd___024root___eval_phase__stl(Vtb_core_bridge_cmd___024root* vlSelf);

VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___eval_settle(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval_settle\n"); );
    // Init
    IData/*31:0*/ __VstlIterCount;
    CData/*0:0*/ __VstlContinue;
    // Body
    __VstlIterCount = 0U;
    vlSelf->__VstlFirstIteration = 1U;
    __VstlContinue = 1U;
    while (__VstlContinue) {
        if (VL_UNLIKELY((0x64U < __VstlIterCount))) {
#ifdef VL_DEBUG
            Vtb_core_bridge_cmd___024root___dump_triggers__stl(vlSelf);
#endif
            VL_FATAL_MT("tb_core_bridge_cmd.v", 9, "", "Settle region did not converge.");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        __VstlContinue = 0U;
        if (Vtb_core_bridge_cmd___024root___eval_phase__stl(vlSelf)) {
            __VstlContinue = 1U;
        }
        vlSelf->__VstlFirstIteration = 0U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___dump_triggers__stl(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VstlTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VstlTriggered.word(0U))) {
        VL_DBG_MSGF("         'stl' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___stl_sequent__TOP__0(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___stl_sequent__TOP__0\n"); );
    // Body
    vlSelf->target_dataslot_ack_out = vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_ack;
    vlSelf->target_dataslot_done_out = vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_done;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__shutdown_timeout_reached 
        = (1U & (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__shutdown_timeout 
                 >> 0x16U));
    if (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__endian_little_s) {
        vlSelf->bridge_rd_data = ((vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                                   << 0x18U) | ((0xff0000U 
                                                 & (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                                                    << 8U)) 
                                                | ((0xff00U 
                                                    & (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                                                       >> 8U)) 
                                                   | (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                                                      >> 0x18U))));
        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in 
            = ((vlSelf->bridge_wr_data << 0x18U) | 
               ((0xff0000U & (vlSelf->bridge_wr_data 
                              << 8U)) | ((0xff00U & 
                                          (vlSelf->bridge_wr_data 
                                           >> 8U)) 
                                         | (vlSelf->bridge_wr_data 
                                            >> 0x18U))));
    } else {
        vlSelf->bridge_rd_data = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out;
        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in 
            = vlSelf->bridge_wr_data;
    }
}

VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___eval_stl(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval_stl\n"); );
    // Body
    if ((1ULL & vlSelf->__VstlTriggered.word(0U))) {
        Vtb_core_bridge_cmd___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___eval_triggers__stl(Vtb_core_bridge_cmd___024root* vlSelf);

VL_ATTR_COLD bool Vtb_core_bridge_cmd___024root___eval_phase__stl(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval_phase__stl\n"); );
    // Init
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtb_core_bridge_cmd___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = vlSelf->__VstlTriggered.any();
    if (__VstlExecute) {
        Vtb_core_bridge_cmd___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___dump_triggers__ico(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___dump_triggers__ico\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VicoTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VicoTriggered.word(0U))) {
        VL_DBG_MSGF("         'ico' region trigger index 0 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___dump_triggers__act(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VactTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 0 is active: @(posedge clk)\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___dump_triggers__nba(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___dump_triggers__nba\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VnbaTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 0 is active: @(posedge clk)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___ctor_var_reset(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___ctor_var_reset\n"); );
    // Body
    vlSelf->clk = VL_RAND_RESET_I(1);
    vlSelf->bridge_endian_little = VL_RAND_RESET_I(1);
    vlSelf->bridge_addr = VL_RAND_RESET_I(32);
    vlSelf->bridge_rd = VL_RAND_RESET_I(1);
    vlSelf->bridge_rd_data = VL_RAND_RESET_I(32);
    vlSelf->bridge_wr = VL_RAND_RESET_I(1);
    vlSelf->bridge_wr_data = VL_RAND_RESET_I(32);
    vlSelf->status_boot_done = VL_RAND_RESET_I(1);
    vlSelf->status_setup_done = VL_RAND_RESET_I(1);
    vlSelf->status_running = VL_RAND_RESET_I(1);
    vlSelf->shutdown_ack_s = VL_RAND_RESET_I(1);
    vlSelf->target_dataslot_flush_in = VL_RAND_RESET_I(1);
    vlSelf->target_dataslot_id_in = VL_RAND_RESET_I(16);
    vlSelf->target_dataslot_ack_out = VL_RAND_RESET_I(1);
    vlSelf->target_dataslot_done_out = VL_RAND_RESET_I(1);
    vlSelf->reset_n = VL_RAND_RESET_I(1);
    vlSelf->shutdown_pending = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestread = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestread_id = VL_RAND_RESET_I(16);
    vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestwrite = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestwrite_id = VL_RAND_RESET_I(16);
    vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestwrite_size = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dataslot_update = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dataslot_update_id = VL_RAND_RESET_I(16);
    vlSelf->tb_core_bridge_cmd__DOT__dataslot_update_size = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dataslot_allcomplete = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__rtc_epoch_seconds = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__rtc_date_bcd = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__rtc_time_bcd = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__rtc_valid = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__osnotify_inmenu = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__savestate_start = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__savestate_load = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_ack = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_done = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_err = VL_RAND_RESET_I(3);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_toggle = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_value = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_valid = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_last_value = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_count = VL_RAND_RESET_I(8);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_enter_toggle = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_exit_toggle = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_enter_count = VL_RAND_RESET_I(8);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_exit_count = VL_RAND_RESET_I(8);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__endian_little_s = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_0 = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_20 = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_24 = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_28 = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_40 = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_44 = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_48 = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_start = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_startval = VL_RAND_RESET_I(16);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd = VL_RAND_RESET_I(16);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode = VL_RAND_RESET_I(16);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__hstate = VL_RAND_RESET_I(4);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_0 = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_4 = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_8 = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_20 = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_24 = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_28 = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_2C = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__tstate = VL_RAND_RESET_I(4);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__shutdown_timeout = VL_RAND_RESET_I(23);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__shutdown_timeout_reached = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_1 = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_queue = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__warm_reset_ready_pending = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_1 = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_queue = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_1 = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_queue = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_1 = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_queue = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_1 = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_queue = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_1 = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_queue = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_addr = VL_RAND_RESET_I(10);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_wren = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_addr = VL_RAND_RESET_I(5);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_1 = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_2 = VL_RAND_RESET_I(1);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_3 = VL_RAND_RESET_I(1);
    for (int __Vi0 = 0; __Vi0 < 1024; ++__Vi0) {
        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem[__Vi0] = VL_RAND_RESET_I(32);
    }
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_a_reg = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_b_reg = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_a_reg2 = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_b_reg2 = VL_RAND_RESET_I(32);
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__i = VL_RAND_RESET_I(32);
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = VL_RAND_RESET_I(1);
    for (int __Vi0 = 0; __Vi0 < 2; ++__Vi0) {
        vlSelf->__Vm_traceActivity[__Vi0] = 0;
    }
}
