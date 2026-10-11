// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vchisburner_cart_safe_idle__pch.h"

//============================================================
// Constructors

Vchisburner_cart_safe_idle::Vchisburner_cart_safe_idle(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vchisburner_cart_safe_idle__Syms(contextp(), _vcname__, this)}
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

Vchisburner_cart_safe_idle::Vchisburner_cart_safe_idle(const char* _vcname__)
    : Vchisburner_cart_safe_idle(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vchisburner_cart_safe_idle::~Vchisburner_cart_safe_idle() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vchisburner_cart_safe_idle___024root___eval_debug_assertions(Vchisburner_cart_safe_idle___024root* vlSelf);
#endif  // VL_DEBUG
void Vchisburner_cart_safe_idle___024root___eval_static(Vchisburner_cart_safe_idle___024root* vlSelf);
void Vchisburner_cart_safe_idle___024root___eval_initial(Vchisburner_cart_safe_idle___024root* vlSelf);
void Vchisburner_cart_safe_idle___024root___eval_settle(Vchisburner_cart_safe_idle___024root* vlSelf);
void Vchisburner_cart_safe_idle___024root___eval(Vchisburner_cart_safe_idle___024root* vlSelf);

void Vchisburner_cart_safe_idle::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vchisburner_cart_safe_idle::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vchisburner_cart_safe_idle___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vchisburner_cart_safe_idle___024root___eval_static(&(vlSymsp->TOP));
        Vchisburner_cart_safe_idle___024root___eval_initial(&(vlSymsp->TOP));
        Vchisburner_cart_safe_idle___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vchisburner_cart_safe_idle___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vchisburner_cart_safe_idle::eventsPending() { return false; }

uint64_t Vchisburner_cart_safe_idle::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "%Error: No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vchisburner_cart_safe_idle::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vchisburner_cart_safe_idle___024root___eval_final(Vchisburner_cart_safe_idle___024root* vlSelf);

VL_ATTR_COLD void Vchisburner_cart_safe_idle::final() {
    Vchisburner_cart_safe_idle___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vchisburner_cart_safe_idle::hierName() const { return vlSymsp->name(); }
const char* Vchisburner_cart_safe_idle::modelName() const { return "Vchisburner_cart_safe_idle"; }
unsigned Vchisburner_cart_safe_idle::threads() const { return 1; }
void Vchisburner_cart_safe_idle::prepareClone() const { contextp()->prepareClone(); }
void Vchisburner_cart_safe_idle::atClone() const {
    contextp()->threadPoolpOnClone();
}

//============================================================
// Trace configuration

VL_ATTR_COLD void Vchisburner_cart_safe_idle::trace(VerilatedVcdC* tfp, int levels, int options) {
    vl_fatal(__FILE__, __LINE__, __FILE__,"'Vchisburner_cart_safe_idle::trace()' called on model that was Verilated without --trace option");
}
