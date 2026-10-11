#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "of_cart.h"

#define READ_TIMEOUT_US 50000u
#define PROBE_COUNT 8u
#define BLOCK_BYTES 32u

static int failures;

static const uint32_t probe_address[PROBE_COUNT] = {
    0x00004000u, 0x00008000u, 0x0007C000u, 0x00080000u,
    0x000FC000u, 0x00100000u, 0x001FC000u, 0x001FFFE0u
};

static const char *probe_name[PROBE_COUNT] = {
    "B001 @4000", "B002 @4000", "B031 @4000", "B032 @4000",
    "B063 @4000", "B064 @4000", "B127 @4000", "B127 @7FE0"
};

static const uint8_t expected[PROBE_COUNT][BLOCK_BYTES] = {
    {0xFA,0x96,0xDB,0xC7,0x12,0x40,0x42,0x40,0x73,0x40,0x7F,0x40,0x9C,0x40,0xAA,0x40,
     0x25,0x58,0xCD,0xD6,0x44,0xF0,0xFE,0xA7,0x28,0x28,0x21,0x10,0xDC,0x0E,0x80,0xF3},
    {0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,
     0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06},
    {0xC3,0x09,0x40,0xC3,0x5C,0x7B,0xC3,0x1E,0x40,0x21,0x00,0xD3,0x36,0x00,0x2C,0x20,
     0xFB,0x3E,0x80,0xE0,0x26,0x3E,0x77,0xE0,0x24,0x3E,0xFF,0xE0,0x25,0xC9,0xCD,0x04},
    {0x34,0x6A,0x03,0x61,0x44,0x19,0x96,0x66,0x03,0xE3,0x7B,0x18,0xB2,0x69,0x03,0x28,
     0x53,0x03,0x49,0x52,0x03,0xDD,0x7B,0x07,0x66,0x79,0x18,0xE9,0x57,0x03,0x26,0x6A},
    {0x7C,0x5F,0xFA,0x80,0xCC,0xFE,0x01,0x28,0x24,0xFA,0x95,0xDB,0xFE,0x01,0x3E,0x7E,
     0xC2,0x15,0x40,0x3E,0x7F,0xEA,0xE8,0xFF,0xFA,0x64,0xC1,0xA7,0xFA,0x70,0xC1,0x20},
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
     0xFF,0x00,0x58,0x01,0x42,0x00,0x17,0x00,0x21,0x01,0xAB,0x01,0xAC,0x00,0x37,0x03},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x77,0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x7B,0x7A,0x5E,0x2D,0x5E,0x2D,0x62,0x35,
     0x62,0x35,0x5E,0x2D,0x5E,0x2D,0x62,0x35,0x62,0x35,0x62,0x35,0x62,0x35,0x62,0xFF}
};

static void report(const char *name, int pass) {
    printf("  %-22s %s\n", name, pass ? "PASS" : "FAIL");
    if (!pass)
        failures++;
}

static int bytes_equal(const uint8_t *a, const uint8_t *b, uint32_t size) {
    for (uint32_t i = 0; i < size; ++i)
        if (a[i] != b[i])
            return 0;
    return 1;
}

int main(void) {
    of_cart_caps_t caps;
    static uint8_t first[PROBE_COUNT][BLOCK_BYTES];
    uint8_t block[BLOCK_BYTES];
    int first_ok[PROBE_COUNT] = {0};
    int repeat_ok[PROBE_COUNT] = {0};

    printf("\033[2J\033[H");
    printf("  GB/GBC MBC5 BANKS smoke.85\n");
    printf("  BANK SELECT ONLY @2000 - NO NOR CMD\n\n");

#ifdef OF_PC
    report("PC fallback closed", of_cart_available() == 0);
    report("PC caps blocked", of_cart_get_caps(&caps) == OF_ERR_NOT_SUPPORTED);
    return failures ? 1 : 0;
#else
    int caps_rc = of_cart_get_caps(&caps);
    report("service available", of_cart_available() == 1);
    report("GB ROM-only caps",
           caps_rc == OF_OK && caps.version == OF_CART_CAPS_VERSION &&
           caps.bus_mask == OF_CART_BUS_MASK_GB_GBC &&
           caps.operation_mask == (OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
                                   OF_CART_OP_ABORT | OF_CART_OP_QUERY) &&
           caps.max_read_bytes == 32u && caps.writes_enabled == 0 &&
           caps.gb_gbc_space_mask == OF_CART_SPACE_MASK_ROM &&
           caps.gba_space_mask == 0);
    report("lifecycle ready",
           caps_rc == OF_OK &&
           (caps.lifecycle_status &
            (OF_CART_LIFECYCLE_STATE_MASK | OF_CART_LIFECYCLE_PLAY |
             OF_CART_LIFECYCLE_POWER | OF_CART_LIFECYCLE_READ_READY)) ==
           (OF_CART_LIFECYCLE_READY | OF_CART_LIFECYCLE_PLAY |
            OF_CART_LIFECYCLE_POWER | OF_CART_LIFECYCLE_READ_READY));
    report("safe idle", of_cart_enter_safe_idle() == OF_OK);

    for (uint32_t i = 0; i < PROBE_COUNT; ++i) {
        int rc = of_cart_read_space(OF_CART_BUS_GB_GBC, OF_CART_SPACE_ROM,
                                    probe_address[i], first[i], BLOCK_BYTES,
                                    READ_TIMEOUT_US);
        first_ok[i] = rc == OF_OK &&
                      bytes_equal(first[i], expected[i], BLOCK_BYTES);
    }
    for (uint32_t i = 0; i < PROBE_COUNT; ++i) {
        int rc = of_cart_read_space(OF_CART_BUS_GB_GBC, OF_CART_SPACE_ROM,
                                    probe_address[i], block, BLOCK_BYTES,
                                    READ_TIMEOUT_US);
        repeat_ok[i] = rc == OF_OK &&
                       bytes_equal(block, expected[i], BLOCK_BYTES) &&
                       bytes_equal(block, first[i], BLOCK_BYTES);
        report(probe_name[i], first_ok[i] && repeat_ok[i]);
    }

    report("all expected bytes",
           first_ok[0] && first_ok[1] && first_ok[2] && first_ok[3] &&
           first_ok[4] && first_ok[5] && first_ok[6] && first_ok[7]);
    report("all repeats identical",
           repeat_ok[0] && repeat_ok[1] && repeat_ok[2] && repeat_ok[3] &&
           repeat_ok[4] && repeat_ok[5] && repeat_ok[6] && repeat_ok[7]);
    report("final safe idle", of_cart_enter_safe_idle() == OF_OK);

    printf("\n  MBC5 PAGING: %s\n", failures ? "FAIL" : "PASS");
    printf("  NOR ERASE/PROGRAM COMMANDS: ABSENT\n");
    fflush(stdout);
    while (1)
        usleep(100000);
#endif
}
