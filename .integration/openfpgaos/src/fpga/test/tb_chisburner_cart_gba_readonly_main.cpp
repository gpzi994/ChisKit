#include "Vchisburner_cart_gba_readonly.h"

#include <cstdint>
#include <cstdio>

static int failures;

static void check(const char *name, bool ok) {
    std::printf("  %-34s %s\n", name, ok ? "OK" : "FAIL");
    if (!ok) ++failures;
}

static void tick(Vchisburner_cart_gba_readonly &dut) {
    dut.clk = 0;
    dut.eval();
    dut.clk = 1;
    dut.eval();
}

static void write_reg(Vchisburner_cart_gba_readonly &dut,
                      uint8_t addr, uint32_t value) {
    dut.reg_addr = addr;
    dut.reg_wdata = value;
    dut.reg_wr = 1;
    tick(dut);
    dut.reg_wr = 0;
}

static uint32_t read_reg(Vchisburner_cart_gba_readonly &dut, uint8_t addr) {
    dut.reg_addr = addr;
    dut.reg_rd = 0;
    dut.eval();
    return dut.reg_rdata;
}

static uint32_t read_reg_pop(Vchisburner_cart_gba_readonly &dut, uint8_t addr) {
    dut.reg_addr = addr;
    dut.reg_rd = 0;
    dut.eval();
    uint32_t value = dut.reg_rdata;
    dut.reg_rd = 1;
    tick(dut);
    dut.reg_rd = 0;
    return value;
}

static bool pins_safe(const Vchisburner_cart_gba_readonly &dut) {
    return dut.bank3_dir == 0 && dut.bank2_dir == 0 &&
           dut.bank1_dir == 0 && dut.bank0_dir == 1 &&
           dut.bank0_out == 0xF && dut.pin30_dir == 0 &&
           dut.pin30_pwroff_reset == 0 && dut.pin31_dir == 0 &&
           dut.writes_enabled == 0;
}

