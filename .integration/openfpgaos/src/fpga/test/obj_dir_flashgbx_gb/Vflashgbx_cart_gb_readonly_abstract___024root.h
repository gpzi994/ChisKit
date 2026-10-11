// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vflashgbx_cart_gb_readonly_abstract.h for the primary calling header

#ifndef VERILATED_VFLASHGBX_CART_GB_READONLY_ABSTRACT___024ROOT_H_
#define VERILATED_VFLASHGBX_CART_GB_READONLY_ABSTRACT___024ROOT_H_  // guard

#include "verilated.h"


class Vflashgbx_cart_gb_readonly_abstract__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vflashgbx_cart_gb_readonly_abstract___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clk,0,0);
    VL_IN8(reset_n,0,0);
    VL_IN8(lifecycle_ok,0,0);
    VL_IN8(reg_wr,0,0);
    VL_IN8(reg_rd,0,0);
    VL_IN8(reg_addr,4,0);
    VL_OUT8(gb_address_dir,0,0);
    VL_IN8(gb_data_in,7,0);
    VL_OUT8(gb_data_dir,0,0);
    VL_OUT8(gb_cs_n,0,0);
    VL_OUT8(gb_rd_n,0,0);
    VL_OUT8(gb_wr_n,0,0);
    VL_OUT8(writes_enabled,0,0);
    CData/*2:0*/ flashgbx_cart_gb_readonly_abstract__DOT__state;
    CData/*0:0*/ flashgbx_cart_gb_readonly_abstract__DOT__last_byte;
    CData/*0:0*/ flashgbx_cart_gb_readonly_abstract__DOT__save_select;
    CData/*0:0*/ flashgbx_cart_gb_readonly_abstract__DOT__active;
    CData/*0:0*/ flashgbx_cart_gb_readonly_abstract__DOT__read_shape_valid;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VicoFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
    CData/*0:0*/ __Vtrigprevexpr___TOP__reset_n__0;
    CData/*0:0*/ __VactContinue;
    VL_OUT16(gb_address,15,0);
    SData/*15:0*/ flashgbx_cart_gb_readonly_abstract__DOT__byte_address;
    VL_IN(lifecycle_status,31,0);
    VL_IN(host_b1_raw,31,0);
    VL_IN(host_diag,31,0);
    VL_IN(reg_wdata,31,0);
    VL_OUT(reg_rdata,31,0);
    IData/*31:0*/ flashgbx_cart_gb_readonly_abstract__DOT__status_reg;
    IData/*31:0*/ flashgbx_cart_gb_readonly_abstract__DOT__error_reg;
    IData/*31:0*/ flashgbx_cart_gb_readonly_abstract__DOT__bus_reg;
    IData/*31:0*/ flashgbx_cart_gb_readonly_abstract__DOT__space_reg;
    IData/*31:0*/ flashgbx_cart_gb_readonly_abstract__DOT__address_reg;
    IData/*31:0*/ flashgbx_cart_gb_readonly_abstract__DOT__length_reg;
    IData/*31:0*/ flashgbx_cart_gb_readonly_abstract__DOT__timeout_reg;
    IData/*31:0*/ flashgbx_cart_gb_readonly_abstract__DOT__query_arg_reg;
    IData/*31:0*/ flashgbx_cart_gb_readonly_abstract__DOT__query_result_reg;
    IData/*31:0*/ flashgbx_cart_gb_readonly_abstract__DOT__bytes_remaining;
    IData/*31:0*/ flashgbx_cart_gb_readonly_abstract__DOT__phase_counter;
    IData/*31:0*/ flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter;
    IData/*31:0*/ flashgbx_cart_gb_readonly_abstract__DOT__fifo_count;
    IData/*31:0*/ flashgbx_cart_gb_readonly_abstract__DOT__fifo_read_index;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<CData/*7:0*/, 32> flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo;
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<1> __VactTriggered;
    VlTriggerVec<1> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vflashgbx_cart_gb_readonly_abstract__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vflashgbx_cart_gb_readonly_abstract___024root(Vflashgbx_cart_gb_readonly_abstract__Syms* symsp, const char* v__name);
    ~Vflashgbx_cart_gb_readonly_abstract___024root();
    VL_UNCOPYABLE(Vflashgbx_cart_gb_readonly_abstract___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
