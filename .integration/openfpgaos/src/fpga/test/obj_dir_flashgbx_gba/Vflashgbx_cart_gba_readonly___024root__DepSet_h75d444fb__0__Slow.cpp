// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vflashgbx_cart_gba_readonly.h for the primary calling header

#include "Vflashgbx_cart_gba_readonly__pch.h"
#include "Vflashgbx_cart_gba_readonly___024root.h"

VL_ATTR_COLD void Vflashgbx_cart_gba_readonly___024root___eval_static(Vflashgbx_cart_gba_readonly___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gba_readonly__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gba_readonly___024root___eval_static\n"); );
}

VL_ATTR_COLD void Vflashgbx_cart_gba_readonly___024root___eval_initial__TOP(Vflashgbx_cart_gba_readonly___024root* vlSelf);

VL_ATTR_COLD void Vflashgbx_cart_gba_readonly___024root___eval_initial(Vflashgbx_cart_gba_readonly___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gba_readonly__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gba_readonly___024root___eval_initial\n"); );
    // Body
    Vflashgbx_cart_gba_readonly___024root___eval_initial__TOP(vlSelf);
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = vlSelf->clk;
    vlSelf->__Vtrigprevexpr___TOP__reset_n__0 = vlSelf->reset_n;
}

VL_ATTR_COLD void Vflashgbx_cart_gba_readonly___024root___eval_initial__TOP(Vflashgbx_cart_gba_readonly___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gba_readonly__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gba_readonly___024root___eval_initial__TOP\n"); );
    // Body
    vlSelf->pin30_out = 1U;
    vlSelf->bank0_dir = 1U;
    vlSelf->pin30_pwroff_reset = 0U;
    vlSelf->pin31_out = 0U;
    vlSelf->pin31_dir = 0U;
    vlSelf->writes_enabled = 0U;
}

