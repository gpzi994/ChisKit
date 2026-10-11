// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vflashgbx_cart_gb_readonly_abstract.h for the primary calling header

#include "Vflashgbx_cart_gb_readonly_abstract__pch.h"
#include "Vflashgbx_cart_gb_readonly_abstract___024root.h"

VL_INLINE_OPT void Vflashgbx_cart_gb_readonly_abstract___024root___ico_sequent__TOP__0(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gb_readonly_abstract__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gb_readonly_abstract___024root___ico_sequent__TOP__0\n"); );
    // Body
    vlSelf->reg_rdata = 0U;
    if ((0x10U & (IData)(vlSelf->reg_addr))) {
        vlSelf->reg_rdata = ((8U & (IData)(vlSelf->reg_addr))
                              ? 0U : ((4U & (IData)(vlSelf->reg_addr))
                                       ? 0U : ((2U 
                                                & (IData)(vlSelf->reg_addr))
                                                ? 0U
                                                : (
                                                   (1U 
                                                    & (IData)(vlSelf->reg_addr))
                                                    ? 0U
                                                    : vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__space_reg))));
    } else if ((8U & (IData)(vlSelf->reg_addr))) {
        if ((4U & (IData)(vlSelf->reg_addr))) {
            vlSelf->reg_rdata = ((2U & (IData)(vlSelf->reg_addr))
                                  ? ((1U & (IData)(vlSelf->reg_addr))
                                      ? vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_result_reg
                                      : vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_arg_reg)
                                  : ((1U & (IData)(vlSelf->reg_addr))
                                      ? vlSelf->host_diag
                                      : vlSelf->host_b1_raw));
        } else if ((2U & (IData)(vlSelf->reg_addr))) {
            vlSelf->reg_rdata = ((1U & (IData)(vlSelf->reg_addr))
                                  ? vlSelf->lifecycle_status
                                  : vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__error_reg);
        } else if ((1U & (IData)(vlSelf->reg_addr))) {
            vlSelf->reg_rdata = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count;
        } else {
            if ((0U < vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count)) {
                vlSelf->reg_rdata = ((0xffffff00U & vlSelf->reg_rdata) 
                                     | vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo
                                     [(0x1fU & vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_read_index)]);
            }
            if ((1U < vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count)) {
                vlSelf->reg_rdata = ((0xffff00ffU & vlSelf->reg_rdata) 
                                     | (vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo
                                        [(0x1fU & ((IData)(1U) 
                                                   + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_read_index))] 
                                        << 8U));
            }
            if ((2U < vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count)) {
                vlSelf->reg_rdata = ((0xff00ffffU & vlSelf->reg_rdata) 
                                     | (vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo
                                        [(0x1fU & ((IData)(2U) 
                                                   + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_read_index))] 
                                        << 0x10U));
            }
            if ((3U < vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count)) {
                vlSelf->reg_rdata = ((0xffffffU & vlSelf->reg_rdata) 
                                     | (vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo
                                        [(0x1fU & ((IData)(3U) 
                                                   + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_read_index))] 
                                        << 0x18U));
            }
        }
    } else {
        vlSelf->reg_rdata = ((4U & (IData)(vlSelf->reg_addr))
                              ? ((2U & (IData)(vlSelf->reg_addr))
                                  ? ((1U & (IData)(vlSelf->reg_addr))
                                      ? vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__timeout_reg
                                      : vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__length_reg)
                                  : ((1U & (IData)(vlSelf->reg_addr))
                                      ? vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__address_reg
                                      : vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__bus_reg))
                              : ((2U & (IData)(vlSelf->reg_addr))
                                  ? ((1U & (IData)(vlSelf->reg_addr))
                                      ? 0U : vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__status_reg)
                                  : ((1U & (IData)(vlSelf->reg_addr))
                                      ? 0x30f01U : 0x31424746U)));
    }
}

void Vflashgbx_cart_gb_readonly_abstract___024root___eval_ico(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gb_readonly_abstract__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gb_readonly_abstract___024root___eval_ico\n"); );
    // Body
    if ((1ULL & vlSelf->__VicoTriggered.word(0U))) {
        Vflashgbx_cart_gb_readonly_abstract___024root___ico_sequent__TOP__0(vlSelf);
    }
}

void Vflashgbx_cart_gb_readonly_abstract___024root___eval_triggers__ico(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf);

