#include "Vchisburner_cart_gba_readonly.h"
#include <array>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <utility>
#include <vector>

#define ck(value) do { if (!(value)) { \
    std::fprintf(stderr, "identify56 RTL assertion line %d\n", __LINE__); \
    throw std::runtime_error("identify56 RTL assertion"); \
} } while (0)

struct FlashModel {
    enum Mode { NORMAL, AUTOSELECT, CFI } mode = NORMAL;
    bool supported = true;
    unsigned unlock = 0;
    uint32_t address = 0;
    bool wr_active = false;
    std::vector<std::pair<uint32_t, uint16_t>> writes;

    uint16_t read(uint32_t word) const {
        if (supported && mode == AUTOSELECT) {
            if (word == 0) return 0x0001;
            if (word == 1) return 0x227e;
            if (word == 2) return 0x2222;
            if (word == 3) return 0x2201;
        }
        if (supported && mode == CFI) {
            if (word == 0x10) return 0x0051;
            if (word == 0x11) return 0x0052;
            if (word == 0x12) return 0x0059;
            if (word == 0x27) return 0x0019;
            return 0;
        }
        return uint16_t(0xa000u + (word & 0xffu));
    }

    void write(uint32_t word, uint16_t data) {
        writes.push_back({word, data});
        if (data == 0x00f0) { mode = NORMAL; unlock = 0; return; }
        if (!supported) return;
        if (word == 0x55 && data == 0x0098) { mode = CFI; unlock = 0; return; }
        if (unlock == 0 && word == 0x555 && data == 0x00aa) { unlock = 1; return; }
        if (unlock == 1 && word == 0x2aa && data == 0x0055) { unlock = 2; return; }
        if (unlock == 2 && word == 0x555 && data == 0x0090) { mode = AUTOSELECT; unlock = 0; return; }
        unlock = 0;
    }
};

static void tick(Vchisburner_cart_gba_readonly &d, FlashModel &m) {
    d.clk = 0; d.eval();
    ck(!d.writes_enabled && !d.pin31_dir && (!d.pin30_dir || d.pin30_out));
    if (d.bank0_out == 0xf && d.bank3_dir && d.bank2_dir && d.bank1_dir)
        m.address = uint32_t(d.bank3_out) | (uint32_t(d.bank2_out) << 8) |
                    (uint32_t(d.bank1_out) << 16);
    if (d.bank0_out == 0xc && !d.bank3_dir && !d.bank2_dir) {
        uint16_t value = m.read(m.address);
        d.bank3_in = uint8_t(value);
        d.bank2_in = uint8_t(value >> 8);
    }
    bool wr_now = d.bank0_out == 0xa && d.bank3_dir && d.bank2_dir;
    if (wr_now && !m.wr_active)
        m.write(m.address, uint16_t(d.bank3_out) | (uint16_t(d.bank2_out) << 8));
    m.wr_active = wr_now;
    d.clk = 1; d.eval();
}

static void wr(Vchisburner_cart_gba_readonly &d, FlashModel &m,
               unsigned addr, uint32_t value) {
    d.reg_addr = addr; d.reg_wdata = value; d.reg_wr = 1; tick(d, m); d.reg_wr = 0;
}

static uint32_t rd(Vchisburner_cart_gba_readonly &d, unsigned addr) {
    d.reg_addr = addr; d.reg_rd = 0; d.eval(); return d.reg_rdata;
}

static bool safe(Vchisburner_cart_gba_readonly &d) {
    return !d.bank3_dir && !d.bank2_dir && !d.bank1_dir && d.bank0_dir &&
           d.bank0_out == 15 && !d.pin30_dir;
}

static void reset(Vchisburner_cart_gba_readonly &d, FlashModel &m) {
    d.reset_n = 0; d.reg_wr = d.reg_rd = 0; d.lifecycle_ok = 1;
    tick(d, m); d.reset_n = 1; tick(d, m); ck(safe(d));
}

static void prepare(Vchisburner_cart_gba_readonly &d, FlashModel &m,
                    unsigned space, uint32_t off, unsigned length) {
    wr(d, m, 3, 1); wr(d, m, 4, 2); wr(d, m, 16, space);
    wr(d, m, 5, off); wr(d, m, 6, length); wr(d, m, 7, 100000);
    wr(d, m, 3, 2);
}

static std::array<uint8_t, 32> identify(Vchisburner_cart_gba_readonly &d,
                                        FlashModel &m) {
    prepare(d, m, 4, 0, 32);
    unsigned cycles = 0;
    while ((rd(d, 2) & 2u) && cycles++ < 10000) tick(d, m);
    ck(cycles < 10000 && (rd(d, 2) & 4u) && rd(d, 9) == 32 && safe(d));
    std::array<uint8_t, 32> out{};
    for (unsigned i = 0; i < 8; ++i) {
        uint32_t word = rd(d, 8);
        for (unsigned j = 0; j < 4; ++j) out[4 * i + j] = uint8_t(word >> (8 * j));
        d.reg_rd = 1; tick(d, m); d.reg_rd = 0;
    }
    return out;
}

int main() {
    Vchisburner_cart_gba_readonly d;
    FlashModel flash;
    reset(d, flash);
    ck(rd(d, 1) == 0x02200f02u);
    struct Bad { unsigned space, off, length; };
    for (auto bad : {Bad{4,2,32}, Bad{4,0,16}, Bad{3,0,32}}) {
        prepare(d, flash, bad.space, bad.off, bad.length);
        ck((rd(d, 2) & 8u) && rd(d, 10) == 1 && safe(d));
    }
    auto got = identify(d, flash);
    for (unsigned i = 0; i < 8; ++i)
        ck(got[i] == uint8_t(i & 1 ? 0xa0 : 0x00 + i / 2));
    const uint8_t id[8] = {1,0,0x7e,0x22,0x22,0x22,1,0x22};
    for (unsigned i = 0; i < 8; ++i) ck(got[8 + i] == id[i]);
    ck(got[16] == 'Q' && got[17] == 0 && got[18] == 'R' &&
       got[19] == 0 && got[20] == 'Y' && got[21] == 0);
    ck(got[24] == 25 && got[25] == 0);
    const std::pair<uint32_t,uint16_t> expected[] = {
        {0,0xf0}, {0x555,0xaa}, {0x2aa,0x55}, {0x555,0x90},
        {0,0xf0}, {0x55,0x98}, {0,0xf0}
    };
    ck(flash.writes.size() == sizeof(expected) / sizeof(expected[0]));
    for (unsigned i = 0; i < flash.writes.size(); ++i) ck(flash.writes[i] == expected[i]);
    for (auto w : flash.writes)
        ck(w.second != 0xa0 && w.second != 0x80 && w.second != 0x10 &&
           w.second != 0x30 && w.second != 0x25 && w.second != 0x29);
    prepare(d, flash, 4, 0, 32);
    ck((rd(d, 2) & 8u) && rd(d, 10) == 1 && safe(d));

    FlashModel generic; generic.supported = false;
    Vchisburner_cart_gba_readonly d2; reset(d2, generic);
    auto plain = identify(d2, generic);
    for (unsigned i = 0; i < 8; ++i) ck(plain[8 + i] == plain[i]);
    ck(!(plain[16] == 'Q' && plain[18] == 'R' && plain[20] == 'Y'));
    ck(generic.mode == FlashModel::NORMAL && safe(d2));
    puts("identify56 RTL: fixed F0/ID/CFI/F0 sequence + NOR/generic snapshots + one-shot + forbidden opcodes PASS");
}
