// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Primary model header
//
// This header should be included by all source files instantiating the design.
// The class here is then constructed to instantiate the design.
// See the Verilator manual for examples.

#ifndef VERILATED_VTB_AXI_PERIPH_H_
#define VERILATED_VTB_AXI_PERIPH_H_  // guard

#include "verilated.h"
#include "svdpi.h"

class Vtb_axi_periph__Syms;
class Vtb_axi_periph___024root;
class VerilatedVcdC;

// This class is the main interface to the Verilated model
class alignas(VL_CACHE_LINE_BYTES) Vtb_axi_periph VL_NOT_FINAL : public VerilatedModel {
  private:
    // Symbol table holding complete model state (owned by this class)
    Vtb_axi_periph__Syms* const vlSymsp;

  public:

    // PORTS
    // The application code writes and reads these signals to
    // propagate new values into/out from the Verilated model.
    VL_IN8(&clk,0,0);
    VL_IN8(&reset_n,0,0);
    VL_IN8(&s_axi_arvalid,0,0);
    VL_OUT8(&s_axi_arready,0,0);
    VL_IN8(&s_axi_arlen,7,0);
    VL_OUT8(&s_axi_rvalid,0,0);
    VL_IN8(&s_axi_rready,0,0);
    VL_OUT8(&s_axi_rresp,1,0);
    VL_OUT8(&s_axi_rlast,0,0);
    VL_IN8(&s_axi_awvalid,0,0);
    VL_OUT8(&s_axi_awready,0,0);
    VL_IN8(&s_axi_awlen,7,0);
    VL_IN8(&s_axi_awburst,1,0);
    VL_IN8(&s_axi_wvalid,0,0);
    VL_OUT8(&s_axi_wready,0,0);
    VL_IN8(&s_axi_wstrb,3,0);
    VL_IN8(&s_axi_wlast,0,0);
    VL_OUT8(&s_axi_bvalid,0,0);
    VL_IN8(&s_axi_bready,0,0);
    VL_OUT8(&s_axi_bresp,1,0);
    VL_OUT8(&dbg_state,3,0);
    VL_OUT8(&dbg_bram_hold,0,0);
    VL_OUT8(&dbg_can_push_beat,0,0);
    VL_OUT8(&dbg_gpu_reg_wr,0,0);
    VL_OUT8(&dbg_gpu_reg_addr,3,0);
    VL_OUT8(&dbg_save_dt_slot,3,0);
    VL_OUT8(&dbg_save_dt_commit,0,0);
    VL_OUT8(&dbg_analogizer_wr_toggle,2,0);
    VL_OUT8(&dbg_snac_pin_out,7,0);
    VL_OUT8(&dbg_snac_pin_dir,7,0);
    VL_OUT8(&dbg_snac_enable,0,0);
    VL_OUT8(&dbg_cart_wr_count,7,0);
    VL_OUT8(&dbg_cart_rd_count,7,0);
    VL_OUT8(&dbg_cart_last_addr,4,0);
    VL_OUT8(&dbg_pal_commit,0,0);
    VL_OUT8(&dbg_analog_color_mode,2,0);
    VL_OUT8(&dbg_pal_commit_count,7,0);
    VL_IN8(&pal_busy_in,0,0);
    VL_IN8(&gpu_swap_req_in,0,0);
    VL_IN8(&gpu_swap_idx_in,1,0);
    VL_IN8(&vsync_in,0,0);
    VL_IN8(&early_vblank_in,0,0);
    VL_IN8(&os_inmenu_in,0,0);
    VL_IN8(&target_dataslot_ack_in,0,0);
    VL_IN8(&target_dataslot_done_in,0,0);
    VL_IN8(&target_dataslot_err_in,2,0);
    VL_IN8(&bridge_wr_idle_in,0,0);
    VL_IN8(&dt_query_valid_in,0,0);
    VL_IN8(&snac_pin_in_in,7,0);
    VL_OUT8(&ext_irq_out,0,0);
    VL_OUT16(&dbg_analog_fb_width,9,0);
    VL_OUT16(&dbg_analog_fb_height,9,0);
    VL_OUT16(&dbg_analog_fb_stride,15,0);
    VL_IN16(&cont1_trig_in,15,0);
    VL_IN16(&cont2_trig_in,15,0);
    VL_IN16(&cont3_trig_in,15,0);
    VL_IN16(&cont4_trig_in,15,0);
    VL_IN(&s_axi_araddr,31,0);
    VL_OUT(&s_axi_rdata,31,0);
    VL_IN(&s_axi_awaddr,31,0);
    VL_IN(&s_axi_wdata,31,0);
    VL_OUT(&dbg_gpu_reg_wdata,31,0);
    VL_OUT(&dbg_save_dt_size,31,0);
    VL_OUT(&dbg_analogizer_wr_settings,31,0);
    VL_OUT(&dbg_analogizer_wr_hoffset,31,0);
    VL_OUT(&dbg_analogizer_wr_voffset,31,0);
    VL_OUT(&dbg_cart_last_wdata,31,0);
    VL_OUT(&dbg_analog_fb_addr,24,0);
    VL_IN(&analogizer_settings_in,31,0);
    VL_IN(&cont1_key_in,31,0);
    VL_IN(&cont1_joy_in,31,0);
    VL_IN(&cont2_key_in,31,0);
    VL_IN(&cont2_joy_in,31,0);
    VL_IN(&cont3_key_in,31,0);
    VL_IN(&cont3_joy_in,31,0);
    VL_IN(&cont4_key_in,31,0);
    VL_IN(&cont4_joy_in,31,0);
    VL_IN(&dt_query_data_in,31,0);

