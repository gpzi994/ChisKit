// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vchisburner_cart_safe_idle.h for the primary calling header

#include "Vchisburner_cart_safe_idle__pch.h"
#include "Vchisburner_cart_safe_idle___024root.h"

VL_ATTR_COLD void Vchisburner_cart_safe_idle___024root___eval_static(Vchisburner_cart_safe_idle___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vchisburner_cart_safe_idle__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vchisburner_cart_safe_idle___024root___eval_static\n"); );
}

VL_ATTR_COLD void Vchisburner_cart_safe_idle___024root___eval_initial__TOP(Vchisburner_cart_safe_idle___024root* vlSelf);

VL_ATTR_COLD void Vchisburner_cart_safe_idle___024root___eval_initial(Vchisburner_cart_safe_idle___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vchisburner_cart_safe_idle__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vchisburner_cart_safe_idle___024root___eval_initial\n"); );
    // Body
    Vchisburner_cart_safe_idle___024root___eval_initial__TOP(vlSelf);
}

VL_ATTR_COLD void Vchisburner_cart_safe_idle___024root___eval_initial__TOP(Vchisburner_cart_safe_idle___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vchisburner_cart_safe_idle__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vchisburner_cart_safe_idle___024root___eval_initial__TOP\n"); );
    // Body
    vlSelf->bank3_out = 0U;
    vlSelf->bank3_dir = 0U;
    vlSelf->bank2_out = 0U;
    vlSelf->bank2_dir = 0U;
    vlSelf->bank1_out = 0U;
    vlSelf->bank1_dir = 0U;
    vlSelf->bank0_out = 0xfU;
    vlSelf->bank0_dir = 1U;
    vlSelf->pin30_out = 1U;
    vlSelf->pin30_dir = 0U;
    vlSelf->pin30_pwroff_reset = 0U;
    vlSelf->pin31_out = 0U;
    vlSelf->pin31_dir = 0U;
}

VL_ATTR_COLD void Vchisburner_cart_safe_idle___024root___eval_final(Vchisburner_cart_safe_idle___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vchisburner_cart_safe_idle__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vchisburner_cart_safe_idle___024root___eval_final\n"); );
}

VL_ATTR_COLD void Vchisburner_cart_safe_idle___024root___eval_settle(Vchisburner_cart_safe_idle___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vchisburner_cart_safe_idle__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vchisburner_cart_safe_idle___024root___eval_settle\n"); );
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vchisburner_cart_safe_idle___024root___dump_triggers__act(Vchisburner_cart_safe_idle___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vchisburner_cart_safe_idle__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vchisburner_cart_safe_idle___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VactTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vchisburner_cart_safe_idle___024root___dump_triggers__nba(Vchisburner_cart_safe_idle___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vchisburner_cart_safe_idle__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vchisburner_cart_safe_idle___024root___dump_triggers__nba\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VnbaTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vchisburner_cart_safe_idle___024root___ctor_var_reset(Vchisburner_cart_safe_idle___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vchisburner_cart_safe_idle__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vchisburner_cart_safe_idle___024root___ctor_var_reset\n"); );
    // Body
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
}
