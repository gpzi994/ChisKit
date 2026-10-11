// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vtb_axi_periph__pch.h"
#include "verilated_vcd_c.h"

//============================================================
// Constructors

Vtb_axi_periph::Vtb_axi_periph(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vtb_axi_periph__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , reset_n{vlSymsp->TOP.reset_n}
    , s_axi_arvalid{vlSymsp->TOP.s_axi_arvalid}
    , s_axi_arready{vlSymsp->TOP.s_axi_arready}
    , s_axi_arlen{vlSymsp->TOP.s_axi_arlen}
    , s_axi_rvalid{vlSymsp->TOP.s_axi_rvalid}
    , s_axi_rready{vlSymsp->TOP.s_axi_rready}
    , s_axi_rresp{vlSymsp->TOP.s_axi_rresp}
    , s_axi_rlast{vlSymsp->TOP.s_axi_rlast}
    , s_axi_awvalid{vlSymsp->TOP.s_axi_awvalid}
    , s_axi_awready{vlSymsp->TOP.s_axi_awready}
    , s_axi_awlen{vlSymsp->TOP.s_axi_awlen}
    , s_axi_awburst{vlSymsp->TOP.s_axi_awburst}
    , s_axi_wvalid{vlSymsp->TOP.s_axi_wvalid}
    , s_axi_wready{vlSymsp->TOP.s_axi_wready}
    , s_axi_wstrb{vlSymsp->TOP.s_axi_wstrb}
    , s_axi_wlast{vlSymsp->TOP.s_axi_wlast}
    , s_axi_bvalid{vlSymsp->TOP.s_axi_bvalid}
    , s_axi_bready{vlSymsp->TOP.s_axi_bready}
    , s_axi_bresp{vlSymsp->TOP.s_axi_bresp}
    , dbg_state{vlSymsp->TOP.dbg_state}
    , dbg_bram_hold{vlSymsp->TOP.dbg_bram_hold}
    , dbg_can_push_beat{vlSymsp->TOP.dbg_can_push_beat}
    , dbg_gpu_reg_wr{vlSymsp->TOP.dbg_gpu_reg_wr}
    , dbg_gpu_reg_addr{vlSymsp->TOP.dbg_gpu_reg_addr}
    , dbg_save_dt_slot{vlSymsp->TOP.dbg_save_dt_slot}
    , dbg_save_dt_commit{vlSymsp->TOP.dbg_save_dt_commit}
    , dbg_analogizer_wr_toggle{vlSymsp->TOP.dbg_analogizer_wr_toggle}
    , dbg_snac_pin_out{vlSymsp->TOP.dbg_snac_pin_out}
    , dbg_snac_pin_dir{vlSymsp->TOP.dbg_snac_pin_dir}
    , dbg_snac_enable{vlSymsp->TOP.dbg_snac_enable}
    , dbg_cart_wr_count{vlSymsp->TOP.dbg_cart_wr_count}
    , dbg_cart_rd_count{vlSymsp->TOP.dbg_cart_rd_count}
    , dbg_cart_last_addr{vlSymsp->TOP.dbg_cart_last_addr}
    , dbg_pal_commit{vlSymsp->TOP.dbg_pal_commit}
    , dbg_analog_color_mode{vlSymsp->TOP.dbg_analog_color_mode}
    , dbg_pal_commit_count{vlSymsp->TOP.dbg_pal_commit_count}
    , pal_busy_in{vlSymsp->TOP.pal_busy_in}
    , gpu_swap_req_in{vlSymsp->TOP.gpu_swap_req_in}
    , gpu_swap_idx_in{vlSymsp->TOP.gpu_swap_idx_in}
    , vsync_in{vlSymsp->TOP.vsync_in}
    , early_vblank_in{vlSymsp->TOP.early_vblank_in}
    , os_inmenu_in{vlSymsp->TOP.os_inmenu_in}
    , target_dataslot_ack_in{vlSymsp->TOP.target_dataslot_ack_in}
    , target_dataslot_done_in{vlSymsp->TOP.target_dataslot_done_in}
    , target_dataslot_err_in{vlSymsp->TOP.target_dataslot_err_in}
    , bridge_wr_idle_in{vlSymsp->TOP.bridge_wr_idle_in}
    , dt_query_valid_in{vlSymsp->TOP.dt_query_valid_in}
    , snac_pin_in_in{vlSymsp->TOP.snac_pin_in_in}
    , ext_irq_out{vlSymsp->TOP.ext_irq_out}
    , dbg_analog_fb_width{vlSymsp->TOP.dbg_analog_fb_width}
    , dbg_analog_fb_height{vlSymsp->TOP.dbg_analog_fb_height}
    , dbg_analog_fb_stride{vlSymsp->TOP.dbg_analog_fb_stride}
    , cont1_trig_in{vlSymsp->TOP.cont1_trig_in}
    , cont2_trig_in{vlSymsp->TOP.cont2_trig_in}
    , cont3_trig_in{vlSymsp->TOP.cont3_trig_in}
    , cont4_trig_in{vlSymsp->TOP.cont4_trig_in}
    , s_axi_araddr{vlSymsp->TOP.s_axi_araddr}
    , s_axi_rdata{vlSymsp->TOP.s_axi_rdata}
    , s_axi_awaddr{vlSymsp->TOP.s_axi_awaddr}
    , s_axi_wdata{vlSymsp->TOP.s_axi_wdata}
    , dbg_gpu_reg_wdata{vlSymsp->TOP.dbg_gpu_reg_wdata}
    , dbg_save_dt_size{vlSymsp->TOP.dbg_save_dt_size}
    , dbg_analogizer_wr_settings{vlSymsp->TOP.dbg_analogizer_wr_settings}
    , dbg_analogizer_wr_hoffset{vlSymsp->TOP.dbg_analogizer_wr_hoffset}
    , dbg_analogizer_wr_voffset{vlSymsp->TOP.dbg_analogizer_wr_voffset}
    , dbg_cart_last_wdata{vlSymsp->TOP.dbg_cart_last_wdata}
    , dbg_analog_fb_addr{vlSymsp->TOP.dbg_analog_fb_addr}
    , analogizer_settings_in{vlSymsp->TOP.analogizer_settings_in}
    , cont1_key_in{vlSymsp->TOP.cont1_key_in}
    , cont1_joy_in{vlSymsp->TOP.cont1_joy_in}
    , cont2_key_in{vlSymsp->TOP.cont2_key_in}
    , cont2_joy_in{vlSymsp->TOP.cont2_joy_in}
    , cont3_key_in{vlSymsp->TOP.cont3_key_in}
    , cont3_joy_in{vlSymsp->TOP.cont3_joy_in}
    , cont4_key_in{vlSymsp->TOP.cont4_key_in}
    , cont4_joy_in{vlSymsp->TOP.cont4_joy_in}
    , dt_query_data_in{vlSymsp->TOP.dt_query_data_in}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vtb_axi_periph::Vtb_axi_periph(const char* _vcname__)
    : Vtb_axi_periph(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vtb_axi_periph::~Vtb_axi_periph() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vtb_axi_periph___024root___eval_debug_assertions(Vtb_axi_periph___024root* vlSelf);
#endif  // VL_DEBUG
void Vtb_axi_periph___024root___eval_static(Vtb_axi_periph___024root* vlSelf);
void Vtb_axi_periph___024root___eval_initial(Vtb_axi_periph___024root* vlSelf);
void Vtb_axi_periph___024root___eval_settle(Vtb_axi_periph___024root* vlSelf);
void Vtb_axi_periph___024root___eval(Vtb_axi_periph___024root* vlSelf);

void Vtb_axi_periph::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vtb_axi_periph::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vtb_axi_periph___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_activity = true;
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vtb_axi_periph___024root___eval_static(&(vlSymsp->TOP));
        Vtb_axi_periph___024root___eval_initial(&(vlSymsp->TOP));
        Vtb_axi_periph___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vtb_axi_periph___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vtb_axi_periph::eventsPending() { return false; }

uint64_t Vtb_axi_periph::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "%Error: No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vtb_axi_periph::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vtb_axi_periph___024root___eval_final(Vtb_axi_periph___024root* vlSelf);

VL_ATTR_COLD void Vtb_axi_periph::final() {
    Vtb_axi_periph___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vtb_axi_periph::hierName() const { return vlSymsp->name(); }
const char* Vtb_axi_periph::modelName() const { return "Vtb_axi_periph"; }
unsigned Vtb_axi_periph::threads() const { return 1; }
void Vtb_axi_periph::prepareClone() const { contextp()->prepareClone(); }
void Vtb_axi_periph::atClone() const {
    contextp()->threadPoolpOnClone();
}
std::unique_ptr<VerilatedTraceConfig> Vtb_axi_periph::traceConfig() const {
    return std::unique_ptr<VerilatedTraceConfig>{new VerilatedTraceConfig{false, false, false}};
};

//============================================================
// Trace configuration

void Vtb_axi_periph___024root__trace_decl_types(VerilatedVcd* tracep);

void Vtb_axi_periph___024root__trace_init_top(Vtb_axi_periph___024root* vlSelf, VerilatedVcd* tracep);

VL_ATTR_COLD static void trace_init(void* voidSelf, VerilatedVcd* tracep, uint32_t code) {
    // Callback from tracep->open()
    Vtb_axi_periph___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vtb_axi_periph___024root*>(voidSelf);
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (!vlSymsp->_vm_contextp__->calcUnusedSigs()) {
        VL_FATAL_MT(__FILE__, __LINE__, __FILE__,
            "Turning on wave traces requires Verilated::traceEverOn(true) call before time 0.");
    }
    vlSymsp->__Vm_baseCode = code;
    tracep->pushPrefix(std::string{vlSymsp->name()}, VerilatedTracePrefixType::SCOPE_MODULE);
    Vtb_axi_periph___024root__trace_decl_types(tracep);
    Vtb_axi_periph___024root__trace_init_top(vlSelf, tracep);
    tracep->popPrefix();
}

VL_ATTR_COLD void Vtb_axi_periph___024root__trace_register(Vtb_axi_periph___024root* vlSelf, VerilatedVcd* tracep);

VL_ATTR_COLD void Vtb_axi_periph::trace(VerilatedVcdC* tfp, int levels, int options) {
    if (tfp->isOpen()) {
        vl_fatal(__FILE__, __LINE__, __FILE__,"'Vtb_axi_periph::trace()' shall not be called after 'VerilatedVcdC::open()'.");
    }
    if (false && levels && options) {}  // Prevent unused
    tfp->spTrace()->addModel(this);
    tfp->spTrace()->addInitCb(&trace_init, &(vlSymsp->TOP));
    Vtb_axi_periph___024root__trace_register(&(vlSymsp->TOP), tfp->spTrace());
}
