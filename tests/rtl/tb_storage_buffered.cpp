#include "Vtb_storage_buffered.h"
#include <verilated.h>
#include <array>
#include <cstdio>
#include <memory>

static constexpr uint32_t CMD = 0xf8001000u;
static constexpr uint32_t TABLE = 0xf8002000u;
static int passed, failed;
static uint32_t swap32(uint32_t x) {
    return (x >> 24) | ((x >> 8) & 0xff00u) |
           ((x << 8) & 0xff0000u) | (x << 24);
}
struct Bus {
    std::unique_ptr<Vtb_storage_buffered> tb{new Vtb_storage_buffered};
    bool little;
    explicit Bus(bool endian): little(endian) {
        tb->clk = 0;
        tb->bridge_addr = 0;
        tb->bridge_rd = 0;
        tb->bridge_wr = 0;
        tb->bridge_wr_data = 0;
        tb->bridge_endian_little = little;
        tb->target_dataslot_flush_in = 0;
        ticks(16);
    }
    void tick() {
        tb->clk = 0; tb->eval();
        tb->clk = 1; tb->eval();
    }
    void ticks(int n) { while (n--) tick(); }
    void write(uint32_t addr, uint32_t value) {
        tb->bridge_addr = addr;
        tb->bridge_wr_data = little ? swap32(value) : value;
        ticks(4);
        tb->bridge_wr = 1; tick();
        tb->bridge_wr = 0; ticks(84);
    }
    // APF io_bridge_peripheral captures the previous response before raising
    // pmp_rd for the new address. Model that buffered-word boundary, not the
    // old test helper's immediate-current-address read. 4 cycles of address
    // setup matches ST_READ_0; no claims about serial pads/CDC or host framing.
    uint32_t transfer(uint32_t addr) {
        tb->bridge_addr = addr;
        ticks(4);
        uint32_t previous = tb->bridge_rd_data;
        ticks(2);
        tb->bridge_rd = 1; tick();
        tb->bridge_rd = 0; ticks(81);
        return little ? swap32(previous) : previous;
    }
    uint32_t read(uint32_t addr) {
        (void)transfer(addr); // discard pipeline priming response
        return transfer(addr);
    }
};
static void check(const char *name, uint32_t got, uint32_t expected) {
    if (got == expected) ++passed;
    else {
        ++failed;
        std::printf("FAIL %s got=%08x expected=%08x\n", name, got, expected);
    }
}
static void run(bool little) {
    Bus bus(little);
    std::array<uint32_t, 64> table{};
    table[2] = 1; table[3] = 137480;
    table[4] = 2; table[5] = 123;
    table[6] = 3; table[7] = 35984;
    table[8] = 4; table[9] = 1048613;
    for (unsigned i = 0; i < table.size(); ++i)
        bus.write(TABLE + i * 4, table[i]);
    bus.tb->target_dataslot_flush_in = 1;
    bus.ticks(16);
    check("FLUSH published", bus.read(CMD), 0x636d0188u);
    // Chained reads: a transfer returns the preceding address's result.
    check("previous command", bus.transfer(CMD + 4), 0x636d0188u);
    check("parameter pointer", bus.transfer(CMD + 0x20), 0x20u);
    check("slot parameter", bus.transfer(TABLE), 4u);
    for (unsigned i = 1; i < table.size(); ++i)
        check("table pipeline", bus.transfer(TABLE + i * 4), table[i - 1]);
    check("table tail", bus.transfer(CMD), table.back());
    check("command after table", bus.transfer(CMD), 0x636d0188u);
    check("single slot", bus.read(CMD + 0x20), 4u);
    check("single table ID", bus.read(TABLE + 8 * 4), 4u);
    check("single table size", bus.read(TABLE + 9 * 4), 1048613u);
    check("trace before host status", bus.tb->storage_trace_out, 0xf3f1u);
    // Responses are deliberately scripted. PASS proves RTL transport under
    // this sequence, not that the actual Pocket host emits these responses.
    bus.write(CMD, 0x62750000u);
    check("scripted busy ACK", bus.tb->target_dataslot_ack_out, 1u);
    bus.write(CMD, 0x6f6b0000u);
    check("scripted completion", bus.tb->target_dataslot_done_out, 1u);
    check("scripted response flags", bus.tb->storage_trace_out, 0x06ffu);
    bus.tb->target_dataslot_flush_in = 0;
    bus.ticks(16);
    check("request release", bus.tb->target_dataslot_ack_out, 0u);
    std::printf("Buffered F8 path, endian=%s completed\n", little ? "little" : "big");
}
int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    run(false);
    run(true);
    std::printf("Buffered command/table: %d PASS, %d FAIL\n", passed, failed);
    std::puts("Not a serial PHY, host firmware, filesystem or full-core simulation.");
    return failed ? 1 : 0;
}
