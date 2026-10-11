#include "Vchisburner_cart_gb_readonly_abstract.h"

#include <cstdint>
#include <cstdio>

static int failures;

static void check(const char *name, bool ok) {
    std::printf("  %-38s %s\n", name, ok ? "OK" : "FAIL");
    if (!ok) ++failures;
}

static void tick(Vchisburner_cart_gb_readonly_abstract &dut) {
    dut.clk = 0;
    dut.eval();
    dut.clk = 1;
    dut.eval();
}

static void write_reg(Vchisburner_cart_gb_readonly_abstract &dut,
                      uint8_t addr, uint32_t value) {
    dut.reg_addr = addr;
    dut.reg_wdata = value;
    dut.reg_wr = 1;
    tick(dut);
    dut.reg_wr = 0;
}

static uint32_t read_reg(Vchisburner_cart_gb_readonly_abstract &dut,
                         uint8_t addr) {
    dut.reg_addr = addr;
    dut.reg_rd = 0;
    dut.eval();
    return dut.reg_rdata;
}

static uint32_t read_reg_pop(Vchisburner_cart_gb_readonly_abstract &dut,
                             uint8_t addr) {
    dut.reg_addr = addr;
    dut.reg_rd = 0;
    dut.eval();
    uint32_t value = dut.reg_rdata;
    dut.reg_rd = 1;
    tick(dut);
    dut.reg_rd = 0;
    return value;
}

static bool pins_safe(const Vchisburner_cart_gb_readonly_abstract &dut) {
    return dut.gb_address_dir == 0 && dut.gb_data_dir == 0 &&
           dut.gb_cs_n == 1 && dut.gb_rd_n == 1 && dut.gb_wr_n == 1 &&
           dut.writes_enabled == 0;
}

static void configure_read(Vchisburner_cart_gb_readonly_abstract &dut,
                           uint32_t bus, uint32_t space, uint32_t address,
                           uint32_t length, uint32_t timeout) {
    write_reg(dut, 3, 1);
    write_reg(dut, 4, bus);
    write_reg(dut, 16, space);
    write_reg(dut, 5, address);
    write_reg(dut, 6, length);
    write_reg(dut, 7, timeout);
}

