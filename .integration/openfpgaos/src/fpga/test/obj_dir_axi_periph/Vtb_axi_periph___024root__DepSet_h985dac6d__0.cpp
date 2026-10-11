// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtb_axi_periph.h for the primary calling header

#include "Vtb_axi_periph__pch.h"
#include "Vtb_axi_periph___024root.h"

VL_INLINE_OPT void Vtb_axi_periph___024root___ico_sequent__TOP__0(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___ico_sequent__TOP__0\n"); );
    // Init
    CData/*3:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__Vfuncout;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__addr;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__addr = 0;
    CData/*3:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__Vfuncout;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__addr;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__addr = 0;
    // Body
    __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__addr 
        = vlSelf->s_axi_araddr;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__Vfuncout 
        = ((0U == (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__addr 
                   >> 0xfU)) ? 1U : ((0x200000U == 
                                      (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__addr 
                                       >> 9U)) ? 2U
                                      : ((0x48U == 
                                          (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__addr 
                                           >> 0x18U))
                                          ? 8U : ((0x4aU 
                                                   == 
                                                   (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__addr 
                                                    >> 0x18U))
                                                   ? 7U
                                                   : 
                                                  ((0x4cU 
                                                    == 
                                                    (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__addr 
                                                     >> 0x18U))
                                                    ? 3U
                                                    : 
                                                   ((0x4dU 
                                                     == 
                                                     (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__addr 
                                                      >> 0x18U))
                                                     ? 5U
                                                     : 
                                                    ((0x49U 
                                                      == 
                                                      (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__addr 
                                                       >> 0x18U))
                                                      ? 9U
                                                      : 
                                                     ((0x4eU 
                                                       == 
                                                       (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__addr 
                                                        >> 0x18U))
                                                       ? 4U
                                                       : 
                                                      ((0x4fU 
                                                        == 
                                                        (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__addr 
                                                         >> 0x18U))
                                                        ? 6U
                                                        : 0U)))))))));
    vlSelf->tb_axi_periph__DOT__dut__DOT__ar_region 
        = __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__Vfuncout;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__addr 
        = vlSelf->s_axi_awaddr;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__Vfuncout 
        = ((0U == (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__addr 
                   >> 0xfU)) ? 1U : ((0x200000U == 
                                      (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__addr 
                                       >> 9U)) ? 2U
                                      : ((0x48U == 
                                          (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__addr 
                                           >> 0x18U))
                                          ? 8U : ((0x4aU 
                                                   == 
                                                   (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__addr 
                                                    >> 0x18U))
                                                   ? 7U
                                                   : 
                                                  ((0x4cU 
                                                    == 
                                                    (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__addr 
                                                     >> 0x18U))
                                                    ? 3U
                                                    : 
                                                   ((0x4dU 
                                                     == 
                                                     (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__addr 
                                                      >> 0x18U))
                                                     ? 5U
                                                     : 
                                                    ((0x49U 
                                                      == 
                                                      (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__addr 
                                                       >> 0x18U))
                                                      ? 9U
                                                      : 
                                                     ((0x4eU 
                                                       == 
                                                       (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__addr 
                                                        >> 0x18U))
                                                       ? 4U
                                                       : 
                                                      ((0x4fU 
                                                        == 
                                                        (__Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__addr 
                                                         >> 0x18U))
                                                        ? 6U
                                                        : 0U)))))))));
    vlSelf->tb_axi_periph__DOT__dut__DOT__aw_region 
        = __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__Vfuncout;
    vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat 
        = (1U & ((~ (IData)(vlSelf->s_axi_rvalid)) 
                 | ((IData)(vlSelf->s_axi_rready) & (IData)(vlSelf->s_axi_rvalid))));
    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata = 0U;
    if ((0x100U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
        if (((vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
              >> 8U) & ((1U <= (0xfU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                        >> 4U))) & 
                        (4U >= (0xfU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                        >> 4U)))))) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_rdata;
        } else if ((0x80U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                          >> 6U)))) {
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 5U)))) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 4U)))) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 3U)))) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 2U)))) {
                                vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                                    = vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key1;
                            }
                        }
                    }
                }
            }
        } else if ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
            if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 2U)))) {
                            vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                                = ((0xf000U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_released_acc) 
                                               << 0xcU)) 
                                   | ((0xf00U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_pressed_acc) 
                                                 << 8U)) 
                                      | (0xffU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key1))));
                        }
                    } else {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                            = ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? ((((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                      >> 0x18U) - (IData)(0x80U)) 
                                    << 0x18U) | (0xff00U 
                                                 & (((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                                      >> 0x10U) 
                                                     - (IData)(0x80U)) 
                                                    << 8U)))
                                : ((((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                      >> 8U) - (IData)(0x80U)) 
                                    << 0x18U) | (0xff00U 
                                                 & ((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                                     - (IData)(0x80U)) 
                                                    << 8U))));
                    }
                } else {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                        = ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_released_acc)
                                : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_pressed_acc))
                            : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key1)
                                : (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg) 
                                    << 8U) | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_fast_reg) 
                                               << 2U) 
                                              | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg)))));
                }
            } else if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 2U)))) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                            = vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count;
                    }
                } else {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                        = ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? (IData)((vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_dout 
                                       >> 0x20U)) : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_dout));
                }
            }
        } else if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                             >> 5U)))) {
            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                          >> 4U)))) {
                if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                            = vlSelf->tb_axi_periph__DOT__dut__DOT__input_seq;
                    }
                } else {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                        = ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask)
                            : (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count) 
                                << 8U) | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_overflow) 
                                           << 3U) | 
                                          (((0x10U 
                                             == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
                                            << 2U) 
                                           | (((0U 
                                                == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
                                               << 1U) 
                                              | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_pending))))));
                }
            }
        }
    } else if ((0x80U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
        if ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
            if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                                = vlSelf->tb_axi_periph__DOT__dut__DOT__irq_mask;
                        }
                    }
                } else {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                        = ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__video_scaler_slot_reg)
                                : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_stride_reg))
                            : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_height_reg) 
                                    << 0x10U) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_width_reg))
                                : 0U));
                }
            } else if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                            = ((1U & (IData)(((0U != 
                                               (0x801fU 
                                                & vlSelf->analogizer_settings_in)) 
                                              | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_en_reg))))
                                ? (((vlSelf->analogizer_settings_in 
                                     >> 0xfU) & ((4U 
                                                  == 
                                                  (0xfU 
                                                   & (vlSelf->analogizer_settings_in 
                                                      >> 0xaU))) 
                                                 | (0xcU 
                                                    == 
                                                    (0xfU 
                                                     & (vlSelf->analogizer_settings_in 
                                                        >> 0xaU)))))
                                    ? 0x276U : 0x20dU)
                                : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__vrr_v_total_reg));
                    }
                } else if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                     >> 2U)))) {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__scanout_fetch_diag;
                }
            } else {
                vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                    = ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? vlSelf->tb_axi_periph__DOT__dut__DOT__sdram_wlast_err
                            : (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__save_dt_word_mode) 
                                << 0xaU) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__save_dt_word)))
                        : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? 0U : (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__term_fb_active) 
                                     << 0x13U) | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_ready_idx) 
                                                   << 3U) 
                                                  | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx) 
                                                      << 1U) 
                                                     | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_pending))))));
            }
        } else {
            vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                = ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                    ? ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? vlSelf->tb_axi_periph__DOT__dut__DOT__timer_counter
                                : (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_irq_pending) 
                                    << 1U) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_enable)))
                            : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? vlSelf->tb_axi_periph__DOT__dut__DOT__timer_period
                                : 0U)) : ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                           ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                               ? (((IData)(vlSelf->dbg_snac_pin_dir) 
                                                   << 8U) 
                                                  | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync))
                                               : vlSelf->tb_axi_periph__DOT__dut__DOT__snac_rx_data)
                                           : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                               ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_div_reg)
                                               : (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_mode_reg) 
                                                   << 8U) 
                                                  | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_en_reg) 
                                                      << 7U) 
                                                     | (0U 
                                                        != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state)))))))
                    : ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_irq_pending)
                                : 0x3ffe35fU) : ((4U 
                                                  & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                  ? vlSelf->dt_query_data_in
                                                  : 
                                                 (((IData)(vlSelf->dt_query_valid_in) 
                                                   << 0x1fU) 
                                                  | (0x7fffffffU 
                                                     & vlSelf->dt_query_data_in))))
                        : ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? 2U : 5U) : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                               ? 0xfffffff9U
                                               : vlSelf->analogizer_settings_in))));
        }
    } else if ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
        if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
            if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 3U)))) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 2U)))) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                            = vlSelf->tb_axi_periph__DOT__dut__DOT__color_mode_reg;
                    }
                }
            } else {
                vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                    = ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? vlSelf->tb_axi_periph__DOT__dut__DOT__mouse_speed_pct
                            : 0U) : vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_rdata);
            }
        } else if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_rdata;
        } else if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                             >> 3U)))) {
            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                          >> 2U)))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                    = (((IData)(vlSelf->pal_busy_in) 
                        << 0x1fU) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__pal_index_reg));
            }
        }
    } else if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
        if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                        = (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_irq_pending) 
                            << 7U) | (((IData)(vlSelf->bridge_wr_idle_in) 
                                       << 6U) | ((0x20U 
                                                  & ((~ 
                                                      ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_ack_sync) 
                                                       >> 2U)) 
                                                     << 5U)) 
                                                 | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_err_latched) 
                                                     << 2U) 
                                                    | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_latched) 
                                                        << 1U) 
                                                       | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_latched))))));
                }
            } else {
                vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                    = ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? vlSelf->tb_axi_periph__DOT__dut__DOT__ds_resp_addr_reg
                        : vlSelf->tb_axi_periph__DOT__dut__DOT__ds_param_addr_reg);
            }
        } else {
            vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                = ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                    ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? vlSelf->tb_axi_periph__DOT__dut__DOT__ds_length_reg
                        : vlSelf->tb_axi_periph__DOT__dut__DOT__ds_bridge_addr_reg)
                    : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_offset_reg
                        : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_id_reg)));
        }
    } else {
        vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
            = ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                ? ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                    ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? vlSelf->tb_axi_periph__DOT__dut__DOT__bridge_dbg_wcnt_s
                        : (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx) 
                            << 1U) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_pending)))
                    : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx)
                        : ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__term_fb_active)
                            ? 0x180000U : vlSelf->tb_axi_periph__DOT__dut__DOT__fb_app_addr)))
                : ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                    ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_keep_app) 
                            << 1U) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__term_fb_active))
                        : (IData)((vlSelf->tb_axi_periph__DOT__dut__DOT__cycle_counter 
                                   >> 0x20U))) : ((4U 
                                                   & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                   ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cycle_counter)
                                                   : 
                                                  (1U 
                                                   | (2U 
                                                      & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_allcomplete_sync) 
                                                         >> 1U))))));
    }
    vlSelf->dbg_bram_hold = (1U & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat)));
    vlSelf->dbg_can_push_beat = vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat;
    vlSelf->tb_axi_periph__DOT__dut__DOT__ram_addr_mux 
        = (0x1fffU & ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state))
                       ? ((IData)(vlSelf->s_axi_arvalid)
                           ? (vlSelf->s_axi_araddr 
                              >> 2U) : ((IData)(vlSelf->s_axi_awvalid)
                                         ? (vlSelf->s_axi_awaddr 
                                            >> 2U) : 0U))
                       : ((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state))
                           ? ((1U & ((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat)) 
                                     | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__beat_is_last)))
                               ? (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 2U) : ((IData)(1U) 
                                            + (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                               >> 2U)))
                           : (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 2U))));
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_pop 
        = ((0U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
           & ((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state)) 
              & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat) 
                 & ((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region)) 
                    & (IData)(((0x154U == (0x1fcU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) 
                               & (0U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count))))))));
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_push 
        = (((0x10U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
            & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_valid)) 
           & ((0x10U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
              | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_pop)));
}

void Vtb_axi_periph___024root___eval_ico(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___eval_ico\n"); );
    // Body
    if ((1ULL & vlSelf->__VicoTriggered.word(0U))) {
        Vtb_axi_periph___024root___ico_sequent__TOP__0(vlSelf);
    }
}

void Vtb_axi_periph___024root___eval_triggers__ico(Vtb_axi_periph___024root* vlSelf);

bool Vtb_axi_periph___024root___eval_phase__ico(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___eval_phase__ico\n"); );
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vtb_axi_periph___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelf->__VicoTriggered.any();
    if (__VicoExecute) {
        Vtb_axi_periph___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vtb_axi_periph___024root___eval_act(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___eval_act\n"); );
}

VL_INLINE_OPT void Vtb_axi_periph___024root___nba_sequent__TOP__0(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___nba_sequent__TOP__0\n"); );
    // Init
    SData/*15:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__0__Vfuncout;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__0__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__0__stride;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__0__stride = 0;
    SData/*9:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__1__Vfuncout;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__1__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__1__w;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__1__w = 0;
    SData/*9:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__2__Vfuncout;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__2__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__2__h;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__2__h = 0;
    SData/*15:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__3__Vfuncout;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__3__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__3__stride;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__3__stride = 0;
    SData/*9:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__4__Vfuncout;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__4__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__4__w;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__4__w = 0;
    SData/*9:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__5__Vfuncout;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__5__Vfuncout = 0;
    SData/*15:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__5__h;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__5__h = 0;
    SData/*9:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_v_total__6__Vfuncout;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_v_total__6__Vfuncout = 0;
    SData/*9:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_v_total__6__vt;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_v_total__6__vt = 0;
    SData/*8:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__psx_stb_cnt;
    __Vdly__tb_axi_periph__DOT__dut__DOT__psx_stb_cnt = 0;
    SData/*15:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__psx_pressed_acc;
    __Vdly__tb_axi_periph__DOT__dut__DOT__psx_pressed_acc = 0;
    SData/*15:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__psx_released_acc;
    __Vdly__tb_axi_periph__DOT__dut__DOT__psx_released_acc = 0;
    CData/*2:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__early_vblank_sync;
    __Vdly__tb_axi_periph__DOT__dut__DOT__early_vblank_sync = 0;
    CData/*2:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__os_inmenu_sync;
    __Vdly__tb_axi_periph__DOT__dut__DOT__os_inmenu_sync = 0;
    CData/*1:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__input_scan_slot;
    __Vdly__tb_axi_periph__DOT__dut__DOT__input_scan_slot = 0;
    SData/*15:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__input_seq;
    __Vdly__tb_axi_periph__DOT__dut__DOT__input_seq = 0;
    CData/*0:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_read;
    __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_read = 0;
    CData/*0:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_write;
    __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_write = 0;
    CData/*0:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_openfile;
    __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_openfile = 0;
    CData/*0:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_getfile;
    __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_getfile = 0;
    CData/*0:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_flush;
    __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_flush = 0;
    CData/*0:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__ds_ack_seen_low;
    __Vdly__tb_axi_periph__DOT__dut__DOT__ds_ack_seen_low = 0;
    CData/*0:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__ds_ack_latched;
    __Vdly__tb_axi_periph__DOT__dut__DOT__ds_ack_latched = 0;
    CData/*0:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__ds_done_seen_low;
    __Vdly__tb_axi_periph__DOT__dut__DOT__ds_done_seen_low = 0;
    IData/*31:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__timer_counter;
    __Vdly__tb_axi_periph__DOT__dut__DOT__timer_counter = 0;
    CData/*0:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_pending;
    __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_pending = 0;
    CData/*0:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame;
    __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame = 0;
    CData/*2:0*/ __Vdly__dbg_analogizer_wr_toggle;
    __Vdly__dbg_analogizer_wr_toggle = 0;
    CData/*0:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK = 0;
    SData/*10:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer = 0;
    CData/*3:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r = 0;
    CData/*7:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps = 0;
    CData/*7:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__rxd__DOT__sp;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__rxd__DOT__sp = 0;
    CData/*3:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__wr_ptr;
    __Vdly__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__wr_ptr = 0;
    CData/*3:0*/ __Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem__v0;
    __Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem__v0 = 0;
    QData/*63:0*/ __Vdlyvval__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem__v0;
    __Vdlyvval__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem__v0 = 0;
    CData/*0:0*/ __Vdlyvset__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem__v0;
    __Vdlyvset__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem__v0 = 0;
    CData/*3:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr;
    __Vdly__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr = 0;
    CData/*4:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__input_fifo_count;
    __Vdly__tb_axi_periph__DOT__dut__DOT__input_fifo_count = 0;
    // Body
    __Vdly__tb_axi_periph__DOT__dut__DOT__early_vblank_sync 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__early_vblank_sync;
    __Vdly__tb_axi_periph__DOT__dut__DOT__os_inmenu_sync 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__os_inmenu_sync;
    __Vdly__tb_axi_periph__DOT__dut__DOT__input_scan_slot 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot;
    vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v0 = 0U;
    vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v1 = 0U;
    vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v2 = 0U;
    vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v3 = 0U;
    __Vdly__dbg_analogizer_wr_toggle = vlSelf->dbg_analogizer_wr_toggle;
    __Vdly__tb_axi_periph__DOT__dut__DOT__input_seq 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__input_seq;
    __Vdly__tb_axi_periph__DOT__dut__DOT__psx_stb_cnt 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__psx_stb_cnt;
    __Vdly__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr;
    __Vdly__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__wr_ptr 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__wr_ptr;
    __Vdlyvset__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem__v0 = 0U;
    __Vdly__tb_axi_periph__DOT__dut__DOT__input_fifo_count 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count;
    __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame;
    __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_pending 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_pending;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__rxd__DOT__sp 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__rxd__DOT__sp;
    __Vdly__tb_axi_periph__DOT__dut__DOT__timer_counter 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__timer_counter;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps;
    __Vdly__tb_axi_periph__DOT__dut__DOT__ds_done_seen_low 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_seen_low;
    __Vdly__tb_axi_periph__DOT__dut__DOT__ds_ack_seen_low 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_seen_low;
    __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_flush 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_flush;
    __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_getfile 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_getfile;
    __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_openfile 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_openfile;
    __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_write 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_write;
    __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_read 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_read;
    __Vdly__tb_axi_periph__DOT__dut__DOT__ds_ack_latched 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_latched;
    __Vdly__tb_axi_periph__DOT__dut__DOT__psx_released_acc 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__psx_released_acc;
    __Vdly__tb_axi_periph__DOT__dut__DOT__psx_pressed_acc 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__psx_pressed_acc;
    __Vdly__tb_axi_periph__DOT__dut__DOT__early_vblank_sync 
        = ((6U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__early_vblank_sync) 
                  << 1U)) | (IData)(vlSelf->early_vblank_in));
    __Vdly__tb_axi_periph__DOT__dut__DOT__os_inmenu_sync 
        = ((6U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__os_inmenu_sync) 
                  << 1U)) | (IData)(vlSelf->os_inmenu_in));
    vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_sync 
        = ((6U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_sync) 
                  << 1U)) | (IData)(vlSelf->vsync_in));
    vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_allcomplete_sync 
        = (1U | (6U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_allcomplete_sync) 
                       << 1U)));
    if (((6U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state)) 
         & (0U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_wstrb)))) {
        if ((1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_wstrb))) {
            vlSelf->__Vdlyvval__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v0 
                = (0xffU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
            vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v0 = 1U;
            vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v0 = 0U;
            vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v0 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__ram_addr_mux;
        }
        if ((2U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_wstrb))) {
            vlSelf->__Vdlyvval__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v1 
                = (0xffU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                            >> 8U));
            vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v1 = 1U;
            vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v1 = 8U;
            vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v1 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__ram_addr_mux;
        }
        if ((4U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_wstrb))) {
            vlSelf->__Vdlyvval__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v2 
                = (0xffU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                            >> 0x10U));
            vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v2 = 1U;
            vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v2 = 0x10U;
            vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v2 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__ram_addr_mux;
        }
        if ((8U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_wstrb))) {
            vlSelf->__Vdlyvval__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v3 
                = (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                   >> 0x18U);
            vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v3 = 1U;
            vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v3 = 0x18U;
            vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v3 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__ram_addr_mux;
        }
    }
    if ((1U & ((~ (IData)(vlSelf->reset_n)) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_clear)))) {
        __Vdly__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__wr_ptr = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__input_fifo_count = 0U;
    } else {
        if (vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_pop) {
            __Vdly__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr 
                = (0xfU & ((IData)(1U) + (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr)));
        }
        if (vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_push) {
            __Vdlyvval__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem__v0 
                = (0x100000000000000ULL | (((QData)((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot)) 
                                            << 0x30U) 
                                           | (((QData)((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fields)) 
                                               << 0x28U) 
                                              | (QData)((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_seq)))));
            __Vdlyvset__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem__v0 = 1U;
            __Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem__v0 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__wr_ptr;
            __Vdly__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__wr_ptr 
                = (0xfU & ((IData)(1U) + (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__wr_ptr)));
        }
        __Vdly__tb_axi_periph__DOT__dut__DOT__input_fifo_count 
            = (0x1fU & ((2U == (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_push) 
                                 << 1U) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_pop)))
                         ? ((IData)(1U) + (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count))
                         : ((1U == (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_push) 
                                     << 1U) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_pop)))
                             ? ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count) 
                                - (IData)(1U)) : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count))));
    }
    if ((1U & ((~ (IData)(vlSelf->reset_n)) | (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg))))) {
        __Vdly__tb_axi_periph__DOT__dut__DOT__psx_pressed_acc = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__psx_released_acc = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__psx_prev_key1 = 0U;
    } else {
        if (vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_clear_edges_pulse) {
            __Vdly__tb_axi_periph__DOT__dut__DOT__psx_pressed_acc = 0U;
            __Vdly__tb_axi_periph__DOT__dut__DOT__psx_released_acc = 0U;
        } else {
            __Vdly__tb_axi_periph__DOT__dut__DOT__psx_pressed_acc 
                = ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_pressed_acc) 
                   | ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key1) 
                      & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_prev_key1))));
            __Vdly__tb_axi_periph__DOT__dut__DOT__psx_released_acc 
                = ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_released_acc) 
                   | ((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key1)) 
                      & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_prev_key1)));
        }
        vlSelf->tb_axi_periph__DOT__dut__DOT__psx_prev_key1 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key1;
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__rtc_valid_s 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__rtc_valid_meta;
    vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_a_reg2 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_a_reg;
    vlSelf->tb_axi_periph__DOT__dut__DOT__bridge_dbg_wcnt_s 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__bridge_dbg_wcnt_meta;
    if ((1U & (~ (IData)(vlSelf->reset_n)))) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_wr_ptr = 0U;
    }
    if (vlSelf->reset_n) {
        __Vdly__tb_axi_periph__DOT__dut__DOT__input_scan_slot 
            = (3U & ((IData)(1U) + (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot)));
        vlSelf->tb_axi_periph__DOT__dut__DOT__cycle_counter 
            = (1ULL + vlSelf->tb_axi_periph__DOT__dut__DOT__cycle_counter);
        if (((IData)(((((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state)) 
                        & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat)) 
                       & (6U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))) 
                      & (8U == (0xcU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)))) 
             & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_empty)))) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr 
                = (0x3ffU & ((IData)(1U) + (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr)));
        }
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                   >> 8U)))) {
            if ((0x80U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 6U)))) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 5U)))) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    __Vdly__dbg_analogizer_wr_toggle 
                                        = ((3U & (IData)(__Vdly__dbg_analogizer_wr_toggle)) 
                                           | (4U & 
                                              ((~ ((IData)(vlSelf->dbg_analogizer_wr_toggle) 
                                                   >> 2U)) 
                                               << 2U)));
                                }
                            } else {
                                __Vdly__dbg_analogizer_wr_toggle 
                                    = ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                        ? ((5U & (IData)(__Vdly__dbg_analogizer_wr_toggle)) 
                                           | (2U & 
                                              ((~ ((IData)(vlSelf->dbg_analogizer_wr_toggle) 
                                                   >> 1U)) 
                                               << 1U)))
                                        : ((6U & (IData)(__Vdly__dbg_analogizer_wr_toggle)) 
                                           | (1U & 
                                              (~ (IData)(vlSelf->dbg_analogizer_wr_toggle)))));
                            }
                        }
                        if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__dt_query_toggle 
                                        = (1U & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__dt_query_toggle)));
                                }
                            }
                        }
                    }
                }
            }
            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                          >> 7U)))) {
                if ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 5U)))) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->dbg_save_dt_commit 
                                        = (1U & (~ (IData)(vlSelf->dbg_save_dt_commit)));
                                }
                            }
                        }
                    }
                }
            }
        }
        vlSelf->dbg_analogizer_wr_toggle = __Vdly__dbg_analogizer_wr_toggle;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__wr_ptr 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__wr_ptr;
        if ((1U & (~ ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
                      & (0x108U == (0x1fcU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)))))) {
            if (vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_valid) {
                if ((0x10U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count))) {
                    __Vdly__tb_axi_periph__DOT__dut__DOT__input_seq 
                        = (0xffffU & ((IData)(1U) + (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_seq)));
                }
            }
        }
    } else {
        __Vdly__tb_axi_periph__DOT__dut__DOT__input_scan_slot = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__cycle_counter = 0ULL;
        vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr = 0U;
        __Vdly__dbg_analogizer_wr_toggle = 0U;
        vlSelf->dbg_analogizer_wr_toggle = __Vdly__dbg_analogizer_wr_toggle;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__wr_ptr 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__wr_ptr;
        vlSelf->dbg_save_dt_commit = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__dt_query_toggle = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__input_seq = 0U;
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_seq 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__input_seq;
    if (__Vdlyvset__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem__v0) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem__v0] 
            = __Vdlyvval__tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem__v0;
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__psx_pressed_acc 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__psx_pressed_acc;
    vlSelf->tb_axi_periph__DOT__dut__DOT__psx_released_acc 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__psx_released_acc;
    if (vlSelf->reset_n) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_dv = 0U;
        if (vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_fifo_we) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_dv = 1U;
            vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_byte 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_fifo_din;
        }
        vlSelf->tb_axi_periph__DOT__dut__DOT__pal_wr = 0U;
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                   >> 8U)))) {
            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                          >> 7U)))) {
                if ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 5U)))) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__pal_wr = 1U;
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__pal_data 
                                        = (0xffffffU 
                                           & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                }
                            }
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->dbg_save_dt_size 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                                }
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->dbg_save_dt_slot 
                                        = (0xfU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                }
                            }
                        }
                    }
                }
            }
            if ((0x80U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 6U)))) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 5U)))) {
                        if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__dt_query_addr 
                                        = (0x3ffU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                }
                            }
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_irq_pending = 0U;
                                }
                            }
                        }
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->dbg_analogizer_wr_voffset 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                                }
                            }
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->dbg_analogizer_wr_hoffset 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                                }
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->dbg_analogizer_wr_settings 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                                }
                            }
                        }
                    }
                    if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__shutdown_ack 
                                        = (1U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                }
                            }
                        }
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_gpio_out_reg 
                                        = (0xffU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                }
                            }
                        }
                    }
                }
                if ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__analog_color_mode_reg 
                                        = (7U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                }
                            }
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__1__w 
                                        = (0xffffU 
                                           & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__1__Vfuncout 
                                        = ((1U > (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__1__w))
                                            ? 1U : 
                                           ((0x320U 
                                             < (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__1__w))
                                             ? 0x320U
                                             : (0x3ffU 
                                                & (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__1__w))));
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_width_reg 
                                        = __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__1__Vfuncout;
                                    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__2__h 
                                        = (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                                           >> 0x10U);
                                    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__2__Vfuncout 
                                        = ((1U > (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__2__h))
                                            ? 1U : 
                                           ((0x258U 
                                             < (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__2__h))
                                             ? 0x258U
                                             : (0x3ffU 
                                                & (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__2__h))));
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_height_reg 
                                        = __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__2__Vfuncout;
                                }
                            }
                        }
                    }
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 5U)))) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__save_dt_word_mode = 1U;
                                }
                            }
                        }
                    }
                }
            } else if ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 5U)))) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 4U)))) {
                        if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 2U)))) {
                                vlSelf->tb_axi_periph__DOT__dut__DOT__save_dt_word_mode = 0U;
                            }
                        }
                    }
                }
            }
        }
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_rising) 
             & (~ ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__os_inmenu_sync) 
                   >> 2U)))) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_irq_pending = 1U;
            __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame = 0U;
        }
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                   >> 8U)))) {
            if ((0x80U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 5U)))) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__save_dt_word 
                                        = (0x3ffU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                }
                            }
                        }
                    }
                    if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__video_scaler_slot_reg 
                                        = (7U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                }
                            }
                        }
                    }
                }
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 6U)))) {
                    if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_en_reg 
                                        = (1U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                                                 >> 7U));
                                }
                            }
                        }
                    }
                }
            }
        }
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                >> 8U))) {
            if ((0x18U == (0x3fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                    >> 2U)))) {
                if ((1U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)) {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_en_reg = 0U;
                }
            }
        }
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                   >> 8U)))) {
            if ((0x80U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 5U)))) {
                        if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_v_total__6__vt 
                                        = (0x3ffU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_v_total__6__Vfuncout 
                                        = ((0x202U 
                                            > (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_v_total__6__vt))
                                            ? 0x202U
                                            : ((0x2eeU 
                                                < (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_v_total__6__vt))
                                                ? 0x2eeU
                                                : (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_v_total__6__vt)));
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__vrr_v_total_reg 
                                        = __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_v_total__6__Vfuncout;
                                }
                            }
                        }
                    }
                    if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__irq_mask 
                                        = (0x3fU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                }
                            }
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__0__stride 
                                        = (0xffffU 
                                           & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__Vstatic__even_stride 
                                        = (0xfffeU 
                                           & (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__0__stride));
                                    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__0__Vfuncout 
                                        = ((2U > (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__Vstatic__even_stride))
                                            ? 2U : 
                                           ((0x800U 
                                             < (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__Vstatic__even_stride))
                                             ? 0x800U
                                             : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__Vstatic__even_stride)));
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_stride_reg 
                                        = __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__0__Vfuncout;
                                }
                            }
                        } else if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 2U)))) {
                                __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__3__stride 
                                    = (0xffffU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                vlSelf->tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__Vstatic__even_stride 
                                    = (0xfffeU & (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__3__stride));
                                __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__3__Vfuncout 
                                    = ((2U > (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__Vstatic__even_stride))
                                        ? 2U : ((0x800U 
                                                 < (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__Vstatic__even_stride))
                                                 ? 0x800U
                                                 : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__Vstatic__even_stride)));
                                vlSelf->tb_axi_periph__DOT__dut__DOT__fb_stride_reg 
                                    = __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__3__Vfuncout;
                            }
                        }
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__5__h 
                                        = (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                                           >> 0x10U);
                                    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__5__Vfuncout 
                                        = ((1U > (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__5__h))
                                            ? 1U : 
                                           ((0x258U 
                                             < (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__5__h))
                                             ? 0x258U
                                             : (0x3ffU 
                                                & (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__5__h))));
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__fb_height_reg 
                                        = __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_height__5__Vfuncout;
                                    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__4__w 
                                        = (0xffffU 
                                           & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                    __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__4__Vfuncout 
                                        = ((1U > (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__4__w))
                                            ? 1U : 
                                           ((0x320U 
                                             < (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__4__w))
                                             ? 0x320U
                                             : (0x3ffU 
                                                & (IData)(__Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__4__w))));
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__fb_width_reg 
                                        = __Vfunc_tb_axi_periph__DOT__dut__DOT__clamp_fb_width__4__Vfuncout;
                                }
                            }
                        }
                    }
                }
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 6U)))) {
                    if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_gpio_dir_reg 
                                        = (0xffU & 
                                           (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                                            >> 8U));
                                }
                            }
                        }
                    }
                }
            }
            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                          >> 7U)))) {
                if ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__color_mode_reg 
                                        = (7U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                }
                            }
                        }
                    }
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 5U)))) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__pal_addr 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__pal_index_reg;
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__pal_index_reg 
                                        = (0xffU & 
                                           ((IData)(1U) 
                                            + (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__pal_index_reg)));
                                } else {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__pal_index_reg 
                                        = (0xffU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                }
                            }
                        }
                    }
                }
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 6U)))) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 5U)))) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__term_fb_active 
                                        = (1U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__analog_keep_app 
                                        = (1U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                                                 >> 1U));
                                }
                            }
                        }
                        if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    if ((1U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)) {
                                        __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_pending = 1U;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if ((((~ ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__os_inmenu_sync) 
                  >> 2U)) & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_pending)) 
             & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_rising))) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__fb_ready_idx;
            __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_pending = 0U;
            __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame = 1U;
        } else if (((((~ ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__os_inmenu_sync) 
                          >> 2U)) & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_pending)) 
                     & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__early_vblank_sync) 
                        >> 2U)) & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame)))) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__fb_ready_idx;
            __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_pending = 0U;
            __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame = 1U;
        }
        if (vlSelf->gpu_swap_req_in) {
            __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_pending = 1U;
        }
        vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame;
        vlSelf->tb_axi_periph__DOT__dut__DOT__early_vblank_sync 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__early_vblank_sync;
        vlSelf->tb_axi_periph__DOT__dut__DOT__os_inmenu_sync 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__os_inmenu_sync;
        vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_pending 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_pending;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXSET = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__RXWT = 0U;
        if (vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__R_CE) {
            __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer 
                = (0x7ffU & ((IData)(1U) + (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer)));
            if ((0U == (0x3ffU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer)))) {
                __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r = 0U;
            } else if ((0x1fU == (0x1fU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer)))) {
                __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r 
                    = (0xfU & (((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__device_id_type)) 
                                & (5U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r)))
                                ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r)
                                : (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__device_id_type) 
                                    & (9U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r)))
                                    ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r)
                                    : ((IData)(1U) 
                                       + (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r)))));
            }
            if ((6U == (0x1fU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer)))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXSET = 1U;
            }
            if ((6U != (0x1fU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer)))) {
                if ((9U != (0x1fU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer)))) {
                    if ((0xcU != (0x1fU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer)))) {
                        if ((0x14U == (0x1fU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer)))) {
                            vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__RXWT = 1U;
                        }
                    }
                }
            }
        }
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_enable) 
             & (0U != vlSelf->tb_axi_periph__DOT__dut__DOT__timer_period))) {
            if ((0U == vlSelf->tb_axi_periph__DOT__dut__DOT__timer_counter)) {
                __Vdly__tb_axi_periph__DOT__dut__DOT__timer_counter 
                    = (vlSelf->tb_axi_periph__DOT__dut__DOT__timer_period 
                       - (IData)(1U));
                vlSelf->tb_axi_periph__DOT__dut__DOT__timer_irq_pending = 1U;
            } else {
                __Vdly__tb_axi_periph__DOT__dut__DOT__timer_counter 
                    = (vlSelf->tb_axi_periph__DOT__dut__DOT__timer_counter 
                       - (IData)(1U));
            }
        }
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                   >> 8U)))) {
            if ((0x80U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 6U)))) {
                    if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    if ((2U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)) {
                                        vlSelf->tb_axi_periph__DOT__dut__DOT__timer_irq_pending = 0U;
                                    }
                                }
                            } else if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                __Vdly__tb_axi_periph__DOT__dut__DOT__timer_counter 
                                    = (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                                       - (IData)(1U));
                            }
                        }
                    }
                }
            }
        }
        vlSelf->tb_axi_periph__DOT__dut__DOT__timer_counter 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__timer_counter;
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (0x104U == (0x1fcU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)))) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key0 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key1 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key2 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key3 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy0 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy1 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy2 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy3 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig0 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig1 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig2 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig3 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_s;
        }
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (0x108U == (0x1fcU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)))) {
            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__input_overflow = 0U;
            }
            if ((1U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key0 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_s;
                vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key1 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_s;
                vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key2 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_s;
                vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key3 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_s;
                vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy0 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_s;
                vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy1 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_s;
                vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy2 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_s;
                vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy3 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_s;
                vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig0 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_s;
                vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig1 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_s;
                vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig2 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_s;
                vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig3 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_s;
            }
        } else if (vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_valid) {
            if ((0x10U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__input_overflow = 1U;
            }
            if ((0x10U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count))) {
                if ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot))) {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key0 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_s;
                    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy0 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_s;
                    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig0 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_s;
                } else if ((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot))) {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key1 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_s;
                    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy1 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_s;
                    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig1 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_s;
                } else if ((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot))) {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key2 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_s;
                    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy2 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_s;
                    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig2 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_s;
                } else {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key3 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_s;
                    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy3 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_s;
                    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig3 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_s;
                }
            }
        }
        if ((1U & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask)))) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key0 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy0 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig0 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_s;
        }
        if ((1U & (~ ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask) 
                      >> 1U)))) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key1 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy1 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig1 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_s;
        }
        if ((1U & (~ ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask) 
                      >> 2U)))) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key2 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy2 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig2 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_s;
        }
        if ((1U & (~ ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask) 
                      >> 3U)))) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key3 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy3 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_s;
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig3 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_s;
        }
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__input_scan_slot;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__input_fifo_count;
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT____VdfgTmp_h6d2737cc__0) 
             & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXWT))) {
            __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps 
                = ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))
                    ? 1U : ((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))
                             ? 0x42U : 0U));
        }
        if (vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__F_CE) {
            if (((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psCLK_gate)) 
                 & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT____VdfgTmp_h6d2737cc__0))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__psx_cmd_o 
                    = (1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps));
                __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps 
                    = (0x80U | (0x7fU & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps) 
                                         >> 1U)));
            } else {
                __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps;
                vlSelf->tb_axi_periph__DOT__dut__DOT__psx_cmd_o = 1U;
            }
        }
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps;
        if ((4U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_ack_sync))) {
            __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_read = 0U;
            __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_write = 0U;
            __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_openfile = 0U;
            __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_getfile = 0U;
            __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_flush = 0U;
        }
        if (vlSelf->tb_axi_periph__DOT__dut__DOT__ds_cmd_active) {
            if ((1U & ((~ ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_ack_sync) 
                           >> 2U)) & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_latched))))) {
                __Vdly__tb_axi_periph__DOT__dut__DOT__ds_ack_seen_low = 1U;
            }
            if ((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_seen_low) 
                  & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_ack_sync) 
                     >> 2U)) & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_latched)))) {
                __Vdly__tb_axi_periph__DOT__dut__DOT__ds_ack_latched = 1U;
            }
            if ((1U & ((~ ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_done_sync) 
                           >> 2U)) & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_latched))))) {
                __Vdly__tb_axi_periph__DOT__dut__DOT__ds_done_seen_low = 1U;
            }
            if (((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_seen_low) 
                   & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_done_sync) 
                      >> 2U)) & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_latched)) 
                 & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_latched)))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_latched = 1U;
                vlSelf->tb_axi_periph__DOT__dut__DOT__ds_err_latched 
                    = ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync
                        [2U]) | ((2U & (vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync
                                        [1U] >> 1U)) 
                                 | (1U & (vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync
                                          [0U] >> 2U))));
                vlSelf->tb_axi_periph__DOT__dut__DOT__ds_cmd_active = 0U;
                vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_irq_pending = 1U;
                __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_read = 0U;
                __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_write = 0U;
                __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_openfile = 0U;
                __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_getfile = 0U;
                __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_flush = 0U;
            }
        }
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                   >> 8U)))) {
            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                          >> 7U)))) {
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 6U)))) {
                    if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    if ((0x80U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)) {
                                        vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_irq_pending = 0U;
                                    }
                                } else if ((1U & (~ 
                                                  ((((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_read) 
                                                       | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_write)) 
                                                      | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_openfile)) 
                                                     | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_getfile)) 
                                                    | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_flush)) 
                                                   | ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_ack_sync) 
                                                      >> 2U))))) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_id 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_id_reg;
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_slotoffset 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_offset_reg;
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_bridgeaddr 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__ds_bridge_addr_reg;
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_length 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__ds_length_reg;
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__target_buffer_param_struct 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__ds_param_addr_reg;
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__target_buffer_resp_struct 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__ds_resp_addr_reg;
                                    __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_read = 0U;
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_cmd_active = 1U;
                                    __Vdly__tb_axi_periph__DOT__dut__DOT__ds_ack_latched = 0U;
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_latched = 0U;
                                    __Vdly__tb_axi_periph__DOT__dut__DOT__ds_ack_seen_low = 0U;
                                    __Vdly__tb_axi_periph__DOT__dut__DOT__ds_done_seen_low = 0U;
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_err_latched = 0U;
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_irq_pending = 0U;
                                    __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_write = 0U;
                                    __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_openfile = 0U;
                                    __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_getfile = 0U;
                                    __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_flush = 0U;
                                    if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)) {
                                        if ((1U & (~ 
                                                   (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                                                    >> 1U)))) {
                                            if ((1U 
                                                 & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)) {
                                                __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_flush = 1U;
                                            } else {
                                                __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_getfile = 1U;
                                            }
                                        }
                                    } else if ((2U 
                                                & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)) {
                                        if ((1U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)) {
                                            __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_openfile = 1U;
                                        } else {
                                            __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_write = 1U;
                                        }
                                    } else if ((1U 
                                                & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)) {
                                        __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_read = 1U;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_dv = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__pal_wr = 0U;
        vlSelf->dbg_save_dt_size = 0U;
        vlSelf->dbg_save_dt_slot = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__dt_query_addr = 0U;
        vlSelf->dbg_analogizer_wr_voffset = 0U;
        vlSelf->dbg_analogizer_wr_hoffset = 0U;
        vlSelf->dbg_analogizer_wr_settings = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__shutdown_ack = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__pal_data = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__analog_color_mode_reg = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_width_reg = 0x140U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_height_reg = 0xf0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__save_dt_word_mode = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_gpio_out_reg = 0xffU;
        vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_irq_pending = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__save_dt_word = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__video_scaler_slot_reg = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_en_reg = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__vrr_v_total_reg = 0x20dU;
        vlSelf->tb_axi_periph__DOT__dut__DOT__color_mode_reg = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__pal_index_reg = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__pal_addr = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__irq_mask = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_gpio_dir_reg = 3U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__fb_height_reg = 0xf0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__fb_width_reg = 0x140U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__fb_stride_reg = 0x140U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_stride_reg = 0x140U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__term_fb_active = 1U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__analog_keep_app = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_overflow = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_byte = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_pending = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame;
        vlSelf->tb_axi_periph__DOT__dut__DOT__early_vblank_sync 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__early_vblank_sync;
        vlSelf->tb_axi_periph__DOT__dut__DOT__os_inmenu_sync 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__os_inmenu_sync;
        vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_pending 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__fb_swap_pending;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXSET = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__RXWT = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__timer_counter = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__timer_irq_pending = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__timer_counter 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__timer_counter;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__input_scan_slot;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__input_fifo_count;
        __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__psx_cmd_o = 1U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps 
            = __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps;
        __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_read = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_write = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_openfile = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_getfile = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_flush = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_id = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_slotoffset = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_bridgeaddr = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_length = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_buffer_param_struct = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_buffer_resp_struct = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__ds_cmd_active = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__ds_ack_latched = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_latched = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__ds_ack_seen_low = 1U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__ds_done_seen_low = 1U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__ds_err_latched = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_irq_pending = 0U;
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_read 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_read;
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_write 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_write;
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_openfile 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_openfile;
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_getfile 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_getfile;
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_flush 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__target_dataslot_flush;
    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_seen_low 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__ds_ack_seen_low;
    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_seen_low 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__ds_done_seen_low;
    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_latched 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__ds_ack_latched;
    vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_rising 
        = (IData)((2U == (6U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_sync))));
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_dout 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem
        [vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr];
    vlSelf->tb_axi_periph__DOT__dut__DOT__rtc_valid_meta = 0U;
    vlSelf->tb_axi_periph__DOT__dut__DOT__bridge_dbg_wcnt_meta = 0U;
    vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_empty 
        = ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr) 
           == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_wr_ptr));
    vlSelf->tb_axi_periph__DOT__dut__DOT__analog_show_term 
        = ((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_keep_app)) 
           & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__term_fb_active));
    vlSelf->tb_axi_periph__DOT__dut__DOT__fb_app_addr 
        = ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx))
            ? 0U : ((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx))
                     ? 0x80000U : 0x100000U));
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_pending 
        = ((0U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
           | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_overflow));
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_s 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_meta;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_s 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_meta;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_s 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_meta;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_s 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_meta;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_s 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_meta;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_s 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_meta;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_s 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_meta;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_s 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_meta;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_s 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_meta;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_s 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_meta;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_s 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_meta;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_s 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_meta;
    if (vlSelf->tb_axi_periph__DOT__dut__DOT__analog_show_term) {
        vlSelf->dbg_analog_color_mode = vlSelf->tb_axi_periph__DOT__dut__DOT__color_mode_reg;
        vlSelf->dbg_analog_fb_width = vlSelf->tb_axi_periph__DOT__dut__DOT__fb_width_reg;
        vlSelf->dbg_analog_fb_height = vlSelf->tb_axi_periph__DOT__dut__DOT__fb_height_reg;
        vlSelf->dbg_analog_fb_stride = vlSelf->tb_axi_periph__DOT__dut__DOT__fb_stride_reg;
        vlSelf->dbg_analog_fb_addr = 0x180000U;
    } else {
        vlSelf->dbg_analog_color_mode = vlSelf->tb_axi_periph__DOT__dut__DOT__analog_color_mode_reg;
        vlSelf->dbg_analog_fb_width = vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_width_reg;
        vlSelf->dbg_analog_fb_height = vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_height_reg;
        vlSelf->dbg_analog_fb_stride = vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_stride_reg;
        vlSelf->dbg_analog_fb_addr = vlSelf->tb_axi_periph__DOT__dut__DOT__fb_app_addr;
    }
    vlSelf->ext_irq_out = (1U & (((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_empty)) 
                                  & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__irq_mask)) 
                                 | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_irq_pending) 
                                     & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__irq_mask) 
                                        >> 3U)) | (
                                                   ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_pending) 
                                                    & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__irq_mask) 
                                                       >> 4U)) 
                                                   | ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_irq_pending) 
                                                      & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__irq_mask) 
                                                         >> 5U))))));
    if (((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__att1r)) 
         & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1))) {
        if (vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__no_gamepad) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key1 = 0U;
            vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 = 0x80808080U;
        } else {
            vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key1 
                = ((0x8000U & ((~ ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1) 
                                   >> 3U)) << 0xfU)) 
                   | ((0x4000U & ((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1)) 
                                  << 0xeU)) | ((0x2000U 
                                                & ((~ 
                                                    ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1) 
                                                     >> 2U)) 
                                                   << 0xdU)) 
                                               | ((0x1000U 
                                                   & ((~ 
                                                       ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1) 
                                                        >> 1U)) 
                                                      << 0xcU)) 
                                                  | ((0x800U 
                                                      & ((~ 
                                                          ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2) 
                                                           >> 1U)) 
                                                         << 0xbU)) 
                                                     | ((0x400U 
                                                         & ((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2)) 
                                                            << 0xaU)) 
                                                        | ((0x200U 
                                                            & ((~ 
                                                                ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2) 
                                                                 >> 3U)) 
                                                               << 9U)) 
                                                           | ((0x100U 
                                                               & ((~ 
                                                                   ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2) 
                                                                    >> 2U)) 
                                                                  << 8U)) 
                                                              | ((0x80U 
                                                                  & ((~ 
                                                                      ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2) 
                                                                       >> 7U)) 
                                                                     << 7U)) 
                                                                 | ((0x40U 
                                                                     & ((~ 
                                                                         ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2) 
                                                                          >> 4U)) 
                                                                        << 6U)) 
                                                                    | ((0x20U 
                                                                        & ((~ 
                                                                            ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2) 
                                                                             >> 6U)) 
                                                                           << 5U)) 
                                                                       | ((0x10U 
                                                                           & ((~ 
                                                                               ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2) 
                                                                                >> 5U)) 
                                                                              << 4U)) 
                                                                          | ((8U 
                                                                              & ((~ 
                                                                                ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1) 
                                                                                >> 5U)) 
                                                                                << 3U)) 
                                                                             | ((4U 
                                                                                & ((~ 
                                                                                ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1) 
                                                                                >> 7U)) 
                                                                                << 2U)) 
                                                                                | ((2U 
                                                                                & ((~ 
                                                                                ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1) 
                                                                                >> 6U)) 
                                                                                << 1U)) 
                                                                                | (1U 
                                                                                & (~ 
                                                                                ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1) 
                                                                                >> 4U))))))))))))))))));
            vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                = (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx4) 
                    << 0x18U) | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx3) 
                                  << 0x10U) | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx6) 
                                                << 8U) 
                                               | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx5))));
        }
    } else if (((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__att2r)) 
                & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2))) {
        if (vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__no_gamepad) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key2 = 0U;
            vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy2 = 0x80808080U;
        } else {
            vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key2 
                = ((0x8000U & ((~ ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1) 
                                   >> 3U)) << 0xfU)) 
                   | ((0x4000U & ((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1)) 
                                  << 0xeU)) | ((0x2000U 
                                                & ((~ 
                                                    ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1) 
                                                     >> 2U)) 
                                                   << 0xdU)) 
                                               | ((0x1000U 
                                                   & ((~ 
                                                       ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1) 
                                                        >> 1U)) 
                                                      << 0xcU)) 
                                                  | ((0x800U 
                                                      & ((~ 
                                                          ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2) 
                                                           >> 1U)) 
                                                         << 0xbU)) 
                                                     | ((0x400U 
                                                         & ((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2)) 
                                                            << 0xaU)) 
                                                        | ((0x200U 
                                                            & ((~ 
                                                                ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2) 
                                                                 >> 3U)) 
                                                               << 9U)) 
                                                           | ((0x100U 
                                                               & ((~ 
                                                                   ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2) 
                                                                    >> 2U)) 
                                                                  << 8U)) 
                                                              | ((0x80U 
                                                                  & ((~ 
                                                                      ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2) 
                                                                       >> 7U)) 
                                                                     << 7U)) 
                                                                 | ((0x40U 
                                                                     & ((~ 
                                                                         ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2) 
                                                                          >> 4U)) 
                                                                        << 6U)) 
                                                                    | ((0x20U 
                                                                        & ((~ 
                                                                            ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2) 
                                                                             >> 6U)) 
                                                                           << 5U)) 
                                                                       | ((0x10U 
                                                                           & ((~ 
                                                                               ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2) 
                                                                                >> 5U)) 
                                                                              << 4U)) 
                                                                          | ((8U 
                                                                              & ((~ 
                                                                                ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1) 
                                                                                >> 5U)) 
                                                                                << 3U)) 
                                                                             | ((4U 
                                                                                & ((~ 
                                                                                ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1) 
                                                                                >> 7U)) 
                                                                                << 2U)) 
                                                                                | ((2U 
                                                                                & ((~ 
                                                                                ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1) 
                                                                                >> 6U)) 
                                                                                << 1U)) 
                                                                                | (1U 
                                                                                & (~ 
                                                                                ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1) 
                                                                                >> 4U))))))))))))))))));
            vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy2 
                = (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx4) 
                    << 0x18U) | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx3) 
                                  << 0x10U) | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx6) 
                                                << 8U) 
                                               | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx5))));
        }
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_meta 
        = vlSelf->cont1_key_in;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_meta 
        = vlSelf->cont2_key_in;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_meta 
        = vlSelf->cont3_key_in;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_meta 
        = vlSelf->cont4_key_in;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_meta 
        = vlSelf->cont1_joy_in;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_meta 
        = vlSelf->cont2_joy_in;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_meta 
        = vlSelf->cont3_joy_in;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_meta 
        = vlSelf->cont4_joy_in;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_meta 
        = vlSelf->cont1_trig_in;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_meta 
        = vlSelf->cont2_trig_in;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_meta 
        = vlSelf->cont3_trig_in;
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_meta 
        = vlSelf->cont4_trig_in;
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fields 
        = ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot))
            ? ((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_s) 
                 != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig0)) 
                << 2U) | (((vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_s 
                            != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy0) 
                           << 1U) | (vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_s 
                                     != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key0)))
            : ((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot))
                ? ((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_s) 
                     != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig1)) 
                    << 2U) | (((vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_s 
                                != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy1) 
                               << 1U) | (vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_s 
                                         != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key1)))
                : ((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot))
                    ? ((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_s) 
                         != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig2)) 
                        << 2U) | (((vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_s 
                                    != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy2) 
                                   << 1U) | (vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_s 
                                             != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key2)))
                    : ((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_s) 
                         != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig3)) 
                        << 2U) | (((vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_s 
                                    != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy3) 
                                   << 1U) | (vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_s 
                                             != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key3))))));
    if (vlSelf->reset_n) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_clear = 0U;
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (0x108U == (0x1fcU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)))) {
            if ((1U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_clear = 1U;
            }
        }
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_clear_edges_pulse = 0U;
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                >> 8U))) {
            if ((0x18U != (0x3fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                    >> 2U)))) {
                if ((0x1fU == (0x3fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                        >> 2U)))) {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_clear_edges_pulse 
                        = (1U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                                 >> 1U));
                }
            }
        }
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                   >> 8U)))) {
            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                          >> 7U)))) {
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 6U)))) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 5U)))) {
                        if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    if ((1U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)) {
                                        vlSelf->tb_axi_periph__DOT__dut__DOT__fb_ready_idx 
                                            = (3U & 
                                               (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                                                >> 1U));
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if ((0x80U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 6U)))) {
                    if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__timer_enable 
                                        = (1U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                }
                            }
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__timer_period 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (vlSelf->gpu_swap_req_in) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__fb_ready_idx 
                = vlSelf->gpu_swap_idx_in;
        }
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (0x104U == (0x1fcU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)))) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask 
                = (0xfU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
        }
    } else {
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_clear = 1U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_clear_edges_pulse = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__fb_ready_idx = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__timer_enable = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__timer_period = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask = 0U;
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_valid 
        = (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask) 
            >> (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot)) 
           & (0U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fields)));
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__att1r 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1;
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__att2r 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2;
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__no_gamepad 
        = ((0xffU == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__RXD_ID)) 
           & (0xffU == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx0)));
    if (vlSelf->reset_n) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXWT = 0U;
        if (vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__R_CE) {
            if ((6U != (0x1fU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer)))) {
                if ((9U == (0x1fU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer)))) {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXWT = 1U;
                }
                if ((9U != (0x1fU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer)))) {
                    if ((0xcU == (0x1fU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer)))) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psCLK_gate = 0U;
                    } else if ((0x14U == (0x1fU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer)))) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psCLK_gate = 1U;
                    }
                }
            }
            if ((0U == (0x3ffU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer)))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1 
                    = (1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer) 
                             >> 0xaU));
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2 
                    = (1U & (~ ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer) 
                                >> 0xaU)));
            } else if (((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__device_id_type)) 
                        & (0x9eU == (0x3ffU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer))))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1 = 1U;
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2 = 1U;
            } else if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__device_id_type) 
                        & (0x11eU == (0x3ffU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer))))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1 = 1U;
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2 = 1U;
            }
        }
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                   >> 8U)))) {
            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                          >> 7U)))) {
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 6U)))) {
                    if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_id_reg 
                                        = (0xffffU 
                                           & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                }
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_offset_reg 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                                }
                            }
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_bridge_addr_reg 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                                }
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_length_reg 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                                }
                            }
                        }
                        if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_param_addr_reg 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                                }
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_resp_addr_reg 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXWT = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psCLK_gate = 1U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_id_reg = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_offset_reg = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__ds_bridge_addr_reg = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__ds_length_reg = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__ds_param_addr_reg = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__ds_resp_addr_reg = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1 = 1U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2 = 1U;
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer;
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT____VdfgTmp_h6d2737cc__0 
        = (1U & ((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1)) 
                 | (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2))));
    if (vlSelf->reset_n) {
        if ((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__device_id_type 
                = ((0x23U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__RXD_ID)) 
                   || ((0x41U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__RXD_ID)) 
                       && ((0x53U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__RXD_ID)) 
                           || ((0x73U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__RXD_ID)) 
                               || ((0xe3U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__RXD_ID)) 
                                   || ((0xf3U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__RXD_ID)) 
                                       || (0x80U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__RXD_ID))))))));
        }
    } else {
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__device_id_type = 0U;
    }
    if (((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXWT)) 
         & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXWT_r))) {
        if (((((((((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r)) 
                   | (2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))) 
                  | (3U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))) 
                 | (4U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))) 
                | (5U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))) 
               | (6U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))) 
              | (7U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))) 
             | (8U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r)))) {
            if ((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__RXD_ID 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXD_DAT;
            } else if ((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx0 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXD_DAT;
            } else if ((3U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXD_DAT;
            } else if ((4U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXD_DAT;
            } else if ((5U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx3 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXD_DAT;
            } else if ((6U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx4 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXD_DAT;
            } else if ((7U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx5 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXD_DAT;
            } else {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx6 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXD_DAT;
            }
        }
    }
    if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_stb) 
         & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg))) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__ack_r 
            = (1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                     >> 4U));
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r;
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXWT_r 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXWT;
    if (vlSelf->reset_n) {
        if (vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__R_CE) {
            __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__rxd__DOT__sp 
                = ((0x80U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                             << 2U)) | (0x7fU & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__rxd__DOT__sp) 
                                                 >> 1U)));
        }
        if (vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXWT) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXD_DAT 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__rxd__DOT__sp;
        }
    } else {
        __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__rxd__DOT__sp = 1U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXD_DAT = 1U;
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__rxd__DOT__sp 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__rxd__DOT__sp;
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXWT 
        = ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT____VdfgTmp_h6d2737cc__0) 
           & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__RXWT));
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__R_CE = 0U;
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__F_CE = 0U;
    if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_stb) 
         & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg))) {
        __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK 
            = (1U & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK)));
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__R_CE 
            = (1U & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK)));
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__F_CE 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK;
    }
    if (vlSelf->reset_n) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_meta;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_meta 
            = vlSelf->snac_pin_in_in;
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                   >> 8U)))) {
            if ((0x80U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 6U)))) {
                    if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    if ((0x80U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)) {
                                        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg = 0U;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_stb_cnt) 
             >= ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_fast_reg)
                  ? 0xc7U : 0x18fU))) {
            __Vdly__tb_axi_periph__DOT__dut__DOT__psx_stb_cnt = 0U;
            vlSelf->tb_axi_periph__DOT__dut__DOT__psx_stb = 1U;
        } else {
            __Vdly__tb_axi_periph__DOT__dut__DOT__psx_stb_cnt 
                = (0x1ffU & ((IData)(1U) + (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_stb_cnt)));
            vlSelf->tb_axi_periph__DOT__dut__DOT__psx_stb = 0U;
        }
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                >> 8U))) {
            if ((0x18U == (0x3fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                    >> 2U)))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg 
                    = (1U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_fast_reg 
                    = (1U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                             >> 2U));
            }
        }
    } else {
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_meta = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__psx_stb_cnt = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__psx_stb = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_fast_reg = 0U;
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK;
    vlSelf->tb_axi_periph__DOT__dut__DOT__psx_stb_cnt 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__psx_stb_cnt;
    vlSelf->dbg_snac_enable = ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_en_reg) 
                               | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg));
}

VL_INLINE_OPT void Vtb_axi_periph___024root___nba_sequent__TOP__1(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___nba_sequent__TOP__1\n"); );
    // Init
    SData/*15:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt = 0;
    IData/*31:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__snac_rx_data;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_rx_data = 0;
    IData/*31:0*/ __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__tx_sr;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__tx_sr = 0;
    CData/*2:0*/ __Vdlyvval__tb_axi_periph__DOT__dut__DOT__target_err_sync__v0;
    __Vdlyvval__tb_axi_periph__DOT__dut__DOT__target_err_sync__v0 = 0;
    CData/*0:0*/ __Vdlyvset__tb_axi_periph__DOT__dut__DOT__target_err_sync__v0;
    __Vdlyvset__tb_axi_periph__DOT__dut__DOT__target_err_sync__v0 = 0;
    CData/*2:0*/ __Vdlyvval__tb_axi_periph__DOT__dut__DOT__target_err_sync__v1;
    __Vdlyvval__tb_axi_periph__DOT__dut__DOT__target_err_sync__v1 = 0;
    CData/*2:0*/ __Vdlyvval__tb_axi_periph__DOT__dut__DOT__target_err_sync__v2;
    __Vdlyvval__tb_axi_periph__DOT__dut__DOT__target_err_sync__v2 = 0;
    CData/*0:0*/ __Vdlyvset__tb_axi_periph__DOT__dut__DOT__target_err_sync__v3;
    __Vdlyvset__tb_axi_periph__DOT__dut__DOT__target_err_sync__v3 = 0;
    // Body
    __Vdlyvset__tb_axi_periph__DOT__dut__DOT__target_err_sync__v0 = 0U;
    __Vdlyvset__tb_axi_periph__DOT__dut__DOT__target_err_sync__v3 = 0U;
    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__periph_rd_data_r 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__periph_rd_data_r;
    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_wstrb 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wstrb;
    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_wdata 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__burst_count 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__burst_count;
    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__state;
    vlSelf->__Vdly__s_axi_rvalid = vlSelf->s_axi_rvalid;
    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_addr 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__tx_sr 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__tx_sr;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt;
    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_rx_data 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_rx_data;
    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state;
    if (vlSelf->reset_n) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_done_sync 
            = ((6U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_done_sync) 
                      << 1U)) | (IData)(vlSelf->target_dataslot_done_in));
        __Vdlyvval__tb_axi_periph__DOT__dut__DOT__target_err_sync__v0 
            = ((6U & (vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync
                      [0U] << 1U)) | (1U & (IData)(vlSelf->target_dataslot_err_in)));
        __Vdlyvset__tb_axi_periph__DOT__dut__DOT__target_err_sync__v0 = 1U;
        __Vdlyvval__tb_axi_periph__DOT__dut__DOT__target_err_sync__v1 
            = ((6U & (vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync
                      [1U] << 1U)) | (1U & ((IData)(vlSelf->target_dataslot_err_in) 
                                            >> 1U)));
        __Vdlyvval__tb_axi_periph__DOT__dut__DOT__target_err_sync__v2 
            = ((6U & (vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync
                      [2U] << 1U)) | (1U & ((IData)(vlSelf->target_dataslot_err_in) 
                                            >> 2U)));
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_ack_sync 
            = ((6U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_ack_sync) 
                      << 1U)) | (IData)(vlSelf->target_dataslot_ack_in));
        if (vlSelf->tb_axi_periph__DOT__pal_commit_w) {
            vlSelf->dbg_pal_commit_count = (0xffU & 
                                            ((IData)(1U) 
                                             + (IData)(vlSelf->dbg_pal_commit_count)));
        }
        vlSelf->tb_axi_periph__DOT__mix_pos_readback_r 
            = (0x3fffffU & ((IData)(0x100U) + (0x1fU 
                                               & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                  >> 2U))));
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_done = 0U;
        if ((4U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state))) {
            if ((2U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state))) {
                if ((1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state))) {
                    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state = 0U;
                } else {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk = 1U;
                    if ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt))) {
                        __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt 
                            = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_div_reg;
                        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state = 3U;
                    } else {
                        __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt 
                            = (0xffffU & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt) 
                                          - (IData)(1U)));
                    }
                }
            } else if ((1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_done = 1U;
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__clk_idle;
                vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state = 0U;
            } else {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk = 1U;
                if ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt))) {
                    if ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__bits_left))) {
                        __Vdly__tb_axi_periph__DOT__dut__DOT__snac_rx_data 
                            = (vlSelf->tb_axi_periph__DOT__dut__DOT__snac_rx_data 
                               << (0x1fU & ((IData)(0x1fU) 
                                            - (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__total_bits))));
                        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk 
                            = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__clk_idle;
                        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state = 5U;
                    } else {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__bits_left 
                            = (0x1fU & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__bits_left) 
                                        - (IData)(1U)));
                        __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt 
                            = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_div_reg;
                        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk = 0U;
                        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state = 3U;
                    }
                } else {
                    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt 
                        = (0xffffU & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt) 
                                      - (IData)(1U)));
                }
            }
        } else if ((2U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state))) {
            if ((1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk = 0U;
                if ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt))) {
                    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__tx_sr 
                        = (vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__tx_sr 
                           << 1U);
                    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_rx_data 
                        = ((vlSelf->tb_axi_periph__DOT__dut__DOT__snac_rx_data 
                            << 1U) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__miso));
                    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_div_reg;
                    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk = 1U;
                    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_mosi 
                        = (1U & (vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__tx_sr 
                                 >> 0x1eU));
                    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state = 4U;
                } else {
                    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt 
                        = (0xffffU & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt) 
                                      - (IData)(1U)));
                }
            } else if ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt))) {
                __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_div_reg;
                vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state 
                    = ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__clk_idle)
                        ? 6U : 3U);
            } else {
                __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt 
                    = (0xffffU & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt) 
                                  - (IData)(1U)));
            }
        } else if ((1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state))) {
            if ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt))) {
                __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_div_reg;
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_latch = 0U;
                vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state = 2U;
            } else {
                __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt 
                    = (0xffffU & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt) 
                                  - (IData)(1U)));
            }
        } else {
            vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk 
                = (1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_mode_reg));
            vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_latch = 0U;
            if (vlSelf->tb_axi_periph__DOT__dut__DOT__snac_start_pulse) {
                __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__tx_sr 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_tx_reg;
                __Vdly__tb_axi_periph__DOT__dut__DOT__snac_rx_data = 0U;
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__bits_left 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_bit_count_reg;
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__total_bits 
                    = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_bit_count_reg;
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__miso_sel 
                    = (1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_mode_reg));
                vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_mosi 
                    = (vlSelf->tb_axi_periph__DOT__dut__DOT__snac_tx_reg 
                       >> 0x1fU);
                if (vlSelf->tb_axi_periph__DOT__dut__DOT__snac_latch_en_reg) {
                    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_div_reg;
                    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_latch = 1U;
                    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state = 1U;
                } else {
                    __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_div_reg;
                    if ((1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_mode_reg))) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk = 1U;
                        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state = 6U;
                    } else {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk = 0U;
                        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state = 3U;
                    }
                }
            }
        }
    } else {
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_done_sync = 0U;
        __Vdlyvset__tb_axi_periph__DOT__dut__DOT__target_err_sync__v3 = 1U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_ack_sync = 0U;
        vlSelf->dbg_pal_commit_count = 0U;
        vlSelf->tb_axi_periph__DOT__mix_pos_readback_r = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__bits_left = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__tx_sr = 0U;
        __Vdly__tb_axi_periph__DOT__dut__DOT__snac_rx_data = 0U;
        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__total_bits = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_mosi = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_latch = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__miso_sel = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_done = 0U;
    }
    if ((1U & (~ (IData)(vlSelf->reset_n)))) {
        vlSelf->dbg_cart_last_wdata = 0U;
        vlSelf->dbg_cart_last_addr = 0U;
        vlSelf->dbg_cart_rd_count = 0U;
        vlSelf->dbg_cart_wr_count = 0U;
    }
    if (__Vdlyvset__tb_axi_periph__DOT__dut__DOT__target_err_sync__v0) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync[0U] 
            = __Vdlyvval__tb_axi_periph__DOT__dut__DOT__target_err_sync__v0;
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync[1U] 
            = __Vdlyvval__tb_axi_periph__DOT__dut__DOT__target_err_sync__v1;
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync[2U] 
            = __Vdlyvval__tb_axi_periph__DOT__dut__DOT__target_err_sync__v2;
    }
    if (__Vdlyvset__tb_axi_periph__DOT__dut__DOT__target_err_sync__v3) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync[0U] = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync[1U] = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync[2U] = 0U;
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt;
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__tx_sr 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__tx_sr;
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_rx_data 
        = __Vdly__tb_axi_periph__DOT__dut__DOT__snac_rx_data;
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__clk_idle 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__miso_sel;
}

