#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "of_output.h"
#include "dynamic59.h"

struct result59 {
    int audit_rc;
    int open_rc;
    int snapshot_rc;
    unsigned snapshot_events;
    uint32_t snapshot_flags;
    uint32_t lost_events;
    struct d59_trace trace;
};

static void title59(void) {
    printf("\033[2J\033[H TF OPEN TRACE smoke.59\n");
    printf("NO CARTRIDGE ACCESS\n");
}

static const char *host_result59(unsigned result) {
    switch (result) {
    case 0: return "OPENED";
    case 1: return "CREATED";
    case 2: return "SLOT UNDEFINED";
    case 3: return "FILE NOT FOUND";
    case 4: return "MALFORMED PATH";
    case 5: return "GENERAL ERROR";
    default: return "NO OK STATUS";
    }
}

static void initial59(void) {
    title59();
    printf("0192 PARAMETER VISIBILITY TEST\n");
    printf("TARGET dyn59-a7c9.bin 8192 BYTES\n");
    printf("TARGET FILE IS NOT IN PACKAGE\n");
    printf("\nA: OPEN CREATE + TRACE ONCE\n");
    printf("\nNO DATA WRITE; NO 0188 FLUSH\n");
    printf("PRESS A THEN SEND ALL 3 PAGES\n");
    fflush(stdout);
}

static int trace_complete59(const struct result59 *r) {
    return !r->audit_rc && !r->snapshot_rc &&
           r->trace.command_0192_tx && r->trace.host_busy_writes &&
           r->trace.host_ok_writes && r->trace.param_unique == D59_PARAM_WORDS;
}

static void result59(const struct result59 *r, unsigned page) {
    title59();
    printf("PAGE %u/3\n", page + 1u);
    if (page == 0u) {
        printf("OPEN RC %d\n", r->open_rc);
        if (r->trace.host_result == D59_NO_RESULT)
            printf("HOST RESULT: NONE\n");
        else
            printf("HOST RESULT %u: %s\n", r->trace.host_result,
                   host_result59(r->trace.host_result));
        printf("STATUS WRITES %u\n", r->trace.status_writes);
        printf("FIRST %08lX\n", (unsigned long)r->trace.first_status);
        printf("LAST  %08lX\n", (unsigned long)r->trace.last_status);
        printf("BUSY %u  OK %u\n", r->trace.host_busy_writes,
               r->trace.host_ok_writes);
        printf("0192 COMMAND TX %u\n", r->trace.command_0192_tx);
        printf("TRACE: %s\n", trace_complete59(r) ? "COMPLETE" : "INCOMPLETE");
    } else if (page == 1u) {
        printf("PARAM READ %u  TX %u\n", r->trace.param_reads,
               r->trace.param_tx);
        printf("UNIQUE %u/%u  DUP %u\n", r->trace.param_unique,
               D59_PARAM_WORDS, r->trace.param_duplicates);
        printf("MATCH %u/%u  CONFLICT %u\n", r->trace.param_matches,
               D59_PARAM_WORDS, r->trace.param_conflicts);
        printf("FIRST ADDR %08lX\n", (unsigned long)r->trace.first_param_addr);
        printf("LAST  ADDR %08lX\n", (unsigned long)r->trace.last_param_addr);
        printf("WORD00 %08lX EXP 2F536176\n", (unsigned long)r->trace.word0);
        printf("WORD01 %08lX EXP 65732F66\n", (unsigned long)r->trace.word1);
        printf("FLAGS  %08lX EXP 03000000\n", (unsigned long)r->trace.flags);
        printf("SIZE   %08lX EXP 00200000\n", (unsigned long)r->trace.size);
    } else {
        printf("EXPECTED PATH:\n");
        printf("/Saves/chisburner/common/\n");
        printf("dyn59-a7c9.bin\n");
        printf("PARAM PTR 20400000 LEN 264\n");
        printf("FLAGS create=1 resize=1\n");
        printf("SNAP RC %d EVENTS %u\n", r->snapshot_rc,
               r->snapshot_events);
        printf("AUDIT ARM RC %d\n", r->audit_rc);
        printf("FULL %u COLLISION %u LOST %lu\n",
               (unsigned)((r->snapshot_flags >> 16) & 1u),
               (unsigned)((r->snapshot_flags >> 17) & 1u),
               (unsigned long)r->lost_events);
        printf("NO DATA WRITE; NO 0188 FLUSH\n");
    }
    printf("LEFT/RIGHT PAGE\n");
    fflush(stdout);
}

