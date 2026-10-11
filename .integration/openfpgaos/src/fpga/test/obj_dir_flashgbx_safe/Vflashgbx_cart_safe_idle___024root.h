// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vflashgbx_cart_safe_idle.h for the primary calling header

#ifndef VERILATED_VFLASHGBX_CART_SAFE_IDLE___024ROOT_H_
#define VERILATED_VFLASHGBX_CART_SAFE_IDLE___024ROOT_H_  // guard

#include "verilated.h"


class Vflashgbx_cart_safe_idle__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vflashgbx_cart_safe_idle___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_OUT8(bank3_out,7,0);
    VL_OUT8(bank3_dir,0,0);
    VL_OUT8(bank2_out,7,0);
    VL_OUT8(bank2_dir,0,0);
    VL_OUT8(bank1_out,7,0);
    VL_OUT8(bank1_dir,0,0);
    VL_OUT8(bank0_out,3,0);
    VL_OUT8(bank0_dir,0,0);
    VL_OUT8(pin30_out,0,0);
    VL_OUT8(pin30_dir,0,0);
    VL_OUT8(pin30_pwroff_reset,0,0);
    VL_OUT8(pin31_out,0,0);
    VL_OUT8(pin31_dir,0,0);
    CData/*0:0*/ __VactContinue;
    IData/*31:0*/ __VactIterCount;
    VlTriggerVec<0> __VactTriggered;
    VlTriggerVec<0> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vflashgbx_cart_safe_idle__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vflashgbx_cart_safe_idle___024root(Vflashgbx_cart_safe_idle__Syms* symsp, const char* v__name);
    ~Vflashgbx_cart_safe_idle___024root();
    VL_UNCOPYABLE(Vflashgbx_cart_safe_idle___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
