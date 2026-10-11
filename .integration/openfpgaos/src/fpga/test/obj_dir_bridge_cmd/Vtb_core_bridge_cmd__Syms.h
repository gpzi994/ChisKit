// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VTB_CORE_BRIDGE_CMD__SYMS_H_
#define VERILATED_VTB_CORE_BRIDGE_CMD__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vtb_core_bridge_cmd.h"

// INCLUDE MODULE CLASSES
#include "Vtb_core_bridge_cmd___024root.h"

// DPI TYPES for DPI Export callbacks (Internal use)

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)Vtb_core_bridge_cmd__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vtb_core_bridge_cmd* const __Vm_modelp;
    bool __Vm_activity = false;  ///< Used by trace routines to determine change occurred
    uint32_t __Vm_baseCode = 0;  ///< Used by trace routines when tracing multiple models
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vtb_core_bridge_cmd___024root  TOP;

    // SCOPE NAMES
    VerilatedScope __Vscope_tb_core_bridge_cmd__dut__idt__altsyncram_component;

    // CONSTRUCTORS
    Vtb_core_bridge_cmd__Syms(VerilatedContext* contextp, const char* namep, Vtb_core_bridge_cmd* modelp);
    ~Vtb_core_bridge_cmd__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