    // CELLS
    // Public to allow access to /* verilator public */ items.
    // Otherwise the application code can consider these internals.

    // Root instance pointer to allow access to model internals,
    // including inlined /* verilator public_flat_* */ items.
    Vtb_axi_periph___024root* const rootp;

    // CONSTRUCTORS
    /// Construct the model; called by application code
    /// If contextp is null, then the model will use the default global context
    /// If name is "", then makes a wrapper with a
    /// single model invisible with respect to DPI scope names.
    explicit Vtb_axi_periph(VerilatedContext* contextp, const char* name = "TOP");
    explicit Vtb_axi_periph(const char* name = "TOP");
    /// Destroy the model; called (often implicitly) by application code
    virtual ~Vtb_axi_periph();
  private:
    VL_UNCOPYABLE(Vtb_axi_periph);  ///< Copying not allowed

  public:
    // API METHODS
    /// Evaluate the model.  Application must call when inputs change.
    void eval() { eval_step(); }
    /// Evaluate when calling multiple units/models per time step.
    void eval_step();
    /// Evaluate at end of a timestep for tracing, when using eval_step().
    /// Application must call after all eval() and before time changes.
    void eval_end_step() {}
    /// Simulation complete, run final blocks.  Application must call on completion.
    void final();
    /// Are there scheduled events to handle?
    bool eventsPending();
    /// Returns time at next time slot. Aborts if !eventsPending()
    uint64_t nextTimeSlot();
    /// Trace signals in the model; called by application code
    void trace(VerilatedVcdC* tfp, int levels, int options = 0);
    /// Retrieve name of this model instance (as passed to constructor).
    const char* name() const;

    // Abstract methods from VerilatedModel
    const char* hierName() const override final;
    const char* modelName() const override final;
    unsigned threads() const override final;
    /// Prepare for cloning the model at the process level (e.g. fork in Linux)
    /// Release necessary resources. Called before cloning.
    void prepareClone() const;
    /// Re-init after cloning the model at the process level (e.g. fork in Linux)
    /// Re-allocate necessary resources. Called after cloning.
    void atClone() const;
    std::unique_ptr<VerilatedTraceConfig> traceConfig() const override final;
};

#endif  // guard
