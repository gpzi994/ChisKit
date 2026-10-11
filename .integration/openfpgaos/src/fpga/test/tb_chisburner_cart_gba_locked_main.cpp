#include "Vchisburner_cart_gba_readonly.h"

#include <cstdint>
#include <cstdio>

static int failures;

static void check(const char *name, bool ok) {
    std::printf("  %-36s %s\n", name, ok ? "OK" : "FAIL");
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

    check("default parameter is safe idle", pins_safe(dut));
    check("readable bus capability is clear", (read_reg(dut, 1) & 3u) == 0);
    check("space capabilities are clear",
          (read_reg(dut, 1) & 0x00FF0000u) == 0);
    check("writes remain structurally absent", dut.writes_enabled == 0);
    check("space resets to ROM", read_reg(dut, 16) == 0);

    write_reg(dut, 4, 2);
    write_reg(dut, 16, 0);
    write_reg(dut, 5, 0x100);
    write_reg(dut, 6, 2);
    write_reg(dut, 7, 100);
    write_reg(dut, 3, 2);
    check("READ is rejected while locked", read_reg(dut, 10) == 1);
    check("rejected READ emits no strobe", pins_safe(dut));

    write_reg(dut, 14, 0x13579BDFu);
    write_reg(dut, 3, 0xFE);
    check("safe PING remains available", read_reg(dut, 15) == 0xECA86420u);
    check("PING emits no strobe", pins_safe(dut));

    if (failures) {
        std::printf("GBA production read lock: %d failure(s)\n", failures);
        return 1;
    }
    std::printf("GBA production read lock: PASS\n");
    return 0;
}
