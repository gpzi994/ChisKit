// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vchisburner_cart_safe_idle.h for the primary calling header

#ifndef VERILATED_VCHISBURNER_CART_SAFE_IDLE___024ROOT_H_
#define VERILATED_VCHISBURNER_CART_SAFE_IDLE___024ROOT_H_  // guard

#include "verilated.h"


class Vchisburner_cart_safe_idle__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vchisburner_cart_safe_idle___024root final : public VerilatedModule {
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
    Vchisburner_cart_safe_idle__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vchisburner_cart_safe_idle___024root(Vchisburner_cart_safe_idle__Syms* symsp, const char* v__name);
    ~Vchisburner_cart_safe_idle___024root();
    VL_UNCOPYABLE(Vchisburner_cart_safe_idle___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