VL_INLINE_OPT void Vtb_axi_periph___024root___nba_sequent__TOP__2(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___nba_sequent__TOP__2\n"); );
    // Body
    if (vlSelf->reset_n) {
        vlSelf->tb_axi_periph__DOT__pal_commit_w = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_start_pulse = 0U;
        if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire) 
             & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                   >> 8U)))) {
            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                          >> 7U)))) {
                if ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 5U)))) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->tb_axi_periph__DOT__pal_commit_w 
                                        = (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                                           >> 0x1fU);
                                }
                            }
                        }
                    }
                }
            }
            if ((0x80U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 6U)))) {
                    if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 4U)))) {
                            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_tx_reg 
                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                                }
                            }
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 3U)))) {
                                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_div_reg 
                                        = (0xffffU 
                                           & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                }
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U)))) {
                                    if ((vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                                         & (0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state)))) {
                                        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_start_pulse = 1U;
                                        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_bit_count_reg 
                                            = (0x1fU 
                                               & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                                                  >> 1U));
                                        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_latch_en_reg 
                                            = (1U & 
                                               (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                                                >> 6U));
                                    }
                                    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_mode_reg 
                                        = (3U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
                                                 >> 8U));
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        vlSelf->tb_axi_periph__DOT__pal_commit_w = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_tx_reg = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_div_reg = 0x1f3U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_start_pulse = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_bit_count_reg = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_latch_en_reg = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__snac_mode_reg = 0U;
    }
    vlSelf->dbg_pal_commit = vlSelf->tb_axi_periph__DOT__pal_commit_w;
}

