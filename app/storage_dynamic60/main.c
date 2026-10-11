#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "of_output.h"
#include "dynamic60.h"

struct result60 {
    int audit_rc;
    int open_rc;
    int snapshot_rc;
    unsigned snapshot_events;
    uint32_t snapshot_flags;
    uint32_t lost_events;
    unsigned mode_existing;
    struct d60_trace trace;
};

static void title60(void) {
    printf("\033[2J\033[H TF OPEN CONTROL smoke.60\n");
    printf("NO CARTRIDGE ACCESS\n");
}

static const char *host_result60(unsigned result) {
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

static const char *path60(unsigned existing) {
    return existing ? D60_PATH_EXISTING : D60_PATH_MISSING;
}

static void initial60(void) {
    title60();
    printf("0192 EXISTING/MISSING CONTROL\n");
    printf("B: OPEN PACKAGED EXISTING FILE\n");
    printf("A: CREATE ABSENT FILE\n");
    printf("RUN B FIRST; SEND ALL 4 PAGES\n");
    printf("THEN COLD BOOT; RUN A\n");
    printf("SEND ALL 4 PAGES AGAIN\n");
    printf("\nNO 0184 DATA WRITE OR 0188 FLUSH\n");
    fflush(stdout);
}

static int trace_complete60(const struct result60 *r) {
    return !r->audit_rc && !r->snapshot_rc &&
           r->trace.command_0192_tx && r->trace.host_busy_writes &&
           r->trace.host_ok_writes && r->trace.param_unique == D60_PARAM_WORDS;
}

static void result60(const struct result60 *r, unsigned page) {
    title60();
    printf("MODE %c  PAGE %u/4\n", r->mode_existing ? 'B' : 'A', page + 1u);
    if (page == 0u) {
        printf("TARGET %s\n", r->mode_existing ? "EXISTING" : "MISSING");
        printf("OPEN RC %d\n", r->open_rc);
        if (r->trace.host_result == D60_NO_RESULT)
            printf("HOST RESULT: NONE\n");
        else
            printf("HOST RESULT %u: %s\n", r->trace.host_result,
                   host_result60(r->trace.host_result));
        printf("STATUS WRITES %u\n", r->trace.status_writes);
        printf("FIRST %08lX\n", (unsigned long)r->trace.first_status);
        printf("LAST  %08lX\n", (unsigned long)r->trace.last_status);
        printf("BUSY %u OK %u CMD %u\n", r->trace.host_busy_writes,
               r->trace.host_ok_writes, r->trace.command_0192_tx);
        printf("TRACE %s\n", trace_complete60(r) ? "COMPLETE" : "INCOMPLETE");
    } else if (page == 1u) {
        printf("PARAM READ %u TX %u\n", r->trace.param_reads,
               r->trace.param_tx);
        printf("UNIQUE %u/%u DUP %u\n", r->trace.param_unique,
               D60_PARAM_WORDS, r->trace.param_duplicates);
        printf("MATCH %u/%u CONFLICT %u\n", r->trace.param_matches,
               D60_PARAM_WORDS, r->trace.param_conflicts);
        printf("FIRST ADDR %08lX\n", (unsigned long)r->trace.first_param_addr);
        printf("LAST  ADDR %08lX\n", (unsigned long)r->trace.last_param_addr);
        printf("WORD00 %08lX\n", (unsigned long)r->trace.word0);
        printf("WORD01 %08lX\n", (unsigned long)r->trace.word1);
        printf("FLAGS  %08lX EXP 03000000\n", (unsigned long)r->trace.flags);
        printf("SIZE   %08lX EXP 00200000\n", (unsigned long)r->trace.size);
    } else if (page == 2u) {
        printf("DUPLICATE DETAIL %u/%u\n", r->trace.duplicate_saved,
               r->trace.param_duplicates);
        unsigned shown = r->trace.duplicate_saved;
        if (shown > 4u) shown = 4u;
        for (unsigned i = 0; i < shown; ++i) {
            const struct d60_duplicate *d = &r->trace.duplicate[i];
            printf("D%u EVT%u OFF %04lX%s\n", i, d->event_index,
                   (unsigned long)(d->address - D60_PARAM_BASE),
                   d->first_value == d->repeat_value ? " SAME" : " DIFF");
            printf(" OLD %08lX NEW %08lX\n", (unsigned long)d->first_value,
                   (unsigned long)d->repeat_value);
        }
        if (!shown) printf("NO DUPLICATE PARAM READS\n");
    } else {
        printf("PATH:\n");
        printf("/Saves/chisburner/common/\n");
        printf("dyn60-%s.bin\n", r->mode_existing ? "existing" : "missing");
        printf("FLAGS CREATE+RESIZE SIZE 8192\n");
        printf("SNAP RC %d EVENTS %u\n", r->snapshot_rc,
               r->snapshot_events);
        printf("FULL %u COLLISION %u LOST %lu\n",
               (unsigned)((r->snapshot_flags >> 16) & 1u),
               (unsigned)((r->snapshot_flags >> 17) & 1u),
               (unsigned long)r->lost_events);
        if (r->mode_existing && r->trace.host_result == 0u)
            printf("CONTROL: PATH/SLOT WORK\n");
        else if (!r->mode_existing && r->trace.host_result == 1u)
            printf("CONTROL: CREATE WORKS\n");
        else if (!r->mode_existing && r->trace.host_result == 3u)
            printf("CONTROL: CREATE NOT APPLIED\n");
        else
            printf("CONTROL: NEED BOTH RUNS\n");
        printf("NO 0184 WRITE; NO 0188 FLUSH\n");
    }
    printf("LEFT/RIGHT PAGE\n");
    fflush(stdout);
}

#ifndef OF_PC
#include "of_bridge_audit.h"
#include "of_bridge_trace.h"
#include "of_input.h"
#include "../storage_snapshot_forensics/snapshot.h"

static struct forensic_snapshot open_snapshot60;

static void run60(struct result60 *r, unsigned existing) {
    memset(r, 0, sizeof(*r));
    r->mode_existing = existing;
    r->trace.host_result = D60_NO_RESULT;
    r->trace.first_param_addr = D60_NO_RESULT;
    r->audit_rc = of_bridge_audit_begin_with(of_bridge_trace_read);
    if (r->audit_rc)
        return;
    r->open_rc = of_output_open(path60(existing), D60_SIZE);
    r->snapshot_rc = forensic_snapshot_read(&open_snapshot60,
                                             of_bridge_trace_read);
    r->snapshot_events = open_snapshot60.count_valid ?
                         (open_snapshot60.flags & 0xffffu) : 0u;
    r->snapshot_flags = open_snapshot60.flags;
    if (!r->snapshot_rc) {
        r->lost_events = open_snapshot60.raw[38];
        r->trace = d60_parse_trace(
            open_snapshot60.raw + OF_BRIDGE_AUDIT_HEADER_WORDS,
            r->snapshot_events, path60(existing));
    }
}
#endif

int main(int argc, char **argv) {
#ifdef OF_PC
    if (argc > 1 && !strcmp(argv[1], "--demo")) {
        struct result60 r;
        memset(&r, 0, sizeof(r));
        r.mode_existing = 1; r.open_rc = 0; r.snapshot_events = 165;
        r.trace.command_0192_tx = 1; r.trace.status_writes = 2;
        r.trace.host_busy_writes = 1; r.trace.host_ok_writes = 1;
        r.trace.host_result = 0; r.trace.first_status = 0x62750000;
        r.trace.last_status = 0x6f6b0000; r.trace.param_reads = 70;
        r.trace.param_tx = 70; r.trace.param_unique = 66;
        r.trace.param_matches = 66; r.trace.param_duplicates = 4;
        r.trace.param_conflicts = 1; r.trace.first_param_addr = D60_PARAM_BASE;
        r.trace.last_param_addr = D60_PARAM_BASE + 260;
        r.trace.word0 = 0x2f536176; r.trace.word1 = 0x65732f66;
        r.trace.flags = 0x03000000; r.trace.size = 0x00200000;
        r.trace.duplicate_saved = 4;
        for (unsigned i = 0; i < 4; ++i) {
            r.trace.duplicate[i].event_index = 70u + i;
            r.trace.duplicate[i].address = D60_PARAM_BASE + i * 4u;
            r.trace.duplicate[i].first_value = 0x11111111u + i;
            r.trace.duplicate[i].repeat_value = i == 2u ? 0x22222222u : 0x11111111u + i;
        }
        initial60();
        for (unsigned page = 0; page < 4; ++page) result60(&r, page);
        return 0;
    }
    uint8_t p[D60_PARAM_BYTES];
    d60_make_param(p, path60(0));
    int pass = d60_wire_word(p) == 0x2f536176u &&
               d60_wire_word(p + 0x100) == 0x03000000u &&
               d60_wire_word(p + 0x104) == 0x00200000u &&
               of_output_open(D60_PATH_MISSING, D60_SIZE) == OF_ERR_NOT_SUPPORTED;
    printf("Dynamic60 PC fallback: %s\n", pass ? "PASS" : "FAIL");
    return pass ? 0 : 1;
#else
    (void)argc; (void)argv;
    initial60();
    if (!of_output_available() || !OF_OUTPUT_SVC_HAS(input_poll_p0)) {
        printf("\nSTOP: REQUIRED SDK SERVICE MISSING\n");
        fflush(stdout);
        while (1) usleep(100000);
    }
    uint32_t last = 0;
    unsigned existing = 0;
    for (;;) {
        of_input_poll_p0();
        uint32_t keys = _of_input_p0.buttons & 0xffffu;
        uint32_t edges = keys & ~last;
        last = keys;
        if (edges & OF_BTN_A) { existing = 0; break; }
        if (edges & OF_BTN_B) { existing = 1; break; }
        usleep(16000);
    }
    title60();
    printf("RUNNING MODE %c 0192 TRACE\n", existing ? 'B' : 'A');
    printf("DO NOT QUIT OR POWER OFF\n");
    fflush(stdout);
    struct result60 r;
    run60(&r, existing);
    unsigned page = 0;
    result60(&r, page);
    last = 0;
    for (;;) {
        of_input_poll_p0();
        uint32_t keys = _of_input_p0.buttons & 0xffffu;
        uint32_t edges = keys & ~last;
        last = keys;
        unsigned previous = page;
        if ((edges & OF_BTN_RIGHT) && page < 3u) ++page;
        if ((edges & OF_BTN_LEFT) && page > 0u) --page;
        if (page != previous) result60(&r, page);
        usleep(16000);
    }
#endif
}
