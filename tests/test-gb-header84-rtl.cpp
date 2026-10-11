#include "Vchisburner_cart_gb_readonly.h"

#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <vector>

static void check(bool ok, const char *what) {
    if (!ok)
        throw std::runtime_error(what);
}

static uint8_t rom_byte(uint16_t address) {
    return static_cast<uint8_t>((address ^ 0xA5u) + (address >> 8));
}

static void check_write_interlock(Vchisburner_cart_gb_readonly &dut) {
    check((dut.bank0_out & 0x4u) != 0, "GB /WR asserted");
    check(dut.bank1_dir == 0, "GB data bus drove cartridge");
    check(dut.writes_enabled == 0, "top-level write enable asserted");
    check(dut.pin30_dir == 0, "GB /RESET pin was driven");
    check(dut.pin30_pwroff_reset == 1, "GB reset sideband not released");
}

static void tick(Vchisburner_cart_gb_readonly &dut) {
    dut.clk = 0;
    dut.eval();
    check_write_interlock(dut);
    if ((dut.bank0_out & 0x2u) == 0 && dut.bank3_dir && dut.bank2_dir) {
        uint16_t address = static_cast<uint16_t>(dut.bank3_out) |
                           (static_cast<uint16_t>(dut.bank2_out) << 8);
        dut.bank3_in = rom_byte(address);
    }
    dut.clk = 1;
    dut.eval();
    check_write_interlock(dut);
}

static uint32_t reg_read(Vchisburner_cart_gb_readonly &dut, unsigned address) {
    dut.reg_addr = address;
    dut.reg_rd = 0;
    dut.eval();
    return dut.reg_rdata;
}

static void reg_write(Vchisburner_cart_gb_readonly &dut, unsigned address,
                      uint32_t value) {
    dut.reg_addr = address;
    dut.reg_wdata = value;
    dut.reg_wr = 1;
    tick(dut);
    dut.reg_wr = 0;
}

static bool safe_idle(Vchisburner_cart_gb_readonly &dut) {
    return dut.bank3_dir == 0 && dut.bank2_dir == 0 &&
           dut.bank1_dir == 0 && dut.bank0_dir == 1 &&
           dut.bank0_out == 0xFu && dut.pin30_dir == 0 &&
           dut.pin31_dir == 0 && dut.writes_enabled == 0;
}

static void reset(Vchisburner_cart_gb_readonly &dut) {
    dut.reset_n = 0;
    dut.reg_wr = 0;
    dut.reg_rd = 0;
    dut.lifecycle_ok = 1;
    dut.lifecycle_status = 0x3Fu;
    dut.host_b1_raw = 0;
    dut.host_diag = 0;
    dut.bank3_in = 0;
    dut.bank2_in = 0;
    tick(dut);
    dut.reset_n = 1;
    tick(dut);
    check(safe_idle(dut), "reset did not reach safe idle");
}

static void begin_read(Vchisburner_cart_gb_readonly &dut, uint32_t address,
                       uint32_t length, uint32_t bus = 1,
                       uint32_t space = 0, uint32_t timeout = 10000) {
    reg_write(dut, 3, 1);       // SAFE_IDLE
    reg_write(dut, 4, bus);
    reg_write(dut, 16, space);
    reg_write(dut, 5, address);
    reg_write(dut, 6, length);
    reg_write(dut, 7, timeout);
    reg_write(dut, 3, 2);       // READ
}

static std::vector<uint8_t> read_block(Vchisburner_cart_gb_readonly &dut,
                                       uint16_t address, uint32_t length) {
    begin_read(dut, address, length);
    unsigned cycles = 0;
    unsigned rd_windows = 0;
    bool previous_rd = false;
    while ((reg_read(dut, 2) & 0x4u) == 0 && cycles++ < 10000) {
        bool rd = (dut.bank0_out & 0x2u) == 0;
        if (rd && !previous_rd)
            rd_windows++;
        previous_rd = rd;
        tick(dut);
    }
    check(cycles < 10000, "read did not finish");
    check(rd_windows == length, "wrong number of /RD windows");
    check(reg_read(dut, 9) == length, "wrong FIFO level");
    check(safe_idle(dut), "read did not return to safe idle");

    std::vector<uint8_t> out;
    while (out.size() < length) {
        uint32_t word = reg_read(dut, 8);
        uint32_t take = length - static_cast<uint32_t>(out.size());
        if (take > 4)
            take = 4;
        for (uint32_t i = 0; i < take; ++i)
            out.push_back(static_cast<uint8_t>(word >> (8u * i)));
        dut.reg_addr = 8;
        dut.reg_rd = 1;
        tick(dut);
        dut.reg_rd = 0;
    }
    check(reg_read(dut, 9) == 0, "FIFO did not drain");
    for (uint32_t i = 0; i < length; ++i)
        check(out[i] == rom_byte(static_cast<uint16_t>(address + i)),
              "read data/address mismatch");
    return out;
}

int main() {
    Vchisburner_cart_gb_readonly dut;
    reset(dut);
    check(reg_read(dut, 1) == 0x00010F01u, "wrong ROM-only capabilities");

    // OS and RTL boundaries agree: no arbitrary Bank 0 reads, save reads,
    // GBA bus operations, zero length, or over-sized commands.
    for (uint32_t address : {0u, 0x00FFu, 0x0110u, 0x0150u}) {
        begin_read(dut, address, 16);
        check(reg_read(dut, 10) == 1 && safe_idle(dut),
              "forbidden address accepted");
    }
    begin_read(dut, 0x0100, 32, 2);
    check(reg_read(dut, 10) == 1 && safe_idle(dut), "GBA bus accepted");
    begin_read(dut, 0x0100, 32, 1, 1);
    check(reg_read(dut, 10) == 1 && safe_idle(dut), "save space accepted");
    begin_read(dut, 0x0100, 0);
    check(reg_read(dut, 10) == 1 && safe_idle(dut), "zero read accepted");

    for (unsigned pass = 0; pass < 2; ++pass) {
        read_block(dut, 0x0100, 32);
        read_block(dut, 0x0120, 32);
        read_block(dut, 0x0140, 16);
    }

    begin_read(dut, 0x0100, 32);
    check(reg_read(dut, 10) == 1 && safe_idle(dut),
          "seventh read escaped one-shot budget");

    reset(dut);
    begin_read(dut, 0x0100, 32);
    tick(dut);
    dut.lifecycle_ok = 0;
    tick(dut);
    check(safe_idle(dut) && reg_read(dut, 10) == 3,
          "lifecycle loss did not fail safely");

    reset(dut);
    begin_read(dut, 0x0100, 32);
    reg_write(dut, 3, 3);       // ABORT
    check(safe_idle(dut) && (reg_read(dut, 2) & 0x10u),
          "abort did not return safe idle");

    std::puts("GB header84 RTL: fixed Bank 0 chunks x2 + exact budget + "
              "data/address + lifecycle/abort + permanent write interlock PASS");
}
