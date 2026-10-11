// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vtb_axi_periph.h for the primary calling header

#include "Vtb_axi_periph__pch.h"
#include "Vtb_axi_periph___024root.h"

VL_ATTR_COLD void Vtb_axi_periph___024root___eval_static(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___eval_static\n"); );
}

VL_ATTR_COLD void Vtb_axi_periph___024root___eval_initial__TOP(Vtb_axi_periph___024root* vlSelf);

VL_ATTR_COLD void Vtb_axi_periph___024root___eval_initial(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___eval_initial\n"); );
    // Body
    Vtb_axi_periph___024root___eval_initial__TOP(vlSelf);
    vlSelf->__Vm_traceActivity[5U] = 1U;
    vlSelf->__Vm_traceActivity[4U] = 1U;
    vlSelf->__Vm_traceActivity[3U] = 1U;
    vlSelf->__Vm_traceActivity[2U] = 1U;
    vlSelf->__Vm_traceActivity[1U] = 1U;
    vlSelf->__Vm_traceActivity[0U] = 1U;
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = vlSelf->clk;
    vlSelf->__Vtrigprevexpr___TOP__reset_n__0 = vlSelf->reset_n;
}

VL_ATTR_COLD void Vtb_axi_periph___024root___eval_initial__TOP(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___eval_initial__TOP\n"); );
    // Body
    vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__i = 0U;
    while (VL_GTS_III(32, 0x2000U, vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__i)) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__mem[(0x1fffU 
                                                             & vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__i)] = 0x13U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__i 
            = ((IData)(1U) + vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__i);
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_a_reg = 0U;
    vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_b_reg = 0U;
    vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_a_reg2 = 0U;
    vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_b_reg2 = 0U;
}

