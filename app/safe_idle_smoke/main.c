#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "of_cart.h"

static int failures;

static void diag_stage(const char *stage, const char *state, int value) {
    printf("\033[2J\033[H");
    printf("  ChisBurner\n");
    printf("  PROTOCOL smoke.24 QUERY PING\n");
    printf("  ======================\n\n");
    printf("  EMPTY SLOT ONLY\n\n");
    printf("  Stage: %s\n", stage);
    printf("  State: %s\n", state);
    printf("  Value: %d\n", value);
    fflush(stdout);
    sleep(2);
}

static void report(const char *name, int pass) {
    printf("  %-22s %s\n", name, pass ? "PASS" : "FAIL");
    if (!pass)
        failures++;
}

int main(void) {
    of_cart_caps_t caps;

    printf("\033[2J\033[H");
    printf("  ChisBurner\n");
    printf("  PROTOCOL smoke.24 QUERY PING\n");
    printf("  =======================\n\n");
    printf("  EMPTY SLOT ONLY\n");
    printf("  Select Play Cartridge\n\n");

#ifdef OF_PC
    report("PC fallback closed", of_cart_available() == 0);
    report("PC caps blocked",
           of_cart_get_caps(&caps) == OF_ERR_NOT_SUPPORTED);
    printf("\n  Desktop fallback test %s\n", failures ? "FAILED" : "PASSED");
    return failures ? 1 : 0;
#else
    of_cart_protocol_info_t protocol_info;
    uint8_t probe = 0;
    int lifecycle_notify_pass = 0;
    int read_blocked_pass = 0;
    int abort_relock_pass = 0;
    uint32_t host_lifecycle_status = 0;
    uint32_t host_b1_raw = 0;
    uint32_t host_diag = 0;
    uint32_t ping_response = 0;
    int query_pass = 0;
    int ping_pass = 0;

    /* Field diagnostic: keep every checkpoint visible long enough to identify
     * the exact SDK/OS boundary that precedes a reset or display loss. */
    diag_stage("D0", "APP RUNNING", 0);

    diag_stage("D1", "BEFORE AVAILABLE", 0);
    int available = of_cart_available();
    diag_stage("D1", "AFTER AVAILABLE", available);
    report("service available", available == 1);

    diag_stage("D2", "BEFORE CAPS", 0);
    int rc = of_cart_get_caps(&caps);
    diag_stage("D2", "AFTER CAPS", rc);
    report("caps query", rc == OF_OK);
    if (rc == OF_OK) {
        report("caps ABI v4", caps.version == OF_CART_CAPS_VERSION);
        report("no readable bus", caps.bus_mask == 0);
        report("GB spaces locked", caps.gb_gbc_space_mask == 0);
        report("GBA spaces locked", caps.gba_space_mask == 0);
        report("safe+abort+query",
               caps.operation_mask ==
                   (OF_CART_OP_SAFE_IDLE | OF_CART_OP_ABORT |
                    OF_CART_OP_QUERY));
        report("read length zero", caps.max_read_bytes == 0);
        report("writes absent", caps.writes_enabled == 0);
        diag_stage("D2L", "HOST LIFECYCLE", (int)caps.lifecycle_status);
        host_lifecycle_status = caps.lifecycle_status;
        host_b1_raw = caps.host_b1_raw;
        host_diag = caps.host_diag;
        report("Play selected",
               (caps.lifecycle_status & OF_CART_LIFECYCLE_PLAY) != 0);
        report("power plan announced",
               (caps.lifecycle_status & OF_CART_LIFECYCLE_POWER) != 0);
        report("state ready",
               (caps.lifecycle_status & OF_CART_LIFECYCLE_STATE_MASK) ==
                   OF_CART_LIFECYCLE_READY);
        report("ready observed",
               (caps.lifecycle_status & OF_CART_LIFECYCLE_READ_READY) != 0);
        lifecycle_notify_pass =
            (caps.lifecycle_status &
             (OF_CART_LIFECYCLE_STATE_MASK |
              OF_CART_LIFECYCLE_PLAY |
              OF_CART_LIFECYCLE_POWER |
              OF_CART_LIFECYCLE_READ_READY)) ==
                (OF_CART_LIFECYCLE_READY |
                 OF_CART_LIFECYCLE_PLAY |
                 OF_CART_LIFECYCLE_POWER |
                 OF_CART_LIFECYCLE_READ_READY);
    }

    diag_stage("D2Q", "BEFORE QUERY INFO", 0);
    rc = of_cart_query_info(&protocol_info);
    diag_stage("D2Q", "AFTER QUERY INFO", rc);
    query_pass = rc == OF_OK &&
                 protocol_info.struct_size == sizeof(protocol_info) &&
                 protocol_info.magic == OF_CART_PROTOCOL_MAGIC &&
                 protocol_info.version == OF_CART_PROTOCOL_VERSION &&
                 protocol_info.features ==
                     (OF_CART_PROTOCOL_QUERY | OF_CART_PROTOCOL_PING);
    report("query info", query_pass);

    diag_stage("D2P", "BEFORE PING", 0);
    rc = of_cart_ping(0x13579BDFu, &ping_response);
    diag_stage("D2P", "AFTER PING", rc);
    ping_pass = rc == OF_OK && ping_response == 0xECA86420u;
    report("ping complement", ping_pass);

    diag_stage("D3", "BEFORE SAFE IDLE", 0);
    rc = of_cart_enter_safe_idle();
    diag_stage("D3", "AFTER SAFE IDLE", rc);
    report("enter safe idle", rc == OF_OK);

    diag_stage("D4", "BEFORE BLOCKED READ", 0);
    rc = of_cart_read(OF_CART_BUS_GB_GBC, 0, &probe, 1, 1);
    diag_stage("D4", "AFTER BLOCKED READ", rc);
    read_blocked_pass = rc == OF_ERR_NOT_SUPPORTED;
    report("read path blocked", read_blocked_pass);

    diag_stage("D5", "BEFORE ABORT", 0);
    rc = of_cart_abort();
    diag_stage("D5", "AFTER ABORT", rc);
    report("abort accepted", rc == OF_OK);

    diag_stage("D6", "BEFORE POST-ABORT", 0);
    rc = of_cart_get_caps(&caps);
    diag_stage("D6", "AFTER POST-ABORT", rc);
    report("post-abort caps", rc == OF_OK);
    if (rc == OF_OK) {
        abort_relock_pass =
            (caps.lifecycle_status &
             (OF_CART_LIFECYCLE_STATE_MASK |
              OF_CART_LIFECYCLE_PLAY |
              OF_CART_LIFECYCLE_POWER |
              OF_CART_LIFECYCLE_READ_READY)) ==
                OF_CART_LIFECYCLE_LOCKED;
        report("abort relocked", abort_relock_pass);
    }

    printf("\033[2J\033[H");
    printf("  ChisBurner\n");
    printf("  PROTOCOL smoke.24 QUERY PING\n\n");
    printf("\n  Voltage mode: physical cartridge mechanism\n");
    printf("  Software voltage control: absent\n");
    printf("  Cartridge write API: absent\n\n");
    printf("  LIFE RAW: %08x\n", host_lifecycle_status);
    printf("  B1 RAW:   %08x\n", host_b1_raw);
    printf("  HOST DIAG:%08x\n", host_diag);
    printf("  COUNT B1/IN/OUT: %u/%u/%u\n",
           (unsigned)((host_diag >> 8) & 0xffu),
           (unsigned)((host_diag >> 16) & 0xffu),
           (unsigned)((host_diag >> 24) & 0xffu));
    printf("  QUERY: %08x V%u.%u F%08x\n",
           protocol_info.magic,
           (unsigned)(protocol_info.version >> 16),
           (unsigned)(protocol_info.version & 0xffffu),
           protocol_info.features);
    printf("  PING:  %08x %s\n", ping_response,
           ping_pass ? "PASS" : "FAIL");
    printf("  HOST NOTIFY:  %s\n", lifecycle_notify_pass ? "PASS" : "FAIL");
    printf("  READ BLOCKED: %s\n", read_blocked_pass ? "PASS" : "FAIL");
    printf("  ABORT RELOCK: %s\n\n", abort_relock_pass ? "PASS" : "FAIL");
    if (failures == 0)
        printf("  PROTOCOL SMOKE: PASS\n");
    else
        printf("  PROTOCOL SMOKE: FAIL (%d)\n", failures);
    printf("  DIAG COMPLETE\n");
    fflush(stdout);

    while (1)
        usleep(100000);
#endif
}
