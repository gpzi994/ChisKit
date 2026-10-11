#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "of_cart.h"
#include "of.h"

enum {
    D69_REG_STATUS = 2u,
    D69_REG_COMMAND = 3u,
    D69_REG_BUS = 4u,
    D69_REG_ADDRESS = 5u,
    D69_REG_LENGTH = 6u,
    D69_REG_TIMEOUT = 7u,
    D69_REG_RX_DATA = 8u,
    D69_REG_RX_LEVEL = 9u,
    D69_REG_ERROR = 10u,
    D69_REG_SPACE = 16u
};

enum {
    D69_CMD_SAFE_IDLE = 1u,
    D69_CMD_READ = 2u,
    D69_BUS_GBA = 2u,
    D69_SPACE_IDENTIFY = 4u,
    D69_STATUS_BUSY = 2u
};

static uint8_t raw69[32];
static uint32_t before_status69, before_error69;
static uint32_t after_status69, after_error69, after_level69, after_profile69;
static int pre_safe_rc69, post_safe_rc69;
static int direct_rc69;

#ifndef D69_BUILD_LABEL
#define D69_BUILD_LABEL "smoke.69"
#endif

#ifndef D69_PLATFORM_LINE
#define D69_PLATFORM_LINE "smoke.66 OS/RBF; IDENTIFY READ ONLY"
#endif

#ifndef D69_EXPECTED_BUFFER_EXP
#define D69_EXPECTED_BUFFER_EXP 0x05u
#endif

#ifndef OF_PC
#define D69_REG32(a) (*(volatile uint32_t *)(uintptr_t)(a))
#define D69_SYSREG_BASE 0x40000000u
#define D69_CART_BASE (D69_SYSREG_BASE + 0x1a0u)
#define D69_PROGRAM_PROFILE (D69_SYSREG_BASE + 0x190u)
#ifndef D69_SPACE_ADDRESS
#define D69_SPACE_ADDRESS (D69_CART_BASE + D69_REG_SPACE * 4u)
#endif

static uint32_t read69(unsigned reg) {
    return D69_REG32(D69_CART_BASE + reg * 4u);
}

static void write69(unsigned reg, uint32_t value) {
    D69_REG32(D69_CART_BASE + reg * 4u) = value;
}

static int run_direct69(void) {
    unsigned wait;
    uint32_t words;

    pre_safe_rc69 = of_cart_enter_safe_idle();
    before_status69 = read69(D69_REG_STATUS);
    before_error69 = read69(D69_REG_ERROR);

    /* This is the hardware-owned, bounded Identify READ transaction only. */
    write69(D69_REG_BUS, D69_BUS_GBA);
    D69_REG32(D69_SPACE_ADDRESS) = D69_SPACE_IDENTIFY;
    write69(D69_REG_ADDRESS, 0u);
    write69(D69_REG_LENGTH, sizeof(raw69));
    write69(D69_REG_TIMEOUT, 5000000u);
    write69(D69_REG_COMMAND, D69_CMD_READ);

    for (wait = 0; wait < 1000u; ++wait) {
        if ((read69(D69_REG_STATUS) & D69_STATUS_BUSY) == 0u) break;
        usleep(1000);
    }
    after_status69 = read69(D69_REG_STATUS);
    after_error69 = read69(D69_REG_ERROR);
    after_level69 = read69(D69_REG_RX_LEVEL);
    after_profile69 = D69_REG32(D69_PROGRAM_PROFILE);
    if (wait == 1000u) direct_rc69 = -1;
    else if (after_error69 != 0u) direct_rc69 = -2;
    else if (after_level69 != sizeof(raw69)) direct_rc69 = -3;
    else direct_rc69 = 0;

    words = after_level69 / 4u;
    if (words > 8u) words = 8u;
    memset(raw69, 0, sizeof(raw69));
    for (unsigned i = 0; i < words; ++i) {
        uint32_t value = read69(D69_REG_RX_DATA);
        raw69[i * 4u + 0u] = (uint8_t)value;
        raw69[i * 4u + 1u] = (uint8_t)(value >> 8);
        raw69[i * 4u + 2u] = (uint8_t)(value >> 16);
        raw69[i * 4u + 3u] = (uint8_t)(value >> 24);
    }
    write69(D69_REG_COMMAND, D69_CMD_SAFE_IDLE);
    post_safe_rc69 = of_cart_enter_safe_idle();
    return direct_rc69;
}
#else
static int run_direct69(void) {
    static const uint8_t fixture[32] = {
        0x2e,0x00,0x00,0x00,0xea,0x24,0xff,0xae,
        0x01,0x00,0x7e,0x22,0x00,0x00,0x00,0x00,
        0x51,0x00,0x52,0x00,0x59,0x00,0x02,0x00,
        0x19,0x00,0x00,0x00,0x00,0x00,D69_EXPECTED_BUFFER_EXP,0x00
    };
    memcpy(raw69, fixture, sizeof(raw69));
    before_status69 = 1u;
    after_status69 = 5u;
    after_level69 = 32u;
    after_profile69 = 1u;
    return 0;
}
#endif

