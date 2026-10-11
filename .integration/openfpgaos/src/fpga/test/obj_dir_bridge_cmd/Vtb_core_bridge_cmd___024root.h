// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vtb_core_bridge_cmd.h for the primary calling header

#ifndef VERILATED_VTB_CORE_BRIDGE_CMD___024ROOT_H_
#define VERILATED_VTB_CORE_BRIDGE_CMD___024ROOT_H_  // guard

#include "verilated.h"


class Vtb_core_bridge_cmd__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vtb_core_bridge_cmd___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        VL_IN8(clk,0,0);
        VL_IN8(bridge_endian_little,0,0);
        VL_IN8(bridge_rd,0,0);
        VL_IN8(bridge_wr,0,0);
        VL_IN8(status_boot_done,0,0);
        VL_IN8(status_setup_done,0,0);
        VL_IN8(status_running,0,0);
        VL_IN8(shutdown_ack_s,0,0);
        VL_IN8(target_dataslot_flush_in,0,0);
        VL_OUT8(target_dataslot_ack_out,0,0);
        VL_OUT8(target_dataslot_done_out,0,0);
        VL_OUT8(reset_n,0,0);
        VL_OUT8(shutdown_pending,0,0);
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dataslot_requestread;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dataslot_requestwrite;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dataslot_update;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dataslot_allcomplete;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__rtc_valid;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__osnotify_inmenu;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__savestate_start;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__savestate_load;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__target_dataslot_ack;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__target_dataslot_done;
        CData/*2:0*/ tb_core_bridge_cmd__DOT__target_dataslot_err;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_toggle;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_valid;
        CData/*7:0*/ tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_count;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__reset_enter_toggle;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__reset_exit_toggle;
        CData/*7:0*/ tb_core_bridge_cmd__DOT__dut__DOT__reset_enter_count;
        CData/*7:0*/ tb_core_bridge_cmd__DOT__dut__DOT__reset_exit_count;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__endian_little_s;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_start;
        CData/*3:0*/ tb_core_bridge_cmd__DOT__dut__DOT__hstate;
        CData/*3:0*/ tb_core_bridge_cmd__DOT__dut__DOT__tstate;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__shutdown_timeout_reached;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_1;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_queue;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__warm_reset_ready_pending;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_1;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_queue;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_1;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_queue;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_1;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_queue;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_1;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_queue;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_1;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_queue;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_wren;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active;
        CData/*4:0*/ tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_addr;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_1;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_2;
        CData/*0:0*/ tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_3;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __VicoFirstIteration;
        CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
        CData/*0:0*/ __VactContinue;
        VL_IN16(target_dataslot_id_in,15,0);
        SData/*15:0*/ tb_core_bridge_cmd__DOT__dataslot_requestread_id;
        SData/*15:0*/ tb_core_bridge_cmd__DOT__dataslot_requestwrite_id;
        SData/*15:0*/ tb_core_bridge_cmd__DOT__dataslot_update_id;
        SData/*15:0*/ tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_startval;
    };
    struct {
        SData/*15:0*/ tb_core_bridge_cmd__DOT__dut__DOT__host_cmd;
        SData/*15:0*/ tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode;
        SData/*9:0*/ tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_addr;
        VL_IN(bridge_addr,31,0);
        VL_OUT(bridge_rd_data,31,0);
        VL_IN(bridge_wr_data,31,0);
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dataslot_requestwrite_size;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dataslot_update_size;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__rtc_epoch_seconds;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__rtc_date_bcd;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__rtc_time_bcd;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_value;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_last_value;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__host_0;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__host_20;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__host_24;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__host_28;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__host_40;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__host_44;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__host_48;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_0;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_4;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_8;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_20;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_24;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_28;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__target_2C;
        IData/*22:0*/ tb_core_bridge_cmd__DOT__dut__DOT__shutdown_timeout;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_a_reg;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_b_reg;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_a_reg2;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_b_reg2;
        IData/*31:0*/ tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__i;
        IData/*31:0*/ __VactIterCount;
        VlUnpacked<IData/*31:0*/, 1024> tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem;
        VlUnpacked<CData/*0:0*/, 2> __Vm_traceActivity;
    };
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<1> __VactTriggered;
    VlTriggerVec<1> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vtb_core_bridge_cmd__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vtb_core_bridge_cmd___024root(Vtb_core_bridge_cmd__Syms* symsp, const char* v__name);
    ~Vtb_core_bridge_cmd___024root();
    VL_UNCOPYABLE(Vtb_core_bridge_cmd___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
