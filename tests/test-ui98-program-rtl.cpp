#include "Vchisburner_cart_auto98.h"
#include <array>
#include <cstdint>
#include <cstdio>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#define ck(value) do { if (!(value)) { \
    std::fprintf(stderr, "program65 RTL assertion line %d\n", __LINE__); \
    throw std::runtime_error("program65 RTL assertion"); \
} } while (0)

struct FlashModel {
    enum Mode { NORMAL, AUTOSELECT, CFI } mode = NORMAL;
    enum PollFault { NO_FAULT, PROGRAM_BUSY_ONCE, PROGRAM_WB_ABORT, PROGRAM_DQ5_FAIL,
                     ERASE_DQ5_FAIL } poll_fault = NO_FAULT;
    bool supported = true;
    bool complete_erase = true;
    uint16_t cfi_command_set = 0x0002;
    uint16_t cfi_buffer_exp = 0x0009;
    bool operation_fault_active = false;
    unsigned unlock = 0;
    unsigned erase_unlock = 0;
    bool buffer_mode = false;
    bool buffer_count_seen = false;
    uint32_t buffer_sector = 0;
    unsigned buffer_words_left = 0;
    uint32_t address = 0;
    bool wr_active = false;
    std::vector<std::pair<uint32_t, uint16_t>> writes;
    std::vector<uint32_t> reads;
    std::map<uint32_t, uint16_t> programmed;

    uint16_t read(uint32_t word) {
        reads.push_back(word);
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
            if (word == 0x13) return cfi_command_set;
            if (word == 0x27) return 0x0019;
            if (word == 0x2a) return cfi_buffer_exp;
            if (word == 0x2c) return 0x0001;
            if (word == 0x2d) return 0x00ff;
            if (word == 0x2e) return 0x0000;
            if (word == 0x2f) return 0x0000;
            if (word == 0x30) return 0x0002;
            return 0;
        }
        if (operation_fault_active) {
            if (poll_fault == PROGRAM_BUSY_ONCE) {
                operation_fault_active = false;
                poll_fault = NO_FAULT;
                return 0x0000;
            }
            if (poll_fault == PROGRAM_WB_ABORT) return 0x0002;
            if (poll_fault == PROGRAM_DQ5_FAIL) return 0x0020;
            if (poll_fault == ERASE_DQ5_FAIL) return 0x0020;
        }
        auto it = programmed.find(word);
        if (it != programmed.end()) return it->second;
        if (!complete_erase) return 0x0000;
        return 0xffff;
    }

    void write(uint32_t word, uint16_t data) {
        writes.push_back({word, data});
        if (data == 0x00f0) {
            mode = NORMAL; unlock = erase_unlock = 0;
            buffer_mode = buffer_count_seen = false;
            operation_fault_active = false;
            poll_fault = NO_FAULT;
            return;
        }
        if (!supported) return;
        if (word == 0x55 && data == 0x0098) {
            mode = CFI; unlock = erase_unlock = 0; return;
        }
        if (buffer_mode) {
            if (!buffer_count_seen) {
                ck(word == buffer_sector && data <= 255);
                buffer_count_seen = true; buffer_words_left = data + 1; return;
            }
            if (buffer_words_left) {
                programmed[word] = data; --buffer_words_left; return;
            }
            ck(word == buffer_sector && data == 0x0029);
            buffer_mode = buffer_count_seen = false;
            if (poll_fault == PROGRAM_BUSY_ONCE ||
                poll_fault == PROGRAM_WB_ABORT ||
                poll_fault == PROGRAM_DQ5_FAIL)
                operation_fault_active = true;
            return;
        }
        if (erase_unlock == 3 && word == 0x555 && data == 0x00aa) {
            erase_unlock = 4; return;
        }
        if (erase_unlock == 4 && word == 0x2aa && data == 0x0055) {
            erase_unlock = 5; return;
        }
        if (erase_unlock == 5 && data == 0x0030) {
            programmed.clear(); erase_unlock = 0;
            if (poll_fault == ERASE_DQ5_FAIL)
                operation_fault_active = true;
            return;
        }
        if (unlock == 0 && word == 0x555 && data == 0x00aa) {
            unlock = 1; return;
        }
        if (unlock == 1 && word == 0x2aa && data == 0x0055) {
            unlock = 2; return;
        }
        if (unlock == 2 && word == 0x555 && data == 0x0090) {
            mode = AUTOSELECT; unlock = 0; return;
        }
        if (unlock == 2 && word == 0x555 && data == 0x0080) {
            erase_unlock = 3; unlock = 0; return;
        }
        if (unlock == 2 && data == 0x0025) {
            buffer_mode = true; buffer_count_seen = false;
            buffer_sector = word; unlock = 0; return;
        }
        unlock = 0;
    }
};

