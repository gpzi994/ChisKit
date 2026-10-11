#include "Vchisburner_cart_gb_mbc5_probe.h"

#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <vector>

static uint8_t selected_bank = 1;
static bool previous_wr = false;
static unsigned write_windows = 0;

static void check(bool ok, const char *what) {
    if (!ok)
        throw std::runtime_error(what);
}

static uint8_t rom_byte(uint32_t logical) {
    return static_cast<uint8_t>((logical ^ 0xA5u) + (logical >> 8) +
                                (logical >> 16));
}

static void observe(Vchisburner_cart_gb_mbc5_probe &dut) {
    bool wr = (dut.bank0_out & 0x4u) == 0;
    bool rd = (dut.bank0_out & 0x2u) == 0;
    check(!(wr && rd), "/WR and /RD active together");
    check(dut.writes_enabled == 0, "destructive write capability asserted");
    check(dut.pin30_dir == 0 && dut.pin30_pwroff_reset == 1,
          "GB reset safety changed");
    if (wr) {
        uint16_t a = static_cast<uint16_t>(dut.bank3_out) |
                     (static_cast<uint16_t>(dut.bank2_out) << 8);
        check(dut.bank3_dir && dut.bank2_dir && dut.bank1_dir,
              "mapper write buses not driven");
        check(a == 0x2000, "write escaped MBC5 low-bank register");
        uint8_t d = dut.bank1_out;
        check(d == 1 || d == 2 || d == 31 || d == 32 || d == 63 ||
              d == 64 || d == 126 || d == 127,
              "mapper write data escaped whitelist");
        if (!previous_wr) {
            selected_bank = d;
            write_windows++;
        }
    }
    if (rd) {
        check(dut.bank3_dir && dut.bank2_dir && !dut.bank1_dir,
              "ROM read bus directions wrong");
        uint16_t a = static_cast<uint16_t>(dut.bank3_out) |
                     (static_cast<uint16_t>(dut.bank2_out) << 8);
        check(a >= 0x4000 && a <= 0x7FFF, "banked read outside ROM window");
        uint32_t logical = static_cast<uint32_t>(selected_bank) * 0x4000u +
                           (a - 0x4000u);
        dut.bank3_in = rom_byte(logical);
    }
    previous_wr = wr;
}

static void tick(Vchisburner_cart_gb_mbc5_probe &dut) {
    dut.clk = 0; dut.eval(); observe(dut);
    dut.clk = 1; dut.eval(); observe(dut);
}

static uint32_t reg_read(Vchisburner_cart_gb_mbc5_probe &dut, unsigned a) {
    dut.reg_addr = a; dut.reg_rd = 0; dut.eval(); return dut.reg_rdata;
}

static void reg_write(Vchisburner_cart_gb_mbc5_probe &dut, unsigned a,
                      uint32_t v) {
    dut.reg_addr = a; dut.reg_wdata = v; dut.reg_wr = 1; tick(dut);
    dut.reg_wr = 0;
}

static bool safe_idle(Vchisburner_cart_gb_mbc5_probe &dut) {
    return !dut.bank3_dir && !dut.bank2_dir && !dut.bank1_dir &&
           dut.bank0_dir && dut.bank0_out == 0xFu && !dut.pin30_dir &&
           !dut.pin31_dir && !dut.writes_enabled;
}

static void reset(Vchisburner_cart_gb_mbc5_probe &dut) {
    dut.reset_n = 0; dut.reg_wr = 0; dut.reg_rd = 0;
    dut.lifecycle_ok = 1; dut.lifecycle_status = 0x3Fu;
    dut.host_b1_raw = 0; dut.host_diag = 0; dut.bank3_in = 0;
    dut.bank2_in = 0; selected_bank = 1; previous_wr = false;
    write_windows = 0; tick(dut); dut.reset_n = 1; tick(dut);
    check(safe_idle(dut), "reset not idle");
}

