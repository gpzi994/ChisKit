// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vchisburner_cart_gba_readonly.h for the primary calling header

#ifndef VERILATED_VCHISBURNER_CART_GBA_READONLY___024ROOT_H_
#define VERILATED_VCHISBURNER_CART_GBA_READONLY___024ROOT_H_  // guard

#include "verilated.h"


class Vchisburner_cart_gba_readonly__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vchisburner_cart_gba_readonly___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        VL_IN8(clk,0,0);
        VL_IN8(reset_n,0,0);
        VL_IN8(lifecycle_ok,0,0);
        VL_IN8(reg_wr,0,0);
        VL_IN8(reg_rd,0,0);
        VL_IN8(reg_addr,4,0);
        VL_IN8(bank3_in,7,0);
        VL_IN8(bank2_in,7,0);
        VL_OUT8(bank3_out,7,0);
        VL_OUT8(bank3_dir,0,0);
        VL_OUT8(bank2_out,7,0);
        VL_OUT8(bank2_dir,0,0);
        VL_OUT8(bank1_out,7,0);
        VL_OUT8(bank1_dir,0,0);
        VL_OUT8(bank0_out,3,0);
        VL_OUT8(bank0_dir,0,0);
        VL_OUT8(pin30_out,0,0);
        VL_OUT8(pin30_dir,0,0);
        VL_OUT8(pin30_pwroff_reset,0,0);
        VL_OUT8(pin31_out,0,0);
        VL_OUT8(pin31_dir,0,0);
        VL_OUT8(writes_enabled,0,0);
        VL_OUT8(busy,0,0);
        VL_OUT8(profile_verified,0,0);
        CData/*3:0*/ chisburner_cart_gba_readonly__DOT__state;
        CData/*0:0*/ chisburner_cart_gba_readonly__DOT__last_word;
        CData/*0:0*/ chisburner_cart_gba_readonly__DOT__identify_active;
        CData/*3:0*/ chisburner_cart_gba_readonly__DOT__probe_phase;
        CData/*0:0*/ chisburner_cart_gba_readonly__DOT__probe_id0_ok;
        CData/*0:0*/ chisburner_cart_gba_readonly__DOT__probe_id1_ok;
        CData/*0:0*/ chisburner_cart_gba_readonly__DOT__probe_q_ok;
        CData/*0:0*/ chisburner_cart_gba_readonly__DOT__probe_r_ok;
        CData/*0:0*/ chisburner_cart_gba_readonly__DOT__probe_y_ok;
        CData/*0:0*/ chisburner_cart_gba_readonly__DOT__probe_cmdset_ok;
        CData/*0:0*/ chisburner_cart_gba_readonly__DOT__probe_size_ok;
        CData/*0:0*/ chisburner_cart_gba_readonly__DOT__probe_buffer_ok;
        CData/*0:0*/ chisburner_cart_gba_readonly__DOT__probe_regions_ok;
        CData/*0:0*/ chisburner_cart_gba_readonly__DOT__probe_sector_count_ok;
        CData/*0:0*/ chisburner_cart_gba_readonly__DOT__probe_sector_size_ok;
        CData/*0:0*/ chisburner_cart_gba_readonly__DOT__read_shape_valid;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __VicoFirstIteration;
        CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
        CData/*0:0*/ __Vtrigprevexpr___TOP__reset_n__0;
        CData/*0:0*/ __VactContinue;
        SData/*15:0*/ chisburner_cart_gba_readonly__DOT__write_data;
        VL_IN(lifecycle_status,31,0);
        VL_IN(host_b1_raw,31,0);
        VL_IN(host_diag,31,0);
        VL_IN(reg_wdata,31,0);
        VL_OUT(reg_rdata,31,0);
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__status_reg;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__error_reg;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__bus_reg;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__space_reg;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__address_reg;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__length_reg;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__timeout_reg;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__query_arg_reg;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__query_result_reg;
        IData/*23:0*/ chisburner_cart_gba_readonly__DOT__word_address;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__words_remaining;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__phase_counter;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__watchdog_counter;
    };
    struct {
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__fifo_count;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__fifo_read_index;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__read_command_count;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__identify_command_count;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__recovery_status;
        IData/*31:0*/ chisburner_cart_gba_readonly__DOT__recovery_error;
        IData/*31:0*/ __VactIterCount;
        VlUnpacked<CData/*7:0*/, 32> chisburner_cart_gba_readonly__DOT__rx_fifo;
    };
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<1> __VactTriggered;
    VlTriggerVec<1> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vchisburner_cart_gba_readonly__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vchisburner_cart_gba_readonly___024root(Vchisburner_cart_gba_readonly__Syms* symsp, const char* v__name);
    ~Vchisburner_cart_gba_readonly___024root();
    VL_UNCOPYABLE(Vchisburner_cart_gba_readonly___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
