// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vtb_core_bridge_cmd__pch.h"
#include "Vtb_core_bridge_cmd.h"
#include "Vtb_core_bridge_cmd___024root.h"

// FUNCTIONS
Vtb_core_bridge_cmd__Syms::~Vtb_core_bridge_cmd__Syms()
{
}

Vtb_core_bridge_cmd__Syms::Vtb_core_bridge_cmd__Syms(VerilatedContext* contextp, const char* namep, Vtb_core_bridge_cmd* modelp)
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
    // Setup scopes
    __Vscope_tb_core_bridge_cmd__dut__idt__altsyncram_component.configure(this, name(), "tb_core_bridge_cmd.dut.idt.altsyncram_component", "altsyncram_component", 0, VerilatedScope::SCOPE_OTHER);
    // Setup export functions
    for (int __Vfinal = 0; __Vfinal < 2; ++__Vfinal) {
        __Vscope_tb_core_bridge_cmd__dut__idt__altsyncram_component.varInsert(__Vfinal,"mem", &(TOP.tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem), false, VLVT_UINT32,VLVD_NODIR|VLVF_PUB_RW,2 ,31,0 ,0,1023);
    }
}
