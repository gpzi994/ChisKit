#include "Vchisburner_cart_gb_mbc5_fullrom.h"

#include <cstdint>
#include <cstdio>
#include <stdexcept>

static uint8_t selected_bank = 1;
static bool previous_wr = false;
static bool previous_rd_bus = false;
static unsigned mapper_write_windows = 0;
static uint64_t physical_read_windows = 0;

static void check(bool ok, const char *what) {
    if (!ok) throw std::runtime_error(what);
}

static uint8_t rom_byte(uint32_t logical) {
    return static_cast<uint8_t>((logical ^ 0xA5u) +
                                (logical >> 8) + (logical >> 16));
}

static void observe(Vchisburner_cart_gb_mbc5_fullrom &dut) {
    bool wr = (dut.bank0_out & 0x4u) == 0;
    bool rd = (dut.bank0_out & 0x2u) == 0;
    check(!(wr && rd), "/WR and /RD active together");
    check(dut.writes_enabled == 0, "destructive capability asserted");
    check(dut.pin30_dir == 0 && dut.pin30_pwroff_reset == 1,
          "GB reset safety changed");
    if (wr) {
        uint16_t a = static_cast<uint16_t>(dut.bank3_out) |
                     (static_cast<uint16_t>(dut.bank2_out) << 8);
        check(dut.bank3_dir && dut.bank2_dir && dut.bank1_dir,
              "mapper write buses not driven");
        check(a == 0x2000, "write escaped MBC5 low-bank register");
        uint8_t d = dut.bank1_out;
        check(d >= 1 && d <= 127, "mapper data escaped 2 MiB bank range");
        if (!previous_wr) {
            selected_bank = d;
            mapper_write_windows++;
        }
    }
    if (rd) {
        check(dut.bank3_dir && dut.bank2_dir && !dut.bank1_dir,
              "ROM read directions wrong");
        uint16_t a = static_cast<uint16_t>(dut.bank3_out) |
                     (static_cast<uint16_t>(dut.bank2_out) << 8);
        uint32_t logical;
        if (a < 0x4000)
            logical = a;
        else {
            check(a <= 0x7FFF, "banked read outside ROM window");
            logical = static_cast<uint32_t>(selected_bank) * 0x4000u +
                      (a - 0x4000u);
        }
        if (!previous_rd_bus) physical_read_windows++;
        uint8_t value = rom_byte(logical);
#ifdef EXPECT_FIRST_BLOCK_DISCARD
        // Model the field failure: only the first physical 32-byte
        // transaction is corrupt.  The RTL must discard it and return the
        // second sampling of the same logical host block.
        if (physical_read_windows <= 32) value ^= 0xFFu;
#endif
        dut.bank3_in = value;
    }
    previous_wr = wr;
    previous_rd_bus = rd;
}

static void tick(Vchisburner_cart_gb_mbc5_fullrom &dut) {
    dut.clk = 0; dut.eval(); observe(dut);
    dut.clk = 1; dut.eval(); observe(dut);
}

static uint32_t reg_read(Vchisburner_cart_gb_mbc5_fullrom &dut, unsigned a) {
    dut.reg_addr = a; dut.reg_rd = 0; dut.eval(); return dut.reg_rdata;
}

static void reg_write(Vchisburner_cart_gb_mbc5_fullrom &dut, unsigned a,
                      uint32_t v) {
    dut.reg_addr = a; dut.reg_wr_addr = a; dut.reg_wdata = v; dut.reg_wr = 1; tick(dut);
    dut.reg_wr = 0;
}

static bool safe_idle(Vchisburner_cart_gb_mbc5_fullrom &dut) {
    return !dut.bank3_dir && !dut.bank2_dir && !dut.bank1_dir &&
           dut.bank0_dir && dut.bank0_out == 0xFu &&
           !dut.pin30_dir && !dut.pin31_dir && !dut.writes_enabled;
}

static void reset(Vchisburner_cart_gb_mbc5_fullrom &dut) {
    dut.reset_n = 0; dut.reg_wr = 0; dut.reg_rd = 0;
    dut.lifecycle_ok = 1; dut.lifecycle_status = 0x3Fu;
    dut.host_b1_raw = 0; dut.host_diag = 0; dut.bank3_in = 0; dut.reg_wr_addr = 0;
    dut.bank2_in = 0; selected_bank = 1; previous_wr = false;
    previous_rd_bus = false;
    mapper_write_windows = 0; tick(dut); dut.reset_n = 1; tick(dut);
    physical_read_windows = 0;
    check(safe_idle(dut), "reset not idle");
}

