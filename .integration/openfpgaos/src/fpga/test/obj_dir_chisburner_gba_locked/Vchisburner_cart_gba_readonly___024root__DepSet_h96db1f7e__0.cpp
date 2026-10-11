// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vchisburner_cart_gba_readonly.h for the primary calling header

#include "Vchisburner_cart_gba_readonly__pch.h"
#include "Vchisburner_cart_gba_readonly__Syms.h"
#include "Vchisburner_cart_gba_readonly___024root.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vchisburner_cart_gba_readonly___024root___dump_triggers__ico(Vchisburner_cart_gba_readonly___024root* vlSelf);
#endif  // VL_DEBUG

void Vchisburner_cart_gba_readonly___024root___eval_triggers__ico(Vchisburner_cart_gba_readonly___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vchisburner_cart_gba_readonly__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vchisburner_cart_gba_readonly___024root___eval_triggers__ico\n"); );
    // Body
    vlSelf->__VicoTriggered.set(0U, (IData)(vlSelf->__VicoFirstIteration));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vchisburner_cart_gba_readonly___024root___dump_triggers__ico(vlSelf);
    }
#endif
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vchisburner_cart_gba_readonly___024root___dump_triggers__act(Vchisburner_cart_gba_readonly___024root* vlSelf);
#endif  // VL_DEBUG

void Vchisburner_cart_gba_readonly___024root___eval_triggers__act(Vchisburner_cart_gba_readonly___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vchisburner_cart_gba_readonly__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vchisburner_cart_gba_readonly___024root___eval_triggers__act\n"); );
    // Body
    vlSelf->__VactTriggered.set(0U, (((IData)(vlSelf->clk) 
                                      & (~ (IData)(vlSelf->__Vtrigprevexpr___TOP__clk__0))) 
                                     | ((~ (IData)(vlSelf->reset_n)) 
                                        & (IData)(vlSelf->__Vtrigprevexpr___TOP__reset_n__0))));
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = vlSelf->clk;
    vlSelf->__Vtrigprevexpr___TOP__reset_n__0 = vlSelf->reset_n;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vchisburner_cart_gba_readonly___024root___dump_triggers__act(vlSelf);
    }
#endif
}