VL_INLINE_OPT void Vtb_axi_periph___024root___nba_comb__TOP__0(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___nba_comb__TOP__0\n"); );
    // Body
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__miso 
        = (1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__miso_sel)
                  ? ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                     >> 5U) : ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                               >> 2U)));
}

extern const VlUnpacked<IData/*31:0*/, 16> Vtb_axi_periph__ConstPool__TABLE_hb3acbca5_0;
extern const VlUnpacked<IData/*31:0*/, 32> Vtb_axi_periph__ConstPool__TABLE_h53952ec2_0;

VL_INLINE_OPT void Vtb_axi_periph___024root___nba_sequent__TOP__3(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___nba_sequent__TOP__3\n"); );
    // Init
    CData/*3:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    CData/*4:0*/ __Vtableidx2;
    __Vtableidx2 = 0;
    // Body
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state 
        = vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state;
    if (vlSelf->reset_n) {
        vlSelf->s_axi_arready = 0U;
        vlSelf->s_axi_awready = 0U;
        vlSelf->s_axi_wready = 0U;
        if (((IData)(vlSelf->s_axi_rvalid) & (IData)(vlSelf->s_axi_rready))) {
            vlSelf->__Vdly__s_axi_rvalid = 0U;
        }
        if (((IData)(vlSelf->s_axi_bvalid) & (IData)(vlSelf->s_axi_bready))) {
            vlSelf->s_axi_bvalid = 0U;
        }
        vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_fifo_we = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_wr = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_rd = 0U;
        vlSelf->dbg_gpu_reg_wr = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_wr = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_irq_clear_wr = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__audio_sample_wr = 0U;
        if ((4U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state))) {
            if ((2U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state))) {
                if ((1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state))) {
                    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = 4U;
                } else {
                    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__burst_count 
                        = (0xffU & ((IData)(1U) + (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__burst_count)));
                    if (vlSelf->tb_axi_periph__DOT__dut__DOT__beat_is_last) {
                        vlSelf->s_axi_bvalid = 1U;
                        vlSelf->s_axi_bresp = 0U;
                        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = 0U;
                    } else {
                        if ((1U & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__burst_is_fixed)))) {
                            vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_addr 
                                = vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr_plus4;
                        }
                        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = 5U;
                    }
                }
            } else if ((1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state))) {
                if (vlSelf->s_axi_wvalid) {
                    vlSelf->s_axi_wready = 1U;
                    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_wdata 
                        = vlSelf->s_axi_wdata;
                    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_wstrb 
                        = vlSelf->s_axi_wstrb;
                    if (vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_ram) {
                        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = 6U;
                    } else {
                        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = 3U;
                        if ((1U & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__burst_is_fixed)))) {
                            vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_addr 
                                = vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr_plus4;
                        }
                    }
                }
            } else {
                vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__periph_rd_data_r 
                    = ((8U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))
                        ? ((4U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))
                            ? 0U : ((2U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))
                                     ? 0U : ((1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))
                                              ? vlSelf->tb_axi_periph__DOT__dut__DOT__hps_rdata
                                              : vlSelf->tb_axi_periph__DOT__dut__DOT__mixer_rdata)))
                        : ((4U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))
                            ? ((2U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))
                                ? ((1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))
                                    ? vlSelf->tb_axi_periph__DOT__gpu_reg_rdata
                                    : vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rdata)
                                : ((1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))
                                    ? 0U : vlSelf->tb_axi_periph__DOT__dut__DOT__cram0_mode_rdata))
                            : ((2U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))
                                ? ((1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))
                                    ? 0U : vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata)
                                : 0U)));
                vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = 2U;
            }
        } else if ((2U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state))) {
            if ((1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state))) {
                vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__burst_count 
                    = (0xffU & ((IData)(1U) + (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__burst_count)));
                if ((0U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_wstrb))) {
                    if (vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_sysreg) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire = 1U;
                    }
                    if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_audio) 
                         & (4U == (0x1cU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)))) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__audio_sample_wr = 1U;
                        vlSelf->tb_axi_periph__DOT__dut__DOT__audio_sample_data 
                            = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                    }
                    if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_cram0) 
                         & (0U == (0xcU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)))) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__cram0_mode 
                            = (1U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                    }
                    if (vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_link) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_wr = 1U;
                        vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_addr 
                            = (0x1fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                        >> 2U));
                        vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_wdata 
                            = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                    }
                    if (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_uart) 
                         & (4U == (0xcU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)))) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_fifo_din 
                            = (0xffU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                        vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_fifo_we = 1U;
                    }
                    if (vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_gpu) {
                        vlSelf->dbg_gpu_reg_wr = 1U;
                        vlSelf->dbg_gpu_reg_addr = 
                            (0xfU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                     >> 2U));
                        vlSelf->dbg_gpu_reg_wdata = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                    }
                    if (vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_mixer) {
                        if ((0x800U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 7U)))) {
                                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 6U)))) {
                                    if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                        if ((1U & (~ 
                                                   (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                    >> 4U)))) {
                                            if ((1U 
                                                 & (~ 
                                                    (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                     >> 3U)))) {
                                                if (
                                                    (4U 
                                                     & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                                    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_irq_clear 
                                                        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                                                    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_irq_clear_wr = 1U;
                                                } else {
                                                    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_enable 
                                                        = 
                                                        (1U 
                                                         & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                                }
                                            }
                                        }
                                    } else if ((0x10U 
                                                & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                        if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                            if ((1U 
                                                 & (~ 
                                                    (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                     >> 2U)))) {
                                                vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_group_packed 
                                                    = 
                                                    ((0xffffffffULL 
                                                      & vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_group_packed) 
                                                     | ((QData)((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)) 
                                                        << 0x20U));
                                            }
                                        } else if (
                                                   (4U 
                                                    & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                            vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_group_packed 
                                                = (
                                                   (0xffffffff00000000ULL 
                                                    & vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_group_packed) 
                                                   | (IData)((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata)));
                                        } else {
                                            vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_3 
                                                = (0xffU 
                                                   & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                        }
                                    } else if ((8U 
                                                & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                        if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                            vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_2 
                                                = (0xffU 
                                                   & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                        } else {
                                            vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_1 
                                                = (0xffU 
                                                   & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                        }
                                    } else if ((4U 
                                                & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                                        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_0 
                                            = (0xffU 
                                               & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                    } else {
                                        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_master_vol 
                                            = (0xffU 
                                               & vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata);
                                    }
                                }
                            }
                        } else {
                            vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_wr = 1U;
                            vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_sel 
                                = (0x1fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                            >> 6U));
                            vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_wdata 
                                = vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata;
                            vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_field 
                                = (0xfU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                           >> 2U));
                        }
                    }
                }
                if (vlSelf->tb_axi_periph__DOT__dut__DOT__beat_is_last) {
                    vlSelf->s_axi_bvalid = 1U;
                    vlSelf->s_axi_bresp = 0U;
                    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = 0U;
                } else {
                    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = 5U;
                }
            } else if (vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat) {
                vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__burst_count 
                    = (0xffU & ((IData)(1U) + (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__burst_count)));
                vlSelf->__Vdly__s_axi_rvalid = 1U;
                vlSelf->s_axi_rdata = vlSelf->tb_axi_periph__DOT__dut__DOT__periph_rd_data_r;
                vlSelf->s_axi_rresp = 0U;
                if (vlSelf->tb_axi_periph__DOT__dut__DOT__beat_is_last) {
                    vlSelf->s_axi_rlast = 1U;
                    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = 0U;
                } else {
                    vlSelf->s_axi_rlast = 0U;
                    if ((1U & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__burst_is_fixed)))) {
                        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_addr 
                            = vlSelf->tb_axi_periph__DOT__dut__DOT__periph_next_addr;
                    }
                    if (vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_link) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_addr 
                            = (0x1fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__periph_next_addr 
                                        >> 2U));
                    }
                    if (vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_gpu) {
                        vlSelf->dbg_gpu_reg_addr = 
                            (0xfU & (vlSelf->tb_axi_periph__DOT__dut__DOT__periph_next_addr 
                                     >> 2U));
                    }
                    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = 7U;
                }
            }
        } else if ((1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state))) {
            if (vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat) {
                vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__burst_count 
                    = (0xffU & ((IData)(1U) + (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__burst_count)));
                vlSelf->__Vdly__s_axi_rvalid = 1U;
                vlSelf->s_axi_rdata = vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_a_reg;
                vlSelf->s_axi_rresp = 0U;
                if (vlSelf->tb_axi_periph__DOT__dut__DOT__beat_is_last) {
                    vlSelf->s_axi_rlast = 1U;
                    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = 0U;
                } else {
                    vlSelf->s_axi_rlast = 0U;
                    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_addr 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr_plus4;
                }
            }
        } else if (((IData)(vlSelf->s_axi_arvalid) 
                    & (~ (IData)(vlSelf->s_axi_rvalid)))) {
            vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__burst_count = 0U;
            vlSelf->s_axi_arready = 1U;
            vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_addr 
                = vlSelf->s_axi_araddr;
            vlSelf->tb_axi_periph__DOT__dut__DOT__burst_len 
                = vlSelf->s_axi_arlen;
            vlSelf->tb_axi_periph__DOT__dut__DOT__req_region 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__ar_region;
            if ((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ar_region))) {
                vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = 1U;
            } else {
                vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = 7U;
                if ((5U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ar_region))) {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_addr 
                        = (0x1fU & (vlSelf->s_axi_araddr 
                                    >> 2U));
                    vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_rd = 1U;
                }
                if ((7U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ar_region))) {
                    vlSelf->dbg_gpu_reg_addr = (0xfU 
                                                & (vlSelf->s_axi_araddr 
                                                   >> 2U));
                }
            }
        } else if (vlSelf->s_axi_awvalid) {
            vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__burst_count = 0U;
            vlSelf->s_axi_awready = 1U;
            vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_addr 
                = vlSelf->s_axi_awaddr;
            vlSelf->tb_axi_periph__DOT__dut__DOT__burst_len 
                = vlSelf->s_axi_awlen;
            vlSelf->tb_axi_periph__DOT__dut__DOT__awburst_latched 
                = vlSelf->s_axi_awburst;
            vlSelf->tb_axi_periph__DOT__dut__DOT__req_region 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__aw_region;
            if (vlSelf->s_axi_wvalid) {
                vlSelf->s_axi_wready = 1U;
                vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_wdata 
                    = vlSelf->s_axi_wdata;
                vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_wstrb 
                    = vlSelf->s_axi_wstrb;
                vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state 
                    = ((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__aw_region))
                        ? 6U : 3U);
            } else {
                vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = 5U;
            }
        }
    } else {
        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__burst_count = 0U;
        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = 0U;
        vlSelf->s_axi_arready = 0U;
        vlSelf->__Vdly__s_axi_rvalid = 0U;
        vlSelf->s_axi_rdata = 0U;
        vlSelf->s_axi_rresp = 0U;
        vlSelf->s_axi_rlast = 0U;
        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__periph_rd_data_r = 0U;
        vlSelf->s_axi_awready = 0U;
        vlSelf->s_axi_wready = 0U;
        vlSelf->s_axi_bvalid = 0U;
        vlSelf->s_axi_bresp = 0U;
        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_addr = 0U;
        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_wdata = 0U;
        vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_wstrb = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__burst_len = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__awburst_latched = 1U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__req_region = 0U;
        vlSelf->dbg_gpu_reg_wr = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__cram0_mode = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_fifo_we = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_fifo_din = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_wr = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_rd = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_addr = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_wdata = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_enable = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_wr = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_sel = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_field = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_wdata = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_irq_clear_wr = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_irq_clear = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_master_vol = 0xffU;
        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_0 = 0xffU;
        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_1 = 0xffU;
        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_2 = 0xffU;
        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_3 = 0xffU;
        vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_group_packed = 0ULL;
        vlSelf->tb_axi_periph__DOT__dut__DOT__audio_sample_wr = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__audio_sample_data = 0U;
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__periph_rd_data_r 
        = vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__periph_rd_data_r;
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_wstrb 
        = vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_wstrb;
    vlSelf->tb_axi_periph__DOT__dut__DOT__burst_count 
        = vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__burst_count;
    vlSelf->tb_axi_periph__DOT__dut__DOT__state = vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state;
    vlSelf->s_axi_rvalid = vlSelf->__Vdly__s_axi_rvalid;
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata 
        = vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_wdata;
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
        = vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_addr;
    __Vtableidx1 = vlSelf->dbg_gpu_reg_addr;
    vlSelf->tb_axi_periph__DOT__gpu_reg_rdata = Vtb_axi_periph__ConstPool__TABLE_hb3acbca5_0
        [__Vtableidx1];
    vlSelf->tb_axi_periph__DOT__dut__DOT__burst_is_fixed 
        = (0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__awburst_latched));
    vlSelf->tb_axi_periph__DOT__dut__DOT__beat_is_last 
        = ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__burst_count) 
           == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__burst_len));
    vlSelf->dbg_state = vlSelf->tb_axi_periph__DOT__dut__DOT__state;
    vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat 
        = (1U & ((~ (IData)(vlSelf->s_axi_rvalid)) 
                 | ((IData)(vlSelf->s_axi_rready) & (IData)(vlSelf->s_axi_rvalid))));
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_uart 
        = (6U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region));
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_link 
        = (5U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region));
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_gpu 
        = (7U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region));
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_sysreg 
        = (2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region));
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_audio 
        = (3U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region));
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_cram0 
        = (4U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region));
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_mixer 
        = (8U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region));
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_ram 
        = (1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region));
    vlSelf->tb_axi_periph__DOT__dut__DOT__hps_rdata 
        = ((0U == (0xfU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                           >> 2U))) ? 0x125U : ((1U 
                                                 == 
                                                 (0xfU 
                                                  & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                     >> 2U)))
                                                 ? 0x33334444U
                                                 : 
                                                ((2U 
                                                  == 
                                                  (0xfU 
                                                   & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                      >> 2U)))
                                                  ? 0x11112222U
                                                  : 
                                                 ((3U 
                                                   == 
                                                   (0xfU 
                                                    & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                       >> 2U)))
                                                   ? 0xabc00U
                                                   : 
                                                  ((4U 
                                                    == 
                                                    (0xfU 
                                                     & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                        >> 2U)))
                                                    ? 0x77778888U
                                                    : 
                                                   ((5U 
                                                     == 
                                                     (0xfU 
                                                      & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                         >> 2U)))
                                                     ? 0x55556666U
                                                     : 
                                                    ((6U 
                                                      == 
                                                      (0xfU 
                                                       & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                          >> 2U)))
                                                      ? 0xbbbbccccU
                                                      : 
                                                     ((7U 
                                                       == 
                                                       (0xfU 
                                                        & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                           >> 2U)))
                                                       ? 0x9999aaaaU
                                                       : 
                                                      ((8U 
                                                        == 
                                                        (0xfU 
                                                         & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                            >> 2U)))
                                                        ? 0xface1234U
                                                        : 
                                                       ((9U 
                                                         == 
                                                         (0xfU 
                                                          & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                             >> 2U)))
                                                         ? 0xe1f05678U
                                                         : 0U))))))))));
    vlSelf->tb_axi_periph__DOT__dut__DOT__cram0_mode_rdata 
        = ((0U == (3U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                         >> 2U))) ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cram0_mode)
            : 0U);
    __Vtableidx2 = (0x1fU & ((vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 2U) - (IData)(8U)));
    vlSelf->tb_axi_periph__DOT__cart_reg_rdata = Vtb_axi_periph__ConstPool__TABLE_h53952ec2_0
        [__Vtableidx2];
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr_plus4 
        = ((0xfffff000U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr) 
           | ((0xffcU & (((IData)(1U) + (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                         >> 2U)) << 2U)) 
              | (3U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)));
    vlSelf->tb_axi_periph__DOT__dut__DOT__mixer_rdata 
        = ((IData)((0x880U == (0x880U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)))
            ? vlSelf->tb_axi_periph__DOT__mix_pos_readback_r
            : ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                ? 0U : ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                         ? ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                             ? ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                 ? 0U : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                          ? 0U : 0xa5a51234U))
                             : ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                 ? 0U : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                          ? 0xc0U : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_enable))))
                         : ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                             ? ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                 ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                     ? 0U : (IData)(
                                                    (vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_group_packed 
                                                     >> 0x20U)))
                                 : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                     ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_group_packed)
                                     : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_3)))
                             : ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                 ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                     ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_2)
                                     : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_1))
                                 : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                     ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_0)
                                     : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_master_vol)))))));
    vlSelf->tb_axi_periph__DOT__dut__DOT__low_page_cont_read 
        = ((~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
               >> 8U)) & ((0x14U <= (0x3fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U))) 
                          & (0x19U >= (0x3fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                >> 2U)))));
    vlSelf->dbg_bram_hold = (1U & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat)));
    vlSelf->dbg_can_push_beat = vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat;
    vlSelf->tb_axi_periph__DOT__dut__DOT__periph_next_addr 
        = ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__awburst_latched))
            ? vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr
            : vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr_plus4);
    if (vlSelf->tb_axi_periph__DOT__dut__DOT__low_page_cont_read) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_sel 
            = (0xfU & ((0x16U >= (0x3fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                           >> 2U)))
                        ? 1U : 2U));
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_field_sel 
            = (3U & ((0x80U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                      ? 3U : ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                               ? ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                   ? ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                       ? 3U : ((8U 
                                                & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                ? 3U
                                                : (
                                                   (4U 
                                                    & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                    ? 3U
                                                    : 2U)))
                                   : ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                       ? ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                           ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                               ? 1U
                                               : 3U)
                                           : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                               ? 2U
                                               : 1U))
                                       : 3U)) : 3U)));
    } else {
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_sel 
            = (0xfU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                       >> 4U));
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_field_sel 
            = (3U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                     >> 2U));
    }
}