static void begin_read(Vchisburner_cart_gb_mbc5_fullrom &dut,
                       uint32_t address, uint32_t length = 32,
                       uint32_t bus = 1, uint32_t space = 0,
                       uint32_t timeout = 10000) {
    reg_write(dut, 3, 1);
    reg_write(dut, 4, bus);
    reg_write(dut, 16, space);
    reg_write(dut, 5, address);
    reg_write(dut, 6, length);
    reg_write(dut, 7, timeout);
    reg_write(dut, 3, 2);
}

static void read_block(Vchisburner_cart_gb_mbc5_fullrom &dut,
                       uint32_t logical) {
    begin_read(dut, logical);
    unsigned cycles = 0, rd_windows = 0;
    bool previous_rd = false;
    while ((reg_read(dut, 2) & 4u) == 0 && cycles++ < 10000) {
        bool rd = (dut.bank0_out & 2u) == 0;
        if (rd && !previous_rd) rd_windows++;
        previous_rd = rd;
        tick(dut);
    }
    check(cycles < 10000, "read timeout");
#ifdef EXPECT_FIRST_BLOCK_DISCARD
    check(rd_windows == (logical == 0 ? 64u : 32u),
          "wrong first-block discard window count");
#else
    check(rd_windows == 32, "wrong read window count");
#endif
    check(selected_bank == 1, "bank 1 not restored before DONE");
    check(safe_idle(dut), "completed command not idle");
    check(reg_read(dut, 9) == 32, "wrong FIFO level");
    for (unsigned word = 0; word < 8; ++word) {
        uint32_t data = reg_read(dut, 8);
        for (unsigned lane = 0; lane < 4; ++lane) {
            unsigned i = word * 4 + lane;
            check(static_cast<uint8_t>(data >> (lane * 8)) ==
                  rom_byte(logical + i), "logical ROM data mismatch");
        }
        dut.reg_addr = 8; dut.reg_rd = 1; tick(dut); dut.reg_rd = 0;
    }
}

int main() {
    Vchisburner_cart_gb_mbc5_fullrom dut;
    reset(dut);
    check(reg_read(dut, 1) == 0x00010F01u, "wrong caps");

    for (uint32_t bad : {0x20u, 0x4000u, 0x1FFFE0u, 0x200000u}) {
        begin_read(dut, bad);
        check(reg_read(dut, 10) == 1 && safe_idle(dut),
              "out-of-order address accepted");
    }
    begin_read(dut, 0, 16);
    check(reg_read(dut, 10) == 1 && safe_idle(dut), "short read accepted");
    begin_read(dut, 0, 32, 2);
    check(reg_read(dut, 10) == 1 && safe_idle(dut), "GBA bus accepted");
    begin_read(dut, 0, 32, 1, 1);
    check(reg_read(dut, 10) == 1 && safe_idle(dut), "save space accepted");

    for (uint32_t logical = 0; logical < 0x200000u; logical += 32)
        read_block(dut, logical);
#ifdef EXPECT_FIRST_BLOCK_DISCARD
    check(physical_read_windows == 65536ull * 32ull + 32ull,
          "wrong total physical read count");
#else
    check(physical_read_windows == 65536ull * 32ull,
          "wrong total physical read count");
#endif
    check(mapper_write_windows == (65536u - 512u) * 2u,
          "mapper select/restore count wrong");

    begin_read(dut, 0);
    check(reg_read(dut, 10) == 1 && safe_idle(dut),
          "read escaped full-ROM command budget");

    reset(dut);
    begin_read(dut, 0);
    while ((dut.bank0_out & 2u) != 0) tick(dut);
    reg_write(dut, 3, 3);
    check(safe_idle(dut) && (reg_read(dut, 2) & 0x10u),
          "abort did not release buses");

    reset(dut);
    begin_read(dut, 0);
    while ((dut.bank0_out & 2u) != 0) tick(dut);
    dut.lifecycle_ok = 0; tick(dut);
    check(safe_idle(dut) && reg_read(dut, 10) == 3,
          "lifecycle loss did not fail safe");

    std::puts("GB MBC5 fullrom86 RTL: exact 2 MiB ascending read + "
              "bank0 direct + 1..127 select/restore + no GBA/NOR/save PASS");
}
