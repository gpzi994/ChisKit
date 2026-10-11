#include "Vchisburner_cart_gba_readonly.h"

#include <cstdint>
#include <cstdio>

static int failures;

static void check(const char *name, bool ok) {
    std::printf("  %-38s %s\n", name, ok ? "OK" : "FAIL");
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

static bool pins_safe(const Vchisburner_cart_gba_readonly &dut) {
    return dut.bank3_dir == 0 && dut.bank2_dir == 0 &&
           dut.bank1_dir == 0 && dut.bank0_dir == 1 &&
           dut.bank0_out == 0xF && dut.pin30_dir == 0 &&
           dut.pin30_pwroff_reset == 0 && dut.pin31_dir == 0 &&
           dut.writes_enabled == 0;
}

static bool run_header_read(Vchisburner_cart_gba_readonly &dut) {
    write_reg(dut, 3, 1);       // SAFE_IDLE
    write_reg(dut, 4, 2);       // GBA
    write_reg(dut, 16, 0);      // ROM
    write_reg(dut, 5, 0xA0);    // fixed header window
    write_reg(dut, 6, 32);
    write_reg(dut, 7, 4000);
    write_reg(dut, 3, 2);       // READ

    bool wr_inactive = true;
    bool cs2_inactive = true;
    int read_windows = 0;
    bool previous_read = false;
    for (int cycle = 0; cycle < 3000 && (read_reg(dut, 2) & 4u) == 0;
         ++cycle) {
        bool read_active = dut.bank0_dir && ((dut.bank0_out & 0x3u) == 0u);
        if (read_active && !previous_read) ++read_windows;
        previous_read = read_active;
        dut.bank3_in = static_cast<uint8_t>(0x40 + read_windows);
        dut.bank2_in = static_cast<uint8_t>(0x80 + read_windows);
        if ((dut.bank0_out & 0x4u) == 0) wr_inactive = false;
        if (dut.pin30_dir && dut.pin30_out == 0) cs2_inactive = false;
        tick(dut);
    }
    return (read_reg(dut, 2) & 4u) != 0 && read_reg(dut, 9) == 32 &&
           read_windows == 16 && wr_inactive && cs2_inactive && pins_safe(dut);
}

int main() {
    Vchisburner_cart_gba_readonly dut;
    dut.clk = 0;
    dut.reset_n = 0;
    dut.lifecycle_ok = 1;
    dut.lifecycle_status = 0;
    dut.host_b1_raw = 0;
    dut.host_diag = 0;
    dut.reg_wr = 0;
    dut.reg_rd = 0;
    dut.reg_addr = 0;
    dut.reg_wdata = 0;
    dut.bank3_in = 0;
    dut.bank2_in = 0;
    tick(dut);
    dut.reset_n = 1;
    tick(dut);

    check("probe reset is safe idle", pins_safe(dut));
    check("probe advertises GBA ROM only",
          (read_reg(dut, 1) & 0x00F00103u) == 0x00100102u);
    check("writes structurally absent", dut.writes_enabled == 0);

    write_reg(dut, 4, 2);
    write_reg(dut, 16, 0);
    write_reg(dut, 5, 0x80);
    write_reg(dut, 6, 32);
    write_reg(dut, 7, 4000);
    write_reg(dut, 3, 2);
    check("address below A0 rejected safely",
          read_reg(dut, 10) == 1 && pins_safe(dut));

    write_reg(dut, 5, 0xA0);
    write_reg(dut, 6, 16);
    write_reg(dut, 3, 2);
    check("shorter header read rejected safely",
          read_reg(dut, 10) == 1 && pins_safe(dut));

    check("first fixed header read completes", run_header_read(dut));
    check("second fixed header read completes", run_header_read(dut));

    write_reg(dut, 3, 1);
    write_reg(dut, 4, 2);
    write_reg(dut, 16, 0);
    write_reg(dut, 5, 0xA0);
    write_reg(dut, 6, 32);
    write_reg(dut, 7, 4000);
    write_reg(dut, 3, 2);
    check("third read rejected by hardware limit",
          read_reg(dut, 10) == 1 && pins_safe(dut));

    if (failures) {
        std::printf("GBA fixed header probe: %d failure(s)\n", failures);
        return 1;
    }
    std::printf("GBA fixed header probe: PASS\n");
    return 0;
}