VL_ATTR_COLD void Vtb_axi_periph___024root___eval_final(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___eval_final\n"); );
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_axi_periph___024root___dump_triggers__stl(Vtb_axi_periph___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vtb_axi_periph___024root___eval_phase__stl(Vtb_axi_periph___024root* vlSelf);

VL_ATTR_COLD void Vtb_axi_periph___024root___eval_settle(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___eval_settle\n"); );
    // Init
    IData/*31:0*/ __VstlIterCount;
    CData/*0:0*/ __VstlContinue;
    // Body
    __VstlIterCount = 0U;
    vlSelf->__VstlFirstIteration = 1U;
    __VstlContinue = 1U;
    while (__VstlContinue) {
        if (VL_UNLIKELY((0x64U < __VstlIterCount))) {
#ifdef VL_DEBUG
            Vtb_axi_periph___024root___dump_triggers__stl(vlSelf);
#endif
            VL_FATAL_MT("tb_axi_periph.v", 17, "", "Settle region did not converge.");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        __VstlContinue = 0U;
        if (Vtb_axi_periph___024root___eval_phase__stl(vlSelf)) {
            __VstlContinue = 1U;
        }
        vlSelf->__VstlFirstIteration = 0U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_axi_periph___024root___dump_triggers__stl(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VstlTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VstlTriggered.word(0U))) {
        VL_DBG_MSGF("         'stl' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

extern const VlUnpacked<IData/*31:0*/, 16> Vtb_axi_periph__ConstPool__TABLE_hb3acbca5_0;
extern const VlUnpacked<IData/*31:0*/, 32> Vtb_axi_periph__ConstPool__TABLE_h53952ec2_0;

VL_ATTR_COLD void Vtb_axi_periph___024root___stl_sequent__TOP__0(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___stl_sequent__TOP__0\n"); );
    // Init
    CData/*3:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__Vfuncout;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__addr;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__7__addr = 0;
    CData/*3:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__Vfuncout;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__Vfuncout = 0;
    IData/*31:0*/ __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__addr;
    __Vfunc_tb_axi_periph__DOT__dut__DOT__decode_region__8__addr = 0;
    CData/*3:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    CData/*4:0*/ __Vtableidx2;
    __Vtableidx2 = 0;
    // Body
    vlSelf->dbg_state = vlSelf->tb_axi_periph__DOT__dut__DOT__state;
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_uart 
        = (6U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region));
    vlSelf->tb_axi_periph__DOT__dut__DOT__burst_is_fixed 
        = (0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__awburst_latched));
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
    vlSelf->dbg_pal_commit = vlSelf->tb_axi_periph__DOT__pal_commit_w;
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__clk_idle 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__miso_sel;
    vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_rising 
        = (IData)((2U == (6U & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_sync))));
    vlSelf->dbg_snac_enable = ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_en_reg) 
                               | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg));
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__miso 
        = (1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__miso_sel)
                  ? ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                     >> 5U) : ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                               >> 2U)));
    __Vtableidx1 = vlSelf->dbg_gpu_reg_addr;
    vlSelf->tb_axi_periph__DOT__gpu_reg_rdata = Vtb_axi_periph__ConstPool__TABLE_hb3acbca5_0
        [__Vtableidx1];
    __Vtableidx2 = (0x1fU & ((vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                              >> 2U) - (IData)(8U)));
    vlSelf->tb_axi_periph__DOT__cart_reg_rdata = Vtb_axi_periph__ConstPool__TABLE_h53952ec2_0
        [__Vtableidx2];
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
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr_plus4 
        = ((0xfffff000U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr) 
           | ((0xffcU & (((IData)(1U) + (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                         >> 2U)) << 2U)) 
              | (3U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)));
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT____VdfgTmp_h6d2737cc__0 
        = (1U & ((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1)) 
                 | (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2))));
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
    vlSelf->tb_axi_periph__DOT__dut__DOT__beat_is_last 
        = ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__burst_count) 
           == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__burst_len));
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_out_mux 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__snac_gpio_out_reg;
    vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_empty 
        = ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr) 
           == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_wr_ptr));
    if ((0U == (3U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                      >> 2U)))) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__cram0_mode_rdata 
            = vlSelf->tb_axi_periph__DOT__dut__DOT__cram0_mode;
        vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rdata 
            = (0xbU | ((0x3ff00U & (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_wr_ptr) 
                                     - (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr)) 
                                    << 8U)) | (4U & 
                                               ((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_empty)) 
                                                << 2U))));
    } else {
        vlSelf->tb_axi_periph__DOT__dut__DOT__cram0_mode_rdata = 0U;
        vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rdata 
            = ((2U == (3U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                             >> 2U))) ? vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_fifo
               [vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr]
                : 0U);
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__analog_show_term 
        = ((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_keep_app)) 
           & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__term_fb_active));
    vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat 
        = (1U & ((~ (IData)(vlSelf->s_axi_rvalid)) 
                 | ((IData)(vlSelf->s_axi_rready) & (IData)(vlSelf->s_axi_rvalid))));
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
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_dout 
        = vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem
        [vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr];
    vlSelf->tb_axi_periph__DOT__dut__DOT__fb_app_addr 
        = ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx))
            ? 0U : ((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx))
                     ? 0x80000U : 0x100000U));
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
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_pending 
        = ((0U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
           | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_overflow));
    vlSelf->tb_axi_periph__DOT__dut__DOT__low_page_cont_read 
        = ((~ (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
               >> 8U)) & ((0x14U <= (0x3fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                              >> 2U))) 
                          & (0x19U >= (0x3fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                >> 2U)))));
    vlSelf->tb_axi_periph__DOT__dut__DOT__periph_next_addr 
        = ((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__awburst_latched))
            ? vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr
            : vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr_plus4);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXWT 
        = ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT____VdfgTmp_h6d2737cc__0) 
           & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__RXWT));
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
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_valid 
        = (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask) 
            >> (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot)) 
           & (0U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fields)));
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
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_push 
        = (((0x10U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
            & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_valid)) 
           & ((0x10U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
              | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_pop)));
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

VL_ATTR_COLD void Vtb_axi_periph___024root___eval_stl(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___eval_stl\n"); );
    // Body
    if ((1ULL & vlSelf->__VstlTriggered.word(0U))) {
        Vtb_axi_periph___024root___stl_sequent__TOP__0(vlSelf);
        vlSelf->__Vm_traceActivity[5U] = 1U;
        vlSelf->__Vm_traceActivity[4U] = 1U;
        vlSelf->__Vm_traceActivity[3U] = 1U;
        vlSelf->__Vm_traceActivity[2U] = 1U;
        vlSelf->__Vm_traceActivity[1U] = 1U;
        vlSelf->__Vm_traceActivity[0U] = 1U;
    }
}

VL_ATTR_COLD void Vtb_axi_periph___024root___eval_triggers__stl(Vtb_axi_periph___024root* vlSelf);

