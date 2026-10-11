#include "Vchisburner_cart_safe_idle.h"
#include "verilated.h"

#include <cstdint>
#include <cstdio>

static int failures = 0;

static void check(const char *name, uint32_t got, uint32_t expected) {
    if (got != expected) {
        std::printf("FAIL %-24s got=0x%x expected=0x%x\n",
                    name, got, expected);
        ++failures;
    }
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Vchisburner_cart_safe_idle dut;
    dut.eval();

    check("bank3-out", dut.bank3_out, 0x00);
    check("bank3-input", dut.bank3_dir, 0);
    check("bank2-out", dut.bank2_out, 0x00);
    check("bank2-input", dut.bank2_dir, 0);
    check("bank1-out", dut.bank1_out, 0x00);
    check("bank1-input", dut.bank1_dir, 0);
    check("controls-inactive", dut.bank0_out, 0x0F);
    check("controls-driven", dut.bank0_dir, 1);
    check("pin30-out", dut.pin30_out, 1);
    check("pin30-input", dut.pin30_dir, 0);
    check("pin30-sideband-off", dut.pin30_pwroff_reset, 0);
    check("pin31-out", dut.pin31_out, 0);
    check("pin31-input", dut.pin31_dir, 0);

    dut.final();
    if (failures != 0) {
        std::printf("chisburner safe-idle: %d failure(s)\n", failures);
        return 1;
    }

    std::printf("chisburner safe-idle: PASS\n");
    return 0;
}