VL_INLINE_OPT void Vtb_axi_periph___024root___nba_comb__TOP__1(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___nba_comb__TOP__1\n"); );
    // Body
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_out_mux 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_gpio_out_reg;
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_dir_mux 
        = (3U | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_gpio_dir_reg));
    if ((0U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state))) {
        if ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_mode_reg))) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_out_mux 
                = ((0xfcU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_out_mux)) 
                   | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_latch) 
                       << 1U) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk)));
            vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_dir_mux 
                = (3U | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_dir_mux));
        } else {
            vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_out_mux 
                = ((0x3fU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_out_mux)) 
                   | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_mosi) 
                       << 7U) | ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk) 
                                 << 6U)));
            vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_dir_mux 
                = (0xc0U | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_dir_mux));
        }
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rdata 
        = ((0U == (3U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                         >> 2U))) ? (0xbU | ((0x3ff00U 
                                              & (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_wr_ptr) 
                                                  - (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr)) 
                                                 << 8U)) 
                                             | (4U 
                                                & ((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_empty)) 
                                                   << 2U))))
            : ((2U == (3U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                             >> 2U))) ? vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_fifo
               [vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr]
                : 0U));
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_pop 
        = ((0U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
           & ((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state)) 
              & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat) 
                 & ((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region)) 
                    & (IData)(((0x154U == (0x1fcU & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) 
                               & (0U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count))))))));
    if ((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_sel))) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_key_rdata 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_s;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_joy_rdata 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_s;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_trig_rdata 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_s;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_irq_en_rdata 
            = (1U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask));
    } else if ((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_sel))) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_key_rdata 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_s;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_joy_rdata 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_s;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_trig_rdata 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_s;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_irq_en_rdata 
            = (1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask) 
                     >> 1U));
    } else if ((3U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_sel))) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_key_rdata 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_s;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_joy_rdata 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_s;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_trig_rdata 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_s;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_irq_en_rdata 
            = (1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask) 
                     >> 2U));
    } else {
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_key_rdata 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_s;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_joy_rdata 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_s;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_trig_rdata 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_s;
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_irq_en_rdata 
            = (1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask) 
                     >> 3U));
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_rdata 
        = ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_field_sel))
            ? (1U | ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_irq_en_rdata) 
                     << 8U)) : ((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_field_sel))
                                 ? vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_key_rdata
                                 : ((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_field_sel))
                                     ? vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_joy_rdata
                                     : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_trig_rdata))));
    if (vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg) {
        vlSelf->dbg_snac_pin_out = (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_cmd_o) 
                                     << 7U) | ((0x40U 
                                                & (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psCLK_gate) 
                                                    | ((~ 
                                                        ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1) 
                                                         ^ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2))) 
                                                       | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK))) 
                                                   << 6U)) 
                                               | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2) 
                                                   << 1U) 
                                                  | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1))));
        vlSelf->dbg_snac_pin_dir = 0xc3U;
    } else {
        vlSelf->dbg_snac_pin_out = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_out_mux;
        vlSelf->dbg_snac_pin_dir = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_dir_mux;
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_push 
        = (((0x10U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
            & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_valid)) 
           & ((0x10U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
              | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_pop)));
    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata = 0U;
    if ((0x100U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
        if (((vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
              >> 8U) & ((1U <= (0xfU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                        >> 4U))) & 
                        (4U >= (0xfU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                        >> 4U)))))) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_rdata;
        } else if ((0x80U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                          >> 6U)))) {
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 5U)))) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 4U)))) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 3U)))) {
                            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                          >> 2U)))) {
                                vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                                    = vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key1;
                            }
                        }
                    }
                }
            }
        } else if ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
            if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 2U)))) {
                            vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                                = ((0xf000U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_released_acc) 
                                               << 0xcU)) 
                                   | ((0xf00U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_pressed_acc) 
                                                 << 8U)) 
                                      | (0xffU & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key1))));
                        }
                    } else {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                            = ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? ((((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                      >> 0x18U) - (IData)(0x80U)) 
                                    << 0x18U) | (0xff00U 
                                                 & (((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                                      >> 0x10U) 
                                                     - (IData)(0x80U)) 
                                                    << 8U)))
                                : ((((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                      >> 8U) - (IData)(0x80U)) 
                                    << 0x18U) | (0xff00U 
                                                 & ((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                                     - (IData)(0x80U)) 
                                                    << 8U))));
                    }
                } else {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                        = ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_released_acc)
                                : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_pressed_acc))
                            : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key1)
                                : (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg) 
                                    << 8U) | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_fast_reg) 
                                               << 2U) 
                                              | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg)))));
                }
            } else if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 2U)))) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                            = vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count;
                    }
                } else {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                        = ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? (IData)((vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_dout 
                                       >> 0x20U)) : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_dout));
                }
            }
        } else if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                             >> 5U)))) {
            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                          >> 4U)))) {
                if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                            = vlSelf->tb_axi_periph__DOT__dut__DOT__input_seq;
                    }
                } else {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                        = ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask)
                            : (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count) 
                                << 8U) | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_overflow) 
                                           << 3U) | 
                                          (((0x10U 
                                             == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
                                            << 2U) 
                                           | (((0U 
                                                == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
                                               << 1U) 
                                              | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_pending))))));
                }
            }
        }
    } else if ((0x80U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
        if ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
            if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                            vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                                = vlSelf->tb_axi_periph__DOT__dut__DOT__irq_mask;
                        }
                    }
                } else {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                        = ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__video_scaler_slot_reg)
                                : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_stride_reg))
                            : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_height_reg) 
                                    << 0x10U) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_width_reg))
                                : 0U));
                }
            } else if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                            = ((1U & (IData)(((0U != 
                                               (0x801fU 
                                                & vlSelf->analogizer_settings_in)) 
                                              | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_en_reg))))
                                ? (((vlSelf->analogizer_settings_in 
                                     >> 0xfU) & ((4U 
                                                  == 
                                                  (0xfU 
                                                   & (vlSelf->analogizer_settings_in 
                                                      >> 0xaU))) 
                                                 | (0xcU 
                                                    == 
                                                    (0xfU 
                                                     & (vlSelf->analogizer_settings_in 
                                                        >> 0xaU)))))
                                    ? 0x276U : 0x20dU)
                                : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__vrr_v_total_reg));
                    }
                } else if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                     >> 2U)))) {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                        = vlSelf->tb_axi_periph__DOT__dut__DOT__scanout_fetch_diag;
                }
            } else {
                vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                    = ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? vlSelf->tb_axi_periph__DOT__dut__DOT__sdram_wlast_err
                            : (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__save_dt_word_mode) 
                                << 0xaU) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__save_dt_word)))
                        : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? 0U : (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__term_fb_active) 
                                     << 0x13U) | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_ready_idx) 
                                                   << 3U) 
                                                  | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx) 
                                                      << 1U) 
                                                     | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_pending))))));
            }
        } else {
            vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                = ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                    ? ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? vlSelf->tb_axi_periph__DOT__dut__DOT__timer_counter
                                : (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_irq_pending) 
                                    << 1U) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_enable)))
                            : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? vlSelf->tb_axi_periph__DOT__dut__DOT__timer_period
                                : 0U)) : ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                           ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                               ? (((IData)(vlSelf->dbg_snac_pin_dir) 
                                                   << 8U) 
                                                  | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync))
                                               : vlSelf->tb_axi_periph__DOT__dut__DOT__snac_rx_data)
                                           : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                               ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_div_reg)
                                               : (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_mode_reg) 
                                                   << 8U) 
                                                  | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_en_reg) 
                                                      << 7U) 
                                                     | (0U 
                                                        != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state)))))))
                    : ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_irq_pending)
                                : 0x3ffe35fU) : ((4U 
                                                  & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                  ? vlSelf->dt_query_data_in
                                                  : 
                                                 (((IData)(vlSelf->dt_query_valid_in) 
                                                   << 0x1fU) 
                                                  | (0x7fffffffU 
                                                     & vlSelf->dt_query_data_in))))
                        : ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                ? 2U : 5U) : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                               ? 0xfffffff9U
                                               : vlSelf->analogizer_settings_in))));
        }
    } else if ((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
        if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
            if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 3U)))) {
                    if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 2U)))) {
                        vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                            = vlSelf->tb_axi_periph__DOT__dut__DOT__color_mode_reg;
                    }
                }
            } else {
                vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                    = ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                            ? vlSelf->tb_axi_periph__DOT__dut__DOT__mouse_speed_pct
                            : 0U) : vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_rdata);
            }
        } else if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
            vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                = vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_rdata;
        } else if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                             >> 3U)))) {
            if ((1U & (~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                          >> 2U)))) {
                vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                    = (((IData)(vlSelf->pal_busy_in) 
                        << 0x1fU) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__pal_index_reg));
            }
        }
    } else if ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
        if ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
            if ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                if ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) {
                    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                        = (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_irq_pending) 
                            << 7U) | (((IData)(vlSelf->bridge_wr_idle_in) 
                                       << 6U) | ((0x20U 
                                                  & ((~ 
                                                      ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_ack_sync) 
                                                       >> 2U)) 
                                                     << 5U)) 
                                                 | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_err_latched) 
                                                     << 2U) 
                                                    | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_latched) 
                                                        << 1U) 
                                                       | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_latched))))));
                }
            } else {
                vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                    = ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? vlSelf->tb_axi_periph__DOT__dut__DOT__ds_resp_addr_reg
                        : vlSelf->tb_axi_periph__DOT__dut__DOT__ds_param_addr_reg);
            }
        } else {
            vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
                = ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                    ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? vlSelf->tb_axi_periph__DOT__dut__DOT__ds_length_reg
                        : vlSelf->tb_axi_periph__DOT__dut__DOT__ds_bridge_addr_reg)
                    : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_offset_reg
                        : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_id_reg)));
        }
    } else {
        vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata 
            = ((0x10U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                ? ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                    ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? vlSelf->tb_axi_periph__DOT__dut__DOT__bridge_dbg_wcnt_s
                        : (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx) 
                            << 1U) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_pending)))
                    : ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx)
                        : ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__term_fb_active)
                            ? 0x180000U : vlSelf->tb_axi_periph__DOT__dut__DOT__fb_app_addr)))
                : ((8U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                    ? ((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                        ? (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_keep_app) 
                            << 1U) | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__term_fb_active))
                        : (IData)((vlSelf->tb_axi_periph__DOT__dut__DOT__cycle_counter 
                                   >> 0x20U))) : ((4U 
                                                   & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                   ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cycle_counter)
                                                   : 
                                                  (1U 
                                                   | (2U 
                                                      & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_allcomplete_sync) 
                                                         >> 1U))))));
    }
}

