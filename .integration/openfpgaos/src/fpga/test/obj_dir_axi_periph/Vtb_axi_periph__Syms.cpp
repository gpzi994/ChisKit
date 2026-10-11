// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vtb_axi_periph__pch.h"
#include "Vtb_axi_periph.h"
#include "Vtb_axi_periph___024root.h"

// FUNCTIONS
Vtb_axi_periph__Syms::~Vtb_axi_periph__Syms()
{
}

Vtb_axi_periph__Syms::Vtb_axi_periph__Syms(VerilatedContext* contextp, const char* namep, Vtb_axi_periph* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP{this, namep}
{
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-10);
    _vm_contextp__->timeprecision(-11);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
    // Setup scopes
    __Vscope_tb_axi_periph__dut__ram.configure(this, name(), "tb_axi_periph.dut.ram", "ram", 0, VerilatedScope::SCOPE_OTHER);
    // Setup export functions
    for (int __Vfinal = 0; __Vfinal < 2; ++__Vfinal) {
        __Vscope_tb_axi_periph__dut__ram.varInsert(__Vfinal,"mem", &(TOP.tb_axi_periph__DOT__dut__DOT__ram__DOT__mem), false, VLVT_UINT32,VLVD_NODIR|VLVF_PUB_RW,2 ,31,0 ,0,8191);
    }
}