int main() {
    Vchisburner_cart_gba_readonly dut;
    dut.clk = 0;
    dut.reset_n = 0;
    dut.lifecycle_ok = 0;
    dut.reg_wr = 0;
    dut.reg_rd = 0;
    dut.reg_addr = 0;
    dut.reg_wdata = 0;
    dut.bank3_in = 0;
    dut.bank2_in = 0;
    tick(dut);
    dut.reset_n = 1;
    tick(dut);

    check("reset returns safe idle", pins_safe(dut));
    check("writes structurally absent", dut.writes_enabled == 0);
    check("ID signature CBR1", read_reg(dut, 0) == 0x31424746u);
    check("GBA read capability present", (read_reg(dut, 1) & 0x102u) == 0x102u);
    check("GBA capability exposes ROM only",
          ((read_reg(dut, 1) >> 21) & 0x1Fu) == 0x1u);
    check("GB space capability absent",
          ((read_reg(dut, 1) >> 16) & 0xFu) == 0);
    check("query capability present", (read_reg(dut, 1) & 0x800u) != 0);
    check("space resets to ROM", read_reg(dut, 16) == 0);

    // The LK-compatible discovery subset must stay entirely inside the
    // register block and leave every cartridge pin in safe idle.
    write_reg(dut, 14, 0);
    write_reg(dut, 3, 0xA1);
    check("ChisBurner query signature", read_reg(dut, 15) == 0x50424746u);
    check("query signature pins safe", pins_safe(dut));
    write_reg(dut, 14, 1);
    write_reg(dut, 3, 0xA1);
    check("query protocol v1.0", read_reg(dut, 15) == 0x00010000u);
    write_reg(dut, 14, 2);
    write_reg(dut, 3, 0xA1);
    check("query feature flags", read_reg(dut, 15) == 0x00000003u);
    write_reg(dut, 14, 0x13579BDFu);
    write_reg(dut, 3, 0xFE);
    check("ping returns complement", read_reg(dut, 15) == 0xECA86420u);
    check("ping reports done", (read_reg(dut, 2) & 4u) != 0);
    check("ping leaves pins safe", pins_safe(dut));
    write_reg(dut, 14, 3);
    write_reg(dut, 3, 0xA1);
    check("unknown query rejected", read_reg(dut, 10) == 1 && pins_safe(dut));

    // A read without lifecycle authorization must not produce any strobe.
    write_reg(dut, 4, 2);
    write_reg(dut, 16, 0);
    write_reg(dut, 5, 0x100);
    write_reg(dut, 6, 4);
    write_reg(dut, 7, 100);
    write_reg(dut, 3, 2);
    check("lifecycle failure stays safe", pins_safe(dut));
    check("lifecycle error reported", read_reg(dut, 10) == 3);

    // Valid two-halfword read.  The peripheral sends byte_address >> 1.
    dut.lifecycle_ok = 1;
    write_reg(dut, 3, 1);
    write_reg(dut, 4, 2);
    write_reg(dut, 16, 0);
    write_reg(dut, 5, 0x100);
    write_reg(dut, 6, 4);
    write_reg(dut, 7, 200);
    write_reg(dut, 3, 2);

    int read_windows = 0;
    int address_windows = 0;
    uint32_t addresses[2] = {0, 0};
    bool previous_read = false;
    bool previous_address = false;
    bool wr_always_inactive = true;
    bool cs2_always_inactive = true;
    for (int cycle = 0; cycle < 160 && (read_reg(dut, 2) & 4u) == 0; ++cycle) {
        bool address_phase = dut.bank3_dir && dut.bank2_dir &&
                             dut.bank1_dir && dut.bank0_out == 0xF;
        if (address_phase && !previous_address && address_windows < 2) {
            addresses[address_windows++] =
                (static_cast<uint32_t>(dut.bank1_out) << 16) |
                (static_cast<uint32_t>(dut.bank2_out) << 8) |
                dut.bank3_out;
        }
        previous_address = address_phase;

        bool read_active = dut.bank0_dir && ((dut.bank0_out & 0x3u) == 0u);
        if (read_active && !previous_read) ++read_windows;
        previous_read = read_active;
        if (read_windows <= 1) {
            dut.bank3_in = 0x34;
            dut.bank2_in = 0x12;
        } else {
            dut.bank3_in = 0xCD;
            dut.bank2_in = 0xAB;
        }
        if ((dut.bank0_out & 0x4u) == 0) wr_always_inactive = false;
        if (dut.pin30_dir && dut.pin30_out == 0) cs2_always_inactive = false;
        tick(dut);
    }
    check("two address phases observed", address_windows == 2);
    check("byte address shifted to 0x80", addresses[0] == 0x80);
    check("second word address is 0x81", addresses[1] == 0x81);
    check("WR# never asserted", wr_always_inactive);
    check("ROM read leaves CS2# inactive", cs2_always_inactive);
    check("read completes in safe idle", pins_safe(dut));
    check("DONE status reported", (read_reg(dut, 2) & 4u) != 0);
    check("four RX bytes available", read_reg(dut, 9) == 4);
    check("RX data is little endian", read_reg_pop(dut, 8) == 0xABCD1234u);
    check("RX pop drains FIFO", read_reg(dut, 9) == 0);

    // GBA save/EEPROM/RTC spaces require different electrical engines.  This
    // ROM backend must reject each one before any cartridge output changes.
    for (uint32_t space = 1; space <= 3; ++space) {
        write_reg(dut, 3, 1);
        write_reg(dut, 4, 2);
        write_reg(dut, 16, space);
        write_reg(dut, 5, 0x100);
        write_reg(dut, 6, 2);
        write_reg(dut, 7, 100);
        write_reg(dut, 3, 2);
        check(space == 1 ? "parallel save space rejected" :
              space == 2 ? "EEPROM space rejected" :
                           "RTC/GPIO space rejected",
              read_reg(dut, 10) == 1 && pins_safe(dut));
    }
    write_reg(dut, 16, 0);
    check("ROM space restored", read_reg(dut, 16) == 0);

    // Odd byte addresses are rejected without leaving safe idle.
    write_reg(dut, 5, 0x101);
    write_reg(dut, 6, 2);
    write_reg(dut, 7, 100);
    write_reg(dut, 3, 2);
    check("odd address rejected", read_reg(dut, 10) == 1 && pins_safe(dut));

    // Wrong-bus, oversize, out-of-range, and zero-timeout requests must be
    // rejected before any cartridge output changes.
    write_reg(dut, 4, 1);
    write_reg(dut, 5, 0x100);
    write_reg(dut, 6, 2);
    write_reg(dut, 7, 100);
    write_reg(dut, 3, 2);
    check("wrong bus rejected", read_reg(dut, 10) == 1 && pins_safe(dut));
    write_reg(dut, 4, 2);
    write_reg(dut, 6, 34);
    write_reg(dut, 3, 2);
    check("oversize read rejected", read_reg(dut, 10) == 1 && pins_safe(dut));
    write_reg(dut, 5, 0x02000000);
    write_reg(dut, 6, 2);
    write_reg(dut, 3, 2);
    check("address range rejected", read_reg(dut, 10) == 1 && pins_safe(dut));
    write_reg(dut, 5, 0x200);
    write_reg(dut, 7, 0);
    write_reg(dut, 3, 2);
    check("zero timeout rejected", read_reg(dut, 10) == 1 && pins_safe(dut));

    // Abort has priority and immediately releases every cartridge output.
    write_reg(dut, 3, 1);
    write_reg(dut, 5, 0x200);
    write_reg(dut, 6, 4);
    write_reg(dut, 7, 100);
    write_reg(dut, 3, 2);
    tick(dut);
    write_reg(dut, 3, 3);
    check("abort immediately safe", pins_safe(dut));
    check("abort status reported", (read_reg(dut, 2) & 0x10u) != 0);

    // Watchdog expiry also returns directly to safe idle.
    write_reg(dut, 3, 1);
    write_reg(dut, 5, 0x200);
    write_reg(dut, 6, 4);
    write_reg(dut, 7, 2);
    write_reg(dut, 3, 2);
    for (int i = 0; i < 5; ++i) tick(dut);
    check("timeout immediately safe", pins_safe(dut));
    check("timeout status reported", (read_reg(dut, 2) & 0x20u) != 0);

    // Lifecycle loss dominates an active transaction and releases the pins.
    write_reg(dut, 3, 1);
    write_reg(dut, 5, 0x200);
    write_reg(dut, 6, 4);
    write_reg(dut, 7, 100);
    write_reg(dut, 3, 2);
    tick(dut);
    dut.lifecycle_ok = 0;
    tick(dut);
    check("lifecycle loss immediately safe", pins_safe(dut));
    check("lifecycle loss reports error", read_reg(dut, 10) == 3);
    dut.lifecycle_ok = 1;

    // Reset must dominate an active cycle and leave no output enabled.
    write_reg(dut, 3, 1);
    write_reg(dut, 7, 100);
    write_reg(dut, 3, 2);
    tick(dut);
    dut.reset_n = 0;
    tick(dut);
    check("reset during read is safe", pins_safe(dut));

    if (failures) {
        std::printf("GBA read-only: %d failure(s)\n", failures);
        return 1;
    }
    std::printf("GBA read-only: PASS\n");
    return 0;
}