VL_INLINE_OPT void Vtb_axi_periph___024root___nba_sequent__TOP__4(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___nba_sequent__TOP__4\n"); );
    // Body
    vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_a_reg 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__mem
        [vlSelf->tb_axi_periph__DOT__dut__DOT__ram_addr_mux];
    if (vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v0) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__mem[vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v0] 
            = (((~ ((IData)(0xffU) << (IData)(vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v0))) 
                & vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__mem
                [vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v0]) 
               | (0xffffffffULL & ((IData)(vlSelf->__Vdlyvval__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v0) 
                                   << (IData)(vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v0))));
    }
    if (vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v1) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__mem[vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v1] 
            = (((~ ((IData)(0xffU) << (IData)(vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v1))) 
                & vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__mem
                [vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v1]) 
               | (0xffffffffULL & ((IData)(vlSelf->__Vdlyvval__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v1) 
                                   << (IData)(vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v1))));
    }
    if (vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v2) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__mem[vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v2] 
            = (((~ ((IData)(0xffU) << (IData)(vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v2))) 
                & vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__mem
                [vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v2]) 
               | (0xffffffffULL & ((IData)(vlSelf->__Vdlyvval__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v2) 
                                   << (IData)(vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v2))));
    }
    if (vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v3) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__mem[vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v3] 
            = (((~ ((IData)(0xffU) << (IData)(vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v3))) 
                & vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__mem
                [vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v3]) 
               | (0xffffffffULL & ((IData)(vlSelf->__Vdlyvval__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v3) 
                                   << (IData)(vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v3))));
    }
}

