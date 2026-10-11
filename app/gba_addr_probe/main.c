#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "of_cart.h"

#define BLOCK_BYTES     32u
#define SAMPLE_COUNT    11u
#define READ_TIMEOUT_US 50000u

static int failures;

static void stage(const char *name, uint32_t current, uint32_t total) {
    printf("\033[2J\033[H");
    printf("  ChisBurner\n");
    printf("  GBA ADDRESS smoke.27\n");
    printf("  ====================\n\n");
    printf("  READ-ONLY: FIXED ADDRESSES\n");
    printf("  32 BYTES x 11 x 2\n\n");
    printf("  Stage: %s\n", name);
    if (total != 0)
        printf("  Progress: %lu/%lu\n", (unsigned long)current,
               (unsigned long)total);
    fflush(stdout);
}

static void report(const char *name, int pass) {
    printf("  %-21s %s\n", name, pass ? "PASS" : "FAIL");
    if (!pass)
        failures++;
}

#ifndef OF_PC
static const uint32_t sample_addresses[SAMPLE_COUNT] = {
    0x000000A0u,
    0x000100A0u,
    0x000200A0u,
    0x000400A0u,
    0x000800A0u,
    0x001000A0u,
    0x002000A0u,
    0x004000A0u,
    0x008000A0u,
    0x010000A0u,
    0x01FFFFE0u
};

static uint8_t first_pass[SAMPLE_COUNT][BLOCK_BYTES];

