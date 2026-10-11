#include "Vchisburner_cart_auto98.h"

#include <cstdint>
#include <cstdio>
#include <stdexcept>

static uint8_t selected_bank = 1;
static bool previous_wr = false;
static bool previous_rd = false;
static unsigned mapper_write_windows = 0;
static uint64_t physical_read_windows = 0;
static bool expect_discard = true;

static void check(bool ok, const char *what) {
    if (!ok) throw std::runtime_error(what);
}

static uint8_t rom_byte(uint32_t logical) {
    return static_cast<uint8_t>((logical * 13u) ^ (logical >> 8) ^
                                (logical >> 14) ^ 0x5au);
}

static uint8_t effective_bank(uint8_t written) {
#ifdef MODEL_MBC3
    return written == 0 ? 1 : static_cast<uint8_t>(written & 0x7fu);
#else
    return written;
#endif
}

static void observe(Vchisburner_cart_auto98 &dut) {
    bool wr = (dut.bank0_out & 4u) == 0;
    bool rd = (dut.bank0_out & 2u) == 0;
    check(!(wr && rd), "/WR and /RD active together");
    check(dut.writes_enabled == 0, "destructive capability asserted");
    check(dut.pin30_dir == 0 && dut.pin30_pwroff_reset == 1,
          "GB reset safety changed");
    if (wr) {
        uint16_t a = static_cast<uint16_t>(dut.bank3_out) |
                     (static_cast<uint16_t>(dut.bank2_out) << 8);
        check(dut.bank3_dir && dut.bank2_dir && dut.bank1_dir,
              "mapper write directions wrong");
        check(a == 0x2000, "write escaped mapper low-bank register");
        check(dut.bank1_out == 0 || dut.bank1_out == 1,
              "mapper write data escaped zero/one probe");
        if (!previous_wr) {
            selected_bank = effective_bank(dut.bank1_out);
            ++mapper_write_windows;
        }
    }
    if (rd) {
        check(dut.bank3_dir && dut.bank2_dir && !dut.bank1_dir,
              "ROM read directions wrong");
        uint16_t a = static_cast<uint16_t>(dut.bank3_out) |
                     (static_cast<uint16_t>(dut.bank2_out) << 8);
        check(a <= 0x7fff, "read escaped GB ROM windows");
        uint32_t logical = a < 0x4000 ? a :
                           static_cast<uint32_t>(selected_bank) * 0x4000u +
                           (a - 0x4000u);
        if (!previous_rd) ++physical_read_windows;
        uint8_t value = rom_byte(logical);
        if (physical_read_windows <= 32) value ^= 0xffu;
        dut.gb_data_in = value;
    }
    previous_wr = wr;
    previous_rd = rd;
}

static void tick(Vchisburner_cart_auto98 &dut) {
    dut.clk=0; dut.eval(); observe(dut);
    dut.clk=1; dut.eval(); observe(dut);
}

static uint32_t reg_read(Vchisburner_cart_auto98 &dut, unsigned a) {
    dut.reg_rd=0; dut.reg_addr=a; tick(dut); dut.reg_rd=1; dut.eval();
    if(a==2)tick(dut); return dut.reg_rdata;
}

static void reg_write(Vchisburner_cart_auto98 &dut,
                      unsigned a, uint32_t v) {
    dut.reg_rd=0; dut.reg_addr=a; dut.reg_wdata=v; dut.reg_wr=1; tick(dut); dut.reg_wr=0; tick(dut);
}

static bool safe_idle(Vchisburner_cart_auto98 &dut) {
    return !dut.bank3_dir && !dut.bank2_dir && !dut.bank1_dir &&
           dut.bank0_dir && dut.bank0_out == 0xfu &&
           !dut.pin30_dir && !dut.pin31_dir && !dut.writes_enabled;
}

static void reset(Vchisburner_cart_auto98 &dut) {
    dut.reset_n=0; dut.reg_wr=0; dut.reg_rd=0; dut.lifecycle_ok=1;
    dut.lifecycle_status=0x3fu; dut.host_b1_raw=0; dut.host_diag=0;
    dut.gb_data_in=0; dut.bank2_in=0; selected_bank=1;
    previous_wr=false; previous_rd=false; mapper_write_windows=0;
    physical_read_windows=0; expect_discard=true;
    tick(dut); dut.reset_n=1; tick(dut);
    check(safe_idle(dut), "reset not safe idle");
}