VL_ATTR_COLD bool Vtb_axi_periph___024root___eval_phase__stl(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___eval_phase__stl\n"); );
    // Init
    CData/*0:0*/ __VstlExecute;
    // Body
    Vtb_axi_periph___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = vlSelf->__VstlTriggered.any();
    if (__VstlExecute) {
        Vtb_axi_periph___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_axi_periph___024root___dump_triggers__ico(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___dump_triggers__ico\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VicoTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VicoTriggered.word(0U))) {
        VL_DBG_MSGF("         'ico' region trigger index 0 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_axi_periph___024root___dump_triggers__act(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VactTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 0 is active: @(posedge clk or negedge reset_n)\n");
    }
    if ((2ULL & vlSelf->__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 1 is active: @(posedge clk)\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vtb_axi_periph___024root___dump_triggers__nba(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___dump_triggers__nba\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VnbaTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 0 is active: @(posedge clk or negedge reset_n)\n");
    }
    if ((2ULL & vlSelf->__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 1 is active: @(posedge clk)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vtb_axi_periph___024root___ctor_var_reset(Vtb_axi_periph___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root___ctor_var_reset\n"); );
    // Body
    vlSelf->clk = VL_RAND_RESET_I(1);
    vlSelf->reset_n = VL_RAND_RESET_I(1);
    vlSelf->s_axi_arvalid = VL_RAND_RESET_I(1);
    vlSelf->s_axi_arready = VL_RAND_RESET_I(1);
    vlSelf->s_axi_araddr = VL_RAND_RESET_I(32);
    vlSelf->s_axi_arlen = VL_RAND_RESET_I(8);
    vlSelf->s_axi_rvalid = VL_RAND_RESET_I(1);
    vlSelf->s_axi_rready = VL_RAND_RESET_I(1);
    vlSelf->s_axi_rdata = VL_RAND_RESET_I(32);
    vlSelf->s_axi_rresp = VL_RAND_RESET_I(2);
    vlSelf->s_axi_rlast = VL_RAND_RESET_I(1);
    vlSelf->s_axi_awvalid = VL_RAND_RESET_I(1);
    vlSelf->s_axi_awready = VL_RAND_RESET_I(1);
    vlSelf->s_axi_awaddr = VL_RAND_RESET_I(32);
    vlSelf->s_axi_awlen = VL_RAND_RESET_I(8);
    vlSelf->s_axi_awburst = VL_RAND_RESET_I(2);
    vlSelf->s_axi_wvalid = VL_RAND_RESET_I(1);
    vlSelf->s_axi_wready = VL_RAND_RESET_I(1);
    vlSelf->s_axi_wdata = VL_RAND_RESET_I(32);
    vlSelf->s_axi_wstrb = VL_RAND_RESET_I(4);
    vlSelf->s_axi_wlast = VL_RAND_RESET_I(1);
    vlSelf->s_axi_bvalid = VL_RAND_RESET_I(1);
    vlSelf->s_axi_bready = VL_RAND_RESET_I(1);
    vlSelf->s_axi_bresp = VL_RAND_RESET_I(2);
    vlSelf->dbg_state = VL_RAND_RESET_I(4);
    vlSelf->dbg_bram_hold = VL_RAND_RESET_I(1);
    vlSelf->dbg_can_push_beat = VL_RAND_RESET_I(1);
    vlSelf->dbg_gpu_reg_wr = VL_RAND_RESET_I(1);
    vlSelf->dbg_gpu_reg_addr = VL_RAND_RESET_I(4);
    vlSelf->dbg_gpu_reg_wdata = VL_RAND_RESET_I(32);
    vlSelf->dbg_save_dt_slot = VL_RAND_RESET_I(4);
    vlSelf->dbg_save_dt_size = VL_RAND_RESET_I(32);
    vlSelf->dbg_save_dt_commit = VL_RAND_RESET_I(1);
    vlSelf->dbg_analogizer_wr_toggle = VL_RAND_RESET_I(3);
    vlSelf->dbg_analogizer_wr_settings = VL_RAND_RESET_I(32);
    vlSelf->dbg_analogizer_wr_hoffset = VL_RAND_RESET_I(32);
    vlSelf->dbg_analogizer_wr_voffset = VL_RAND_RESET_I(32);
    vlSelf->dbg_snac_pin_out = VL_RAND_RESET_I(8);
    vlSelf->dbg_snac_pin_dir = VL_RAND_RESET_I(8);
    vlSelf->dbg_snac_enable = VL_RAND_RESET_I(1);
    vlSelf->dbg_cart_wr_count = VL_RAND_RESET_I(8);
    vlSelf->dbg_cart_rd_count = VL_RAND_RESET_I(8);
    vlSelf->dbg_cart_last_addr = VL_RAND_RESET_I(5);
    vlSelf->dbg_cart_last_wdata = VL_RAND_RESET_I(32);
    vlSelf->dbg_pal_commit = VL_RAND_RESET_I(1);
    vlSelf->dbg_analog_fb_addr = VL_RAND_RESET_I(25);
    vlSelf->dbg_analog_color_mode = VL_RAND_RESET_I(3);
    vlSelf->dbg_analog_fb_width = VL_RAND_RESET_I(10);
    vlSelf->dbg_analog_fb_height = VL_RAND_RESET_I(10);
    vlSelf->dbg_analog_fb_stride = VL_RAND_RESET_I(16);
    vlSelf->dbg_pal_commit_count = VL_RAND_RESET_I(8);
    vlSelf->pal_busy_in = VL_RAND_RESET_I(1);
    vlSelf->analogizer_settings_in = VL_RAND_RESET_I(32);
    vlSelf->gpu_swap_req_in = VL_RAND_RESET_I(1);
    vlSelf->gpu_swap_idx_in = VL_RAND_RESET_I(2);
    vlSelf->vsync_in = VL_RAND_RESET_I(1);
    vlSelf->early_vblank_in = VL_RAND_RESET_I(1);
    vlSelf->os_inmenu_in = VL_RAND_RESET_I(1);
    vlSelf->target_dataslot_ack_in = VL_RAND_RESET_I(1);
    vlSelf->target_dataslot_done_in = VL_RAND_RESET_I(1);
    vlSelf->target_dataslot_err_in = VL_RAND_RESET_I(3);
    vlSelf->bridge_wr_idle_in = VL_RAND_RESET_I(1);
    vlSelf->cont1_key_in = VL_RAND_RESET_I(32);
    vlSelf->cont1_joy_in = VL_RAND_RESET_I(32);
    vlSelf->cont1_trig_in = VL_RAND_RESET_I(16);
    vlSelf->cont2_key_in = VL_RAND_RESET_I(32);
    vlSelf->cont2_joy_in = VL_RAND_RESET_I(32);
    vlSelf->cont2_trig_in = VL_RAND_RESET_I(16);
    vlSelf->cont3_key_in = VL_RAND_RESET_I(32);
    vlSelf->cont3_joy_in = VL_RAND_RESET_I(32);
    vlSelf->cont3_trig_in = VL_RAND_RESET_I(16);
    vlSelf->cont4_key_in = VL_RAND_RESET_I(32);
    vlSelf->cont4_joy_in = VL_RAND_RESET_I(32);
    vlSelf->cont4_trig_in = VL_RAND_RESET_I(16);
    vlSelf->dt_query_data_in = VL_RAND_RESET_I(32);
    vlSelf->dt_query_valid_in = VL_RAND_RESET_I(1);
    vlSelf->snac_pin_in_in = VL_RAND_RESET_I(8);
    vlSelf->ext_irq_out = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__gpu_reg_rdata = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__pal_commit_w = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__cart_reg_rdata = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__mix_pos_readback_r = VL_RAND_RESET_I(22);
    vlSelf->tb_axi_periph__DOT__dut__DOT__pal_wr = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__pal_addr = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__pal_data = VL_RAND_RESET_I(24);
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_read = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_write = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_openfile = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_getfile = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_flush = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_id = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_slotoffset = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_bridgeaddr = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_length = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_buffer_param_struct = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_buffer_resp_struct = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__audio_sample_wr = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__audio_sample_data = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cram0_mode = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_enable = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_wr = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_sel = VL_RAND_RESET_I(5);
    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_field = VL_RAND_RESET_I(4);
    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_wdata = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_irq_clear_wr = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_irq_clear = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_master_vol = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_0 = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_1 = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_2 = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_3 = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_group_packed = VL_RAND_RESET_Q(64);
    vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_wr = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_rd = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_addr = VL_RAND_RESET_I(5);
    vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_wdata = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__save_dt_word = VL_RAND_RESET_I(10);
    vlSelf->tb_axi_periph__DOT__dut__DOT__save_dt_word_mode = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_dv = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_byte = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__mouse_speed_pct = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__sdram_wlast_err = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__scanout_fetch_diag = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__shutdown_ack = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__dt_query_addr = VL_RAND_RESET_I(10);
    vlSelf->tb_axi_periph__DOT__dut__DOT__dt_query_toggle = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_en_reg = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_mode_reg = VL_RAND_RESET_I(2);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_div_reg = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_tx_reg = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_gpio_out_reg = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_gpio_dir_reg = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_start_pulse = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_fast_reg = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_clear_edges_pulse = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_done = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_rx_data = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_mosi = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_latch = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_meta = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_bit_count_reg = VL_RAND_RESET_I(5);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_latch_en_reg = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__psx_stb_cnt = VL_RAND_RESET_I(9);
    vlSelf->tb_axi_periph__DOT__dut__DOT__psx_stb = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key1 = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key2 = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy2 = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__psx_cmd_o = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__psx_prev_key1 = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__psx_pressed_acc = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__psx_released_acc = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_out_mux = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_dir_mux = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ram_addr_mux = VL_RAND_RESET_I(13);
    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cycle_counter = VL_RAND_RESET_Q(64);
    vlSelf->tb_axi_periph__DOT__dut__DOT__color_mode_reg = VL_RAND_RESET_I(3);
    vlSelf->tb_axi_periph__DOT__dut__DOT__fb_width_reg = VL_RAND_RESET_I(10);
    vlSelf->tb_axi_periph__DOT__dut__DOT__fb_height_reg = VL_RAND_RESET_I(10);
    vlSelf->tb_axi_periph__DOT__dut__DOT__fb_stride_reg = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__analog_color_mode_reg = VL_RAND_RESET_I(3);
    vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_width_reg = VL_RAND_RESET_I(10);
    vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_height_reg = VL_RAND_RESET_I(10);
    vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_stride_reg = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__video_scaler_slot_reg = VL_RAND_RESET_I(3);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_id_reg = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_offset_reg = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_bridge_addr_reg = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_length_reg = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_param_addr_reg = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_resp_addr_reg = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__pal_index_reg = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__timer_period = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__timer_counter = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__timer_enable = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__timer_irq_pending = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__irq_mask = VL_RAND_RESET_I(6);
    vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_irq_pending = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_irq_pending = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_pending = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx = VL_RAND_RESET_I(2);
    vlSelf->tb_axi_periph__DOT__dut__DOT__fb_ready_idx = VL_RAND_RESET_I(2);
    vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_pending = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__term_fb_active = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__analog_keep_app = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__vrr_v_total_reg = VL_RAND_RESET_I(10);
    vlSelf->tb_axi_periph__DOT__dut__DOT__fb_app_addr = VL_RAND_RESET_I(25);
    vlSelf->tb_axi_periph__DOT__dut__DOT__analog_show_term = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_allcomplete_sync = VL_RAND_RESET_I(3);
    vlSelf->tb_axi_periph__DOT__dut__DOT__bridge_dbg_wcnt_meta = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__bridge_dbg_wcnt_s = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_sync = VL_RAND_RESET_I(3);
    vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_rising = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__early_vblank_sync = VL_RAND_RESET_I(3);
    vlSelf->tb_axi_periph__DOT__dut__DOT__os_inmenu_sync = VL_RAND_RESET_I(3);
    vlSelf->tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__Vstatic__even_stride = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_ack_sync = VL_RAND_RESET_I(3);
    vlSelf->tb_axi_periph__DOT__dut__DOT__target_done_sync = VL_RAND_RESET_I(3);
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync[__Vi0] = VL_RAND_RESET_I(3);
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__rtc_valid_meta = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__rtc_valid_s = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_meta = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_s = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_meta = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_s = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_meta = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_s = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_meta = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_s = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_meta = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_s = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_meta = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_s = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_meta = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_s = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_meta = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_s = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_meta = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_s = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_meta = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_s = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_meta = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_s = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_meta = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_s = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask = VL_RAND_RESET_I(4);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_overflow = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_seq = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key0 = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key1 = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key2 = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key3 = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy0 = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy1 = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy2 = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy3 = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig0 = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig1 = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig2 = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig3 = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot = VL_RAND_RESET_I(2);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fields = VL_RAND_RESET_I(3);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_valid = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count = VL_RAND_RESET_I(5);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_dout = VL_RAND_RESET_Q(64);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_clear = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_cmd_active = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_latched = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_latched = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_seen_low = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_seen_low = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ds_err_latched = VL_RAND_RESET_I(3);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_key_rdata = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_joy_rdata = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_trig_rdata = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_irq_en_rdata = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_rdata = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__low_page_cont_read = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_sel = VL_RAND_RESET_I(4);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_field_sel = VL_RAND_RESET_I(2);
    for (int __Vi0 = 0; __Vi0 < 1024; ++__Vi0) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_fifo[__Vi0] = VL_RAND_RESET_I(8);
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_wr_ptr = VL_RAND_RESET_I(10);
    vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr = VL_RAND_RESET_I(10);
    vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_empty = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_fifo_we = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_fifo_din = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rdata = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__cram0_mode_rdata = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__mixer_rdata = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__periph_rd_data_r = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__state = VL_RAND_RESET_I(3);
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_wstrb = VL_RAND_RESET_I(4);
    vlSelf->tb_axi_periph__DOT__dut__DOT__burst_len = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__burst_count = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__awburst_latched = VL_RAND_RESET_I(2);
    vlSelf->tb_axi_periph__DOT__dut__DOT__burst_is_fixed = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_region = VL_RAND_RESET_I(4);
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_ram = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_sysreg = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_audio = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_cram0 = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_link = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_uart = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_gpu = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_is_mixer = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__hps_rdata = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__beat_is_last = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr_plus4 = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__periph_next_addr = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ar_region = VL_RAND_RESET_I(4);
    vlSelf->tb_axi_periph__DOT__dut__DOT__aw_region = VL_RAND_RESET_I(4);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state = VL_RAND_RESET_I(3);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt = VL_RAND_RESET_I(16);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__bits_left = VL_RAND_RESET_I(5);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__total_bits = VL_RAND_RESET_I(5);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__tx_sr = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__miso_sel = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__miso = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__clk_idle = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx0 = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1 = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2 = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx3 = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx4 = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx5 = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx6 = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__RXD_ID = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__att1r = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__att2r = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__no_gamepad = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__R_CE = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__F_CE = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__ack_r = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__device_id_type = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXWT = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_TXSET = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXD_DAT = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXWT_r = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r = VL_RAND_RESET_I(4);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer = VL_RAND_RESET_I(11);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__RXWT = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXWT = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXSET = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psCLK_gate = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1 = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2 = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT____VdfgTmp_h6d2737cc__0 = 0;
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps = VL_RAND_RESET_I(8);
    vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__rxd__DOT__sp = VL_RAND_RESET_I(8);
    for (int __Vi0 = 0; __Vi0 < 8192; ++__Vi0) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__mem[__Vi0] = VL_RAND_RESET_I(32);
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_a_reg = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_b_reg = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_a_reg2 = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_b_reg2 = VL_RAND_RESET_I(32);
    vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__i = VL_RAND_RESET_I(32);
    for (int __Vi0 = 0; __Vi0 < 16; ++__Vi0) {
        vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[__Vi0] = VL_RAND_RESET_Q(64);
    }
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__wr_ptr = VL_RAND_RESET_I(4);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr = VL_RAND_RESET_I(4);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_pop = VL_RAND_RESET_I(1);
    vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_push = VL_RAND_RESET_I(1);
    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state = VL_RAND_RESET_I(3);
    vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v0 = 0;
    vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v0 = 0;
    vlSelf->__Vdlyvval__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v0 = VL_RAND_RESET_I(8);
    vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v0 = 0;
    vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v1 = 0;
    vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v1 = 0;
    vlSelf->__Vdlyvval__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v1 = VL_RAND_RESET_I(8);
    vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v1 = 0;
    vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v2 = 0;
    vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v2 = 0;
    vlSelf->__Vdlyvval__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v2 = VL_RAND_RESET_I(8);
    vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v2 = 0;
    vlSelf->__Vdlyvdim0__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v3 = 0;
    vlSelf->__Vdlyvlsb__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v3 = 0;
    vlSelf->__Vdlyvval__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v3 = VL_RAND_RESET_I(8);
    vlSelf->__Vdlyvset__tb_axi_periph__DOT__dut__DOT__ram__DOT__mem__v3 = 0;
    vlSelf->__Vdly__s_axi_rvalid = VL_RAND_RESET_I(1);
    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__state = VL_RAND_RESET_I(3);
    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__burst_count = VL_RAND_RESET_I(8);
    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_addr = VL_RAND_RESET_I(32);
    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_wdata = VL_RAND_RESET_I(32);
    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__req_wstrb = VL_RAND_RESET_I(4);
    vlSelf->__Vdly__tb_axi_periph__DOT__dut__DOT__periph_rd_data_r = VL_RAND_RESET_I(32);
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = VL_RAND_RESET_I(1);
    vlSelf->__Vtrigprevexpr___TOP__reset_n__0 = VL_RAND_RESET_I(1);
    for (int __Vi0 = 0; __Vi0 < 6; ++__Vi0) {
        vlSelf->__Vm_traceActivity[__Vi0] = 0;
    }
}
