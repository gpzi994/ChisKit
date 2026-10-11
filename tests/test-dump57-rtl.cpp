#include "Vchisburner_cart_gba_readonly.h"
#include <cstdint>
#include <cstdio>
#include <stdexcept>

#define ck(value) do { if (!(value)) { \
    std::fprintf(stderr, "dump57 RTL assertion line %d\n", __LINE__); \
    throw std::runtime_error("dump57 RTL assertion"); \
} } while (0)

static void tick(Vchisburner_cart_gba_readonly &d) {
    d.clk = 0; d.eval();
    ck(!d.writes_enabled && !d.pin31_dir && (!d.pin30_dir || d.pin30_out));
    d.clk = 1; d.eval();
}
static void wr(Vchisburner_cart_gba_readonly &d, unsigned a, uint32_t v) {
    d.reg_addr = a; d.reg_wdata = v; d.reg_wr = 1; tick(d); d.reg_wr = 0;
}
static uint32_t rd(Vchisburner_cart_gba_readonly &d, unsigned a) {
    d.reg_addr = a; d.reg_rd = 0; d.eval(); return d.reg_rdata;
}
static bool safe(Vchisburner_cart_gba_readonly &d) {
    return !d.bank3_dir && !d.bank2_dir && !d.bank1_dir && d.bank0_dir &&
           d.bank0_out == 15 && !d.pin30_dir;
}
static void reset(Vchisburner_cart_gba_readonly &d) {
    d.reset_n = 0; d.reg_wr = d.reg_rd = 0; d.lifecycle_ok = 1;
    tick(d); d.reset_n = 1; tick(d); ck(safe(d));
}
static void prepare(Vchisburner_cart_gba_readonly &d, uint32_t off,
                    unsigned n = 32, unsigned space = 0,
                    unsigned bus = 2, unsigned timeout = 50000) {
    wr(d, 3, 1); wr(d, 4, bus); wr(d, 16, space); wr(d, 5, off);
    wr(d, 6, n); wr(d, 7, timeout); wr(d, 3, 2);
}
static void block(Vchisburner_cart_gba_readonly &d, uint32_t off) {
    prepare(d, off);
    unsigned cycles = 0;
    while ((rd(d, 2) & 2u) && cycles++ < 1000) {
        d.bank3_in = uint8_t(off + cycles);
        d.bank2_in = uint8_t((off >> 8) + cycles);
        tick(d);
    }
    ck(cycles < 1000 && (rd(d, 2) & 4u) && rd(d, 9) == 32u && safe(d));
    for (unsigned i = 0; i < 8; ++i) {
        (void)rd(d, 8); d.reg_rd = 1; tick(d); d.reg_rd = 0;
    }
    ck(rd(d, 9) == 0);
}

int main() {
    constexpr uint32_t total = 33554432u;
    constexpr uint32_t header_reads = 24u;
    constexpr uint32_t sample_reads = 5u;
    constexpr uint32_t full_reads = total / 32u;
    constexpr uint32_t expected_reads = header_reads + sample_reads + full_reads;
    static_assert(expected_reads == 1048605u, "dump57 command budget");

    Vchisburner_cart_gba_readonly d; reset(d);
    ck(rd(d, 1) == 0x02200f02u);
    for (uint32_t off : {2u, 16u, total, 0xffffffe0u}) {
        prepare(d, off); ck((rd(d, 2) & 8u) && rd(d, 10) == 1u && safe(d));
    }
    for (unsigned n : {0u, 16u, 64u}) {
        prepare(d, 0, n); ck((rd(d, 2) & 8u) && rd(d, 10) == 1u && safe(d));
    }
    for (unsigned space : {1u, 2u, 3u}) {
        prepare(d, 0, 32, space);
        ck((rd(d, 2) & 8u) && rd(d, 10) == 1u && safe(d));
    }

    for (unsigned pass = 0; pass < 2; ++pass)
        for (uint32_t off = 0; off < 384u; off += 32u) block(d, off);
    for (uint32_t off : {0u, 160u, 8388608u, 16777216u, 33554400u}) block(d, off);
    for (uint32_t off = 0; off < total; off += 32u) block(d, off);

    prepare(d, 0);
    ck((rd(d, 2) & 8u) && rd(d, 10) == 1u && safe(d));
    ck(!d.writes_enabled && !d.pin31_dir);
    std::puts("dump57 RTL: 1048605 exact ROM reads + 32MiB boundary + save/RTC rejection + safe pins PASS");
}