static void begin_read(Vchisburner_cart_auto98 &dut,
                       uint32_t address, uint32_t length=32,
                       uint32_t bus=1, uint32_t space=5) {
    reg_write(dut,4,bus); reg_write(dut,16,space);
    reg_write(dut,5,address); reg_write(dut,6,length); reg_write(dut,7,100000);
    reg_write(dut,3,2);
    (void)reg_read(dut,2); // Real OS polls status, including rejected commands.
}

static void read_block(Vchisburner_cart_auto98 &dut,
                       uint32_t request, uint32_t logical) {
    uint64_t before=physical_read_windows;
    begin_read(dut,request);
    unsigned cycles=0;
    while ((reg_read(dut,2)&4u)==0 && cycles++<100000) tick(dut);
    check(cycles<100000,"command timeout");
    uint64_t expect_reads=request==0 && expect_discard ? 64u : 32u;
    check(physical_read_windows-before==expect_reads,
          "wrong physical read/discard count");
    if (request==0 && expect_discard) expect_discard=false;
    check(selected_bank==1,"bank one not restored");
    check(safe_idle(dut),"command completion not safe idle");
    check(reg_read(dut,9)==32,"wrong FIFO level");
    for (unsigned word=0;word<8;++word) {
        uint32_t data=reg_read(dut,8);
        for (unsigned lane=0;lane<4;++lane) {
            unsigned i=word*4+lane;
            check(static_cast<uint8_t>(data>>(lane*8))==rom_byte(logical+i),
                  "mapped ROM byte mismatch");
        }
        dut.reg_addr=8; dut.reg_rd=1; tick(dut); dut.reg_rd=0;
    }
}

int main() {
    static const uint32_t request[11]={
        0x0000,0x0020,0x4000,0x4020,0x0000,0x0020,
        0x4000,0x4020,0x0100,0x0120,0x0140};
    Vchisburner_cart_auto98 dut;
    reset(dut);
    check(reg_read(dut,1)==0x02313f03u,"caps changed");

    begin_read(dut,0x20);
    check(reg_read(dut,10)==1 && safe_idle(dut),"out-of-sequence accepted");
    begin_read(dut,0,16);
    check(reg_read(dut,10)==1 && safe_idle(dut),"short read accepted");
    begin_read(dut,0,32,1,1);
    check(reg_read(dut,10)!=0 && safe_idle(dut),"non-ROM space accepted");

    for (unsigned i=0;i<11;++i) {
        uint32_t logical=request[i];
        if (request[i]>=0x4000) {
#ifdef MODEL_MBC3
            logical=0x4000u+(request[i]-0x4000u);
#else
            logical=request[i]-0x4000u;
#endif
        }
        read_block(dut,request[i],logical);
    }
    check(mapper_write_windows==8,"wrong select/restore count");
    begin_read(dut,0);
    check(reg_read(dut,10)==1 && safe_idle(dut),"read budget escaped");

    // Refresh and Analyze both begin with SAFE.  A second complete mapper
    // trace must start at index zero without restarting the core.
    reg_write(dut,3,1); expect_discard=true;
    check(safe_idle(dut),"SAFE refresh boundary unsafe");
    for (unsigned i=0;i<11;++i) {
        uint32_t logical=request[i];
        if (request[i]>=0x4000) {
#ifdef MODEL_MBC3
            logical=0x4000u+(request[i]-0x4000u);
#else
            logical=request[i]-0x4000u;
#endif
        }
        read_block(dut,request[i],logical);
    }
    check(mapper_write_windows==16,"refresh did not restart mapper sequence");

    reset(dut); begin_read(dut,0);
    while ((dut.bank0_out&2u)!=0) tick(dut);
    reg_write(dut,3,3);
    tick(dut); // local per-engine write-dispatch stage
    check(safe_idle(dut) && (reg_read(dut,2)&0x10u),"abort unsafe");

    reset(dut); begin_read(dut,0);
    while ((dut.bank0_out&2u)!=0) tick(dut);
    dut.lifecycle_ok=0; tick(dut);
    check(safe_idle(dut),"lifecycle fail-safe broken");

#ifdef MODEL_MBC3
    std::puts("mapper-auto93 RTL: MBC3 zero->one behavior + repeat/header + safety PASS");
#else
    std::puts("mapper-auto93 RTL: MBC5 zero-bank behavior + repeat/header + safety PASS");
#endif
}
