// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vflashgbx_cart_gb_readonly_abstract__pch.h"

//============================================================
// Constructors

Vflashgbx_cart_gb_readonly_abstract::Vflashgbx_cart_gb_readonly_abstract(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vflashgbx_cart_gb_readonly_abstract__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , reset_n{vlSymsp->TOP.reset_n}
    , lifecycle_ok{vlSymsp->TOP.lifecycle_ok}
    , reg_wr{vlSymsp->TOP.reg_wr}
    , reg_rd{vlSymsp->TOP.reg_rd}
    , reg_addr{vlSymsp->TOP.reg_addr}
    , gb_address_dir{vlSymsp->TOP.gb_address_dir}
    , gb_data_in{vlSymsp->TOP.gb_data_in}
    , gb_data_dir{vlSymsp->TOP.gb_data_dir}
    , gb_cs_n{vlSymsp->TOP.gb_cs_n}
    , gb_rd_n{vlSymsp->TOP.gb_rd_n}
    , gb_wr_n{vlSymsp->TOP.gb_wr_n}
    , writes_enabled{vlSymsp->TOP.writes_enabled}
    , gb_address{vlSymsp->TOP.gb_address}
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

Vflashgbx_cart_gb_readonly_abstract::Vflashgbx_cart_gb_readonly_abstract(const char* _vcname__)
    : Vflashgbx_cart_gb_readonly_abstract(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vflashgbx_cart_gb_readonly_abstract::~Vflashgbx_cart_gb_readonly_abstract() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vflashgbx_cart_gb_readonly_abstract___024root___eval_debug_assertions(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf);
#endif  // VL_DEBUG
void Vflashgbx_cart_gb_readonly_abstract___024root___eval_static(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf);
void Vflashgbx_cart_gb_readonly_abstract___024root___eval_initial(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf);
void Vflashgbx_cart_gb_readonly_abstract___024root___eval_settle(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf);
void Vflashgbx_cart_gb_readonly_abstract___024root___eval(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf);

void Vflashgbx_cart_gb_readonly_abstract::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vflashgbx_cart_gb_readonly_abstract::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vflashgbx_cart_gb_readonly_abstract___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vflashgbx_cart_gb_readonly_abstract___024root___eval_static(&(vlSymsp->TOP));
        Vflashgbx_cart_gb_readonly_abstract___024root___eval_initial(&(vlSymsp->TOP));
        Vflashgbx_cart_gb_readonly_abstract___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vflashgbx_cart_gb_readonly_abstract___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vflashgbx_cart_gb_readonly_abstract::eventsPending() { return false; }

uint64_t Vflashgbx_cart_gb_readonly_abstract::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "%Error: No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vflashgbx_cart_gb_readonly_abstract::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vflashgbx_cart_gb_readonly_abstract___024root___eval_final(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf);

VL_ATTR_COLD void Vflashgbx_cart_gb_readonly_abstract::final() {
    Vflashgbx_cart_gb_readonly_abstract___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vflashgbx_cart_gb_readonly_abstract::hierName() const { return vlSymsp->name(); }
const char* Vflashgbx_cart_gb_readonly_abstract::modelName() const { return "Vflashgbx_cart_gb_readonly_abstract"; }
unsigned Vflashgbx_cart_gb_readonly_abstract::threads() const { return 1; }
void Vflashgbx_cart_gb_readonly_abstract::prepareClone() const { contextp()->prepareClone(); }
void Vflashgbx_cart_gb_readonly_abstract::atClone() const {
    contextp()->threadPoolpOnClone();
}

//============================================================
// Trace configuration

VL_ATTR_COLD void Vflashgbx_cart_gb_readonly_abstract::trace(VerilatedVcdC* tfp, int levels, int options) {
    vl_fatal(__FILE__, __LINE__, __FILE__,"'Vflashgbx_cart_gb_readonly_abstract::trace()' called on model that was Verilated without --trace option");
}
