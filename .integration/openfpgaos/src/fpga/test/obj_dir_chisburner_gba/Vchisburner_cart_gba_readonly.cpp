// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vchisburner_cart_gba_readonly__pch.h"

//============================================================
// Constructors

Vchisburner_cart_gba_readonly::Vchisburner_cart_gba_readonly(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vchisburner_cart_gba_readonly__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , reset_n{vlSymsp->TOP.reset_n}
    , lifecycle_ok{vlSymsp->TOP.lifecycle_ok}
    , reg_wr{vlSymsp->TOP.reg_wr}
    , reg_rd{vlSymsp->TOP.reg_rd}
    , reg_addr{vlSymsp->TOP.reg_addr}
    , bank3_in{vlSymsp->TOP.bank3_in}
    , bank2_in{vlSymsp->TOP.bank2_in}
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
    , writes_enabled{vlSymsp->TOP.writes_enabled}
    , busy{vlSymsp->TOP.busy}
    , profile_verified{vlSymsp->TOP.profile_verified}
    , lifecycle_status{vlSymsp->TOP.lifecycle_status}
    , host_b1_raw{vlSymsp->TOP.host_b1_raw}
    , host_diag{vlSymsp->TOP.host_diag}
    , reg_wdata{vlSymsp->TOP.reg_wdata}
    , reg_rdata{vlSymsp->TOP.reg_rdata}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vchisburner_cart_gba_readonly::Vchisburner_cart_gba_readonly(const char* _vcname__)
    : Vchisburner_cart_gba_readonly(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vchisburner_cart_gba_readonly::~Vchisburner_cart_gba_readonly() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vchisburner_cart_gba_readonly___024root___eval_debug_assertions(Vchisburner_cart_gba_readonly___024root* vlSelf);
#endif  // VL_DEBUG
void Vchisburner_cart_gba_readonly___024root___eval_static(Vchisburner_cart_gba_readonly___024root* vlSelf);
void Vchisburner_cart_gba_readonly___024root___eval_initial(Vchisburner_cart_gba_readonly___024root* vlSelf);
void Vchisburner_cart_gba_readonly___024root___eval_settle(Vchisburner_cart_gba_readonly___024root* vlSelf);
void Vchisburner_cart_gba_readonly___024root___eval(Vchisburner_cart_gba_readonly___024root* vlSelf);

void Vchisburner_cart_gba_readonly::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vchisburner_cart_gba_readonly::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vchisburner_cart_gba_readonly___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vchisburner_cart_gba_readonly___024root___eval_static(&(vlSymsp->TOP));
        Vchisburner_cart_gba_readonly___024root___eval_initial(&(vlSymsp->TOP));
        Vchisburner_cart_gba_readonly___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vchisburner_cart_gba_readonly___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vchisburner_cart_gba_readonly::eventsPending() { return false; }

uint64_t Vchisburner_cart_gba_readonly::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "%Error: No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vchisburner_cart_gba_readonly::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vchisburner_cart_gba_readonly___024root___eval_final(Vchisburner_cart_gba_readonly___024root* vlSelf);

VL_ATTR_COLD void Vchisburner_cart_gba_readonly::final() {
    Vchisburner_cart_gba_readonly___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vchisburner_cart_gba_readonly::hierName() const { return vlSymsp->name(); }
const char* Vchisburner_cart_gba_readonly::modelName() const { return "Vchisburner_cart_gba_readonly"; }
unsigned Vchisburner_cart_gba_readonly::threads() const { return 1; }
void Vchisburner_cart_gba_readonly::prepareClone() const { contextp()->prepareClone(); }
void Vchisburner_cart_gba_readonly::atClone() const {
    contextp()->threadPoolpOnClone();
}

//============================================================
// Trace configuration

VL_ATTR_COLD void Vchisburner_cart_gba_readonly::trace(VerilatedVcdC* tfp, int levels, int options) {
    vl_fatal(__FILE__, __LINE__, __FILE__,"'Vchisburner_cart_gba_readonly::trace()' called on model that was Verilated without --trace option");
}
