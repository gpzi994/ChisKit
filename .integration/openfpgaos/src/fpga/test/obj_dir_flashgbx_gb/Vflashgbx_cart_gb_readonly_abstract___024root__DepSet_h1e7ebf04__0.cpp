// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vflashgbx_cart_gb_readonly_abstract.h for the primary calling header

#include "Vflashgbx_cart_gb_readonly_abstract__pch.h"
#include "Vflashgbx_cart_gb_readonly_abstract__Syms.h"
#include "Vflashgbx_cart_gb_readonly_abstract___024root.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vflashgbx_cart_gb_readonly_abstract___024root___dump_triggers__ico(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf);
#endif  // VL_DEBUG

void Vflashgbx_cart_gb_readonly_abstract___024root___eval_triggers__ico(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gb_readonly_abstract__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gb_readonly_abstract___024root___eval_triggers__ico\n"); );
    // Body
    vlSelf->__VicoTriggered.set(0U, (IData)(vlSelf->__VicoFirstIteration));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vflashgbx_cart_gb_readonly_abstract___024root___dump_triggers__ico(vlSelf);
    }
#endif
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vflashgbx_cart_gb_readonly_abstract___024root___dump_triggers__act(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf);
#endif  // VL_DEBUG

void Vflashgbx_cart_gb_readonly_abstract___024root___eval_triggers__act(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gb_readonly_abstract__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gb_readonly_abstract___024root___eval_triggers__act\n"); );
    // Body
    vlSelf->__VactTriggered.set(0U, (((IData)(vlSelf->clk) 
                                      & (~ (IData)(vlSelf->__Vtrigprevexpr___TOP__clk__0))) 
                                     | ((~ (IData)(vlSelf->reset_n)) 
                                        & (IData)(vlSelf->__Vtrigprevexpr___TOP__reset_n__0))));
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = vlSelf->clk;
    vlSelf->__Vtrigprevexpr___TOP__reset_n__0 = vlSelf->reset_n;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vflashgbx_cart_gb_readonly_abstract___024root___dump_triggers__act(vlSelf);
    }
#endif
}
