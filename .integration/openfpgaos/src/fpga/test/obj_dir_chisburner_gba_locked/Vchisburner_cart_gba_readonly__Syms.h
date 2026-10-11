// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VCHISBURNER_CART_GBA_READONLY__SYMS_H_
#define VERILATED_VCHISBURNER_CART_GBA_READONLY__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vchisburner_cart_gba_readonly.h"

// INCLUDE MODULE CLASSES
#include "Vchisburner_cart_gba_readonly___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)Vchisburner_cart_gba_readonly__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vchisburner_cart_gba_readonly* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vchisburner_cart_gba_readonly___024root TOP;

    // CONSTRUCTORS
    Vchisburner_cart_gba_readonly__Syms(VerilatedContext* contextp, const char* namep, Vchisburner_cart_gba_readonly* modelp);
    ~Vchisburner_cart_gba_readonly__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