static uint16_t word69(unsigned byte_offset) {
    return (uint16_t)raw69[byte_offset] |
           (uint16_t)((uint16_t)raw69[byte_offset + 1u] << 8);
}

static const char *lane69(void) {
    uint16_t q = word69(16u), r = word69(18u), y = word69(20u);
    if (q == 0x0051u && r == 0x0052u && y == 0x0059u) return "BANK3/LOW NATIVE";
    if (q == 0x5100u && r == 0x5200u && y == 0x5900u) return "BANK2/HIGH LANE";
    if (q == 0x0052u && r == 0x0051u && y == 0x005au) return "BANK3 D0/D1 SWAP";
    if (q == 0x5200u && r == 0x5100u && y == 0x5a00u) return "BANK2 D0/D1 SWAP";
    return "QRY UNKNOWN";
}

static void bytes69(unsigned start) {
    printf("%02X%02X %02X%02X %02X%02X %02X%02X\n",
           raw69[start], raw69[start + 1u],
           raw69[start + 2u], raw69[start + 3u],
           raw69[start + 4u], raw69[start + 5u],
           raw69[start + 6u], raw69[start + 7u]);
}

static void page69(unsigned page) {
    printf("\033[2J\033[H GBA RAW IDENT %s %u/4\n", D69_BUILD_LABEL, page);
    printf("%s\n", D69_PLATFORM_LINE);
    if (page == 1u) {
        printf("DIRECT RC %d PRE SAFE %d POST %d\n", direct_rc69, pre_safe_rc69, post_safe_rc69);
        printf("BEFORE ST %08lX ERR %08lX\n",
               (unsigned long)before_status69, (unsigned long)before_error69);
        printf("AFTER ST %08lX ERR %08lX\n",
               (unsigned long)after_status69, (unsigned long)after_error69);
        printf("RX LEVEL %lu PROFILE %08lX\n",
               (unsigned long)after_level69, (unsigned long)after_profile69);
        printf("NO ARM/ERASE/PROGRAM COMMANDS\n");
    } else if (page == 2u) {
        printf("RAW 32 BYTES; PAIRS ARE BUS ORDER\n");
        printf("BASE 0..3:\n"); bytes69(0u);
        printf("JEDEC ID 0..3:\n"); bytes69(8u);
        printf("CFI 10..13:\n"); bytes69(16u);
        printf("CFI 27..2A:\n"); bytes69(24u);
    } else if (page == 3u) {
        printf("DECODE AS 16-BIT LITTLE ENDIAN\n");
        printf("ID0 %04X ID1 %04X\n", word69(8u), word69(10u));
        printf("Q %04X R %04X Y %04X CMD %04X\n",
               word69(16u), word69(18u), word69(20u), word69(22u));
        printf("SIZE %04X W28 %04X W29 %04X BUF %04X\n",
               word69(24u), word69(26u), word69(28u), word69(30u));
        printf("LANE: %s\n", lane69());
    } else {
        uint16_t q = word69(16u), r = word69(18u), y = word69(20u);
        int public_ok = word69(8u) == 0x0001u && word69(10u) == 0x227eu &&
                        q == 0x0051u && r == 0x0052u && y == 0x0059u &&
                        word69(22u) == 0x0002u && word69(24u) == 0x0019u &&
                        word69(30u) == D69_EXPECTED_BUFFER_EXP;
        printf("DECISION\n");
        printf("PUBLIC FIXED CHECKS: %s\n", public_ok ? "PASS" : "FAIL");
        printf("HARDWARE PROFILE: %s\n", (after_profile69 & 1u) ? "PASS" : "FAIL");
        if (public_ok && !(after_profile69 & 1u))
            printf("FAIL IS HIDDEN CFI GEOMETRY\n");
        else if (!public_ok)
            printf("SEE PAGE 3 FOR FIXED FIELD MISMATCH\n");
        else
            printf("IDENTIFY PROFILE PASSES\n");
        printf("SEND PHOTOS OF ALL FOUR PAGES\n");
    }
    printf("LEFT/RIGHT: PAGE\n");
    fflush(stdout);
}

int main(void) {
    run_direct69();
#ifdef OF_PC
    for (unsigned page = 1; page <= 4u; ++page) page69(page);
    return 0;
#else
    unsigned page = 1u, previous = ~0u;
    page69(page);
    while (1) {
        of_input_poll_p0();
        unsigned held = _of_input_p0.buttons;
        unsigned edge = held & ~previous;
        previous = held;
        if (edge & OF_BTN_RIGHT) {
            page = page == 4u ? 1u : page + 1u;
            page69(page);
        } else if (edge & OF_BTN_LEFT) {
            page = page == 1u ? 4u : page - 1u;
            page69(page);
        }
        usleep(16000);
    }
#endif
}
