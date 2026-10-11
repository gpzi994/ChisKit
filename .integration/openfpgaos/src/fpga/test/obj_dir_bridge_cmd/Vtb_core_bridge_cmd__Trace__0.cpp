// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vtb_core_bridge_cmd__Syms.h"


void Vtb_core_bridge_cmd___024root__trace_chg_0_sub_0(Vtb_core_bridge_cmd___024root* vlSelf, VerilatedVcd::Buffer* bufp);

void Vtb_core_bridge_cmd___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root__trace_chg_0\n"); );
    // Init
    Vtb_core_bridge_cmd___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vtb_core_bridge_cmd___024root*>(voidSelf);
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (VL_UNLIKELY(!vlSymsp->__Vm_activity)) return;
    // Body
    Vtb_core_bridge_cmd___024root__trace_chg_0_sub_0((&vlSymsp->TOP), bufp);
}

void Vtb_core_bridge_cmd___024root__trace_chg_0_sub_0(Vtb_core_bridge_cmd___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    if (false && vlSelf) {}  // Prevent unused
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root__trace_chg_0_sub_0\n"); );
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode + 1);
    // Body
    if (VL_UNLIKELY(vlSelf->__Vm_traceActivity[0U])) {
        bufp->chgIData(oldp+0,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__i),32);
    }
    if (VL_UNLIKELY(vlSelf->__Vm_traceActivity[1U])) {
        bufp->chgBit(oldp+1,(vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestread));
        bufp->chgSData(oldp+2,(vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestread_id),16);
        bufp->chgBit(oldp+3,(vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestwrite));
        bufp->chgSData(oldp+4,(vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestwrite_id),16);
        bufp->chgIData(oldp+5,(vlSelf->tb_core_bridge_cmd__DOT__dataslot_requestwrite_size),32);
        bufp->chgBit(oldp+6,(vlSelf->tb_core_bridge_cmd__DOT__dataslot_update));
        bufp->chgSData(oldp+7,(vlSelf->tb_core_bridge_cmd__DOT__dataslot_update_id),16);
        bufp->chgIData(oldp+8,(vlSelf->tb_core_bridge_cmd__DOT__dataslot_update_size),32);
        bufp->chgBit(oldp+9,(vlSelf->tb_core_bridge_cmd__DOT__dataslot_allcomplete));
        bufp->chgIData(oldp+10,(vlSelf->tb_core_bridge_cmd__DOT__rtc_epoch_seconds),32);
        bufp->chgIData(oldp+11,(vlSelf->tb_core_bridge_cmd__DOT__rtc_date_bcd),32);
        bufp->chgIData(oldp+12,(vlSelf->tb_core_bridge_cmd__DOT__rtc_time_bcd),32);
        bufp->chgBit(oldp+13,(vlSelf->tb_core_bridge_cmd__DOT__rtc_valid));
        bufp->chgBit(oldp+14,(vlSelf->tb_core_bridge_cmd__DOT__osnotify_inmenu));
        bufp->chgBit(oldp+15,(vlSelf->tb_core_bridge_cmd__DOT__savestate_start));
        bufp->chgBit(oldp+16,(vlSelf->tb_core_bridge_cmd__DOT__savestate_load));
        bufp->chgBit(oldp+17,(vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_ack));
        bufp->chgBit(oldp+18,(vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_done));
        bufp->chgCData(oldp+19,(vlSelf->tb_core_bridge_cmd__DOT__target_dataslot_err),3);
        bufp->chgIData(oldp+20,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_a_reg),32);
        bufp->chgBit(oldp+21,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_toggle));
        bufp->chgIData(oldp+22,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_value),32);
        bufp->chgBit(oldp+23,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_valid));
        bufp->chgIData(oldp+24,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_last_value),32);
        bufp->chgCData(oldp+25,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__osnotify_cart_count),8);
        bufp->chgBit(oldp+26,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_enter_toggle));
        bufp->chgBit(oldp+27,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_exit_toggle));
        bufp->chgCData(oldp+28,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_enter_count),8);
        bufp->chgCData(oldp+29,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__reset_exit_count),8);
        bufp->chgIData(oldp+30,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_rd_data_out),32);
        bufp->chgBit(oldp+31,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__endian_little_s));
        bufp->chgIData(oldp+32,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_0),32);
        bufp->chgIData(oldp+33,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_20),32);
        bufp->chgIData(oldp+34,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_24),32);
        bufp->chgIData(oldp+35,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_28),32);
        bufp->chgIData(oldp+36,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_40),32);
        bufp->chgIData(oldp+37,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_44),32);
        bufp->chgIData(oldp+38,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_48),32);
        bufp->chgBit(oldp+39,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_start));
        bufp->chgSData(oldp+40,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd_startval),16);
        bufp->chgSData(oldp+41,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_cmd),16);
        bufp->chgSData(oldp+42,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__host_resultcode),16);
        bufp->chgCData(oldp+43,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__hstate),4);
        bufp->chgIData(oldp+44,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_0),32);
        bufp->chgIData(oldp+45,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_4),32);
        bufp->chgIData(oldp+46,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_8),32);
        bufp->chgIData(oldp+47,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_20),32);
        bufp->chgIData(oldp+48,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_24),32);
        bufp->chgIData(oldp+49,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_28),32);
        bufp->chgIData(oldp+50,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_2C),32);
        bufp->chgCData(oldp+51,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__tstate),4);
        bufp->chgIData(oldp+52,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__shutdown_timeout),23);
        bufp->chgBit(oldp+53,((1U & (vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__shutdown_timeout 
                                     >> 0x16U))));
        bufp->chgBit(oldp+54,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_1));
        bufp->chgBit(oldp+55,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__status_setup_done_queue));
        bufp->chgBit(oldp+56,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__warm_reset_ready_pending));
        bufp->chgBit(oldp+57,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_1));
        bufp->chgBit(oldp+58,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_read_queue));
        bufp->chgBit(oldp+59,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_1));
        bufp->chgBit(oldp+60,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_write_queue));
        bufp->chgBit(oldp+61,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_1));
        bufp->chgBit(oldp+62,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_getfile_queue));
        bufp->chgBit(oldp+63,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_1));
        bufp->chgBit(oldp+64,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_openfile_queue));
        bufp->chgBit(oldp+65,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_1));
        bufp->chgBit(oldp+66,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__target_dataslot_flush_queue));
        bufp->chgIData(oldp+67,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_b_reg),32);
        bufp->chgSData(oldp+68,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_addr),10);
        bufp->chgBit(oldp+69,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_wren));
        bufp->chgBit(oldp+70,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active));
        bufp->chgCData(oldp+71,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_addr),5);
        bufp->chgIData(oldp+72,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_a_reg2),32);
        bufp->chgIData(oldp+73,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__idt__DOT__altsyncram_component__DOT__q_b_reg2),32);
        bufp->chgBit(oldp+74,(((~ (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_3)) 
                               & (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__endian_little_s))));
        bufp->chgBit(oldp+75,(((~ (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__endian_little_s)) 
                               & (IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_3))));
        bufp->chgBit(oldp+76,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_1));
        bufp->chgBit(oldp+77,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_2));
        bufp->chgBit(oldp+78,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__s01__DOT__stage_3));
    }
    bufp->chgBit(oldp+79,(vlSelf->clk));
    bufp->chgBit(oldp+80,(vlSelf->bridge_endian_little));
    bufp->chgIData(oldp+81,(vlSelf->bridge_addr),32);
    bufp->chgBit(oldp+82,(vlSelf->bridge_rd));
    bufp->chgIData(oldp+83,(vlSelf->bridge_rd_data),32);
    bufp->chgBit(oldp+84,(vlSelf->bridge_wr));
    bufp->chgIData(oldp+85,(vlSelf->bridge_wr_data),32);
    bufp->chgBit(oldp+86,(vlSelf->status_boot_done));
    bufp->chgBit(oldp+87,(vlSelf->status_setup_done));
    bufp->chgBit(oldp+88,(vlSelf->status_running));
    bufp->chgBit(oldp+89,(vlSelf->shutdown_ack_s));
    bufp->chgBit(oldp+90,(vlSelf->target_dataslot_flush_in));
    bufp->chgSData(oldp+91,(vlSelf->target_dataslot_id_in),16);
    bufp->chgBit(oldp+92,(vlSelf->target_dataslot_ack_out));
    bufp->chgBit(oldp+93,(vlSelf->target_dataslot_done_out));
    bufp->chgBit(oldp+94,(vlSelf->reset_n));
    bufp->chgBit(oldp+95,(vlSelf->shutdown_pending));
    bufp->chgIData(oldp+96,(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in),32);
    bufp->chgIData(oldp+97,(((IData)(vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__b_datatable_clear_active)
                              ? 0U : vlSelf->tb_core_bridge_cmd__DOT__dut__DOT__bridge_wr_data_in)),32);
}

void Vtb_core_bridge_cmd___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vtb_core_bridge_cmd___024root__trace_cleanup\n"); );
    // Init
    Vtb_core_bridge_cmd___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vtb_core_bridge_cmd___024root*>(voidSelf);
    Vtb_core_bridge_cmd__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    vlSymsp->__Vm_activity = false;
    vlSymsp->TOP.__Vm_traceActivity[0U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[1U] = 0U;
}