VL_ATTR_COLD void Vflashgbx_cart_gba_readonly___024root___eval_final(Vflashgbx_cart_gba_readonly___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gba_readonly__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gba_readonly___024root___eval_final\n"); );
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vflashgbx_cart_gba_readonly___024root___dump_triggers__stl(Vflashgbx_cart_gba_readonly___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vflashgbx_cart_gba_readonly___024root___eval_phase__stl(Vflashgbx_cart_gba_readonly___024root* vlSelf);

VL_ATTR_COLD void Vflashgbx_cart_gba_readonly___024root___eval_settle(Vflashgbx_cart_gba_readonly___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gba_readonly__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gba_readonly___024root___eval_settle\n"); );
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
            Vflashgbx_cart_gba_readonly___024root___dump_triggers__stl(vlSelf);
#endif
            VL_FATAL_MT("../targets/pocket/flashgbx_cart_gba_readonly.v", 14, "", "Settle region did not converge.");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        __VstlContinue = 0U;
        if (Vflashgbx_cart_gba_readonly___024root___eval_phase__stl(vlSelf)) {
            __VstlContinue = 1U;
        }
        vlSelf->__VstlFirstIteration = 0U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vflashgbx_cart_gba_readonly___024root___dump_triggers__stl(Vflashgbx_cart_gba_readonly___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gba_readonly__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gba_readonly___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VstlTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VstlTriggered.word(0U))) {
        VL_DBG_MSGF("         'stl' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vflashgbx_cart_gba_readonly___024root___stl_sequent__TOP__0(Vflashgbx_cart_gba_readonly___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gba_readonly__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gba_readonly___024root___stl_sequent__TOP__0\n"); );
    // Body
    vlSelf->pin30_dir = 0U;
    vlSelf->bank0_out = 0xfU;
    vlSelf->bank1_dir = 0U;
    vlSelf->bank2_dir = 0U;
    vlSelf->bank3_dir = 0U;
    vlSelf->busy = (0U != (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__state));
    vlSelf->bank1_out = 0U;
    vlSelf->bank2_out = 0U;
    vlSelf->bank3_out = 0U;
    if ((8U & (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__state))) {
        if ((1U & (~ ((IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__state) 
                      >> 2U)))) {
            if ((2U & (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__state))) {
                if ((1U & (~ (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__state)))) {
                    vlSelf->pin30_dir = 1U;
                    vlSelf->bank0_out = ((9U == (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__state))
                                          ? 0xaU : 0xeU);
                    vlSelf->bank1_dir = 1U;
                    vlSelf->bank2_dir = 1U;
                    vlSelf->bank3_dir = 1U;
                    vlSelf->bank1_out = (0xffU & (vlSelf->flashgbx_cart_gba_readonly__DOT__word_address 
                                                  >> 0x10U));
                    vlSelf->bank2_out = (0xffU & ((IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__write_data) 
                                                  >> 8U));
                    vlSelf->bank3_out = (0xffU & (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__write_data));
                }
            } else {
                vlSelf->pin30_dir = 1U;
                vlSelf->bank0_out = ((9U == (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__state))
                                      ? 0xaU : 0xeU);
                vlSelf->bank1_dir = 1U;
                vlSelf->bank2_dir = 1U;
                vlSelf->bank3_dir = 1U;
                vlSelf->bank1_out = (0xffU & (vlSelf->flashgbx_cart_gba_readonly__DOT__word_address 
                                              >> 0x10U));
                vlSelf->bank2_out = (0xffU & ((IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__write_data) 
                                              >> 8U));
                vlSelf->bank3_out = (0xffU & (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__write_data));
            }
        }
    } else if ((4U & (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__state))) {
        vlSelf->pin30_dir = 1U;
        if ((2U & (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__state))) {
            if ((1U & (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__state))) {
                vlSelf->bank0_out = 0xeU;
                vlSelf->bank1_out = (0xffU & (vlSelf->flashgbx_cart_gba_readonly__DOT__word_address 
                                              >> 0x10U));
                vlSelf->bank2_out = (0xffU & (vlSelf->flashgbx_cart_gba_readonly__DOT__word_address 
                                              >> 8U));
            } else {
                vlSelf->bank0_out = 0xfU;
                vlSelf->bank1_out = (0xffU & (vlSelf->flashgbx_cart_gba_readonly__DOT__word_address 
                                              >> 0x10U));
                vlSelf->bank2_out = (0xffU & (vlSelf->flashgbx_cart_gba_readonly__DOT__word_address 
                                              >> 8U));
            }
            vlSelf->bank2_dir = 1U;
            vlSelf->bank3_dir = 1U;
            vlSelf->bank3_out = (0xffU & vlSelf->flashgbx_cart_gba_readonly__DOT__word_address);
        } else if ((1U & (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__state))) {
            vlSelf->bank0_out = 0xeU;
            vlSelf->bank1_out = (0xffU & (vlSelf->flashgbx_cart_gba_readonly__DOT__word_address 
                                          >> 0x10U));
        } else {
            vlSelf->bank0_out = 0xcU;
            vlSelf->bank1_out = (0xffU & (vlSelf->flashgbx_cart_gba_readonly__DOT__word_address 
                                          >> 0x10U));
        }
        vlSelf->bank1_dir = 1U;
    } else if ((2U & (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__state))) {
        vlSelf->pin30_dir = 1U;
        vlSelf->bank0_out = 0xeU;
        vlSelf->bank1_dir = 1U;
        if ((1U & (~ (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__state)))) {
            vlSelf->bank2_dir = 1U;
            vlSelf->bank3_dir = 1U;
            vlSelf->bank2_out = (0xffU & (vlSelf->flashgbx_cart_gba_readonly__DOT__word_address 
                                          >> 8U));
            vlSelf->bank3_out = (0xffU & vlSelf->flashgbx_cart_gba_readonly__DOT__word_address);
        }
        vlSelf->bank1_out = (0xffU & ((1U & (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__state))
                                       ? (vlSelf->flashgbx_cart_gba_readonly__DOT__word_address 
                                          >> 0x10U)
                                       : (vlSelf->flashgbx_cart_gba_readonly__DOT__word_address 
                                          >> 0x10U)));
    } else if ((1U & (IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__state))) {
        vlSelf->pin30_dir = 1U;
        vlSelf->bank0_out = 0xfU;
        vlSelf->bank1_dir = 1U;
        vlSelf->bank2_dir = 1U;
        vlSelf->bank3_dir = 1U;
        vlSelf->bank1_out = (0xffU & (vlSelf->flashgbx_cart_gba_readonly__DOT__word_address 
                                      >> 0x10U));
        vlSelf->bank2_out = (0xffU & (vlSelf->flashgbx_cart_gba_readonly__DOT__word_address 
                                      >> 8U));
        vlSelf->bank3_out = (0xffU & vlSelf->flashgbx_cart_gba_readonly__DOT__word_address);
    }
    vlSelf->flashgbx_cart_gba_readonly__DOT__read_shape_valid 
        = ((2U == vlSelf->flashgbx_cart_gba_readonly__DOT__bus_reg) 
           & ((0U == vlSelf->flashgbx_cart_gba_readonly__DOT__space_reg) 
              & ((~ vlSelf->flashgbx_cart_gba_readonly__DOT__address_reg) 
                 & ((0U != vlSelf->flashgbx_cart_gba_readonly__DOT__length_reg) 
                    & ((~ vlSelf->flashgbx_cart_gba_readonly__DOT__length_reg) 
                       & ((0x20U >= vlSelf->flashgbx_cart_gba_readonly__DOT__length_reg) 
                          & ((0x1ffffffULL >= (0x1ffffffffULL 
                                               & (((QData)((IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__address_reg)) 
                                                   + (QData)((IData)(vlSelf->flashgbx_cart_gba_readonly__DOT__length_reg))) 
                                                  - 1ULL))) 
                             & ((0x2000000U >= vlSelf->flashgbx_cart_gba_readonly__DOT__length_reg) 
                                & ((vlSelf->flashgbx_cart_gba_readonly__DOT__address_reg 
                                    <= ((IData)(0x2000000U) 
                                        - vlSelf->flashgbx_cart_gba_readonly__DOT__length_reg)) 
                                   & (0U != vlSelf->flashgbx_cart_gba_readonly__DOT__timeout_reg))))))))));
    vlSelf->reg_rdata = 0U;
    if ((0x10U & (IData)(vlSelf->reg_addr))) {
        vlSelf->reg_rdata = ((8U & (IData)(vlSelf->reg_addr))
                              ? 0U : ((4U & (IData)(vlSelf->reg_addr))
                                       ? 0U : ((2U 
                                                & (IData)(vlSelf->reg_addr))
                                                ? 0U
                                                : (
                                                   (1U 
                                                    & (IData)(vlSelf->reg_addr))
                                                    ? 0U
                                                    : vlSelf->flashgbx_cart_gba_readonly__DOT__space_reg))));
    } else if ((8U & (IData)(vlSelf->reg_addr))) {
        if ((4U & (IData)(vlSelf->reg_addr))) {
            vlSelf->reg_rdata = ((2U & (IData)(vlSelf->reg_addr))
                                  ? ((1U & (IData)(vlSelf->reg_addr))
                                      ? vlSelf->flashgbx_cart_gba_readonly__DOT__query_result_reg
                                      : vlSelf->flashgbx_cart_gba_readonly__DOT__query_arg_reg)
                                  : ((1U & (IData)(vlSelf->reg_addr))
                                      ? vlSelf->host_diag
                                      : vlSelf->host_b1_raw));
        } else if ((2U & (IData)(vlSelf->reg_addr))) {
            vlSelf->reg_rdata = ((1U & (IData)(vlSelf->reg_addr))
                                  ? vlSelf->lifecycle_status
                                  : vlSelf->flashgbx_cart_gba_readonly__DOT__error_reg);
        } else if ((1U & (IData)(vlSelf->reg_addr))) {
            vlSelf->reg_rdata = vlSelf->flashgbx_cart_gba_readonly__DOT__fifo_count;
        } else {
            if ((0U < vlSelf->flashgbx_cart_gba_readonly__DOT__fifo_count)) {
                vlSelf->reg_rdata = ((0xffffff00U & vlSelf->reg_rdata) 
                                     | vlSelf->flashgbx_cart_gba_readonly__DOT__rx_fifo
                                     [(0x1fU & vlSelf->flashgbx_cart_gba_readonly__DOT__fifo_read_index)]);
            }
            if ((1U < vlSelf->flashgbx_cart_gba_readonly__DOT__fifo_count)) {
                vlSelf->reg_rdata = ((0xffff00ffU & vlSelf->reg_rdata) 
                                     | (vlSelf->flashgbx_cart_gba_readonly__DOT__rx_fifo
                                        [(0x1fU & ((IData)(1U) 
                                                   + vlSelf->flashgbx_cart_gba_readonly__DOT__fifo_read_index))] 
                                        << 8U));
            }
            if ((2U < vlSelf->flashgbx_cart_gba_readonly__DOT__fifo_count)) {
                vlSelf->reg_rdata = ((0xff00ffffU & vlSelf->reg_rdata) 
                                     | (vlSelf->flashgbx_cart_gba_readonly__DOT__rx_fifo
                                        [(0x1fU & ((IData)(2U) 
                                                   + vlSelf->flashgbx_cart_gba_readonly__DOT__fifo_read_index))] 
                                        << 0x10U));
            }
            if ((3U < vlSelf->flashgbx_cart_gba_readonly__DOT__fifo_count)) {
                vlSelf->reg_rdata = ((0xffffffU & vlSelf->reg_rdata) 
                                     | (vlSelf->flashgbx_cart_gba_readonly__DOT__rx_fifo
                                        [(0x1fU & ((IData)(3U) 
                                                   + vlSelf->flashgbx_cart_gba_readonly__DOT__fifo_read_index))] 
                                        << 0x18U));
            }
        }
    } else {
        vlSelf->reg_rdata = ((4U & (IData)(vlSelf->reg_addr))
                              ? ((2U & (IData)(vlSelf->reg_addr))
                                  ? ((1U & (IData)(vlSelf->reg_addr))
                                      ? vlSelf->flashgbx_cart_gba_readonly__DOT__timeout_reg
                                      : vlSelf->flashgbx_cart_gba_readonly__DOT__length_reg)
                                  : ((1U & (IData)(vlSelf->reg_addr))
                                      ? vlSelf->flashgbx_cart_gba_readonly__DOT__address_reg
                                      : vlSelf->flashgbx_cart_gba_readonly__DOT__bus_reg))
                              : ((2U & (IData)(vlSelf->reg_addr))
                                  ? ((1U & (IData)(vlSelf->reg_addr))
                                      ? 0U : vlSelf->flashgbx_cart_gba_readonly__DOT__status_reg)
                                  : ((1U & (IData)(vlSelf->reg_addr))
                                      ? 0x100f02U : 0x31424746U)));
    }
}

VL_ATTR_COLD void Vflashgbx_cart_gba_readonly___024root___eval_stl(Vflashgbx_cart_gba_readonly___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gba_readonly__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gba_readonly___024root___eval_stl\n"); );
    // Body
    if ((1ULL & vlSelf->__VstlTriggered.word(0U))) {
        Vflashgbx_cart_gba_readonly___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD void Vflashgbx_cart_gba_readonly___024root___eval_triggers__stl(Vflashgbx_cart_gba_readonly___024root* vlSelf);

VL_ATTR_COLD bool Vflashgbx_cart_gba_readonly___024root___eval_phase__stl(Vflashgbx_cart_gba_readonly___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gba_readonly__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gba_readonly___024root___eval_phase__stl\n"); );
    // Init
    CData/*0:0*/ __VstlExecute;
    // Body
    Vflashgbx_cart_gba_readonly___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = vlSelf->__VstlTriggered.any();
    if (__VstlExecute) {
        Vflashgbx_cart_gba_readonly___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vflashgbx_cart_gba_readonly___024root___dump_triggers__ico(Vflashgbx_cart_gba_readonly___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gba_readonly__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gba_readonly___024root___dump_triggers__ico\n"); );
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
VL_ATTR_COLD void Vflashgbx_cart_gba_readonly___024root___dump_triggers__act(Vflashgbx_cart_gba_readonly___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gba_readonly__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gba_readonly___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VactTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 0 is active: @(posedge clk or negedge reset_n)\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vflashgbx_cart_gba_readonly___024root___dump_triggers__nba(Vflashgbx_cart_gba_readonly___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gba_readonly__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gba_readonly___024root___dump_triggers__nba\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VnbaTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 0 is active: @(posedge clk or negedge reset_n)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vflashgbx_cart_gba_readonly___024root___ctor_var_reset(Vflashgbx_cart_gba_readonly___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gba_readonly__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gba_readonly___024root___ctor_var_reset\n"); );
    // Body
    vlSelf->clk = VL_RAND_RESET_I(1);
    vlSelf->reset_n = VL_RAND_RESET_I(1);
    vlSelf->lifecycle_ok = VL_RAND_RESET_I(1);
    vlSelf->lifecycle_status = VL_RAND_RESET_I(32);
    vlSelf->host_b1_raw = VL_RAND_RESET_I(32);
    vlSelf->host_diag = VL_RAND_RESET_I(32);
    vlSelf->reg_wr = VL_RAND_RESET_I(1);
    vlSelf->reg_rd = VL_RAND_RESET_I(1);
    vlSelf->reg_addr = VL_RAND_RESET_I(5);
    vlSelf->reg_wdata = VL_RAND_RESET_I(32);
    vlSelf->reg_rdata = VL_RAND_RESET_I(32);
    vlSelf->bank3_in = VL_RAND_RESET_I(8);
    vlSelf->bank2_in = VL_RAND_RESET_I(8);
    vlSelf->bank3_out = VL_RAND_RESET_I(8);
    vlSelf->bank3_dir = VL_RAND_RESET_I(1);
    vlSelf->bank2_out = VL_RAND_RESET_I(8);
    vlSelf->bank2_dir = VL_RAND_RESET_I(1);
    vlSelf->bank1_out = VL_RAND_RESET_I(8);
    vlSelf->bank1_dir = VL_RAND_RESET_I(1);
    vlSelf->bank0_out = VL_RAND_RESET_I(4);
    vlSelf->bank0_dir = VL_RAND_RESET_I(1);
    vlSelf->pin30_out = VL_RAND_RESET_I(1);
    vlSelf->pin30_dir = VL_RAND_RESET_I(1);
    vlSelf->pin30_pwroff_reset = VL_RAND_RESET_I(1);
    vlSelf->pin31_out = VL_RAND_RESET_I(1);
    vlSelf->pin31_dir = VL_RAND_RESET_I(1);
    vlSelf->writes_enabled = VL_RAND_RESET_I(1);
    vlSelf->busy = VL_RAND_RESET_I(1);
    vlSelf->profile_verified = VL_RAND_RESET_I(1);
    vlSelf->flashgbx_cart_gba_readonly__DOT__state = VL_RAND_RESET_I(4);
    vlSelf->flashgbx_cart_gba_readonly__DOT__status_reg = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__error_reg = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__bus_reg = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__space_reg = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__address_reg = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__length_reg = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__timeout_reg = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__query_arg_reg = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__query_result_reg = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__word_address = VL_RAND_RESET_I(24);
    vlSelf->flashgbx_cart_gba_readonly__DOT__words_remaining = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__phase_counter = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__watchdog_counter = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__fifo_count = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__fifo_read_index = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__read_command_count = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__identify_command_count = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__last_word = VL_RAND_RESET_I(1);
    vlSelf->flashgbx_cart_gba_readonly__DOT__identify_active = VL_RAND_RESET_I(1);
    vlSelf->flashgbx_cart_gba_readonly__DOT__probe_phase = VL_RAND_RESET_I(4);
    vlSelf->flashgbx_cart_gba_readonly__DOT__write_data = VL_RAND_RESET_I(16);
    vlSelf->flashgbx_cart_gba_readonly__DOT__recovery_status = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__recovery_error = VL_RAND_RESET_I(32);
    vlSelf->flashgbx_cart_gba_readonly__DOT__probe_id0_ok = VL_RAND_RESET_I(1);
    vlSelf->flashgbx_cart_gba_readonly__DOT__probe_id1_ok = VL_RAND_RESET_I(1);
    vlSelf->flashgbx_cart_gba_readonly__DOT__probe_q_ok = VL_RAND_RESET_I(1);
    vlSelf->flashgbx_cart_gba_readonly__DOT__probe_r_ok = VL_RAND_RESET_I(1);
    vlSelf->flashgbx_cart_gba_readonly__DOT__probe_y_ok = VL_RAND_RESET_I(1);
    vlSelf->flashgbx_cart_gba_readonly__DOT__probe_cmdset_ok = VL_RAND_RESET_I(1);
    vlSelf->flashgbx_cart_gba_readonly__DOT__probe_size_ok = VL_RAND_RESET_I(1);
    vlSelf->flashgbx_cart_gba_readonly__DOT__probe_buffer_ok = VL_RAND_RESET_I(1);
    vlSelf->flashgbx_cart_gba_readonly__DOT__probe_regions_ok = VL_RAND_RESET_I(1);
    vlSelf->flashgbx_cart_gba_readonly__DOT__probe_sector_count_ok = VL_RAND_RESET_I(1);
    vlSelf->flashgbx_cart_gba_readonly__DOT__probe_sector_size_ok = VL_RAND_RESET_I(1);
    for (int __Vi0 = 0; __Vi0 < 32; ++__Vi0) {
        vlSelf->flashgbx_cart_gba_readonly__DOT__rx_fifo[__Vi0] = VL_RAND_RESET_I(8);
    }
    vlSelf->flashgbx_cart_gba_readonly__DOT__read_shape_valid = VL_RAND_RESET_I(1);
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = VL_RAND_RESET_I(1);
    vlSelf->__Vtrigprevexpr___TOP__reset_n__0 = VL_RAND_RESET_I(1);
}