static void begin_read(Vchisburner_cart_gb_mbc5_probe &dut, uint32_t address,
                       uint32_t length = 32, uint32_t bus = 1,
                       uint32_t space = 0, uint32_t timeout = 20000) {
    reg_write(dut, 3, 1);
    reg_write(dut, 4, bus);
    reg_write(dut, 16, space);
    reg_write(dut, 5, address);
    reg_write(dut, 6, length);
    reg_write(dut, 7, timeout);
    reg_write(dut, 3, 2);
}

static void read_block(Vchisburner_cart_gb_mbc5_probe &dut, uint32_t logical) {
    begin_read(dut, logical);
    unsigned cycles = 0, rd_windows = 0;
    bool previous_rd = false;
    while ((reg_read(dut, 2) & 4u) == 0 && cycles++ < 20000) {
        bool rd = (dut.bank0_out & 2u) == 0;
        if (rd && !previous_rd) rd_windows++;
        previous_rd = rd;
        tick(dut);
    }
    check(cycles < 20000, "read timeout");
    check(rd_windows == 32, "wrong read window count");
    check(selected_bank == 1, "bank 1 was not restored before DONE");
    check(safe_idle(dut), "completed command not idle");
    check(reg_read(dut, 9) == 32, "wrong FIFO level");
    std::vector<uint8_t> out;
    while (out.size() < 32) {
        uint32_t word = reg_read(dut, 8);
        for (unsigned i = 0; i < 4; ++i)
            out.push_back(static_cast<uint8_t>(word >> (8u * i)));
        dut.reg_addr = 8; dut.reg_rd = 1; tick(dut); dut.reg_rd = 0;
    }
    for (unsigned i = 0; i < 32; ++i)
        check(out[i] == rom_byte(logical + i), "logical bank data mismatch");
}

int main() {
    Vchisburner_cart_gb_mbc5_probe dut;
    reset(dut);
    check(reg_read(dut, 1) == 0x00010F01u, "wrong caps");

    for (uint32_t bad : {0u, 0x100u, 0xC000u, 0x1FC020u, 0x200000u}) {
        begin_read(dut, bad);
        check(reg_read(dut, 10) == 1 && safe_idle(dut),
              "forbidden address accepted");
    }
    begin_read(dut, 0x4000, 16);
    check(reg_read(dut, 10) == 1 && safe_idle(dut), "short read accepted");
    begin_read(dut, 0x4000, 32, 2);
    check(reg_read(dut, 10) == 1 && safe_idle(dut), "GBA bus accepted");
    begin_read(dut, 0x4000, 32, 1, 1);
    check(reg_read(dut, 10) == 1 && safe_idle(dut), "save space accepted");

    const uint32_t a[8] = {0x4000,0x8000,0x7C000,0x80000,
                           0xFC000,0x100000,0x1FC000,0x1FFFE0};
    for (unsigned pass = 0; pass < 2; ++pass)
        for (uint32_t v : a)
            read_block(dut, v);
    check(write_windows == 32, "expected select+restore pair per read");

    begin_read(dut, 0x4000);
    check(reg_read(dut, 10) == 1 && safe_idle(dut),
          "17th read escaped command budget");

    reset(dut);
    begin_read(dut, 0x4000);
    while ((dut.bank0_out & 4u) != 0) tick(dut);
    reg_write(dut, 3, 3);
    check(safe_idle(dut) && (reg_read(dut, 2) & 0x10u),
          "abort did not release buses");

    reset(dut);
    begin_read(dut, 0x4000);
    while ((dut.bank0_out & 2u) != 0) tick(dut);
    dut.lifecycle_ok = 0; tick(dut);
    check(safe_idle(dut) && reg_read(dut, 10) == 3,
          "lifecycle loss did not fail safe");

    std::puts("GB MBC5 banks85 RTL: exact sequence x2 + select/restore @2000 + "
              "32-byte banked reads + no NOR/save/high-bank writes PASS");
}