VL_INLINE_OPT void Vtb_axi_periph___024root___nba_sequent__TOP__5(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___nba_sequent__TOP__5\n"); );
    // Body
    vlSelf->tb_axi_periph__DOT__dut__DOT__ram_addr_mux 
        = (0x1fffU & ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state))
                       ? ((IData)(vlSelf->s_axi_arvalid)
                           ? (vlSelf->s_axi_araddr 
                              >> 2U) : ((IData)(vlSelf->s_axi_awvalid)
                                         ? (vlSelf->s_axi_awaddr 
                                            >> 2U) : 0U))
                       : ((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state))
                           ? ((1U & ((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat)) 
                                     | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__beat_is_last)))
                               ? (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                  >> 2U) : ((IData)(1U) 
                                            + (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                               >> 2U)))
                           : (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 2U))));
}

void Vtb_axi_periph___024root___eval_nba(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___eval_nba\n"); );
    // Body
    if ((2ULL & vlSelf->__VnbaTriggered.word(0U))) {
        Vtb_axi_periph___024root___nba_sequent__TOP__0(vlSelf);
        vlSelf->__Vm_traceActivity[1U] = 1U;
    }
    if ((1ULL & vlSelf->__VnbaTriggered.word(0U))) {
        Vtb_axi_periph___024root___nba_sequent__TOP__1(vlSelf);
        vlSelf->__Vm_traceActivity[2U] = 1U;
    }
    if ((2ULL & vlSelf->__VnbaTriggered.word(0U))) {
        Vtb_axi_periph___024root___nba_sequent__TOP__2(vlSelf);
        vlSelf->__Vm_traceActivity[3U] = 1U;
    }
    if ((3ULL & vlSelf->__VnbaTriggered.word(0U))) {
        Vtb_axi_periph___024root___nba_comb__TOP__0(vlSelf);
    }
    if ((1ULL & vlSelf->__VnbaTriggered.word(0U))) {
        Vtb_axi_periph___024root___nba_sequent__TOP__3(vlSelf);
        vlSelf->__Vm_traceActivity[4U] = 1U;
    }
    if ((3ULL & vlSelf->__VnbaTriggered.word(0U))) {
        Vtb_axi_periph___024root___nba_comb__TOP__1(vlSelf);
        vlSelf->__Vm_traceActivity[5U] = 1U;
    }
    if ((2ULL & vlSelf->__VnbaTriggered.word(0U))) {
        Vtb_axi_periph___024root___nba_sequent__TOP__4(vlSelf);
    }
    if ((1ULL & vlSelf->__VnbaTriggered.word(0U))) {
        Vtb_axi_periph___024root___nba_sequent__TOP__5(vlSelf);
    }
}

