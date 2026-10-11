// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vtb_axi_periph__Syms.h"


void Vtb_axi_periph___024root__trace_chg_0_sub_0(Vtb_axi_periph___024root* vlSelf, VerilatedVcd::Buffer* bufp);

void Vtb_axi_periph___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root__trace_chg_0\n"); );
    // Init
    Vtb_axi_periph___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vtb_axi_periph___024root*>(voidSelf);
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (VL_UNLIKELY(!vlSymsp->__Vm_activity)) return;
    // Body
    Vtb_axi_periph___024root__trace_chg_0_sub_0((&vlSymsp->TOP), bufp);
}

void Vtb_axi_periph___024root__trace_chg_0_sub_0(Vtb_axi_periph___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root__trace_chg_0_sub_0\n"); );
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode + 1);
    // Body
    if (VL_UNLIKELY(vlSelf->__Vm_traceActivity[0U])) {
        bufp->chgIData(oldp+0,(vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_b_reg),32);
        bufp->chgIData(oldp+1,(vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_b_reg2),32);
        bufp->chgIData(oldp+2,(vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__i),32);
    }
    if (VL_UNLIKELY(vlSelf->__Vm_traceActivity[1U])) {
        bufp->chgCData(oldp+3,(vlSelf->tb_axi_periph__DOT__dut__DOT__color_mode_reg),3);
        bufp->chgIData(oldp+4,(((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__term_fb_active)
                                 ? 0x180000U : vlSelf->tb_axi_periph__DOT__dut__DOT__fb_app_addr)),25);
        bufp->chgSData(oldp+5,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_width_reg),10);
        bufp->chgSData(oldp+6,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_height_reg),10);
        bufp->chgSData(oldp+7,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_stride_reg),16);
        bufp->chgCData(oldp+8,(vlSelf->tb_axi_periph__DOT__dut__DOT__video_scaler_slot_reg),3);
        bufp->chgBit(oldp+9,(vlSelf->tb_axi_periph__DOT__dut__DOT__pal_wr));
        bufp->chgCData(oldp+10,(vlSelf->tb_axi_periph__DOT__dut__DOT__pal_addr),8);
        bufp->chgIData(oldp+11,(vlSelf->tb_axi_periph__DOT__dut__DOT__pal_data),24);
        bufp->chgBit(oldp+12,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_read));
        bufp->chgBit(oldp+13,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_write));
        bufp->chgBit(oldp+14,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_openfile));
        bufp->chgBit(oldp+15,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_getfile));
        bufp->chgBit(oldp+16,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_flush));
        bufp->chgSData(oldp+17,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_id),16);
        bufp->chgIData(oldp+18,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_slotoffset),32);
        bufp->chgIData(oldp+19,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_bridgeaddr),32);
        bufp->chgIData(oldp+20,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_dataslot_length),32);
        bufp->chgIData(oldp+21,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_buffer_param_struct),32);
        bufp->chgIData(oldp+22,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_buffer_resp_struct),32);
        bufp->chgSData(oldp+23,(vlSelf->tb_axi_periph__DOT__dut__DOT__save_dt_word),10);
        bufp->chgBit(oldp+24,(vlSelf->tb_axi_periph__DOT__dut__DOT__save_dt_word_mode));
        bufp->chgBit(oldp+25,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_dv));
        bufp->chgCData(oldp+26,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_byte),8);
        bufp->chgBit(oldp+27,(vlSelf->tb_axi_periph__DOT__dut__DOT__shutdown_ack));
        bufp->chgBit(oldp+28,(((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_enable) 
                               & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_irq_pending))));
        bufp->chgBit(oldp+29,((1U & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_empty)))));
        bufp->chgSData(oldp+30,(vlSelf->tb_axi_periph__DOT__dut__DOT__dt_query_addr),10);
        bufp->chgBit(oldp+31,(vlSelf->tb_axi_periph__DOT__dut__DOT__dt_query_toggle));
        bufp->chgBit(oldp+32,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_pending));
        bufp->chgBit(oldp+33,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_en_reg));
        bufp->chgCData(oldp+34,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_gpio_out_reg),8);
        bufp->chgCData(oldp+35,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_gpio_dir_reg),8);
        bufp->chgBit(oldp+36,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_enable_reg));
        bufp->chgBit(oldp+37,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_fast_reg));
        bufp->chgBit(oldp+38,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_clear_edges_pulse));
        bufp->chgCData(oldp+39,((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_cmd_o) 
                                  << 7U) | ((0x40U 
                                             & (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psCLK_gate) 
                                                 | ((~ 
                                                     ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1) 
                                                      ^ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2))) 
                                                    | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK))) 
                                                << 6U)) 
                                            | (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2) 
                                                << 1U) 
                                               | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1))))),8);
        bufp->chgSData(oldp+40,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key1),16);
        bufp->chgSData(oldp+41,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_pressed_acc),16);
        bufp->chgSData(oldp+42,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_released_acc),16);
        bufp->chgSData(oldp+43,((0xff00U & ((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                             - (IData)(0x80U)) 
                                            << 8U))),16);
        bufp->chgSData(oldp+44,((0xff00U & (((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                              >> 8U) 
                                             - (IData)(0x80U)) 
                                            << 8U))),16);
        bufp->chgSData(oldp+45,((0xff00U & (((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                              >> 0x10U) 
                                             - (IData)(0x80U)) 
                                            << 8U))),16);
        bufp->chgSData(oldp+46,((0xff00U & (((vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1 
                                              >> 0x18U) 
                                             - (IData)(0x80U)) 
                                            << 8U))),16);
        bufp->chgCData(oldp+47,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_meta),8);
        bufp->chgCData(oldp+48,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync),8);
        bufp->chgBit(oldp+49,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                                     >> 2U))));
        bufp->chgBit(oldp+50,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                                     >> 5U))));
        bufp->chgSData(oldp+51,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_stb_cnt),9);
        bufp->chgBit(oldp+52,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_stb));
        bufp->chgSData(oldp+53,(((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_fast_reg)
                                  ? 0xc7U : 0x18fU)),9);
        bufp->chgSData(oldp+54,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key2),16);
        bufp->chgIData(oldp+55,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy1),32);
        bufp->chgIData(oldp+56,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy2),32);
        bufp->chgBit(oldp+57,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psCLK_gate) 
                                     | ((~ ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1) 
                                            ^ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2))) 
                                        | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK))))));
        bufp->chgBit(oldp+58,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_cmd_o));
        bufp->chgBit(oldp+59,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT1));
        bufp->chgBit(oldp+60,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psATT2));
        bufp->chgSData(oldp+61,(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_prev_key1),16);
        bufp->chgQData(oldp+62,((((QData)((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_key2)) 
                                  << 0x20U) | (QData)((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__psx_joy2)))),48);
        bufp->chgQData(oldp+64,(vlSelf->tb_axi_periph__DOT__dut__DOT__cycle_counter),64);
        bufp->chgCData(oldp+66,(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_color_mode_reg),3);
        bufp->chgSData(oldp+67,(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_width_reg),10);
        bufp->chgSData(oldp+68,(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_height_reg),10);
        bufp->chgSData(oldp+69,(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_fb_stride_reg),16);
        bufp->chgSData(oldp+70,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_id_reg),16);
        bufp->chgIData(oldp+71,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_slot_offset_reg),32);
        bufp->chgIData(oldp+72,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_bridge_addr_reg),32);
        bufp->chgIData(oldp+73,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_length_reg),32);
        bufp->chgIData(oldp+74,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_param_addr_reg),32);
        bufp->chgIData(oldp+75,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_resp_addr_reg),32);
        bufp->chgCData(oldp+76,(vlSelf->tb_axi_periph__DOT__dut__DOT__pal_index_reg),8);
        bufp->chgIData(oldp+77,(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_period),32);
        bufp->chgIData(oldp+78,(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_counter),32);
        bufp->chgBit(oldp+79,(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_enable));
        bufp->chgBit(oldp+80,(vlSelf->tb_axi_periph__DOT__dut__DOT__timer_irq_pending));
        bufp->chgCData(oldp+81,(vlSelf->tb_axi_periph__DOT__dut__DOT__irq_mask),6);
        bufp->chgBit(oldp+82,(vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_irq_pending));
        bufp->chgBit(oldp+83,(vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_irq_pending));
        bufp->chgBit(oldp+84,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_pending));
        bufp->chgCData(oldp+85,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_display_idx),2);
        bufp->chgCData(oldp+86,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_ready_idx),2);
        bufp->chgBit(oldp+87,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_swap_consumed_this_frame));
        bufp->chgBit(oldp+88,(vlSelf->tb_axi_periph__DOT__dut__DOT__term_fb_active));
        bufp->chgBit(oldp+89,(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_keep_app));
        bufp->chgSData(oldp+90,(vlSelf->tb_axi_periph__DOT__dut__DOT__vrr_v_total_reg),10);
        bufp->chgIData(oldp+91,(vlSelf->tb_axi_periph__DOT__dut__DOT__fb_app_addr),25);
        bufp->chgBit(oldp+92,(vlSelf->tb_axi_periph__DOT__dut__DOT__analog_show_term));
        bufp->chgCData(oldp+93,(vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_allcomplete_sync),3);
        bufp->chgBit(oldp+94,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__dataslot_allcomplete_sync) 
                                     >> 2U))));
        bufp->chgIData(oldp+95,(vlSelf->tb_axi_periph__DOT__dut__DOT__bridge_dbg_wcnt_meta),32);
        bufp->chgIData(oldp+96,(vlSelf->tb_axi_periph__DOT__dut__DOT__bridge_dbg_wcnt_s),32);
        bufp->chgCData(oldp+97,(vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_sync),3);
        bufp->chgBit(oldp+98,(vlSelf->tb_axi_periph__DOT__dut__DOT__vsync_rising));
        bufp->chgCData(oldp+99,(vlSelf->tb_axi_periph__DOT__dut__DOT__early_vblank_sync),3);
        bufp->chgBit(oldp+100,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__early_vblank_sync) 
                                      >> 2U))));
        bufp->chgCData(oldp+101,(vlSelf->tb_axi_periph__DOT__dut__DOT__os_inmenu_sync),3);
        bufp->chgBit(oldp+102,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__os_inmenu_sync) 
                                      >> 2U))));
        bufp->chgSData(oldp+103,(vlSelf->tb_axi_periph__DOT__dut__DOT__clamp_fb_stride__Vstatic__even_stride),16);
        bufp->chgBit(oldp+104,(vlSelf->tb_axi_periph__DOT__dut__DOT__rtc_valid_meta));
        bufp->chgBit(oldp+105,(vlSelf->tb_axi_periph__DOT__dut__DOT__rtc_valid_s));
        bufp->chgIData(oldp+106,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_meta),32);
        bufp->chgIData(oldp+107,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_s),32);
        bufp->chgIData(oldp+108,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_meta),32);
        bufp->chgIData(oldp+109,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_s),32);
        bufp->chgSData(oldp+110,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_meta),16);
        bufp->chgSData(oldp+111,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_s),16);
        bufp->chgIData(oldp+112,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_meta),32);
        bufp->chgIData(oldp+113,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_s),32);
        bufp->chgIData(oldp+114,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_meta),32);
        bufp->chgIData(oldp+115,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_s),32);
        bufp->chgSData(oldp+116,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_meta),16);
        bufp->chgSData(oldp+117,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_s),16);
        bufp->chgIData(oldp+118,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_meta),32);
        bufp->chgIData(oldp+119,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_s),32);
        bufp->chgIData(oldp+120,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_meta),32);
        bufp->chgIData(oldp+121,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_s),32);
        bufp->chgSData(oldp+122,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_meta),16);
        bufp->chgSData(oldp+123,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_s),16);
        bufp->chgIData(oldp+124,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_meta),32);
        bufp->chgIData(oldp+125,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_s),32);
        bufp->chgIData(oldp+126,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_meta),32);
        bufp->chgIData(oldp+127,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_s),32);
        bufp->chgSData(oldp+128,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_meta),16);
        bufp->chgSData(oldp+129,(vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_s),16);
        bufp->chgCData(oldp+130,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_irq_mask),4);
        bufp->chgBit(oldp+131,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_overflow));
        bufp->chgSData(oldp+132,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_seq),16);
        bufp->chgIData(oldp+133,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key0),32);
        bufp->chgIData(oldp+134,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key1),32);
        bufp->chgIData(oldp+135,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key2),32);
        bufp->chgIData(oldp+136,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key3),32);
        bufp->chgIData(oldp+137,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy0),32);
        bufp->chgIData(oldp+138,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy1),32);
        bufp->chgIData(oldp+139,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy2),32);
        bufp->chgIData(oldp+140,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy3),32);
        bufp->chgSData(oldp+141,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig0),16);
        bufp->chgSData(oldp+142,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig1),16);
        bufp->chgSData(oldp+143,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig2),16);
        bufp->chgSData(oldp+144,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig3),16);
        bufp->chgCData(oldp+145,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot),2);
        bufp->chgCData(oldp+146,(((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_trig_s) 
                                    != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig0)) 
                                   << 2U) | (((vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_joy_s 
                                               != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy0) 
                                              << 1U) 
                                             | (vlSelf->tb_axi_periph__DOT__dut__DOT__cont1_key_s 
                                                != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key0)))),3);
        bufp->chgCData(oldp+147,(((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_trig_s) 
                                    != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig1)) 
                                   << 2U) | (((vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_joy_s 
                                               != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy1) 
                                              << 1U) 
                                             | (vlSelf->tb_axi_periph__DOT__dut__DOT__cont2_key_s 
                                                != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key1)))),3);
        bufp->chgCData(oldp+148,(((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_trig_s) 
                                    != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig2)) 
                                   << 2U) | (((vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_joy_s 
                                               != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy2) 
                                              << 1U) 
                                             | (vlSelf->tb_axi_periph__DOT__dut__DOT__cont3_key_s 
                                                != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key2)))),3);
        bufp->chgCData(oldp+149,(((((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_trig_s) 
                                    != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_trig3)) 
                                   << 2U) | (((vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_joy_s 
                                               != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_joy3) 
                                              << 1U) 
                                             | (vlSelf->tb_axi_periph__DOT__dut__DOT__cont4_key_s 
                                                != vlSelf->tb_axi_periph__DOT__dut__DOT__input_prev_key3)))),3);
        bufp->chgCData(oldp+150,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fields),3);
        bufp->chgBit(oldp+151,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_valid));
        bufp->chgBit(oldp+152,((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count))));
        bufp->chgBit(oldp+153,((0x10U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count))));
        bufp->chgCData(oldp+154,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count),5);
        bufp->chgQData(oldp+155,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_dout),64);
        bufp->chgQData(oldp+157,((0x100000000000000ULL 
                                  | (((QData)((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_scan_slot)) 
                                      << 0x30U) | (
                                                   ((QData)((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fields)) 
                                                    << 0x28U) 
                                                   | (QData)((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_seq)))))),64);
        bufp->chgBit(oldp+159,(((0x10U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)) 
                                & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_valid))));
        bufp->chgBit(oldp+160,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_clear));
        bufp->chgBit(oldp+161,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_cmd_active));
        bufp->chgBit(oldp+162,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_latched));
        bufp->chgBit(oldp+163,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_latched));
        bufp->chgBit(oldp+164,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_ack_seen_low));
        bufp->chgBit(oldp+165,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_done_seen_low));
        bufp->chgCData(oldp+166,(vlSelf->tb_axi_periph__DOT__dut__DOT__ds_err_latched),3);
        bufp->chgSData(oldp+167,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_wr_ptr),10);
        bufp->chgSData(oldp+168,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr),10);
        bufp->chgSData(oldp+169,((0x3ffU & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_wr_ptr) 
                                            - (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr)))),10);
        bufp->chgBit(oldp+170,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_empty));
        bufp->chgBit(oldp+171,((0x3ffU == (0x3ffU & 
                                           ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_wr_ptr) 
                                            - (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr))))));
        bufp->chgCData(oldp+172,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_fifo
                                 [vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr]),8);
        bufp->chgQData(oldp+173,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[0]),64);
        bufp->chgQData(oldp+175,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[1]),64);
        bufp->chgQData(oldp+177,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[2]),64);
        bufp->chgQData(oldp+179,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[3]),64);
        bufp->chgQData(oldp+181,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[4]),64);
        bufp->chgQData(oldp+183,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[5]),64);
        bufp->chgQData(oldp+185,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[6]),64);
        bufp->chgQData(oldp+187,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[7]),64);
        bufp->chgQData(oldp+189,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[8]),64);
        bufp->chgQData(oldp+191,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[9]),64);
        bufp->chgQData(oldp+193,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[10]),64);
        bufp->chgQData(oldp+195,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[11]),64);
        bufp->chgQData(oldp+197,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[12]),64);
        bufp->chgQData(oldp+199,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[13]),64);
        bufp->chgQData(oldp+201,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[14]),64);
        bufp->chgQData(oldp+203,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__mem[15]),64);
        bufp->chgCData(oldp+205,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__wr_ptr),4);
        bufp->chgCData(oldp+206,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__rd_ptr),4);
        bufp->chgIData(oldp+207,(vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_a_reg2),32);
        bufp->chgBit(oldp+208,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                                      >> 4U))));
        bufp->chgCData(oldp+209,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx0),8);
        bufp->chgCData(oldp+210,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx1),8);
        bufp->chgCData(oldp+211,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx2),8);
        bufp->chgCData(oldp+212,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx3),8);
        bufp->chgCData(oldp+213,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx4),8);
        bufp->chgCData(oldp+214,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx5),8);
        bufp->chgCData(oldp+215,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__rx6),8);
        bufp->chgCData(oldp+216,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__RXD_ID),8);
        bufp->chgBit(oldp+217,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__att1r));
        bufp->chgBit(oldp+218,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__att2r));
        bufp->chgBit(oldp+219,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__no_gamepad));
        bufp->chgBit(oldp+220,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__i_CLK));
        bufp->chgBit(oldp+221,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__R_CE));
        bufp->chgBit(oldp+222,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__F_CE));
        bufp->chgBit(oldp+223,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__ack_r));
        bufp->chgBit(oldp+224,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__device_id_type));
        bufp->chgCData(oldp+225,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r),4);
        bufp->chgBit(oldp+226,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXWT));
        bufp->chgBit(oldp+227,(((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT____VdfgTmp_h6d2737cc__0) 
                                & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXWT))));
        bufp->chgBit(oldp+228,(((~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psCLK_gate)) 
                                & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT____VdfgTmp_h6d2737cc__0))));
        bufp->chgCData(oldp+229,(((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))
                                   ? 1U : ((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__o_byte_cnt_r))
                                            ? 0x42U
                                            : 0U))),8);
        bufp->chgCData(oldp+230,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXD_DAT),8);
        bufp->chgBit(oldp+231,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__W_RXWT_r));
        bufp->chgBit(oldp+232,(((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT____VdfgTmp_h6d2737cc__0) 
                                & (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXSET))));
        bufp->chgSData(oldp+233,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__Timer),11);
        bufp->chgBit(oldp+234,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__RXWT));
        bufp->chgBit(oldp+235,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXWT));
        bufp->chgBit(oldp+236,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__TXSET));
        bufp->chgBit(oldp+237,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__pls__DOT__psCLK_gate));
        bufp->chgCData(oldp+238,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__rxd__DOT__sp),8);
        bufp->chgCData(oldp+239,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_hw_psx__DOT__ds1__DOT__txd__DOT__ps),8);
    }
    if (VL_UNLIKELY(vlSelf->__Vm_traceActivity[2U])) {
        bufp->chgIData(oldp+240,(vlSelf->tb_axi_periph__DOT__mix_pos_readback_r),22);
        bufp->chgBit(oldp+241,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_done));
        bufp->chgIData(oldp+242,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_rx_data),32);
        bufp->chgBit(oldp+243,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_clk));
        bufp->chgBit(oldp+244,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_mosi));
        bufp->chgBit(oldp+245,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift_latch));
        bufp->chgCData(oldp+246,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_ack_sync),3);
        bufp->chgCData(oldp+247,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_done_sync),3);
        bufp->chgCData(oldp+248,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync[0]),3);
        bufp->chgCData(oldp+249,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync[1]),3);
        bufp->chgCData(oldp+250,(vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync[2]),3);
        bufp->chgBit(oldp+251,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_ack_sync) 
                                      >> 2U))));
        bufp->chgBit(oldp+252,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__target_done_sync) 
                                      >> 2U))));
        bufp->chgCData(oldp+253,(((4U & vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync
                                   [2U]) | ((2U & (
                                                   vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync
                                                   [1U] 
                                                   >> 1U)) 
                                            | (1U & 
                                               (vlSelf->tb_axi_periph__DOT__dut__DOT__target_err_sync
                                                [0U] 
                                                >> 2U))))),3);
        bufp->chgSData(oldp+254,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__div_cnt),16);
        bufp->chgCData(oldp+255,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__bits_left),5);
        bufp->chgCData(oldp+256,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__total_bits),5);
        bufp->chgIData(oldp+257,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__tx_sr),32);
        bufp->chgBit(oldp+258,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__miso_sel));
    }
    if (VL_UNLIKELY(vlSelf->__Vm_traceActivity[3U])) {
        bufp->chgBit(oldp+259,(vlSelf->tb_axi_periph__DOT__pal_commit_w));
        bufp->chgCData(oldp+260,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_mode_reg),2);
        bufp->chgSData(oldp+261,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_div_reg),16);
        bufp->chgIData(oldp+262,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_tx_reg),32);
        bufp->chgBit(oldp+263,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_start_pulse));
        bufp->chgCData(oldp+264,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_bit_count_reg),5);
        bufp->chgBit(oldp+265,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_latch_en_reg));
    }
    if (VL_UNLIKELY(vlSelf->__Vm_traceActivity[4U])) {
        bufp->chgIData(oldp+266,(vlSelf->tb_axi_periph__DOT__gpu_reg_rdata),32);
        bufp->chgCData(oldp+267,((0x1fU & ((vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                            >> 2U) 
                                           - (IData)(8U)))),5);
        bufp->chgIData(oldp+268,(vlSelf->tb_axi_periph__DOT__dut__DOT__req_wdata),32);
        bufp->chgIData(oldp+269,(vlSelf->tb_axi_periph__DOT__cart_reg_rdata),32);
        bufp->chgCData(oldp+270,((0x1fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                           >> 2U))),5);
        bufp->chgBit(oldp+271,(vlSelf->tb_axi_periph__DOT__dut__DOT__audio_sample_wr));
        bufp->chgIData(oldp+272,(vlSelf->tb_axi_periph__DOT__dut__DOT__audio_sample_data),32);
        bufp->chgBit(oldp+273,(vlSelf->tb_axi_periph__DOT__dut__DOT__cram0_mode));
        bufp->chgBit(oldp+274,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_enable));
        bufp->chgBit(oldp+275,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_wr));
        bufp->chgCData(oldp+276,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_sel),5);
        bufp->chgCData(oldp+277,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_field),4);
        bufp->chgIData(oldp+278,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_wdata),32);
        bufp->chgBit(oldp+279,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_irq_clear_wr));
        bufp->chgIData(oldp+280,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_irq_clear),32);
        bufp->chgCData(oldp+281,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_master_vol),8);
        bufp->chgCData(oldp+282,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_0),8);
        bufp->chgCData(oldp+283,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_1),8);
        bufp->chgCData(oldp+284,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_2),8);
        bufp->chgCData(oldp+285,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_3),8);
        bufp->chgQData(oldp+286,(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_group_packed),64);
        bufp->chgBit(oldp+288,(vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_wr));
        bufp->chgBit(oldp+289,(vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_rd));
        bufp->chgCData(oldp+290,(vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_addr),5);
        bufp->chgIData(oldp+291,(vlSelf->tb_axi_periph__DOT__dut__DOT__link_reg_wdata),32);
        bufp->chgBit(oldp+292,((0U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state))));
        bufp->chgBit(oldp+293,(((6U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state)) 
                                & (0U != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_wstrb)))));
        bufp->chgBit(oldp+294,(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_wr_fire));
        bufp->chgBit(oldp+295,((1U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                      >> 8U))));
        bufp->chgCData(oldp+296,((0x3fU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                           >> 2U))),6);
        bufp->chgBit(oldp+297,(((vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                 >> 8U) & ((1U <= (0xfU 
                                                   & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                      >> 4U))) 
                                           & (4U >= 
                                              (0xfU 
                                               & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                  >> 4U)))))));
        bufp->chgBit(oldp+298,(vlSelf->tb_axi_periph__DOT__dut__DOT__low_page_cont_read));
        bufp->chgCData(oldp+299,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_sel),4);
        bufp->chgCData(oldp+300,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_field_sel),2);
        bufp->chgBit(oldp+301,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_fifo_we));
        bufp->chgCData(oldp+302,(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_tx_fifo_din),8);
        bufp->chgIData(oldp+303,(((0U == (3U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                >> 2U)))
                                   ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__cram0_mode)
                                   : 0U)),32);
        bufp->chgBit(oldp+304,((IData)((0x880U == (0x880U 
                                                   & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)))));
        bufp->chgIData(oldp+305,(((0x40U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                   ? 0U : ((0x20U & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                            ? ((0x10U 
                                                & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                ? (
                                                   (8U 
                                                    & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                    ? 0U
                                                    : 
                                                   ((4U 
                                                     & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                     ? 0U
                                                     : 0xa5a51234U))
                                                : (
                                                   (8U 
                                                    & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                    ? 0U
                                                    : 
                                                   ((4U 
                                                     & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                     ? 0xc0U
                                                     : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_enable))))
                                            : ((0x10U 
                                                & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                ? (
                                                   (8U 
                                                    & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                    ? 
                                                   ((4U 
                                                     & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                     ? 0U
                                                     : (IData)(
                                                               (vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_group_packed 
                                                                >> 0x20U)))
                                                    : 
                                                   ((4U 
                                                     & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                     ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_voice_group_packed)
                                                     : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_3)))
                                                : (
                                                   (8U 
                                                    & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                    ? 
                                                   ((4U 
                                                     & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                     ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_2)
                                                     : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_1))
                                                    : 
                                                   ((4U 
                                                     & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)
                                                     ? (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_group_vol_0)
                                                     : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__mix_master_vol))))))),32);
        bufp->chgIData(oldp+306,(vlSelf->tb_axi_periph__DOT__dut__DOT__mixer_rdata),32);
        bufp->chgIData(oldp+307,(vlSelf->tb_axi_periph__DOT__dut__DOT__periph_rd_data_r),32);
        bufp->chgCData(oldp+308,(vlSelf->tb_axi_periph__DOT__dut__DOT__state),3);
        bufp->chgIData(oldp+309,(vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr),32);
        bufp->chgCData(oldp+310,(vlSelf->tb_axi_periph__DOT__dut__DOT__req_wstrb),4);
        bufp->chgCData(oldp+311,(vlSelf->tb_axi_periph__DOT__dut__DOT__burst_len),8);
        bufp->chgCData(oldp+312,(vlSelf->tb_axi_periph__DOT__dut__DOT__burst_count),8);
        bufp->chgCData(oldp+313,(vlSelf->tb_axi_periph__DOT__dut__DOT__awburst_latched),2);
        bufp->chgBit(oldp+314,((0U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__awburst_latched))));
        bufp->chgCData(oldp+315,(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region),4);
        bufp->chgBit(oldp+316,((1U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
        bufp->chgBit(oldp+317,((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
        bufp->chgBit(oldp+318,((3U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
        bufp->chgBit(oldp+319,((4U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
        bufp->chgBit(oldp+320,((5U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
        bufp->chgBit(oldp+321,((6U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
        bufp->chgBit(oldp+322,((7U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
        bufp->chgBit(oldp+323,((8U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
        bufp->chgBit(oldp+324,((9U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region))));
        bufp->chgIData(oldp+325,(((0U == (0xfU & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                  >> 2U)))
                                   ? 0x125U : ((1U 
                                                == 
                                                (0xfU 
                                                 & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                    >> 2U)))
                                                ? 0x33334444U
                                                : (
                                                   (2U 
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
                                                           : 0U))))))))))),32);
        bufp->chgBit(oldp+326,(vlSelf->tb_axi_periph__DOT__dut__DOT__beat_is_last));
        bufp->chgIData(oldp+327,(vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr_plus4),32);
        bufp->chgIData(oldp+328,(vlSelf->tb_axi_periph__DOT__dut__DOT__periph_next_addr),32);
        bufp->chgSData(oldp+329,((0x1fffU & ((IData)(1U) 
                                             + (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                >> 2U)))),13);
        bufp->chgCData(oldp+330,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__state),3);
    }
    if (VL_UNLIKELY(vlSelf->__Vm_traceActivity[5U])) {
        bufp->chgCData(oldp+331,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_out_mux),8);
        bufp->chgCData(oldp+332,(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_dir_mux),8);
        bufp->chgIData(oldp+333,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_key_rdata),32);
        bufp->chgIData(oldp+334,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_joy_rdata),32);
        bufp->chgSData(oldp+335,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_trig_rdata),16);
        bufp->chgBit(oldp+336,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_irq_en_rdata));
        bufp->chgIData(oldp+337,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_slot_rdata),32);
    }
    bufp->chgBit(oldp+338,(vlSelf->clk));
    bufp->chgBit(oldp+339,(vlSelf->reset_n));
    bufp->chgBit(oldp+340,(vlSelf->s_axi_arvalid));
    bufp->chgBit(oldp+341,(vlSelf->s_axi_arready));
    bufp->chgIData(oldp+342,(vlSelf->s_axi_araddr),32);
    bufp->chgCData(oldp+343,(vlSelf->s_axi_arlen),8);
    bufp->chgBit(oldp+344,(vlSelf->s_axi_rvalid));
    bufp->chgBit(oldp+345,(vlSelf->s_axi_rready));
    bufp->chgIData(oldp+346,(vlSelf->s_axi_rdata),32);
    bufp->chgCData(oldp+347,(vlSelf->s_axi_rresp),2);
    bufp->chgBit(oldp+348,(vlSelf->s_axi_rlast));
    bufp->chgBit(oldp+349,(vlSelf->s_axi_awvalid));
    bufp->chgBit(oldp+350,(vlSelf->s_axi_awready));
    bufp->chgIData(oldp+351,(vlSelf->s_axi_awaddr),32);
    bufp->chgCData(oldp+352,(vlSelf->s_axi_awlen),8);
    bufp->chgCData(oldp+353,(vlSelf->s_axi_awburst),2);
    bufp->chgBit(oldp+354,(vlSelf->s_axi_wvalid));
    bufp->chgBit(oldp+355,(vlSelf->s_axi_wready));
    bufp->chgIData(oldp+356,(vlSelf->s_axi_wdata),32);
    bufp->chgCData(oldp+357,(vlSelf->s_axi_wstrb),4);
    bufp->chgBit(oldp+358,(vlSelf->s_axi_wlast));
    bufp->chgBit(oldp+359,(vlSelf->s_axi_bvalid));
    bufp->chgBit(oldp+360,(vlSelf->s_axi_bready));
    bufp->chgCData(oldp+361,(vlSelf->s_axi_bresp),2);
    bufp->chgCData(oldp+362,(vlSelf->dbg_state),4);
    bufp->chgBit(oldp+363,(vlSelf->dbg_bram_hold));
    bufp->chgBit(oldp+364,(vlSelf->dbg_can_push_beat));
    bufp->chgBit(oldp+365,(vlSelf->dbg_gpu_reg_wr));
    bufp->chgCData(oldp+366,(vlSelf->dbg_gpu_reg_addr),4);
    bufp->chgIData(oldp+367,(vlSelf->dbg_gpu_reg_wdata),32);
    bufp->chgCData(oldp+368,(vlSelf->dbg_save_dt_slot),4);
    bufp->chgIData(oldp+369,(vlSelf->dbg_save_dt_size),32);
    bufp->chgBit(oldp+370,(vlSelf->dbg_save_dt_commit));
    bufp->chgCData(oldp+371,(vlSelf->dbg_analogizer_wr_toggle),3);
    bufp->chgIData(oldp+372,(vlSelf->dbg_analogizer_wr_settings),32);
    bufp->chgIData(oldp+373,(vlSelf->dbg_analogizer_wr_hoffset),32);
    bufp->chgIData(oldp+374,(vlSelf->dbg_analogizer_wr_voffset),32);
    bufp->chgCData(oldp+375,(vlSelf->dbg_snac_pin_out),8);
    bufp->chgCData(oldp+376,(vlSelf->dbg_snac_pin_dir),8);
    bufp->chgBit(oldp+377,(vlSelf->dbg_snac_enable));
    bufp->chgCData(oldp+378,(vlSelf->dbg_cart_wr_count),8);
    bufp->chgCData(oldp+379,(vlSelf->dbg_cart_rd_count),8);
    bufp->chgCData(oldp+380,(vlSelf->dbg_cart_last_addr),5);
    bufp->chgIData(oldp+381,(vlSelf->dbg_cart_last_wdata),32);
    bufp->chgBit(oldp+382,(vlSelf->dbg_pal_commit));
    bufp->chgIData(oldp+383,(vlSelf->dbg_analog_fb_addr),25);
    bufp->chgCData(oldp+384,(vlSelf->dbg_analog_color_mode),3);
    bufp->chgSData(oldp+385,(vlSelf->dbg_analog_fb_width),10);
    bufp->chgSData(oldp+386,(vlSelf->dbg_analog_fb_height),10);
    bufp->chgSData(oldp+387,(vlSelf->dbg_analog_fb_stride),16);
    bufp->chgCData(oldp+388,(vlSelf->dbg_pal_commit_count),8);
    bufp->chgBit(oldp+389,(vlSelf->pal_busy_in));
    bufp->chgIData(oldp+390,(vlSelf->analogizer_settings_in),32);
    bufp->chgBit(oldp+391,(vlSelf->gpu_swap_req_in));
    bufp->chgCData(oldp+392,(vlSelf->gpu_swap_idx_in),2);
    bufp->chgBit(oldp+393,(vlSelf->vsync_in));
    bufp->chgBit(oldp+394,(vlSelf->early_vblank_in));
    bufp->chgBit(oldp+395,(vlSelf->os_inmenu_in));
    bufp->chgBit(oldp+396,(vlSelf->target_dataslot_ack_in));
    bufp->chgBit(oldp+397,(vlSelf->target_dataslot_done_in));
    bufp->chgCData(oldp+398,(vlSelf->target_dataslot_err_in),3);
    bufp->chgBit(oldp+399,(vlSelf->bridge_wr_idle_in));
    bufp->chgIData(oldp+400,(vlSelf->cont1_key_in),32);
    bufp->chgIData(oldp+401,(vlSelf->cont1_joy_in),32);
    bufp->chgSData(oldp+402,(vlSelf->cont1_trig_in),16);
    bufp->chgIData(oldp+403,(vlSelf->cont2_key_in),32);
    bufp->chgIData(oldp+404,(vlSelf->cont2_joy_in),32);
    bufp->chgSData(oldp+405,(vlSelf->cont2_trig_in),16);
    bufp->chgIData(oldp+406,(vlSelf->cont3_key_in),32);
    bufp->chgIData(oldp+407,(vlSelf->cont3_joy_in),32);
    bufp->chgSData(oldp+408,(vlSelf->cont3_trig_in),16);
    bufp->chgIData(oldp+409,(vlSelf->cont4_key_in),32);
    bufp->chgIData(oldp+410,(vlSelf->cont4_joy_in),32);
    bufp->chgSData(oldp+411,(vlSelf->cont4_trig_in),16);
    bufp->chgIData(oldp+412,(vlSelf->dt_query_data_in),32);
    bufp->chgBit(oldp+413,(vlSelf->dt_query_valid_in));
    bufp->chgCData(oldp+414,(vlSelf->snac_pin_in_in),8);
    bufp->chgBit(oldp+415,(vlSelf->ext_irq_out));
    bufp->chgSData(oldp+416,(((1U & (IData)(((0U != 
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
                               : (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__vrr_v_total_reg))),10);
    bufp->chgBit(oldp+417,((1U & (~ (IData)(vlSelf->reset_n)))));
    bufp->chgIData(oldp+418,(vlSelf->tb_axi_periph__DOT__dut__DOT__ram__DOT__q_a_reg),32);
    bufp->chgSData(oldp+419,(vlSelf->tb_axi_periph__DOT__dut__DOT__ram_addr_mux),13);
    bufp->chgIData(oldp+420,(vlSelf->tb_axi_periph__DOT__dut__DOT__sysreg_rdata),32);
    bufp->chgBit(oldp+421,((1U & (vlSelf->analogizer_settings_in 
                                  >> 0xfU))));
    bufp->chgCData(oldp+422,((0xfU & (vlSelf->analogizer_settings_in 
                                      >> 0xaU))),4);
    bufp->chgBit(oldp+423,(((4U == (0xfU & (vlSelf->analogizer_settings_in 
                                            >> 0xaU))) 
                            | (0xcU == (0xfU & (vlSelf->analogizer_settings_in 
                                                >> 0xaU))))));
    bufp->chgBit(oldp+424,((0U != (0x1fU & vlSelf->analogizer_settings_in))));
    bufp->chgBit(oldp+425,((1U & (IData)(((0U != (0x801fU 
                                                  & vlSelf->analogizer_settings_in)) 
                                          | (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_en_reg))))));
    bufp->chgSData(oldp+426,((((vlSelf->analogizer_settings_in 
                                >> 0xfU) & ((4U == 
                                             (0xfU 
                                              & (vlSelf->analogizer_settings_in 
                                                 >> 0xaU))) 
                                            | (0xcU 
                                               == (0xfU 
                                                   & (vlSelf->analogizer_settings_in 
                                                      >> 0xaU)))))
                               ? 0x276U : 0x20dU)),10);
    bufp->chgBit(oldp+427,(((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__state)) 
                            & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat) 
                               & ((2U == (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__req_region)) 
                                  & (IData)(((0x154U 
                                              == (0x1fcU 
                                                  & vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr)) 
                                             & (0U 
                                                != (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__input_fifo_count)))))))));
    bufp->chgIData(oldp+428,(((0U == (3U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                            >> 2U)))
                               ? (0xbU | ((0x3ff00U 
                                           & (((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_wr_ptr) 
                                               - (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr)) 
                                              << 8U)) 
                                          | (4U & (
                                                   (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_empty)) 
                                                   << 2U))))
                               : ((2U == (3U & (vlSelf->tb_axi_periph__DOT__dut__DOT__req_addr 
                                                >> 2U)))
                                   ? vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_fifo
                                  [vlSelf->tb_axi_periph__DOT__dut__DOT__uart_rx_rd_ptr]
                                   : 0U))),32);
    bufp->chgBit(oldp+429,(vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat));
    bufp->chgBit(oldp+430,((1U & (~ (IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__can_push_beat)))));
    bufp->chgCData(oldp+431,(vlSelf->tb_axi_periph__DOT__dut__DOT__ar_region),4);
    bufp->chgCData(oldp+432,(vlSelf->tb_axi_periph__DOT__dut__DOT__aw_region),4);
    bufp->chgBit(oldp+433,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_pop));
    bufp->chgBit(oldp+434,(vlSelf->tb_axi_periph__DOT__dut__DOT__input_event_fifo__DOT__do_push));
    bufp->chgBit(oldp+435,((1U & ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_shift__DOT__miso_sel)
                                   ? ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                                      >> 5U) : ((IData)(vlSelf->tb_axi_periph__DOT__dut__DOT__snac_pin_in_sync) 
                                                >> 2U)))));
}

void Vtb_axi_periph___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_axi_periph___024root__trace_cleanup\n"); );
    // Init
    Vtb_axi_periph___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vtb_axi_periph___024root*>(voidSelf);
    Vtb_axi_periph__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    vlSymsp->__Vm_activity = false;
    vlSymsp->TOP.__Vm_traceActivity[0U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[1U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[2U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[3U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[4U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[5U] = 0U;
}
