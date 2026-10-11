// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtb_core_bridge_cmd.h for the primary calling header

#include "Vtb_core_bridge_cmd__pch.h"
#include "Vtb_core_bridge_cmd___024root.h"

VL_INLINE_OPT void Vtb_core_bridge_cmd___024root___ico_sequent__TOP__0(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___ico_sequent__TOP__0\n"); );
    // Body
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in 
        = ((IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__endian_little_s)
            ? ((vlSelf->bridge_wr_data << 0x18U) | 
               ((0xff0000U & (vlSelf->bridge_wr_data 
                              << 8U)) | ((0xff00U & 
                                          (vlSelf->bridge_wr_data 
                                           >> 8U)) 
                                         | (vlSelf->bridge_wr_data 
                                            >> 0x18U))))
            : vlSelf->bridge_wr_data);
}

void Vtb_core_bridge_cmd___024root___eval_ico(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval_ico\n"); );
    // Body
    if ((1ULL & vlSelf->__VicoTriggered.word(0U))) {
        Vtb_core_bridge_cmd___024root___ico_sequent__TOP__0(vlSelf);
    }
}

void Vtb_core_bridge_cmd___024root___eval_triggers__ico(Vtb_core_bridge_cmd___024root* vlSelf);

bool Vtb_core_bridge_cmd___024root___eval_phase__ico(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval_phase__ico\n"); );
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vtb_core_bridge_cmd___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelf->__VicoTriggered.any();
    if (__VicoExecute) {
        Vtb_core_bridge_cmd___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vtb_core_bridge_cmd___024root___eval_act(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval_act\n"); );
}

VL_INLINE_OPT void Vtb_core_bridge_cmd___024root___nba_sequent__TOP__0(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___nba_sequent__TOP__0\n"); );
    // Init
    CData/*0:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_queue;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_queue = 0;
    CData/*0:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_queue;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_queue = 0;
    SData/*9:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_addr;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_addr = 0;
    IData/*31:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_0;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_0 = 0;
    CData/*0:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_start;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_start = 0;
    IData/*31:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0 = 0;
    CData/*0:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active = 0;
    SData/*15:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_cmd;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_cmd = 0;
    CData/*3:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0;
    SData/*15:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode = 0;
    CData/*3:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__tstate;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__tstate = 0;
    CData/*0:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__warm_reset_ready_pending;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__warm_reset_ready_pending = 0;
    CData/*0:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_queue;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_queue = 0;
    CData/*0:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_queue;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_queue = 0;
    CData/*0:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_queue;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_queue = 0;
    CData/*0:0*/ __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_queue;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_queue = 0;
    SData/*9:0*/ __Vdlyvdim0__tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem__v0;
    __Vdlyvdim0__tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem__v0 = 0;
    IData/*31:0*/ __Vdlyvval__tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem__v0;
    __Vdlyvval__tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem__v0 = 0;
    CData/*0:0*/ __Vdlyvset__tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem__v0;
    __Vdlyvset__tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem__v0 = 0;
    // Body
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_queue 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_queue;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_queue 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_queue;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_queue 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_queue;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_queue 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_queue;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__warm_reset_ready_pending 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__warm_reset_ready_pending;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__tstate 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__tstate;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__hstate;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_cmd 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_0;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_start 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_start;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_0 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_0;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_addr 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_addr;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_queue 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_queue;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_queue 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_queue;
    __Vdlyvset__tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem__v0 = 0U;
    if (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_wren) {
        __Vdlyvval__tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem__v0 
            = ((IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active)
                ? 0U : vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in);
        __Vdlyvset__tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem__v0 = 1U;
        __Vdlyvdim0__tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem__v0 
            = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_addr;
    }
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_1 = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_1 = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_1 = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_1 = 0U;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_a_reg2 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_a_reg;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_b_reg2 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_b_reg;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_3 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__endian_little_s;
    if (((IData)(vlSelf->status_setup_done) & (~ (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_1)))) {
        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_queue = 1U;
    }
    if (((IData)(vlSelf->target_dataslot_flush_in) 
         & (~ (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_1)))) {
        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_queue = 1U;
    }
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_wren = 0U;
    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_addr 
        = (0x3ffU & (vlSelf->bridge_addr >> 2U));
    if (vlSelf->bridge_wr) {
        if ((0xf8000000U == (0xff00ff00U & vlSelf->bridge_addr))) {
            if ((0U == (0xffU & vlSelf->bridge_addr))) {
                __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_0 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in;
                if ((0x434dU == (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in 
                                 >> 0x10U))) {
                    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_start = 1U;
                }
            }
        } else if ((0xf8001000U == (0xff00ff00U & vlSelf->bridge_addr))) {
            if ((0U == (0xffU & vlSelf->bridge_addr))) {
                __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in;
            }
        } else if ((0xf8002000U == (0xff00f000U & vlSelf->bridge_addr))) {
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_wren = 1U;
        }
    }
    if (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active) {
        if ((0x10U > (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_addr))) {
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_wren = 1U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_addr 
                = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_addr;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_addr 
                = (0x1fU & ((IData)(1U) + (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_addr)));
        } else {
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active = 0U;
        }
    }
    if (vlSelf->bridge_rd) {
        if ((0xf8000000U == (0xff00ff00U & vlSelf->bridge_addr))) {
            if ((0U == (0xffU & vlSelf->bridge_addr))) {
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_0;
            } else if ((4U == (0xffU & vlSelf->bridge_addr))) {
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out = 0x20U;
            } else if ((8U == (0xffU & vlSelf->bridge_addr))) {
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out = 0x40U;
            } else if ((0x40U == (0xffU & vlSelf->bridge_addr))) {
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_40;
            } else if ((0x44U == (0xffU & vlSelf->bridge_addr))) {
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_44;
            } else if ((0x48U == (0xffU & vlSelf->bridge_addr))) {
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_48;
            } else if ((0x4cU == (0xffU & vlSelf->bridge_addr))) {
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out = 0U;
            }
        } else if ((0xf8001000U == (0xff00ff00U & vlSelf->bridge_addr))) {
            if ((0U == (0xffU & vlSelf->bridge_addr))) {
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_0;
            } else if ((4U == (0xffU & vlSelf->bridge_addr))) {
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_4;
            } else if ((8U == (0xffU & vlSelf->bridge_addr))) {
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_8;
            } else if ((0x20U == (0xffU & vlSelf->bridge_addr))) {
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_20;
            } else if ((0x24U == (0xffU & vlSelf->bridge_addr))) {
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_24;
            } else if ((0x28U == (0xffU & vlSelf->bridge_addr))) {
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_28;
            } else if ((0x2cU == (0xffU & vlSelf->bridge_addr))) {
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_2C;
            }
        } else if ((0xf8002000U == (0xff00f000U & vlSelf->bridge_addr))) {
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_b_reg;
        }
    }
    if ((0U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__hstate))) {
        vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestread = 0U;
        vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestwrite = 0U;
        vlSelf->tb_core_bridge_cmd__DOT__dataslot_update = 0U;
        vlSelf->tb_core_bridge_cmd__DOT__savestate_start = 0U;
        vlSelf->tb_core_bridge_cmd__DOT__savestate_load = 0U;
        if (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_start) {
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_start = 0U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_cmd 
                = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_startval;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 1U;
        }
    } else if ((1U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__hstate))) {
        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_0 
            = (0x42550000U | (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd));
        if (((((((((0U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd)) 
                   | (0x10U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) 
                  | (0x11U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) 
                 | (0x80U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) 
                | (0x82U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) 
               | (0x8aU == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) 
              | (0x8fU == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) 
             | (0x90U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd)))) {
            if ((0U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) {
                __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode = 1U;
                __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0xeU;
                if (vlSelf->status_boot_done) {
                    __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode = 2U;
                    if (vlSelf->status_setup_done) {
                        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode = 3U;
                    } else if (vlSelf->status_running) {
                        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode = 4U;
                    }
                }
            } else if ((0x10U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) {
                if (vlSelf->shutdown_pending) {
                    if (((IData)(vlSelf->shutdown_ack_s) 
                         | (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__shutdown_timeout_reached))) {
                        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_enter_toggle 
                            = (1U & (~ (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_enter_toggle)));
                        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_enter_count 
                            = (0xffU & ((IData)(1U) 
                                        + (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_enter_count)));
                        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_addr = 0U;
                        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_valid = 0U;
                        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_value = 0U;
                        vlSelf->shutdown_pending = 0U;
                        vlSelf->tb_core_bridge_cmd__DOT__dataslot_allcomplete = 0U;
                        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__tstate = 0U;
                        vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_ack = 0U;
                        vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_done = 0U;
                        vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_err = 0U;
                        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_queue = 0U;
                        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__warm_reset_ready_pending 
                            = ((IData)(vlSelf->reset_n) 
                               & (IData)(vlSelf->status_setup_done));
                        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_queue = 0U;
                        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_queue = 0U;
                        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_queue = 0U;
                        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_queue = 0U;
                        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_queue = 0U;
                        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active = 1U;
                        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 3U;
                        vlSelf->reset_n = 0U;
                    } else {
                        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__shutdown_timeout 
                            = (0x7fffffU & ((IData)(1U) 
                                            + vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__shutdown_timeout));
                    }
                } else {
                    vlSelf->shutdown_pending = 1U;
                    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__shutdown_timeout = 0U;
                }
            } else if ((0x11U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) {
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_exit_toggle 
                    = (1U & (~ (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_exit_toggle)));
                vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_exit_count 
                    = (0xffU & ((IData)(1U) + (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_exit_count)));
                vlSelf->reset_n = 1U;
                vlSelf->shutdown_pending = 0U;
                __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0xdU;
            } else if ((0x80U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) {
                vlSelf->tb_core_bridge_cmd__DOT__dataslot_allcomplete = 0U;
                vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestread = 1U;
                vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestread_id 
                    = (0xffffU & vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_20);
                __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode = 0U;
                __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0xeU;
            } else if ((0x82U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) {
                vlSelf->tb_core_bridge_cmd__DOT__dataslot_allcomplete = 0U;
                vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestwrite = 1U;
                vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestwrite_id 
                    = (0xffffU & vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_20);
                vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestwrite_size 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_24;
                __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode = 0U;
                __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0xeU;
            } else if ((0x8aU == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) {
                vlSelf->tb_core_bridge_cmd__DOT__dataslot_update = 1U;
                vlSelf->tb_core_bridge_cmd__DOT__dataslot_update_id 
                    = (0xffffU & vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_20);
                vlSelf->tb_core_bridge_cmd__DOT__dataslot_update_size 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_24;
                __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0xdU;
            } else if ((0x8fU == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) {
                vlSelf->tb_core_bridge_cmd__DOT__dataslot_allcomplete = 1U;
                __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0xdU;
            } else {
                vlSelf->tb_core_bridge_cmd__DOT__rtc_valid = 1U;
                vlSelf->tb_core_bridge_cmd__DOT__rtc_epoch_seconds 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_20;
                vlSelf->tb_core_bridge_cmd__DOT__rtc_date_bcd 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_24;
                vlSelf->tb_core_bridge_cmd__DOT__rtc_time_bcd 
                    = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_28;
                __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0xdU;
            }
        } else if ((0xa0U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) {
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_40 = 0U;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_44 = 0U;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_48 = 0U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode = 0U;
            if ((1U & vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_20)) {
                vlSelf->tb_core_bridge_cmd__DOT__savestate_start = 1U;
            } else {
                __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0xeU;
            }
        } else if ((0xa4U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) {
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_40 = 0U;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_44 = 0U;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_48 = 0U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode = 0U;
            if ((1U & vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_20)) {
                vlSelf->tb_core_bridge_cmd__DOT__savestate_load = 1U;
            } else {
                __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0xeU;
            }
        } else if ((0xb0U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) {
            vlSelf->tb_core_bridge_cmd__DOT__osnotify_inmenu 
                = (1U & vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_20);
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0xdU;
        } else if ((0xb1U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))) {
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_count 
                = (0xffU & ((IData)(1U) + (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_count)));
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_toggle 
                = (1U & (~ (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_toggle)));
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_value 
                = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_20;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_valid = 1U;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_last_value 
                = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_20;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0xdU;
        } else {
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate 
                = ((0xb2U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))
                    ? 0xdU : ((0xb8U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd))
                               ? 0xdU : 0xfU));
        }
    } else if ((2U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__hstate))) {
        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0U;
    } else if ((3U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__hstate))) {
        if ((1U & (~ (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active)))) {
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_queue 
                = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__warm_reset_ready_pending;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0xdU;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__warm_reset_ready_pending = 0U;
        }
    } else if ((0xdU == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__hstate))) {
        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_0 = 0x4f4b0000U;
        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0U;
    } else if ((0xeU == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__hstate))) {
        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_0 
            = (0x4f4b0000U | (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode));
        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0U;
    } else if ((0xfU == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__hstate))) {
        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_0 = 0x4f4bffffU;
        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate = 0U;
    }
    if (vlSelf->bridge_wr) {
        if ((0xf8000000U != (0xff00ff00U & vlSelf->bridge_addr))) {
            if ((0xf8001000U == (0xff00ff00U & vlSelf->bridge_addr))) {
                if ((0U != (0xffU & vlSelf->bridge_addr))) {
                    if ((4U == (0xffU & vlSelf->bridge_addr))) {
                        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_4 
                            = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in;
                    }
                    if ((4U != (0xffU & vlSelf->bridge_addr))) {
                        if ((8U == (0xffU & vlSelf->bridge_addr))) {
                            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_8 
                                = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in;
                        }
                    }
                }
            }
        }
        if ((0xf8000000U == (0xff00ff00U & vlSelf->bridge_addr))) {
            if ((0U == (0xffU & vlSelf->bridge_addr))) {
                if ((0x434dU == (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in 
                                 >> 0x10U))) {
                    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_startval 
                        = (0xffffU & vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in);
                }
            }
            if ((0U != (0xffU & vlSelf->bridge_addr))) {
                if ((0x20U == (0xffU & vlSelf->bridge_addr))) {
                    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_20 
                        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in;
                }
                if ((0x20U != (0xffU & vlSelf->bridge_addr))) {
                    if ((0x24U == (0xffU & vlSelf->bridge_addr))) {
                        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_24 
                            = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in;
                    }
                    if ((0x24U != (0xffU & vlSelf->bridge_addr))) {
                        if ((0x28U == (0xffU & vlSelf->bridge_addr))) {
                            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_28 
                                = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in;
                        }
                    }
                }
            }
        }
    }
    if ((0U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__tstate))) {
        if ((1U & (~ (IData)(vlSelf->target_dataslot_flush_in)))) {
            vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_ack = 0U;
        }
        if (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_queue) {
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_queue = 0U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__tstate = 1U;
        } else if (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_queue) {
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_queue = 0U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0 
                = (0x180U | (0xffff0000U & __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0));
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_20 
                = vlSelf->target_dataslot_id_in;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_24 = 0U;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_28 = 0U;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_2C = 0U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__tstate = 2U;
        } else if (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_queue) {
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_queue = 0U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0 
                = (0x184U | (0xffff0000U & __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0));
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_20 
                = vlSelf->target_dataslot_id_in;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_24 = 0U;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_28 = 0U;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_2C = 0U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__tstate = 2U;
        } else if (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_queue) {
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_queue = 0U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0 
                = (0x190U | (0xffff0000U & __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0));
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_20 
                = vlSelf->target_dataslot_id_in;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_24 = 0U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__tstate = 2U;
        } else if (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_queue) {
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_queue = 0U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0 
                = (0x192U | (0xffff0000U & __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0));
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_20 
                = vlSelf->target_dataslot_id_in;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_24 = 0U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__tstate = 2U;
        } else if (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_queue) {
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_queue = 0U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0 
                = (0x188U | (0xffff0000U & __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0));
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_20 
                = vlSelf->target_dataslot_id_in;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_24 = 0U;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_28 = 0U;
            vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_2C = 0U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__tstate = 2U;
        }
    } else if ((1U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__tstate))) {
        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0 = 0x636d0140U;
        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__tstate = 0xeU;
    } else if ((2U == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__tstate))) {
        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0 
            = (0x636d0000U | (0xffffU & __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0));
        vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_done = 0U;
        vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_err = 0U;
        __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__tstate = 0xfU;
    } else if ((0xfU == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__tstate))) {
        if ((0x6275U == (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_0 
                         >> 0x10U))) {
            vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_ack = 1U;
        }
        if ((0x6f6bU == (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_0 
                         >> 0x10U))) {
            vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_ack = 1U;
            vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_err 
                = (7U & vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_0);
            vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_done = 1U;
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__tstate = 0U;
        }
    } else if ((0xeU == (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__tstate))) {
        if ((0x6f6bU == (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_0 
                         >> 0x10U))) {
            __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__tstate = 0U;
        }
    }
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_a_reg 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem
        [0U];
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__endian_little_s 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_2;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_queue 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_queue;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_queue 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_queue;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_0 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_0;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_start 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_start;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_0 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_0;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_cmd;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__hstate 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__hstate;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__tstate 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__tstate;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__warm_reset_ready_pending 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__warm_reset_ready_pending;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_queue 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_queue;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_queue 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_queue;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_queue 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_queue;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_queue 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_queue;
    vlSelf->target_dataslot_ack_out = vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_ack;
    vlSelf->target_dataslot_done_out = vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_done;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__shutdown_timeout_reached 
        = (1U & (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__shutdown_timeout 
                 >> 0x16U));
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_1 
        = vlSelf->status_setup_done;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_1 
        = vlSelf->target_dataslot_flush_in;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_b_reg 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem
        [vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_addr];
    if (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__endian_little_s) {
        vlSelf->bridge_rd_data = ((vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                                   << 0x18U) | ((0xff0000U 
                                                 & (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                                                    << 8U)) 
                                                | ((0xff00U 
                                                    & (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                                                       >> 8U)) 
                                                   | (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out 
                                                      >> 0x18U))));
        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in 
            = ((vlSelf->bridge_wr_data << 0x18U) | 
               ((0xff0000U & (vlSelf->bridge_wr_data 
                              << 8U)) | ((0xff00U & 
                                          (vlSelf->bridge_wr_data 
                                           >> 8U)) 
                                         | (vlSelf->bridge_wr_data 
                                            >> 0x18U))));
    } else {
        vlSelf->bridge_rd_data = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out;
        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in 
            = vlSelf->bridge_wr_data;
    }
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_2 
        = vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_1;
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_addr 
        = __Vdly__tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_addr;
    if (__Vdlyvset__tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem__v0) {
        vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem[__Vdlyvdim0__tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem__v0] 
            = __Vdlyvval__tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__mem__v0;
    }
    vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_1 
        = vlSelf->bridge_endian_little;
}

void Vtb_core_bridge_cmd___024root___eval_nba(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval_nba\n"); );
    // Body
    if ((1ULL & vlSelf->__VnbaTriggered.word(0U))) {
        Vtb_core_bridge_cmd___024root___nba_sequent__TOP__0(vlSelf);
        vlSelf->__Vm_traceActivity[1U] = 1U;
    }
}

void Vtb_core_bridge_cmd___024root___eval_triggers__act(Vtb_core_bridge_cmd___024root* vlSelf);

bool Vtb_core_bridge_cmd___024root___eval_phase__act(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval_phase__act\n"); );
    // Init
    VlTriggerVec<1> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vtb_core_bridge_cmd___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelf->__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelf->__VactTriggered, vlSelf->__VnbaTriggered);
        vlSelf->__VnbaTriggered.thisOr(vlSelf->__VactTriggered);
        Vtb_core_bridge_cmd___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vtb_core_bridge_cmd___024root___eval_phase__nba(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval_phase__nba\n"); );
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelf->__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vtb_core_bridge_cmd___024root___eval_nba(vlSelf);
        vlSelf->__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___dump_triggers__ico(Vtb_core_bridge_cmd___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___dump_triggers__nba(Vtb_core_bridge_cmd___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_core_bridge_cmd___024root___dump_triggers__act(Vtb_core_bridge_cmd___024root* vlSelf);
#endif  // VL_DEBUG

void Vtb_core_bridge_cmd___024root___eval(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval\n"); );
    // Init
    IData/*31:0*/ __VicoIterCount;
    CData/*0:0*/ __VicoContinue;
    IData/*31:0*/ __VnbaIterCount;
    CData/*0:0*/ __VnbaContinue;
    // Body
    __VicoIterCount = 0U;
    vlSelf->__VicoFirstIteration = 1U;
    __VicoContinue = 1U;
    while (__VicoContinue) {
        if (VL_UNLIKELY((0x64U < __VicoIterCount))) {
#ifdef VL_DEBUG
            Vtb_core_bridge_cmd___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("tb_core_bridge_cmd.v", 9, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vtb_core_bridge_cmd___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelf->__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY((0x64U < __VnbaIterCount))) {
#ifdef VL_DEBUG
            Vtb_core_bridge_cmd___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("tb_core_bridge_cmd.v", 9, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelf->__VactIterCount = 0U;
        vlSelf->__VactContinue = 1U;
        while (vlSelf->__VactContinue) {
            if (VL_UNLIKELY((0x64U < vlSelf->__VactIterCount))) {
#ifdef VL_DEBUG
                Vtb_core_bridge_cmd___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("tb_core_bridge_cmd.v", 9, "", "Active region did not converge.");
            }
            vlSelf->__VactIterCount = ((IData)(1U) 
                                       + vlSelf->__VactIterCount);
            vlSelf->__VactContinue = 0U;
            if (Vtb_core_bridge_cmd___024root___eval_phase__act(vlSelf)) {
                vlSelf->__VactContinue = 1U;
            }
        }
        if (Vtb_core_bridge_cmd___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vtb_core_bridge_cmd___024root___eval_debug_assertions(Vtb_core_bridge_cmd___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root___eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((vlSelf->clk & 0xfeU))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY((vlSelf->bridge_endian_little & 0xfeU))) {
        Verilated::overWidthError("bridge_endian_little");}
    if (VL_UNLIKELY((vlSelf->bridge_rd & 0xfeU))) {
        Verilated::overWidthError("bridge_rd");}
    if (VL_UNLIKELY((vlSelf->bridge_wr & 0xfeU))) {
        Verilated::overWidthError("bridge_wr");}
    if (VL_UNLIKELY((vlSelf->status_boot_done & 0xfeU))) {
        Verilated::overWidthError("status_boot_done");}
    if (VL_UNLIKELY((vlSelf->status_setup_done & 0xfeU))) {
        Verilated::overWidthError("status_setup_done");}
    if (VL_UNLIKELY((vlSelf->status_running & 0xfeU))) {
        Verilated::overWidthError("status_running");}
    if (VL_UNLIKELY((vlSelf->shutdown_ack_s & 0xfeU))) {
        Verilated::overWidthError("shutdown_ack_s");}
    if (VL_UNLIKELY((vlSelf->target_dataslot_flush_in 
                     & 0xfeU))) {
        Verilated::overWidthError("target_dataslot_flush_in");}
}
#endif  // VL_DEBUG
