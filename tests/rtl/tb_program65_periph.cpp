#define main unused_upstream_main
#include "../../.integration/openfpgaos/src/fpga/test/tb_axi_periph_main.cpp"
#undef main

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    tb = new Vtb_axi_periph;
    tb->storage_trace_in = 0;
    tb->events_status_in = 0x55aa0001u;
    tb->events_count_in = 0x00070100u;
    tb->events_data_in = 0xdf3f1c60u;
    reset_sequence();

    check_eq("cart base reg 0", mmio_read32(0x400001a0u), 0x31424746u);
    check_eq("cart base reg 15", mmio_read32(0x400001dcu), 0xeca86420u);
    check_eq("cart space reg 16", mmio_read32(0x4000015cu), 0x16161616u);

    for (uint32_t i = 0; i < 7; ++i) {
        uint32_t address = 0x40000184u + i * 4u;
        uint32_t logical = 17u + i;
        uint32_t packed_bcd = ((logical / 10u) << 4) | (logical % 10u);
        uint32_t expected = packed_bcd * 0x01010101u;
        check_eq("cart programmer read", mmio_read32(address), expected);
        axi_write_single(address, 0xa5000000u | logical);
        check_eq("cart programmer write addr", tb->dbg_cart_last_addr,
                 logical);
        check_eq("cart programmer write data", tb->dbg_cart_last_wdata,
                 0xa5000000u | logical);
    }

    axi_write_single(0x4000015cu, 4u);
    check_eq("cart space write addr", tb->dbg_cart_last_addr, 16u);
    check_eq("cart space write data", tb->dbg_cart_last_wdata, 4u);

    uint32_t cart_reads_before_audit = tb->dbg_cart_rd_count;
    check_eq("audit identity independent", mmio_read32(0x400001e0u),
             0x46474237u);
    check_eq("audit identity not cart", tb->dbg_cart_rd_count,
             cart_reads_before_audit);
    check_eq("audit armed once", tb->events_arms, 1u);
    check_eq("audit status independent", mmio_read32(0x400001e4u),
             0x55aa0001u);
    check_eq("audit freeze once", tb->events_freezes, 1u);
    check_eq("legacy trace independent", mmio_read32(0x400001f0u),
             0xd0000060u);

    printf("Program65 periph: %d passed, %d failed\n", passes, fails);
    delete tb;
    return fails ? 1 : 0;
}