static bool pins_safe(Vchisburner_cart_auto98 &d) {
    return !d.bank3_dir && !d.bank2_dir && !d.bank1_dir &&
           d.bank0_dir && d.bank0_out == 15 && !d.pin30_dir;
}

static void tick(Vchisburner_cart_auto98 &d, FlashModel &m) {
    d.clk = 0; d.eval();
    ck(!d.pin31_dir && (!d.pin30_dir || d.pin30_out));
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
        m.write(m.address, uint16_t(d.bank3_out) |
                          (uint16_t(d.bank2_out) << 8));
    m.wr_active = wr_now;
    d.clk = 1; d.eval();
}

static void wr(Vchisburner_cart_auto98 &d, FlashModel &m,
               unsigned addr, uint32_t value) {
    d.reg_rd = 0; d.reg_addr = addr; d.reg_wdata = value; d.reg_wr = 1;
    tick(d, m); d.reg_wr = 0; tick(d,m);
}

static FlashModel *current_model;
static uint32_t rd(Vchisburner_cart_auto98 &d, unsigned addr) {
    d.reg_rd = 0; d.reg_addr = addr; tick(d,*current_model);
    d.reg_rd = 1; d.eval();
    if(addr==2)tick(d,*current_model);
    return d.reg_rdata;
}

static void reset(Vchisburner_cart_auto98 &d, FlashModel &m) {
    current_model=&m;
    d.reset_n = 0; d.reg_wr = d.reg_rd = 0; d.lifecycle_ok = 1;
    d.lifecycle_status = 0x80000303u;
    tick(d, m); d.reset_n = 1; tick(d, m); ck(pins_safe(d));
    wr(d,m,4,2);
}

static void run(Vchisburner_cart_auto98 &d, FlashModel &m,
                unsigned limit = 200000) {
    unsigned cycles = 0;
    while ((rd(d, 2) & 2u) && cycles++ < limit) tick(d, m);
    ck(cycles < limit && pins_safe(d));
}

static void identify(Vchisburner_cart_auto98 &d, FlashModel &m) {
    wr(d, m, 4, 2); wr(d, m, 16, 4); wr(d, m, 5, 0);
    wr(d, m, 6, 32); wr(d, m, 7, 100000); wr(d, m, 3, 2);
    run(d, m);
    ck((rd(d, 2) & 4u) && rd(d, 9) == 32);
    for (unsigned i = 0; i < 8; ++i) {
        (void)rd(d, 8); d.reg_rd = 1; tick(d, m); d.reg_rd = 0;
    }
}

static void arm(Vchisburner_cart_auto98 &d, FlashModel &m) {
    wr(d, m, 19, 0x53323947u); wr(d, m, 19, 0x3635324cu);
    ck((rd(d, 20) & 2u) != 0);
}

static void load_program_payload(Vchisburner_cart_auto98 &d,
                                 FlashModel &m, unsigned length) {
    ck(length >= 2 && length <= 512 && (length & 3u) == 0);
    wr(d, m, 18, 0);
    for (unsigned i = 0; i < length / 4; ++i)
        wr(d, m, 17, ((4*i) & 0xff) | (((4*i+1) & 0xff) << 8) |
                         (((4*i+2) & 0xff) << 16) |
                         (((4*i+3) & 0xff) << 24));
    ck(rd(d, 18) == length);
}

