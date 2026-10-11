// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vflashgbx_cart_safe_idle__pch.h"

//============================================================
// Constructors

Vflashgbx_cart_safe_idle::Vflashgbx_cart_safe_idle(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vflashgbx_cart_safe_idle__Syms(contextp(), _vcname__, this)}
    , bank3_out{vlSymsp->TOP.bank3_out}
    , bank3_dir{vlSymsp->TOP.bank3_dir}
    , bank2_out{vlSymsp->TOP.bank2_out}
    , bank2_dir{vlSymsp->TOP.bank2_dir}
    , bank1_out{vlSymsp->TOP.bank1_out}
    , bank1_dir{vlSymsp->TOP.bank1_dir}
    , bank0_out{vlSymsp->TOP.bank0_out}
    , bank0_dir{vlSymsp->TOP.bank0_dir}
    , pin30_out{vlSymsp->TOP.pin30_out}
    , pin30_dir{vlSymsp->TOP.pin30_dir}
    , pin30_pwroff_reset{vlSymsp->TOP.pin30_pwroff_reset}
    , pin31_out{vlSymsp->TOP.pin31_out}
    , pin31_dir{vlSymsp->TOP.pin31_dir}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vflashgbx_cart_safe_idle::Vflashgbx_cart_safe_idle(const char* _vcname__)
    : Vflashgbx_cart_safe_idle(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vflashgbx_cart_safe_idle::~Vflashgbx_cart_safe_idle() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vflashgbx_cart_safe_idle___024root___eval_debug_assertions(Vflashgbx_cart_safe_idle___024root* vlSelf);
#endif  // VL_DEBUG
void Vflashgbx_cart_safe_idle___024root___eval_static(Vflashgbx_cart_safe_idle___024root* vlSelf);
void Vflashgbx_cart_safe_idle___024root___eval_initial(Vflashgbx_cart_safe_idle___024root* vlSelf);
void Vflashgbx_cart_safe_idle___024root___eval_settle(Vflashgbx_cart_safe_idle___024root* vlSelf);
void Vflashgbx_cart_safe_idle___024root___eval(Vflashgbx_cart_safe_idle___024root* vlSelf);

void Vflashgbx_cart_safe_idle::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vflashgbx_cart_safe_idle::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vflashgbx_cart_safe_idle___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vflashgbx_cart_safe_idle___024root___eval_static(&(vlSymsp->TOP));
        Vflashgbx_cart_safe_idle___024root___eval_initial(&(vlSymsp->TOP));
        Vflashgbx_cart_safe_idle___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vflashgbx_cart_safe_idle___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vflashgbx_cart_safe_idle::eventsPending() { return false; }

uint64_t Vflashgbx_cart_safe_idle::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "%Error: No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vflashgbx_cart_safe_idle::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vflashgbx_cart_safe_idle___024root___eval_final(Vflashgbx_cart_safe_idle___024root* vlSelf);

VL_ATTR_COLD void Vflashgbx_cart_safe_idle::final() {
    Vflashgbx_cart_safe_idle___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vflashgbx_cart_safe_idle::hierName() const { return vlSymsp->name(); }
const char* Vflashgbx_cart_safe_idle::modelName() const { return "Vflashgbx_cart_safe_idle"; }
unsigned Vflashgbx_cart_safe_idle::threads() const { return 1; }
void Vflashgbx_cart_safe_idle::prepareClone() const { contextp()->prepareClone(); }
void Vflashgbx_cart_safe_idle::atClone() const {
    contextp()->threadPoolpOnClone();
}

//============================================================
// Trace configuration

VL_ATTR_COLD void Vflashgbx_cart_safe_idle::trace(VerilatedVcdC* tfp, int levels, int options) {
    vl_fatal(__FILE__, __LINE__, __FILE__,"'Vflashgbx_cart_safe_idle::trace()' called on model that was Verilated without --trace option");
}
