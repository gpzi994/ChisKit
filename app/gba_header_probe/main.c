#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "of_cart.h"

#define HEADER_ADDRESS 0x000000A0u
#define HEADER_LENGTH  32u
#define READ_TIMEOUT_US 50000u

static int failures;

static void stage(const char *name, int value) {
    printf("\033[2J\033[H");
    printf("  ChisBurner\n");
    printf("  GBA HEADER smoke.25\n");
    printf("  ===================\n\n");
    printf("  READ-ONLY: A0-BF, 2 READS\n");
    printf("  Stage: %s\n", name);
    printf("  Value: %d\n", value);
    fflush(stdout);
    sleep(1);
}

static void report(const char *name, int pass) {
    printf("  %-21s %s\n", name, pass ? "PASS" : "FAIL");
    if (!pass)
        failures++;
}

static int bytes_equal(const uint8_t *a, const uint8_t *b, uint32_t size) {
    for (uint32_t i = 0; i < size; ++i) {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

static void copy_ascii(char *out, const uint8_t *in, uint32_t size) {
    for (uint32_t i = 0; i < size; ++i)
        out[i] = (in[i] >= 0x20u && in[i] <= 0x7Eu) ? (char)in[i] : '.';
    out[size] = '\0';
}

static uint8_t expected_header_checksum(const uint8_t *header) {
    uint8_t sum = 0;
    for (uint32_t i = 0; i <= 0x1Cu; ++i)
        sum = (uint8_t)(sum + header[i]);
    return (uint8_t)(0u - sum - 0x19u);
}

int main(void) {
    of_cart_caps_t caps;
    uint8_t header_a[HEADER_LENGTH] = {0};
    uint8_t header_b[HEADER_LENGTH] = {0};
    char title[13];
    char game_code[5];
    char maker[3];

    stage("APP RUNNING", 0);

#ifdef OF_PC
    report("PC fallback closed", of_cart_available() == 0);
    report("PC caps blocked",
           of_cart_get_caps(&caps) == OF_ERR_NOT_SUPPORTED);
    printf("\n  Desktop fallback %s\n", failures ? "FAIL" : "PASS");
    return failures ? 1 : 0;
#else
    int available = of_cart_available();
    stage("SERVICE AVAILABLE", available);
    report("service available", available == 1);

    int caps_rc = of_cart_get_caps(&caps);
    stage("CAPS", caps_rc);
    report("caps query", caps_rc == OF_OK);

    int caps_pass = caps_rc == OF_OK &&
                    caps.version == OF_CART_CAPS_VERSION &&
                    caps.bus_mask == OF_CART_BUS_MASK_GBA &&
                    caps.operation_mask ==
                        (OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
                         OF_CART_OP_ABORT | OF_CART_OP_QUERY) &&
                    caps.max_read_bytes == HEADER_LENGTH &&
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
    stage("SAFE IDLE", idle_rc);
    report("safe idle", idle_rc == OF_OK);

    int read_a_rc = of_cart_read_space(OF_CART_BUS_GBA, OF_CART_SPACE_ROM,
                                       HEADER_ADDRESS, header_a,
                                       HEADER_LENGTH, READ_TIMEOUT_US);
    stage("HEADER READ 1", read_a_rc);
    report("header read 1", read_a_rc == OF_OK);

    int read_b_rc = of_cart_read_space(OF_CART_BUS_GBA, OF_CART_SPACE_ROM,
                                       HEADER_ADDRESS, header_b,
                                       HEADER_LENGTH, READ_TIMEOUT_US);
    stage("HEADER READ 2", read_b_rc);
    report("header read 2", read_b_rc == OF_OK);

    int repeat_pass = read_a_rc == OF_OK && read_b_rc == OF_OK &&
                      bytes_equal(header_a, header_b, HEADER_LENGTH);
    int fixed_pass = read_a_rc == OF_OK && header_a[0x12] == 0x96u;
    uint8_t checksum_expected = expected_header_checksum(header_a);
    int checksum_pass = read_a_rc == OF_OK &&
                        header_a[0x1D] == checksum_expected;

    report("repeat identical", repeat_pass);
    report("fixed byte 96", fixed_pass);
    report("header checksum", checksum_pass);

    int final_idle_rc = of_cart_enter_safe_idle();
    report("final safe idle", final_idle_rc == OF_OK);

    copy_ascii(title, &header_a[0x00], 12);
    copy_ascii(game_code, &header_a[0x0C], 4);
    copy_ascii(maker, &header_a[0x10], 2);

    printf("\033[2J\033[H");
    printf("  ChisBurner\n");
    printf("  GBA HEADER smoke.25\n\n");
    printf("  READ1/READ2: %d / %d\n", read_a_rc, read_b_rc);
    printf("  TITLE: %.12s\n", title);
    printf("  CODE:  %.4s  MAKER: %.2s\n", game_code, maker);
    printf("  FIXED: %02x  VER: %02x\n",
           (unsigned)header_a[0x12], (unsigned)header_a[0x1C]);
    printf("  CHECK: %02x  EXPECT: %02x\n",
           (unsigned)header_a[0x1D], (unsigned)checksum_expected);
    printf("  RAW A0-A7: ");
    for (uint32_t i = 0; i < 8; ++i)
        printf("%02x", (unsigned)header_a[i]);
    printf("\n\n");
    printf("  REPEAT:  %s\n", repeat_pass ? "PASS" : "FAIL");
    printf("  FIXED96: %s\n", fixed_pass ? "PASS" : "FAIL");
    printf("  CHECKSUM:%s\n", checksum_pass ? "PASS" : "FAIL");
    printf("  WRITES:  STRUCTURALLY ABSENT\n\n");
    printf("  GBA HEADER PROBE: %s", failures ? "FAIL" : "PASS");
    if (failures)
        printf(" (%d)", failures);
    printf("\n  DIAG COMPLETE\n");
    fflush(stdout);

    while (1)
        usleep(100000);
#endif
}
