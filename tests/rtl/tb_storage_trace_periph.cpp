// Reuse only AXI drivers plus the existing storage completion test.
#define main unused_upstream_main
#include "../../.integration/openfpgaos/src/fpga/test/tb_axi_periph_main.cpp"
#undef main

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    tb = new Vtb_axi_periph;
    tb->storage_trace_in = 0;
    reset_sequence();
    check_eq("public-status-unchanged-idle", mmio_read32(0x4000003cu), 0x60u);
    check_eq("trace-tag-idle", mmio_read32(0x400001f0u), 0xd0000060u);
    axi_write_single(0x40000038u, 5u);
    tb->storage_trace_in = 0xf3c1u;
    for (int i = 0; i < 12; ++i) tick();
    check_eq("trace-request-active-no-ack", mmio_read32(0x400001f0u), 0xdf3c1c60u);
    check_eq("public-status-no-debug-bits", mmio_read32(0x4000003cu), 0x60u);
    tb->target_dataslot_ack_in = 1;
    for (int i = 0; i < 12; ++i) tick();
    check_eq("trace-cpu-ack-request-cleared", mmio_read32(0x400001f0u), 0xdf3c1a41u);
    tb->target_dataslot_done_in = 1;
    tb->storage_trace_in = 0x06ffu;
    for (int i = 0; i < 12; ++i) tick();
    check_eq("trace-cpu-completion", mmio_read32(0x400001f0u), 0xd06ff3c3u);
    check_eq("public-status-completion", mmio_read32(0x4000003cu), 0xc3u);
    reset_sequence();
    test_dataslot_completion_irq();
    printf("Trace periph: %d passed, %d failed\n", passes, fails);
    delete tb;
    return fails ? 1 : 0;
}
