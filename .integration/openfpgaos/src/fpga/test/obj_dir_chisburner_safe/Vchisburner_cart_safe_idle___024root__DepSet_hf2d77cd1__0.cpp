// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vchisburner_cart_safe_idle.h for the primary calling header

#include "Vchisburner_cart_safe_idle__pch.h"
#include "Vchisburner_cart_safe_idle__Syms.h"
#include "Vchisburner_cart_safe_idle___024root.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vchisburner_cart_safe_idle___024root___dump_triggers__act(Vchisburner_cart_safe_idle___024root* vlSelf);
#endif  // VL_DEBUG

void Vchisburner_cart_safe_idle___024root___eval_triggers__act(Vchisburner_cart_safe_idle___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vchisburner_cart_safe_idle__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vchisburner_cart_safe_idle___024root___eval_triggers__act\n"); );
    // Body
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vchisburner_cart_safe_idle___024root___dump_triggers__act(vlSelf);
    }
#endif
}