bool Vflashgbx_cart_gb_readonly_abstract___024root___eval_phase__ico(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gb_readonly_abstract__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gb_readonly_abstract___024root___eval_phase__ico\n"); );
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vflashgbx_cart_gb_readonly_abstract___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelf->__VicoTriggered.any();
    if (__VicoExecute) {
        Vflashgbx_cart_gb_readonly_abstract___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vflashgbx_cart_gb_readonly_abstract___024root___eval_act(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gb_readonly_abstract__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gb_readonly_abstract___024root___eval_act\n"); );
}

VL_INLINE_OPT void Vflashgbx_cart_gb_readonly_abstract___024root___nba_sequent__TOP__0(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gb_readonly_abstract__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gb_readonly_abstract___024root___nba_sequent__TOP__0\n"); );
    // Init
    QData/*32:0*/ flashgbx_cart_gb_readonly_abstract__DOT__read_last_address;
    flashgbx_cart_gb_readonly_abstract__DOT__read_last_address = 0;
    CData/*2:0*/ __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state = 0;
    IData/*31:0*/ __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter = 0;
    IData/*31:0*/ __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter = 0;
    IData/*31:0*/ __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__fifo_count;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__fifo_count = 0;
    IData/*31:0*/ __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__space_reg;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__space_reg = 0;
    IData/*31:0*/ __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__query_arg_reg;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__query_arg_reg = 0;
    IData/*31:0*/ __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__length_reg;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__length_reg = 0;
    IData/*31:0*/ __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__address_reg;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__address_reg = 0;
    SData/*15:0*/ __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__byte_address;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__byte_address = 0;
    IData/*31:0*/ __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__bytes_remaining;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__bytes_remaining = 0;
    CData/*0:0*/ __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__last_byte;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__last_byte = 0;
    CData/*4:0*/ __Vdlyvdim0__flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo__v0;
    __Vdlyvdim0__flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo__v0 = 0;
    CData/*7:0*/ __Vdlyvval__flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo__v0;
    __Vdlyvval__flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo__v0 = 0;
    CData/*0:0*/ __Vdlyvset__flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo__v0;
    __Vdlyvset__flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo__v0 = 0;
    // Body
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__last_byte 
        = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__last_byte;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__bytes_remaining 
        = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__bytes_remaining;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter 
        = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter 
        = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__phase_counter;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__byte_address 
        = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__byte_address;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state 
        = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__query_arg_reg 
        = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_arg_reg;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__fifo_count 
        = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count;
    __Vdlyvset__flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo__v0 = 0U;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__space_reg 
        = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__space_reg;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__address_reg 
        = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__address_reg;
    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__length_reg 
        = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__length_reg;
    if (vlSelf->reset_n) {
        if ((((IData)(vlSelf->reg_wr) & (3U == (IData)(vlSelf->reg_addr))) 
             & (3U == vlSelf->reg_wdata))) {
            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state = 0U;
            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__status_reg = 0x11U;
            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__error_reg = 0U;
            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter = 0U;
            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter = 0U;
        } else if (((IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__active) 
                    & (~ (IData)(vlSelf->lifecycle_ok)))) {
            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state = 0U;
            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__status_reg = 9U;
            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__error_reg = 3U;
            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter = 0U;
            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter = 0U;
        } else if (((IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__active) 
                    & (((IData)(1U) + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter) 
                       >= vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__timeout_reg))) {
            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state = 0U;
            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__status_reg = 0x29U;
            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__error_reg = 4U;
            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter = 0U;
            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter = 0U;
        } else {
            if (vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__active) {
                __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter 
                    = ((IData)(1U) + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter);
            }
            if (((((IData)(vlSelf->reg_rd) & (8U == (IData)(vlSelf->reg_addr))) 
                  & (~ (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__active))) 
                 & (0U != vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count))) {
                if ((4U < vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count)) {
                    vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_read_index 
                        = ((IData)(4U) + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_read_index);
                    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__fifo_count 
                        = (vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count 
                           - (IData)(4U));
                } else {
                    vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_read_index = 0U;
                    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__fifo_count = 0U;
                }
            }
            if (((IData)(vlSelf->reg_wr) & (~ (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__active)))) {
                if ((0x10U & (IData)(vlSelf->reg_addr))) {
                    if ((1U & (~ ((IData)(vlSelf->reg_addr) 
                                  >> 3U)))) {
                        if ((1U & (~ ((IData)(vlSelf->reg_addr) 
                                      >> 2U)))) {
                            if ((1U & (~ ((IData)(vlSelf->reg_addr) 
                                          >> 1U)))) {
                                if ((1U & (~ (IData)(vlSelf->reg_addr)))) {
                                    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__space_reg 
                                        = vlSelf->reg_wdata;
                                }
                            }
                        }
                    }
                } else if ((8U & (IData)(vlSelf->reg_addr))) {
                    if ((4U & (IData)(vlSelf->reg_addr))) {
                        if ((2U & (IData)(vlSelf->reg_addr))) {
                            if ((1U & (~ (IData)(vlSelf->reg_addr)))) {
                                __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__query_arg_reg 
                                    = vlSelf->reg_wdata;
                            }
                        }
                    }
                } else if ((4U & (IData)(vlSelf->reg_addr))) {
                    if ((2U & (IData)(vlSelf->reg_addr))) {
                        if ((1U & (IData)(vlSelf->reg_addr))) {
                            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__timeout_reg 
                                = vlSelf->reg_wdata;
                        } else {
                            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__length_reg 
                                = vlSelf->reg_wdata;
                        }
                    } else if ((1U & (IData)(vlSelf->reg_addr))) {
                        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__address_reg 
                            = vlSelf->reg_wdata;
                    } else {
                        vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__bus_reg 
                            = vlSelf->reg_wdata;
                    }
                } else if ((2U & (IData)(vlSelf->reg_addr))) {
                    if ((1U & (IData)(vlSelf->reg_addr))) {
                        if ((1U == vlSelf->reg_wdata)) {
                            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__fifo_count = 0U;
                            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_read_index = 0U;
                            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state = 0U;
                            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__status_reg = 1U;
                            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__error_reg = 0U;
                            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter = 0U;
                        } else if ((2U == vlSelf->reg_wdata)) {
                            if ((1U & ((~ (IData)(vlSelf->lifecycle_ok)) 
                                       | (~ (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__read_shape_valid))))) {
                                vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__status_reg = 9U;
                                vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__error_reg 
                                    = ((IData)(vlSelf->lifecycle_ok)
                                        ? 1U : 3U);
                            } else {
                                __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__fifo_count = 0U;
                                vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_read_index = 0U;
                                __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state = 1U;
                                vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__status_reg = 2U;
                                vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__error_reg = 0U;
                                __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__byte_address 
                                    = (0xffffU & vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__address_reg);
                                __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__bytes_remaining 
                                    = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__length_reg;
                                __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter = 0U;
                                __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter = 0U;
                                __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__last_byte = 0U;
                                vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__save_select 
                                    = (1U == vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__space_reg);
                            }
                        } else if ((0xa1U == vlSelf->reg_wdata)) {
                            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state = 0U;
                            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__error_reg = 0U;
                            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__status_reg = 5U;
                            if ((0U == vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_arg_reg)) {
                                vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_result_reg = 0x50424746U;
                            } else if ((1U == vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_arg_reg)) {
                                vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_result_reg = 0x10000U;
                            } else if ((2U == vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_arg_reg)) {
                                vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_result_reg = 3U;
                            } else {
                                vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_result_reg = 0U;
                                vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__error_reg = 1U;
                                vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__status_reg = 9U;
                            }
                        } else if ((0xfeU == vlSelf->reg_wdata)) {
                            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state = 0U;
                            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__error_reg = 0U;
                            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__status_reg = 5U;
                            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_result_reg 
                                = (~ vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_arg_reg);
                        } else if ((3U != vlSelf->reg_wdata)) {
                            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__status_reg = 9U;
                            vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__error_reg = 1U;
                        }
                    }
                }
            }
            if ((4U & (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state))) {
                if ((1U & (~ ((IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state) 
                              >> 1U)))) {
                    if ((1U & (~ (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state)))) {
                        if ((2U <= ((IData)(1U) + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__phase_counter))) {
                            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter = 0U;
                            if (vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__last_byte) {
                                __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state = 0U;
                                vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__status_reg = 5U;
                                __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter = 0U;
                            } else {
                                __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__byte_address 
                                    = (0xffffU & ((IData)(1U) 
                                                  + (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__byte_address)));
                                __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state = 1U;
                            }
                        } else {
                            __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter 
                                = ((IData)(1U) + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__phase_counter);
                        }
                    }
                }
            } else if ((2U & (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state))) {
                if ((1U & (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state))) {
                    if ((3U <= ((IData)(1U) + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__phase_counter))) {
                        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__bytes_remaining 
                            = (vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__bytes_remaining 
                               - (IData)(1U));
                        __Vdlyvval__flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo__v0 
                            = vlSelf->gb_data_in;
                        __Vdlyvset__flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo__v0 = 1U;
                        __Vdlyvdim0__flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo__v0 
                            = (0x1fU & vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count);
                        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__last_byte 
                            = (1U == vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__bytes_remaining);
                        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state = 4U;
                        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter = 0U;
                        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__fifo_count 
                            = ((IData)(1U) + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count);
                    } else {
                        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter 
                            = ((IData)(1U) + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__phase_counter);
                    }
                } else if ((2U <= ((IData)(1U) + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__phase_counter))) {
                    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state = 3U;
                    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter = 0U;
                } else {
                    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter 
                        = ((IData)(1U) + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__phase_counter);
                }
            } else if ((1U & (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state))) {
                if ((2U <= ((IData)(1U) + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__phase_counter))) {
                    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state = 2U;
                    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter = 0U;
                } else {
                    __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter 
                        = ((IData)(1U) + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__phase_counter);
                }
            }
        }
    } else {
        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state = 0U;
        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__byte_address = 0U;
        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__bytes_remaining = 0U;
        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter = 0U;
        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter = 0U;
        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__fifo_count = 0U;
        vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_read_index = 0U;
        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__last_byte = 0U;
        vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__status_reg = 1U;
        vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__error_reg = 0U;
        vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__bus_reg = 0U;
        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__space_reg = 0U;
        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__address_reg = 0U;
        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__length_reg = 0U;
        vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__timeout_reg = 0U;
        __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__query_arg_reg = 0U;
        vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_result_reg = 0U;
        vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__save_select = 0U;
    }
    vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__phase_counter 
        = __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__phase_counter;
    vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter 
        = __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__watchdog_counter;
    vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__bytes_remaining 
        = __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__bytes_remaining;
    vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__last_byte 
        = __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__last_byte;
    vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__byte_address 
        = __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__byte_address;
    vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state 
        = __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__state;
    vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count 
        = __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__fifo_count;
    vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_arg_reg 
        = __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__query_arg_reg;
    if (__Vdlyvset__flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo__v0) {
        vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo[__Vdlyvdim0__flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo__v0] 
            = __Vdlyvval__flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo__v0;
    }
    vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__space_reg 
        = __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__space_reg;
    vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__length_reg 
        = __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__length_reg;
    vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__address_reg 
        = __Vdly__flashgbx_cart_gb_readonly_abstract__DOT__address_reg;
    vlSelf->gb_address_dir = 0U;
    vlSelf->gb_rd_n = 1U;
    if ((1U & (~ ((IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state) 
                  >> 2U)))) {
        if ((2U & (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state))) {
            if ((1U & (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state))) {
                vlSelf->gb_rd_n = 0U;
            }
        }
    }
    vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__active 
        = (0U != (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state));
    vlSelf->gb_address = 0U;
    vlSelf->gb_cs_n = 1U;
    if ((4U & (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state))) {
        if ((1U & (~ ((IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state) 
                      >> 1U)))) {
            if ((1U & (~ (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state)))) {
                vlSelf->gb_address_dir = 1U;
                vlSelf->gb_address = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__byte_address;
                vlSelf->gb_cs_n = (1U & (~ (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__save_select)));
            }
        }
    } else if ((2U & (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state))) {
        vlSelf->gb_address_dir = 1U;
        vlSelf->gb_address = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__byte_address;
        vlSelf->gb_cs_n = (1U & ((1U & (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state))
                                  ? (~ (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__save_select))
                                  : (~ (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__save_select))));
    } else if ((1U & (IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__state))) {
        vlSelf->gb_address_dir = 1U;
        vlSelf->gb_address = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__byte_address;
    }
    flashgbx_cart_gb_readonly_abstract__DOT__read_last_address 
        = (0x1ffffffffULL & (((QData)((IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__address_reg)) 
                              + (QData)((IData)(vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__length_reg))) 
                             - 1ULL));
    vlSelf->reg_rdata = 0U;
    if ((0x10U & (IData)(vlSelf->reg_addr))) {
        vlSelf->reg_rdata = ((8U & (IData)(vlSelf->reg_addr))
                              ? 0U : ((4U & (IData)(vlSelf->reg_addr))
                                       ? 0U : ((2U 
                                                & (IData)(vlSelf->reg_addr))
                                                ? 0U
                                                : (
                                                   (1U 
                                                    & (IData)(vlSelf->reg_addr))
                                                    ? 0U
                                                    : vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__space_reg))));
    } else if ((8U & (IData)(vlSelf->reg_addr))) {
        if ((4U & (IData)(vlSelf->reg_addr))) {
            vlSelf->reg_rdata = ((2U & (IData)(vlSelf->reg_addr))
                                  ? ((1U & (IData)(vlSelf->reg_addr))
                                      ? vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_result_reg
                                      : vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__query_arg_reg)
                                  : ((1U & (IData)(vlSelf->reg_addr))
                                      ? vlSelf->host_diag
                                      : vlSelf->host_b1_raw));
        } else if ((2U & (IData)(vlSelf->reg_addr))) {
            vlSelf->reg_rdata = ((1U & (IData)(vlSelf->reg_addr))
                                  ? vlSelf->lifecycle_status
                                  : vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__error_reg);
        } else if ((1U & (IData)(vlSelf->reg_addr))) {
            vlSelf->reg_rdata = vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count;
        } else {
            if ((0U < vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count)) {
                vlSelf->reg_rdata = ((0xffffff00U & vlSelf->reg_rdata) 
                                     | vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo
                                     [(0x1fU & vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_read_index)]);
            }
            if ((1U < vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count)) {
                vlSelf->reg_rdata = ((0xffff00ffU & vlSelf->reg_rdata) 
                                     | (vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo
                                        [(0x1fU & ((IData)(1U) 
                                                   + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_read_index))] 
                                        << 8U));
            }
            if ((2U < vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count)) {
                vlSelf->reg_rdata = ((0xff00ffffU & vlSelf->reg_rdata) 
                                     | (vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo
                                        [(0x1fU & ((IData)(2U) 
                                                   + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_read_index))] 
                                        << 0x10U));
            }
            if ((3U < vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_count)) {
                vlSelf->reg_rdata = ((0xffffffU & vlSelf->reg_rdata) 
                                     | (vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__rx_fifo
                                        [(0x1fU & ((IData)(3U) 
                                                   + vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__fifo_read_index))] 
                                        << 0x18U));
            }
        }
    } else {
        vlSelf->reg_rdata = ((4U & (IData)(vlSelf->reg_addr))
                              ? ((2U & (IData)(vlSelf->reg_addr))
                                  ? ((1U & (IData)(vlSelf->reg_addr))
                                      ? vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__timeout_reg
                                      : vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__length_reg)
                                  : ((1U & (IData)(vlSelf->reg_addr))
                                      ? vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__address_reg
                                      : vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__bus_reg))
                              : ((2U & (IData)(vlSelf->reg_addr))
                                  ? ((1U & (IData)(vlSelf->reg_addr))
                                      ? 0U : vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__status_reg)
                                  : ((1U & (IData)(vlSelf->reg_addr))
                                      ? 0x30f01U : 0x31424746U)));
    }
    vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__read_shape_valid 
        = ((1U == vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__bus_reg) 
           & ((0U != vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__length_reg) 
              & ((0x20U >= vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__length_reg) 
                 & ((((0U == vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__space_reg) 
                      & ((0U == (vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__address_reg 
                                 >> 0x10U)) & ((0x7fffU 
                                                >= 
                                                (0xffffU 
                                                 & vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__address_reg)) 
                                               & (0x7fffULL 
                                                  >= flashgbx_cart_gb_readonly_abstract__DOT__read_last_address)))) 
                     | ((1U == vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__space_reg) 
                        & ((0U == (vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__address_reg 
                                   >> 0x10U)) & ((0xa000U 
                                                  <= 
                                                  (0xffffU 
                                                   & vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__address_reg)) 
                                                 & ((0xbfffU 
                                                     >= 
                                                     (0xffffU 
                                                      & vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__address_reg)) 
                                                    & (0xbfffULL 
                                                       >= flashgbx_cart_gb_readonly_abstract__DOT__read_last_address)))))) 
                    & (0U != vlSelf->flashgbx_cart_gb_readonly_abstract__DOT__timeout_reg)))));
}

void Vflashgbx_cart_gb_readonly_abstract___024root___eval_nba(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gb_readonly_abstract__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gb_readonly_abstract___024root___eval_nba\n"); );
    // Body
    if ((1ULL & vlSelf->__VnbaTriggered.word(0U))) {
        Vflashgbx_cart_gb_readonly_abstract___024root___nba_sequent__TOP__0(vlSelf);
    }
}

void Vflashgbx_cart_gb_readonly_abstract___024root___eval_triggers__act(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf);

bool Vflashgbx_cart_gb_readonly_abstract___024root___eval_phase__act(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gb_readonly_abstract__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gb_readonly_abstract___024root___eval_phase__act\n"); );
    // Init
    VlTriggerVec<1> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vflashgbx_cart_gb_readonly_abstract___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelf->__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelf->__VactTriggered, vlSelf->__VnbaTriggered);
        vlSelf->__VnbaTriggered.thisOr(vlSelf->__VactTriggered);
        Vflashgbx_cart_gb_readonly_abstract___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vflashgbx_cart_gb_readonly_abstract___024root___eval_phase__nba(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gb_readonly_abstract__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gb_readonly_abstract___024root___eval_phase__nba\n"); );
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelf->__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vflashgbx_cart_gb_readonly_abstract___024root___eval_nba(vlSelf);
        vlSelf->__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vflashgbx_cart_gb_readonly_abstract___024root___dump_triggers__ico(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vflashgbx_cart_gb_readonly_abstract___024root___dump_triggers__nba(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vflashgbx_cart_gb_readonly_abstract___024root___dump_triggers__act(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf);
#endif  // VL_DEBUG

void Vflashgbx_cart_gb_readonly_abstract___024root___eval(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gb_readonly_abstract__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gb_readonly_abstract___024root___eval\n"); );
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
            Vflashgbx_cart_gb_readonly_abstract___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("../targets/pocket/flashgbx_cart_gb_readonly_abstract.v", 13, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vflashgbx_cart_gb_readonly_abstract___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelf->__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY((0x64U < __VnbaIterCount))) {
#ifdef VL_DEBUG
            Vflashgbx_cart_gb_readonly_abstract___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("../targets/pocket/flashgbx_cart_gb_readonly_abstract.v", 13, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelf->__VactIterCount = 0U;
        vlSelf->__VactContinue = 1U;
        while (vlSelf->__VactContinue) {
            if (VL_UNLIKELY((0x64U < vlSelf->__VactIterCount))) {
#ifdef VL_DEBUG
                Vflashgbx_cart_gb_readonly_abstract___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("../targets/pocket/flashgbx_cart_gb_readonly_abstract.v", 13, "", "Active region did not converge.");
            }
            vlSelf->__VactIterCount = ((IData)(1U) 
                                       + vlSelf->__VactIterCount);
            vlSelf->__VactContinue = 0U;
            if (Vflashgbx_cart_gb_readonly_abstract___024root___eval_phase__act(vlSelf)) {
                vlSelf->__VactContinue = 1U;
            }
        }
        if (Vflashgbx_cart_gb_readonly_abstract___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vflashgbx_cart_gb_readonly_abstract___024root___eval_debug_assertions(Vflashgbx_cart_gb_readonly_abstract___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vflashgbx_cart_gb_readonly_abstract__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vflashgbx_cart_gb_readonly_abstract___024root___eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((vlSelf->clk & 0xfeU))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY((vlSelf->reset_n & 0xfeU))) {
        Verilated::overWidthError("reset_n");}
    if (VL_UNLIKELY((vlSelf->lifecycle_ok & 0xfeU))) {
        Verilated::overWidthError("lifecycle_ok");}
    if (VL_UNLIKELY((vlSelf->reg_wr & 0xfeU))) {
        Verilated::overWidthError("reg_wr");}
    if (VL_UNLIKELY((vlSelf->reg_rd & 0xfeU))) {
        Verilated::overWidthError("reg_rd");}
    if (VL_UNLIKELY((vlSelf->reg_addr & 0xe0U))) {
        Verilated::overWidthError("reg_addr");}
}
#endif  // VL_DEBUG
