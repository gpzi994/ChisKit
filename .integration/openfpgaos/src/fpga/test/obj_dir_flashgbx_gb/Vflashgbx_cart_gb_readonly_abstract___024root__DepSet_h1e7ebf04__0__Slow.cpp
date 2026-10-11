// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vflashgbx_cart_gb_readonly_abstract.h for the primary calling header

#include "Vflashgbx_cart_gb_readonly_abstract__pch.h"
#include "Vflashgbx_cart_gb_readonly_abstract__Syms.h"
#include "Vflashgbx_cart_gb_readonly_abstract___024root.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vflashgbx_cart_gb_readonly_abstract___024root___dump_triggers__stl(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf);
#endif  // VL_DEBUG

VL_ATTR_COLD void Vflashgbx_cart_gb_readonly_abstract___024root___eval_triggers__stl(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gb_readonly_abstract__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gb_readonly_abstract___024root___eval_triggers__stl\n"); );
    // Body
    vlSelf->__VstlTriggered.set(0U, (IData)(vlSelf->__VstlFirstIteration));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vflashgbx_cart_gb_readonly_abstract___024root___dump_triggers__stl(vlSelf);
    }
#endif
}