void Vtb_axi_periph___024root___eval_triggers__act(Vtb_axi_periph___024root* vlSelf);

bool Vtb_axi_periph___024root___eval_phase__act(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___eval_phase__act\n"); );
    // Init
    VlTriggerVec<2> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vtb_axi_periph___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelf->__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelf->__VactTriggered, vlSelf->__VnbaTriggered);
        vlSelf->__VnbaTriggered.thisOr(vlSelf->__VactTriggered);
        Vtb_axi_periph___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vtb_axi_periph___024root___eval_phase__nba(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___eval_phase__nba\n"); );
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelf->__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vtb_axi_periph___024root___eval_nba(vlSelf);
        vlSelf->__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_axi_periph___024root___dump_triggers__ico(Vtb_axi_periph___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_axi_periph___024root___dump_triggers__nba(Vtb_axi_periph___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_axi_periph___024root___dump_triggers__act(Vtb_axi_periph___024root* vlSelf);
#endif  // VL_DEBUG

void Vtb_axi_periph___024root___eval(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___eval\n"); );
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
            Vtb_axi_periph___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("tb_axi_periph.v", 17, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vtb_axi_periph___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelf->__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY((0x64U < __VnbaIterCount))) {
#ifdef VL_DEBUG
            Vtb_axi_periph___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("tb_axi_periph.v", 17, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelf->__VactIterCount = 0U;
        vlSelf->__VactContinue = 1U;
        while (vlSelf->__VactContinue) {
            if (VL_UNLIKELY((0x64U < vlSelf->__VactIterCount))) {
#ifdef VL_DEBUG
                Vtb_axi_periph___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("tb_axi_periph.v", 17, "", "Active region did not converge.");
            }
            vlSelf->__VactIterCount = ((IData)(1U) 
                                       + vlSelf->__VactIterCount);
            vlSelf->__VactContinue = 0U;
            if (Vtb_axi_periph___024root___eval_phase__act(vlSelf)) {
                vlSelf->__VactContinue = 1U;
            }
        }
        if (Vtb_axi_periph___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vtb_axi_periph___024root___eval_debug_assertions(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((vlSelf->clk & 0xfeU))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY((vlSelf->reset_n & 0xfeU))) {
        Verilated::overWidthError("reset_n");}
    if (VL_UNLIKELY((vlSelf->s_axi_arvalid & 0xfeU))) {
        Verilated::overWidthError("s_axi_arvalid");}
    if (VL_UNLIKELY((vlSelf->s_axi_rready & 0xfeU))) {
        Verilated::overWidthError("s_axi_rready");}
    if (VL_UNLIKELY((vlSelf->s_axi_awvalid & 0xfeU))) {
        Verilated::overWidthError("s_axi_awvalid");}
    if (VL_UNLIKELY((vlSelf->s_axi_awburst & 0xfcU))) {
        Verilated::overWidthError("s_axi_awburst");}
    if (VL_UNLIKELY((vlSelf->s_axi_wvalid & 0xfeU))) {
        Verilated::overWidthError("s_axi_wvalid");}
    if (VL_UNLIKELY((vlSelf->s_axi_wstrb & 0xf0U))) {
        Verilated::overWidthError("s_axi_wstrb");}
    if (VL_UNLIKELY((vlSelf->s_axi_wlast & 0xfeU))) {
        Verilated::overWidthError("s_axi_wlast");}
    if (VL_UNLIKELY((vlSelf->s_axi_bready & 0xfeU))) {
        Verilated::overWidthError("s_axi_bready");}
    if (VL_UNLIKELY((vlSelf->pal_busy_in & 0xfeU))) {
        Verilated::overWidthError("pal_busy_in");}
    if (VL_UNLIKELY((vlSelf->gpu_swap_req_in & 0xfeU))) {
        Verilated::overWidthError("gpu_swap_req_in");}
    if (VL_UNLIKELY((vlSelf->gpu_swap_idx_in & 0xfcU))) {
        Verilated::overWidthError("gpu_swap_idx_in");}
    if (VL_UNLIKELY((vlSelf->vsync_in & 0xfeU))) {
        Verilated::overWidthError("vsync_in");}
    if (VL_UNLIKELY((vlSelf->early_vblank_in & 0xfeU))) {
        Verilated::overWidthError("early_vblank_in");}
    if (VL_UNLIKELY((vlSelf->os_inmenu_in & 0xfeU))) {
        Verilated::overWidthError("os_inmenu_in");}
    if (VL_UNLIKELY((vlSelf->target_dataslot_ack_in 
                     & 0xfeU))) {
        Verilated::overWidthError("target_dataslot_ack_in");}
    if (VL_UNLIKELY((vlSelf->target_dataslot_done_in 
                     & 0xfeU))) {
        Verilated::overWidthError("target_dataslot_done_in");}
    if (VL_UNLIKELY((vlSelf->target_dataslot_err_in 
                     & 0xf8U))) {
        Verilated::overWidthError("target_dataslot_err_in");}
    if (VL_UNLIKELY((vlSelf->bridge_wr_idle_in & 0xfeU))) {
        Verilated::overWidthError("bridge_wr_idle_in");}
    if (VL_UNLIKELY((vlSelf->dt_query_valid_in & 0xfeU))) {
        Verilated::overWidthError("dt_query_valid_in");}
}
#endif  // VL_DEBUG