#ifndef OF_PC
#include "of_bridge_audit.h"
#include "of_bridge_trace.h"
#include "of_input.h"
#include "../storage_snapshot_forensics/snapshot.h"

static struct forensic_snapshot open_snapshot59;

static void run59(struct result59 *r) {
    memset(r, 0, sizeof(*r));
    r->trace.host_result = D59_NO_RESULT;
    r->trace.first_param_addr = D59_NO_RESULT;
    r->audit_rc = of_bridge_audit_begin_with(of_bridge_trace_read);
    if (r->audit_rc)
        return;
    r->open_rc = of_output_open(D59_PATH, D59_SIZE);
    r->snapshot_rc = forensic_snapshot_read(&open_snapshot59,
                                             of_bridge_trace_read);
    r->snapshot_events = open_snapshot59.count_valid ?
                         (open_snapshot59.flags & 0xffffu) : 0u;
    r->snapshot_flags = open_snapshot59.flags;
    if (!r->snapshot_rc) {
        r->lost_events = open_snapshot59.raw[38];
        r->trace = d59_parse_trace(
            open_snapshot59.raw + OF_BRIDGE_AUDIT_HEADER_WORDS,
            r->snapshot_events);
    }
}
#endif

int main(int argc, char **argv) {
#ifdef OF_PC
    if (argc > 1 && !strcmp(argv[1], "--demo")) {
        struct result59 r;
        memset(&r, 0, sizeof(r));
        r.open_rc = -3; r.snapshot_events = 160;
        r.trace.command_0192_tx = 1; r.trace.status_writes = 2;
        r.trace.host_busy_writes = 1; r.trace.host_ok_writes = 1;
        r.trace.host_result = 3; r.trace.first_status = 0x62750000;
        r.trace.last_status = 0x6f6b0003; r.trace.param_reads = 66;
        r.trace.param_tx = 66; r.trace.param_unique = 66;
        r.trace.param_matches = 66; r.trace.first_param_addr = D59_PARAM_BASE;
        r.trace.last_param_addr = D59_PARAM_BASE + 260;
        r.trace.word0 = 0x2f536176; r.trace.word1 = 0x65732f66;
        r.trace.flags = 0x03000000; r.trace.size = 0x00200000;
        initial59();
        for (unsigned page = 0; page < 3; ++page) result59(&r, page);
        return 0;
    }
    uint8_t p[D59_PARAM_BYTES];
    d59_make_param(p);
    int pass = d59_wire_word(p) == 0x2f536176u &&
               d59_wire_word(p + 0x100) == 0x03000000u &&
               d59_wire_word(p + 0x104) == 0x00200000u &&
               of_output_open(D59_PATH, D59_SIZE) == OF_ERR_NOT_SUPPORTED;
    printf("Dynamic59 PC fallback: %s\n", pass ? "PASS" : "FAIL");
    return pass ? 0 : 1;
#else
    (void)argc; (void)argv;
    initial59();
    if (!of_output_available() || !OF_OUTPUT_SVC_HAS(input_poll_p0)) {
        printf("\nSTOP: REQUIRED SDK SERVICE MISSING\n");
        fflush(stdout);
        while (1) usleep(100000);
    }
    uint32_t last = 0;
    for (;;) {
        of_input_poll_p0();
        uint32_t keys = _of_input_p0.buttons & 0xffffu;
        uint32_t edges = keys & ~last;
        last = keys;
        if (edges & OF_BTN_A) break;
        usleep(16000);
    }
    title59();
    printf("RUNNING 0192 + PARAM TRACE\n");
    printf("DO NOT QUIT OR POWER OFF\n");
    fflush(stdout);
    struct result59 r;
    run59(&r);
    unsigned page = 0;
    result59(&r, page);
    last = 0;
    for (;;) {
        of_input_poll_p0();
        uint32_t keys = _of_input_p0.buttons & 0xffffu;
        uint32_t edges = keys & ~last;
        last = keys;
        unsigned previous = page;
        if ((edges & OF_BTN_RIGHT) && page < 2u) ++page;
        if ((edges & OF_BTN_LEFT) && page > 0u) --page;
        if (page != previous) result59(&r, page);
        usleep(16000);
    }
#endif
}
