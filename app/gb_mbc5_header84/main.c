#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "of_cart.h"

#define READ_TIMEOUT_US 50000u
#define HEADER_BYTES    0x50u

static int failures;

static const uint8_t nintendo_logo[48] = {
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
    0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
    0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
    0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
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

static uint8_t header_checksum(const uint8_t *header) {
    uint8_t sum = 0;
    for (uint32_t i = 0x34u; i <= 0x4Cu; ++i)
        sum = (uint8_t)(sum - header[i] - 1u);
    return sum;
}

static int read_header(uint8_t *header) {
    int rc = of_cart_read_space(OF_CART_BUS_GB_GBC, OF_CART_SPACE_ROM,
                                0x0100u, &header[0x00], 32u,
                                READ_TIMEOUT_US);
    if (rc != OF_OK)
        return rc;
    rc = of_cart_read_space(OF_CART_BUS_GB_GBC, OF_CART_SPACE_ROM,
                            0x0120u, &header[0x20], 32u,
                            READ_TIMEOUT_US);
    if (rc != OF_OK)
        return rc;
    return of_cart_read_space(OF_CART_BUS_GB_GBC, OF_CART_SPACE_ROM,
                              0x0140u, &header[0x40], 16u,
                              READ_TIMEOUT_US);
}

static void copy_title(char *out, const uint8_t *header) {
    for (uint32_t i = 0; i < 15u; ++i) {
        uint8_t c = header[0x34u + i];
        out[i] = (c >= 0x20u && c <= 0x7Eu) ? (char)c : '.';
    }
    out[15] = '\0';
}

int main(void) {
    of_cart_caps_t caps;
    uint8_t header_a[HEADER_BYTES] = {0};
    uint8_t header_b[HEADER_BYTES] = {0};
    char title[16];

    printf("\033[2J\033[H");
    printf("  GB/GBC MBC5 HEADER smoke.84\n");
    printf("  READ ONLY - BANK 0 - NO WR\n\n");

#ifdef OF_PC
    report("PC fallback closed", of_cart_available() == 0);
    report("PC caps blocked",
           of_cart_get_caps(&caps) == OF_ERR_NOT_SUPPORTED);
    return failures ? 1 : 0;
#else
    int caps_rc = of_cart_get_caps(&caps);
    report("service available", of_cart_available() == 1);
    report("caps query", caps_rc == OF_OK);
    report("GB ROM-only caps",
           caps_rc == OF_OK &&
           caps.version == OF_CART_CAPS_VERSION &&
           caps.bus_mask == OF_CART_BUS_MASK_GB_GBC &&
           caps.operation_mask ==
               (OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
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
    int read_a_rc = read_header(header_a);
    int read_b_rc = read_header(header_b);
    report("header pass 1", read_a_rc == OF_OK);
    report("header pass 2", read_b_rc == OF_OK);
    report("repeat identical",
           read_a_rc == OF_OK && read_b_rc == OF_OK &&
           bytes_equal(header_a, header_b, HEADER_BYTES));
    report("Nintendo logo",
           read_a_rc == OF_OK &&
           bytes_equal(&header_a[0x04], nintendo_logo, sizeof(nintendo_logo)));

    uint8_t calculated = header_checksum(header_a);
    report("header checksum",
           read_a_rc == OF_OK && header_a[0x4D] == calculated);
    report("MBC5+RAM+BATTERY",
           read_a_rc == OF_OK && header_a[0x47] == 0x1Bu);
    report("ROM 2 MiB / RAM 32K",
           read_a_rc == OF_OK && header_a[0x48] == 0x06u &&
           header_a[0x49] == 0x03u);
    report("CGB compatible",
           read_a_rc == OF_OK && header_a[0x43] == 0x80u);
    report("known header 3B",
           read_a_rc == OF_OK && header_a[0x4D] == 0x3Bu);
    report("final safe idle", of_cart_enter_safe_idle() == OF_OK);

    copy_title(title, header_a);
    uint16_t global = ((uint16_t)header_a[0x4E] << 8) | header_a[0x4F];
    printf("\n  TITLE: %.15s\n", title);
    printf("  TYPE/CGB: %02X / %02X\n",
           (unsigned)header_a[0x47], (unsigned)header_a[0x43]);
    printf("  ROM/RAM:  %02X / %02X\n",
           (unsigned)header_a[0x48], (unsigned)header_a[0x49]);
    printf("  HCHK: %02X CALC %02X\n",
           (unsigned)header_a[0x4D], (unsigned)calculated);
    printf("  GLOBAL: %04X (INFO ONLY)\n", (unsigned)global);
    printf("  FULL-ROM RECALC REF: 0028\n");
    printf("  CHECKSUM IS NOT A FAIL GATE\n\n");
    printf("  GB HEADER PROBE: %s\n", failures ? "FAIL" : "PASS");
    printf("  WRITES: STRUCTURALLY ABSENT\n");
    fflush(stdout);

    while (1)
        usleep(100000);
#endif
}
