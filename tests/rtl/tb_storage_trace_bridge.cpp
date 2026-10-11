// Reuse the established host-bus driver, not its upstream test entry point.
#define main unused_upstream_main
#include "../../.integration/openfpgaos/src/fpga/test/tb_core_bridge_cmd_main.cpp"
#undef main

static void settle() { for (int i = 0; i < 12; ++i) tick(); }
int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    tb = new Vtb_core_bridge_cmd;
    init_inputs();
    tb->target_dataslot_flush_in = 1;
    settle();
    check_eq("no-host-poll-request-launch-queue", tb->storage_trace_out, 0xf3c1u);
    check_eq("no-host-poll-no-ack", tb->target_dataslot_ack_out, 0u);
    check_eq("poll-command-value", read32(TARGET_CMD), 0x636d0188u);
    settle();
    check_eq("command-read-sticky", tb->storage_trace_out, 0xf3e1u);
    check_eq("poll-slot-value", read32(TARGET_CMD + 0x20u), 4u);
    settle();
    check_eq("slot-read-sticky", tb->storage_trace_out, 0xf3f1u);
    write32(TARGET_CMD, 0x62750000u);
    settle();
    check_eq("busy-response-observed", tb->storage_trace_out, 0xf4fdu);
    write32(TARGET_CMD, 0x6f6b0000u);
    settle();
    check_eq("ok-response-all-flags", tb->storage_trace_out, 0x06ffu);
    check_eq("ok-holds-ack", tb->target_dataslot_ack_out, 1u);
    tb->target_dataslot_flush_in = 0;
    settle();
    check_eq("request-low-clears-ack", tb->target_dataslot_ack_out, 0u);

    tb->target_dataslot_flush_in = 1;
    settle();
    check_eq("second-flush-clears-history", tb->storage_trace_out, 0xf3c1u);
    write32(TARGET_CMD, 0x6f6b0000u);
    settle();
    check_eq("immediate-ok-without-busy", tb->storage_trace_out, 0x06cbu);
    check_eq("immediate-ok-done", tb->target_dataslot_done_out, 1u);
    tb->target_dataslot_flush_in = 0;
    settle();
    // Existing command/handshake regressions, still without enabling trace
    // as an input to the command FSM.
    test_dataslot_flush_command();
    test_ready_reissued_after_warm_reset();
    test_reset_enter_clears_readonly_datatable_only();
    std::printf("Trace bridge: %d passed, %d failed\n", passes, fails);
    delete tb;
    return fails ? 1 : 0;
}
