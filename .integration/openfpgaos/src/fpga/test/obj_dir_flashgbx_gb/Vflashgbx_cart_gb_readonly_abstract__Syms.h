// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VFLASHGBX_CART_GB_READONLY_ABSTRACT__SYMS_H_
#define VERILATED_VFLASHGBX_CART_GB_READONLY_ABSTRACT__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vflashgbx_cart_gb_readonly_abstract.h"

// INCLUDE MODULE CLASSES
#include "Vflashgbx_cart_gb_readonly_abstract___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)Vflashgbx_cart_gb_readonly_abstract__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vflashgbx_cart_gb_readonly_abstract* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vflashgbx_cart_gb_readonly_abstract___024root TOP;

    // CONSTRUCTORS
    Vflashgbx_cart_gb_readonly_abstract__Syms(VerilatedContext* contextp, const char* namep, Vflashgbx_cart_gb_readonly_abstract* modelp);
    ~Vflashgbx_cart_gb_readonly_abstract__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
