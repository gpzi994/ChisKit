// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vchisburner_cart_safe_idle__pch.h"
#include "Vchisburner_cart_safe_idle.h"
#include "Vchisburner_cart_safe_idle___024root.h"

// FUNCTIONS
Vchisburner_cart_safe_idle__Syms::~Vchisburner_cart_safe_idle__Syms()
{
}

Vchisburner_cart_safe_idle__Syms::Vchisburner_cart_safe_idle__Syms(VerilatedContext* contextp, const char* namep, Vchisburner_cart_safe_idle* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP{this, namep}
{
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-12);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
}