int main() {
    Vchisburner_cart_gb_readonly_abstract dut;
    dut.clk = 0;
    dut.reset_n = 0;
    dut.lifecycle_ok = 0;
    dut.lifecycle_status = 0;
    dut.host_b1_raw = 0;
    dut.host_diag = 0;
    dut.reg_wr = 0;
    dut.reg_rd = 0;
    dut.reg_addr = 0;
    dut.reg_wdata = 0;
    dut.gb_data_in = 0;
    tick(dut);
    dut.reset_n = 1;
    tick(dut);

    check("reset returns abstract safe idle", pins_safe(dut));
    check("writes structurally absent", dut.writes_enabled == 0);
    check("ID signature CBR1", read_reg(dut, 0) == 0x31424746u);
    check("GB/GBC read capability present",
          (read_reg(dut, 1) & 0x101u) == 0x101u);
    check("GB spaces expose ROM and save",
          ((read_reg(dut, 1) >> 16) & 0xFu) == 0x3u);
    check("GBA space capability absent",
          ((read_reg(dut, 1) >> 20) & 0xFu) == 0);
    check("GBA capability absent", (read_reg(dut, 1) & 0x2u) == 0);
    check("space resets to ROM", read_reg(dut, 16) == 0);

    // Discovery commands use no protocol-level cartridge signals.
    write_reg(dut, 14, 0);
    write_reg(dut, 3, 0xA1);
    check("ChisBurner query signature", read_reg(dut, 15) == 0x50424746u);
    write_reg(dut, 14, 1);
    write_reg(dut, 3, 0xA1);
    check("query protocol v1.0", read_reg(dut, 15) == 0x00010000u);
    write_reg(dut, 14, 2);
    write_reg(dut, 3, 0xA1);
    check("query feature flags", read_reg(dut, 15) == 0x00000003u);
    write_reg(dut, 14, 0x13579BDFu);
    write_reg(dut, 3, 0xFE);
    check("ping returns complement", read_reg(dut, 15) == 0xECA86420u);
    check("discovery leaves bus safe", pins_safe(dut));

    // Lifecycle authorization is required before any select or read strobe.
    configure_read(dut, 1, 0, 0x0100, 4, 100);
    write_reg(dut, 3, 2);
    check("lifecycle failure stays safe", pins_safe(dut));
    check("lifecycle error reported", read_reg(dut, 10) == 3);

    // Valid four-byte read over the abstract GB/GBC bus.
    dut.lifecycle_ok = 1;
    configure_read(dut, 1, 0, 0x0100, 4, 200);
    write_reg(dut, 3, 2);

    uint16_t addresses[4] = {0, 0, 0, 0};
    int read_windows = 0;
    bool previous_read = false;
    bool write_never_asserted = true;
    bool data_never_output = true;
    bool rom_cs_never_asserted = true;
    for (int cycle = 0; cycle < 180 && (read_reg(dut, 2) & 4u) == 0;
         ++cycle) {
        bool read_active = dut.gb_rd_n == 0;
        if (read_active && !previous_read && read_windows < 4)
            addresses[read_windows++] = dut.gb_address;
        previous_read = read_active;
        dut.gb_data_in = static_cast<uint8_t>((dut.gb_address & 0xFFu) ^ 0x5Au);
        if (dut.gb_wr_n == 0) write_never_asserted = false;
        if (dut.gb_data_dir != 0) data_never_output = false;
        if (dut.gb_cs_n == 0) rom_cs_never_asserted = false;
        tick(dut);
    }

    check("four read windows observed", read_windows == 4);
    check("byte addresses increment exactly",
          addresses[0] == 0x0100 && addresses[1] == 0x0101 &&
          addresses[2] == 0x0102 && addresses[3] == 0x0103);
    check("WR# never asserted", write_never_asserted);
    check("data bus never drives output", data_never_output);
    check("ROM read leaves CS# inactive", rom_cs_never_asserted);
    check("read completes in safe idle", pins_safe(dut));
    check("DONE status reported", (read_reg(dut, 2) & 4u) != 0);
    check("four RX bytes available", read_reg(dut, 9) == 4);
    check("RX data preserves byte order",
          read_reg_pop(dut, 8) == 0x59585B5Au);
    check("RX pop drains FIFO", read_reg(dut, 9) == 0);

    // Save-memory reads use the same address/data buses but assert CS# around
    // each RD# window.  Writes remain structurally impossible.
    configure_read(dut, 1, 1, 0xA000, 4, 200);
    write_reg(dut, 3, 2);
    uint16_t save_addresses[4] = {0, 0, 0, 0};
    int save_read_windows = 0;
    previous_read = false;
    bool save_cs_asserted_for_every_read = true;
    for (int cycle = 0; cycle < 180 && (read_reg(dut, 2) & 4u) == 0;
         ++cycle) {
        bool read_active = dut.gb_rd_n == 0;
        if (read_active && dut.gb_cs_n != 0)
            save_cs_asserted_for_every_read = false;
        if (read_active && !previous_read && save_read_windows < 4)
            save_addresses[save_read_windows++] = dut.gb_address;
        previous_read = read_active;
        dut.gb_data_in = static_cast<uint8_t>((dut.gb_address & 0xFFu) ^ 0xA5u);
        tick(dut);
    }
    check("four save read windows observed", save_read_windows == 4);
    check("save addresses increment exactly",
          save_addresses[0] == 0xA000 && save_addresses[1] == 0xA001 &&
          save_addresses[2] == 0xA002 && save_addresses[3] == 0xA003);
    check("save read asserts CS# with RD#", save_cs_asserted_for_every_read);
    check("save read completes in safe idle", pins_safe(dut));
    check("save RX data preserves byte order",
          read_reg_pop(dut, 8) == 0xA6A7A4A5u);
    check("save RX pop drains FIFO", read_reg(dut, 9) == 0);

    // Address and space must agree.  A native address in the other space, or
    // a space unsupported by GB/GBC, is rejected before RD#/CS# can assert.
    configure_read(dut, 1, 0, 0xA000, 1, 100);
    write_reg(dut, 3, 2);
    check("ROM space rejects save window",
          read_reg(dut, 10) == 1 && pins_safe(dut));
    configure_read(dut, 1, 1, 0x0100, 1, 100);
    write_reg(dut, 3, 2);
    check("save space rejects ROM window",
          read_reg(dut, 10) == 1 && pins_safe(dut));
    configure_read(dut, 1, 2, 0x0100, 1, 100);
    write_reg(dut, 3, 2);
    check("unsupported GB space rejected",
          read_reg(dut, 10) == 1 && pins_safe(dut));

    // Malformed requests never leave safe idle.
    configure_read(dut, 2, 0, 0x0100, 1, 100);
    write_reg(dut, 3, 2);
    check("wrong bus rejected", read_reg(dut, 10) == 1 && pins_safe(dut));
    configure_read(dut, 1, 0, 0x0100, 0, 100);
    write_reg(dut, 3, 2);
    check("zero length rejected", read_reg(dut, 10) == 1 && pins_safe(dut));
    configure_read(dut, 1, 0, 0x0100, 33, 100);
    write_reg(dut, 3, 2);
    check("oversize read rejected", read_reg(dut, 10) == 1 && pins_safe(dut));
    configure_read(dut, 1, 0, 0x8000, 1, 100);
    write_reg(dut, 3, 2);
    check("unmapped range rejected", read_reg(dut, 10) == 1 && pins_safe(dut));
    configure_read(dut, 1, 0, 0x7FFF, 2, 100);
    write_reg(dut, 3, 2);
    check("ROM boundary crossing rejected",
          read_reg(dut, 10) == 1 && pins_safe(dut));
    configure_read(dut, 1, 1, 0xBFFF, 2, 100);
    write_reg(dut, 3, 2);
    check("save boundary crossing rejected",
          read_reg(dut, 10) == 1 && pins_safe(dut));
    configure_read(dut, 1, 0, 0xFFFF, 2, 100);
    write_reg(dut, 3, 2);
    check("address wrap rejected", read_reg(dut, 10) == 1 && pins_safe(dut));

    // Abort and watchdog expiry both release all protocol outputs immediately.
    configure_read(dut, 1, 0, 0x0200, 4, 100);
    write_reg(dut, 3, 2);
    tick(dut);
    write_reg(dut, 3, 3);
    check("abort immediately safe", pins_safe(dut));
    check("abort status reported", (read_reg(dut, 2) & 0x10u) != 0);

    configure_read(dut, 1, 0, 0x0200, 4, 2);
    write_reg(dut, 3, 2);
    for (int i = 0; i < 5; ++i) tick(dut);
    check("timeout immediately safe", pins_safe(dut));
    check("timeout status reported", (read_reg(dut, 2) & 0x20u) != 0);

    // Lifecycle loss and reset dominate an active transaction.
    configure_read(dut, 1, 0, 0x0200, 4, 100);
    write_reg(dut, 3, 2);
    tick(dut);
    dut.lifecycle_ok = 0;
    tick(dut);
    check("lifecycle loss immediately safe", pins_safe(dut));
    check("lifecycle loss reports error", read_reg(dut, 10) == 3);

    dut.lifecycle_ok = 1;
    configure_read(dut, 1, 0, 0x0200, 4, 100);
    write_reg(dut, 3, 2);
    tick(dut);
    dut.reset_n = 0;
    tick(dut);
    check("reset during read is safe", pins_safe(dut));

    if (failures) {
        std::printf("GB/GBC abstract read-only: %d failure(s)\n", failures);
        return 1;
    }
    std::printf("GB/GBC abstract read-only: PASS\n");
    return 0;
}
