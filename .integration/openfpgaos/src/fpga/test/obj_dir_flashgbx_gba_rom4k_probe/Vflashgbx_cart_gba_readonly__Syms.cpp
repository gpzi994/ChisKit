// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vflashgbx_cart_gba_readonly__pch.h"
#include "Vflashgbx_cart_gba_readonly.h"
#include "Vflashgbx_cart_gba_readonly___024root.h"

// FUNCTIONS
Vflashgbx_cart_gba_readonly__Syms::~Vflashgbx_cart_gba_readonly__Syms()
{
}

Vflashgbx_cart_gba_readonly__Syms::Vflashgbx_cart_gba_readonly__Syms(VerilatedContext* contextp, const char* namep, Vflashgbx_cart_gba_readonly* modelp)
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
