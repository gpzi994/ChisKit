// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vchisburner_cart_gb_readonly_abstract__pch.h"
#include "Vchisburner_cart_gb_readonly_abstract.h"
#include "Vchisburner_cart_gb_readonly_abstract___024root.h"

// FUNCTIONS
Vchisburner_cart_gb_readonly_abstract__Syms::~Vchisburner_cart_gb_readonly_abstract__Syms()
{
}

Vchisburner_cart_gb_readonly_abstract__Syms::Vchisburner_cart_gb_readonly_abstract__Syms(VerilatedContext* contextp, const char* namep, Vchisburner_cart_gb_readonly_abstract* modelp)
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