int main() {
    Vchisburner_cart_auto98 d;
    FlashModel flash;
    reset(d, flash);
#ifdef PROGRAM131_TEST
    // Stream131 intentionally compiles out the legacy S29-only writer/FIFO
    // (cart_auto98 selected engine 3 becomes read-only). Check that boundary;
    // the private production stream is exercised by test-nor108-router.cpp.
    ck(rd(d,1)==0x02313f03u && !d.writes_enabled);
    auto before_disabled=flash.writes.size();
    wr(d,flash,5,0);wr(d,flash,6,512);wr(d,flash,7,100000);
    wr(d,flash,19,0x53323947u);wr(d,flash,19,0x3635324cu);
    wr(d,flash,3,5);run(d,flash);
    ck(!d.writes_enabled && flash.writes.size()==before_disabled && pins_safe(d));
    std::puts("PASS Stream131 legacy S29 writer compiled out: cannot program through old ABI");
    return 0;
#endif
    ck(rd(d, 1) == 0x02313f03u && rd(d, 23) == 512u &&
       !d.writes_enabled);

    // A correctly armed command still cannot write before hardware ID/CFI.
    std::size_t before = flash.writes.size();
    wr(d, flash, 5, 0); wr(d, flash, 6, 0x20000); wr(d, flash, 7, 100000);
    arm(d, flash); wr(d, flash, 3, 4);
    ck((rd(d, 2) & 8u) && rd(d, 10) == 5u);
    ck(flash.writes.size() == before && pins_safe(d));

    identify(d, flash);
    ck(d.writes_enabled && (rd(d, 20) & 1u));

    // Alignment/geometry is enforced before /WR can pulse.
    before = flash.writes.size();
    wr(d, flash, 5, 2); wr(d, flash, 6, 0x20000); wr(d, flash, 7, 100000);
    arm(d, flash); wr(d, flash, 3, 4);
    ck((rd(d, 2) & 8u) && rd(d, 10) == 7u && flash.writes.size() == before);

    // One 128 KiB sector erase; no chip-erase opcode 0x10 is ever generated.
    before = flash.writes.size();
    wr(d, flash, 5, 0x00040000); wr(d, flash, 6, 0x00020000);
    wr(d, flash, 7, 100000); arm(d, flash); wr(d, flash, 3, 4); run(d, flash);
    ck((rd(d, 2) & 4u) && flash.writes.size() == before + 7);
    const std::pair<uint32_t,uint16_t> erase_seq[] = {
        {0x555,0xaa},{0x2aa,0x55},{0x555,0x80},{0x555,0xaa},
        {0x2aa,0x55},{0x20000,0x30},{0,0xf0}
    };
    for (unsigned i = 0; i < 7; ++i) ck(flash.writes[before+i] == erase_seq[i]);

    // A destructive command consumes its arm; a repeat cannot pulse /WR.
    before = flash.writes.size(); wr(d, flash, 3, 4);
    ck((rd(d, 2) & 8u) && rd(d, 10) == 6u && flash.writes.size() == before);

    // Load exactly 512 bytes and execute one full AMD buffer-program sequence.
    load_program_payload(d, flash, 512);
    wr(d, flash, 5, 0x00040000); wr(d, flash, 6, 512);
    wr(d, flash, 7, 100000); arm(d, flash);
    before = flash.writes.size(); wr(d, flash, 3, 5); run(d, flash);
    ck((rd(d, 2) & 4u) && flash.writes.size() == before + 262);
    ck(flash.writes[before+0] == std::make_pair(0x555u, uint16_t(0xaa)));
    ck(flash.writes[before+1] == std::make_pair(0x2aau, uint16_t(0x55)));
    ck(flash.writes[before+2] == std::make_pair(0x20000u, uint16_t(0x25)));
    ck(flash.writes[before+3] == std::make_pair(0x20000u, uint16_t(255)));
    for (unsigned i = 0; i < 256; ++i)
        ck(flash.writes[before+4+i] ==
           std::make_pair(0x20000u+i,
                          uint16_t((2*i & 0xff) | (((2*i+1) & 0xff) << 8))));
    ck(flash.writes[before+260] == std::make_pair(0x20000u, uint16_t(0x29)));
    ck(flash.writes[before+261] == std::make_pair(0u, uint16_t(0xf0)));

    // A short final block is legal when it remains inside the same 512-byte
    // boundary; this is required by the 8,705,280-byte test ROM (256-byte tail).
    load_program_payload(d, flash, 256);
    wr(d, flash, 5, 0x00040200); wr(d, flash, 6, 256);
    wr(d, flash, 7, 100000); arm(d, flash);
    flash.poll_fault = FlashModel::PROGRAM_BUSY_ONCE;
    flash.reads.clear();
    before = flash.writes.size(); wr(d, flash, 3, 5); run(d, flash);
    ck((rd(d, 2) & 4u) && flash.writes.size() == before + 134);
    ck(flash.writes[before+3] == std::make_pair(0x20000u, uint16_t(127)));
    ck(flash.reads.size() >= 2 &&
       flash.reads[flash.reads.size()-2] == 0x2017fu &&
       flash.reads.back() == 0x2017fu);

    // The service rounds a 2-byte final transfer to one staged dword.  Only
    // the low half is programmed and its DQ7 controls completion polling.
    wr(d, flash, 18, 0);
    wr(d, flash, 17, 0xffff3412u);
    wr(d, flash, 5, 0x00040400); wr(d, flash, 6, 2);
    wr(d, flash, 7, 100000); arm(d, flash);
    before = flash.writes.size(); wr(d, flash, 3, 5); run(d, flash);
    ck((rd(d, 2) & 4u) && flash.writes.size() == before + 7);
    ck(flash.writes[before+3] == std::make_pair(0x20000u, uint16_t(0)));
    ck(flash.writes[before+4] ==
       std::make_pair(0x20200u, uint16_t(0x3412)));

    // A 512-byte write may not cross an internal 512-byte buffer boundary.
    load_program_payload(d, flash, 512);
    wr(d, flash, 5, 0x00040300); wr(d, flash, 6, 512);
    wr(d, flash, 7, 100000); arm(d, flash);
    before = flash.writes.size(); wr(d, flash, 3, 5);
    ck((rd(d, 2) & 8u) && rd(d, 10) == 7u &&
       flash.writes.size() == before);

    // DQ1 denotes write-buffer abort and must use the AMD AA/55/F0
    // three-cycle recovery sequence before software sees the failure.
    flash.poll_fault = FlashModel::PROGRAM_WB_ABORT;
    load_program_payload(d, flash, 512);
    wr(d, flash, 5, 0x00040400); wr(d, flash, 6, 512);
    wr(d, flash, 7, 100000); arm(d, flash);
    before = flash.writes.size(); wr(d, flash, 3, 5); run(d, flash);
    ck((rd(d, 2) & 8u) && rd(d, 10) == 10u);
    ck(flash.writes[flash.writes.size()-3] ==
       std::make_pair(0x555u, uint16_t(0xaa)));
    ck(flash.writes[flash.writes.size()-2] ==
       std::make_pair(0x2aau, uint16_t(0x55)));
    ck(flash.writes[flash.writes.size()-1] ==
       std::make_pair(0x555u, uint16_t(0xf0)));

    // DQ5 is re-read once; a persistent failure is reported only after reset.
    flash.poll_fault = FlashModel::PROGRAM_DQ5_FAIL;
    load_program_payload(d, flash, 512);
    wr(d, flash, 5, 0x00040600); wr(d, flash, 6, 512);
    wr(d, flash, 7, 100000); arm(d, flash);
    before = flash.writes.size(); wr(d, flash, 3, 5); run(d, flash);
    ck((rd(d, 2) & 8u) && rd(d, 10) == 9u);
    ck(flash.writes.back() == std::make_pair(0u, uint16_t(0xf0)));

    for (auto w : flash.writes) ck(w.second != 0x0010);

    // Removing the cartridge invalidates the cached profile and disarms.
    d.lifecycle_ok = 0; tick(d, flash); ck(!d.writes_enabled);
    d.lifecycle_ok = 1; tick(d, flash);wr(d,flash,4,2);tick(d,flash);
    before = flash.writes.size(); arm(d, flash); wr(d, flash, 3, 5);
    ck((rd(d, 2) & 8u) && rd(d, 10) == 5u && flash.writes.size() == before);

    // QRY alone is insufficient: the hardware gate rejects a non-AMD primary
    // command set even though the public 32-byte snapshot still completes.
    flash.cfi_command_set = 1;
    identify(d, flash);
    ck(!d.writes_enabled && !(rd(d, 20) & 1u));

    // GL-N reports a 32-byte write-buffer exponent (05h).  It must not pass
    // this exact GL-S target gate even though both families share the leading
    // manufacturer/device words; this programmer requires the 512-byte GL-S
    // buffer reported by CFI exponent 09h.
    flash.cfi_command_set = 2;
    flash.cfi_buffer_exp = 5;
    identify(d, flash);
    ck(!d.writes_enabled && !(rd(d, 20) & 1u));

    std::puts("program81 RTL: exact GL-S CFI gate + arm + sector erase + 512-byte/short-tail buffer program + boundary gate + DQ poll recovery + no chip erase PASS");
}