static uint32_t crc32_update(uint32_t crc, const uint8_t *data,
                             uint32_t size) {
    for (uint32_t i = 0; i < size; ++i) {
        crc ^= data[i];
        for (uint32_t bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ ((0u - (crc & 1u)) & 0xEDB88320u);
    }
    return crc;
}

static int bytes_equal(const uint8_t *a, const uint8_t *b, uint32_t size) {
    for (uint32_t i = 0; i < size; ++i) {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

static uint8_t expected_header_checksum(const uint8_t *header_a0) {
    uint8_t sum = 0;
    for (uint32_t i = 0; i <= 0x1Cu; ++i)
        sum = (uint8_t)(sum + header_a0[i]);
    return (uint8_t)(0u - sum - 0x19u);
}

static uint32_t detect_mirror_size(void) {
    for (uint32_t i = 1; i < SAMPLE_COUNT - 1; ++i) {
        if (bytes_equal(first_pass[0], first_pass[i], 16u))
            return sample_addresses[i] - 0xA0u;
    }
    return 0x02000000u;
}

static void print_size(uint32_t size) {
    if (size >= 0x00100000u)
        printf("  MIRROR SIZE: %lu MiB (INFO)\n",
               (unsigned long)(size >> 20));
    else
        printf("  MIRROR SIZE: %lu KiB (INFO)\n",
               (unsigned long)(size >> 10));
}
#endif

int main(void) {
    of_cart_caps_t caps;
    stage("APP RUNNING", 0, 0);

#ifdef OF_PC
    report("PC fallback closed", of_cart_available() == 0);
    report("PC caps blocked",
           of_cart_get_caps(&caps) == OF_ERR_NOT_SUPPORTED);
    printf("\n  Desktop fallback %s\n", failures ? "FAIL" : "PASS");
    return failures ? 1 : 0;
#else
    report("service available", of_cart_available() == 1);

    int caps_rc = of_cart_get_caps(&caps);
    report("caps query", caps_rc == OF_OK);
    int caps_pass = caps_rc == OF_OK &&
                    caps.version == OF_CART_CAPS_VERSION &&
                    caps.bus_mask == OF_CART_BUS_MASK_GBA &&
                    caps.operation_mask ==
                        (OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
                         OF_CART_OP_ABORT | OF_CART_OP_QUERY) &&
                    caps.max_read_bytes == BLOCK_BYTES &&
                    caps.writes_enabled == 0 &&
                    caps.gb_gbc_space_mask == 0 &&
                    caps.gba_space_mask == OF_CART_SPACE_MASK_ROM;
    report("GBA ROM-only caps", caps_pass);

    int lifecycle_pass = caps_rc == OF_OK &&
        (caps.lifecycle_status &
         (OF_CART_LIFECYCLE_STATE_MASK |
          OF_CART_LIFECYCLE_PLAY |
          OF_CART_LIFECYCLE_POWER |
          OF_CART_LIFECYCLE_READ_READY)) ==
        (OF_CART_LIFECYCLE_READY |
         OF_CART_LIFECYCLE_PLAY |
         OF_CART_LIFECYCLE_POWER |
         OF_CART_LIFECYCLE_READ_READY);
    report("lifecycle ready", lifecycle_pass);

    int idle_rc = of_cart_enter_safe_idle();
    report("safe idle", idle_rc == OF_OK);
    sleep(1);

    uint32_t crc1_state = 0xFFFFFFFFu;
    uint32_t crc2_state = 0xFFFFFFFFu;
    uint32_t samples1 = 0;
    uint32_t samples2 = 0;
    int read1_rc = OF_OK;
    int read2_rc = OF_OK;
    int repeat_data = 1;
    int high_tail_repeat = 0;
    uint8_t block[BLOCK_BYTES];

    for (uint32_t i = 0; i < SAMPLE_COUNT; ++i) {
        int rc = of_cart_read_space(OF_CART_BUS_GBA, OF_CART_SPACE_ROM,
                                    sample_addresses[i], first_pass[i],
                                    BLOCK_BYTES, READ_TIMEOUT_US);
        if (rc != OF_OK) {
            read1_rc = rc;
            break;
        }
        crc1_state = crc32_update(crc1_state, first_pass[i], BLOCK_BYTES);
        samples1++;
        stage("ADDRESS PASS 1", samples1, SAMPLE_COUNT);
    }

    if (read1_rc == OF_OK && samples1 == SAMPLE_COUNT) {
        for (uint32_t i = 0; i < SAMPLE_COUNT; ++i) {
            int rc = of_cart_read_space(OF_CART_BUS_GBA, OF_CART_SPACE_ROM,
                                        sample_addresses[i], block,
                                        BLOCK_BYTES, READ_TIMEOUT_US);
            if (rc != OF_OK) {
                read2_rc = rc;
                break;
            }
            crc2_state = crc32_update(crc2_state, block, BLOCK_BYTES);
            int same = bytes_equal(first_pass[i], block, BLOCK_BYTES);
            if (!same)
                repeat_data = 0;
            if (i == SAMPLE_COUNT - 1)
                high_tail_repeat = same;
            samples2++;
            stage("ADDRESS PASS 2", samples2, SAMPLE_COUNT);
        }
    } else {
        read2_rc = OF_ERR_INVALID_STATE;
        repeat_data = 0;
    }

    uint32_t crc1 = ~crc1_state;
    uint32_t crc2 = ~crc2_state;
    int pass1_ok = read1_rc == OF_OK && samples1 == SAMPLE_COUNT;
    int pass2_ok = read2_rc == OF_OK && samples2 == SAMPLE_COUNT;
    int crc_ok = pass1_ok && pass2_ok && crc1 == crc2;
    int data_ok = pass1_ok && pass2_ok && repeat_data;
    int header_ok = pass1_ok && first_pass[0][0x12u] == 0x96u &&
                    first_pass[0][0x1Du] ==
                        expected_header_checksum(first_pass[0]);
    uint32_t mirror_size = pass1_ok ? detect_mirror_size() : 0;
    int final_idle_rc = of_cart_enter_safe_idle();

    if (!pass1_ok) failures++;
    if (!pass2_ok) failures++;
    if (!crc_ok) failures++;
    if (!data_ok) failures++;
    if (!high_tail_repeat) failures++;
    if (!header_ok) failures++;
    if (final_idle_rc != OF_OK) failures++;

    printf("\033[2J\033[H");
    printf("  ChisBurner\n");
    printf("  GBA ADDRESS smoke.27\n\n");
    printf("  PASS1: %lu/11 RC=%d\n", (unsigned long)samples1, read1_rc);
    printf("  PASS2: %lu/11 RC=%d\n", (unsigned long)samples2, read2_rc);
    printf("  CRC1: %08lx\n", (unsigned long)crc1);
    printf("  CRC2: %08lx\n", (unsigned long)crc2);
    if (mirror_size != 0)
        print_size(mirror_size);
    printf("\n  CRC MATCH:  %s\n", crc_ok ? "PASS" : "FAIL");
    printf("  DATA MATCH: %s\n", data_ok ? "PASS" : "FAIL");
    printf("  HIGH 1FFFFE0: %s\n", high_tail_repeat ? "PASS" : "FAIL");
    printf("  GBA HEADER: %s\n", header_ok ? "PASS" : "FAIL");
    printf("  FINAL IDLE: %s\n", final_idle_rc == OF_OK ? "PASS" : "FAIL");
    printf("  WRITES: STRUCTURALLY ABSENT\n\n");
    printf("  GBA ADDRESS PROBE: %s", failures ? "FAIL" : "PASS");
    if (failures)
        printf(" (%d)", failures);
    printf("\n  DIAG COMPLETE\n");
    fflush(stdout);

    while (1)
        usleep(100000);
#endif
}
