#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "of_output.h"
#include "dynamic58.h"

#define D58_PATH "/Saves/chisburner/common/dyn58-a7c9.bin"
#define D58_SLOT_PATH "slot:4"
#define D58_EXPECTED_CRC 0xd46fa793u

static uint8_t expected58[D58_BYTES] __attribute__((aligned(64)));
#ifndef OF_PC
static uint8_t actual58[D58_BYTES] __attribute__((aligned(64)));
#endif

struct result58 {
    unsigned mode;
    int audit_rc;
    int open_rc;
    long slot_size;
    int snapshot_rc;
    unsigned snapshot_events;
    struct d58_trace trace;
    int write_rc;
    unsigned write_blocks;
    int read_rc;
    unsigned read_bytes;
    unsigned matches;
    uint32_t first_bad;
    uint32_t expected_crc;
    uint32_t actual_crc;
};

static void title58(void) {
    printf("\033[2J\033[H TF DYNAMIC smoke.58\n");
    printf("NO CARTRIDGE ACCESS\n");
}

static void initial58(void) {
    title58();
    printf("TARGET dyn58-a7c9.bin  8192 BYTES\n");
    printf("TARGET FILE IS NOT IN THE PACKAGE\n");
    printf("\nA: CREATE + WRITE + READBACK\n");
    printf("B: OPEN + READ-ONLY VERIFY\n");
    printf("\nAPF 0188 FLUSH: NOT CALLED\n");
    printf("0192 SETS FINAL SIZE BEFORE WRITES\n");
    printf("\nFIRST RUN: PRESS A\n");
    printf("AFTER PASS: QUIT, POWER CYCLE\n");
    printf("SECOND RUN: PRESS B\n");
    fflush(stdout);
}

static const char *open_kind58(const struct result58 *r) {
    if (r->snapshot_rc)
        return "TRACE UNAVAILABLE";
    if (r->trace.host_ok1_writes)
        return "CREATED (HOST OK1)";
    if (r->trace.host_ok0_writes)
        return "EXISTING (HOST OK0)";
    return "NO HOST COMPLETION";
}

static int data_pass58(const struct result58 *r) {
    return r->open_rc == 0 && r->slot_size == (long)D58_BYTES &&
           r->write_rc == 0 && r->read_rc == 0 &&
           r->read_bytes == D58_BYTES && r->matches == D58_BYTES &&
           r->expected_crc == D58_EXPECTED_CRC &&
           r->actual_crc == D58_EXPECTED_CRC;
}

static int mode_pass58(const struct result58 *r) {
    if (!data_pass58(r) || r->audit_rc || r->snapshot_rc ||
        !r->trace.command_0192_reads)
        return 0;
    if (r->mode == 1u)
        return r->trace.host_ok1_writes == 1u;
    return r->trace.host_ok0_writes == 1u;
}

static void result58(const struct result58 *r, unsigned page) {
    title58();
    printf("MODE %c  PAGE %u/2\n", r->mode == 1u ? 'A' : 'B', page + 1u);
    if (page == 0u) {
        printf("OPEN RC %d  SIZE %ld/8192\n", r->open_rc, r->slot_size);
        printf("HOST OPEN: %s\n", open_kind58(r));
        printf("WRITE %u/%u BLOCKS RC %d\n", r->write_blocks,
               r->mode == 1u ? D58_BLOCKS : 0u, r->write_rc);
        printf("READ %u/8192 RC %d\n", r->read_bytes, r->read_rc);
        printf("MATCH %u/8192\n", r->matches);
        if (r->first_bad == D58_NO_BAD)
            printf("FIRST BAD: NONE\n");
        else
            printf("FIRST BAD: %lu\n", (unsigned long)r->first_bad);
        printf("CRC %08lX EXPECT %08lX\n", (unsigned long)r->actual_crc,
               (unsigned long)r->expected_crc);
        printf("APF 0188 FLUSH: NOT CALLED\n");
        printf("RESULT: %s\n", mode_pass58(r) ? "PASS" : "FAIL");
        if (r->mode == 1u && mode_pass58(r))
            printf("QUIT; POWER OFF/ON; RUN 58; B\n");
        else if (r->mode == 2u && mode_pass58(r))
            printf("DYNAMIC FILE PERSISTENCE: PASS\n");
        else
            printf("STOP; SEND BOTH PAGES\n");
    } else {
        printf("OPEN TRACE SNAP RC %d EVENTS %u\n", r->snapshot_rc,
               r->snapshot_events);
        printf("0192 READS %u\n", r->trace.command_0192_reads);
        printf("HOST BUSY WRITES %u\n", r->trace.host_busy_writes);
        printf("HOST OK0 OPENED %u\n", r->trace.host_ok0_writes);
        printf("HOST OK1 CREATED %u\n", r->trace.host_ok1_writes);
        printf("OTHER STATUS WRITES %u\n", r->trace.other_status_writes);
        printf("AUDIT ARM RC %d\n", r->audit_rc);
        printf("SNAPSHOT TAKEN BEFORE DATA I/O\n");
        printf("A REQUIRES EXACTLY ONE HOST OK1\n");
        printf("B REQUIRES EXACTLY ONE HOST OK0\n");
        printf("NO PREALLOCATED TARGET FILE\n");
        printf("NO 0188 COMMAND IN THIS BUILD\n");
    }
    printf("LEFT/RIGHT PAGE\n");
    fflush(stdout);
}

#ifndef OF_PC
#include "of_bridge_audit.h"
#include "of_bridge_trace.h"
#include "of_file.h"
#include "of_input.h"
#include "../storage_snapshot_forensics/snapshot.h"

static struct forensic_snapshot open_snapshot58;

static int read_slot58(uint8_t *out, unsigned *read_bytes) {
    FILE *f = fopen(D58_SLOT_PATH, "rb");
    if (!f)
        return -1;
    *read_bytes = (unsigned)fread(out, 1, D58_BYTES, f);
    int close_rc = fclose(f);
    return (*read_bytes == D58_BYTES && close_rc == 0) ? 0 : -1;
}

static void run58(unsigned mode, struct result58 *r) {
    memset(r, 0, sizeof(*r));
    r->mode = mode;
    r->slot_size = -1;
    r->write_rc = 0;
    r->read_rc = -1;
    r->first_bad = D58_NO_BAD;
    d58_fill(expected58, 0, D58_BYTES);
    r->expected_crc = d58_crc32(expected58, D58_BYTES);

    r->audit_rc = of_bridge_audit_begin_with(of_bridge_trace_read);
    if (r->audit_rc)
        return;

    r->open_rc = of_output_open(D58_PATH, D58_BYTES);
    r->snapshot_rc = forensic_snapshot_read(&open_snapshot58,
                                             of_bridge_trace_read);
    r->snapshot_events = open_snapshot58.count_valid ?
                         (open_snapshot58.flags & 0xffffu) : 0u;
    if (!r->snapshot_rc) {
        r->trace = d58_parse_open_trace(
            open_snapshot58.raw + OF_BRIDGE_AUDIT_HEADER_WORDS,
            r->snapshot_events);
    }
    if (r->open_rc)
        return;

    r->slot_size = OF_SVC->file_size(D58_SLOT_PATH);
    if (r->slot_size != (long)D58_BYTES)
        return;

    if (mode == 1u) {
        for (unsigned block = 0; block < D58_BLOCKS; ++block) {
            uint32_t offset = block * D58_CHUNK;
            r->write_rc = of_output_write(offset, expected58 + offset,
                                           D58_CHUNK);
            if (r->write_rc)
                return;
            r->write_blocks = block + 1u;
        }
    }

    r->read_rc = read_slot58(actual58, &r->read_bytes);
    if (r->read_rc)
        return;
    r->actual_crc = d58_crc32(actual58, D58_BYTES);
    r->matches = d58_compare(actual58, expected58, D58_BYTES,
                             &r->first_bad);
}
#endif

int main(int argc, char **argv) {
#ifdef OF_PC
    d58_fill(expected58, 0, D58_BYTES);
    uint32_t crc = d58_crc32(expected58, D58_BYTES);
    if (argc > 1 && !strcmp(argv[1], "--demo")) {
        struct result58 r;
        memset(&r, 0, sizeof(r));
        r.mode = 1; r.open_rc = 0; r.slot_size = D58_BYTES;
        r.snapshot_events = 6; r.trace.command_0192_reads = 2;
        r.trace.host_busy_writes = 1; r.trace.host_ok1_writes = 1;
        r.write_blocks = D58_BLOCKS; r.read_bytes = D58_BYTES;
        r.matches = D58_BYTES; r.first_bad = D58_NO_BAD;
        r.expected_crc = r.actual_crc = D58_EXPECTED_CRC;
        initial58(); result58(&r, 0); result58(&r, 1);
        r.mode = 2; r.trace.host_ok1_writes = 0; r.trace.host_ok0_writes = 1;
        r.write_blocks = 0; result58(&r, 0); result58(&r, 1);
        return 0;
    }
    int pass = crc == D58_EXPECTED_CRC && of_output_available() == 0 &&
               of_output_open(D58_PATH, D58_BYTES) == OF_ERR_NOT_SUPPORTED;
    printf("Dynamic58 PC fallback: %s CRC=%08lx\n", pass ? "PASS" : "FAIL",
           (unsigned long)crc);
    return pass ? 0 : 1;
#else
    (void)argc; (void)argv;
    initial58();
    if (!of_output_available() || !OF_OUTPUT_SVC_HAS(file_size) ||
        !OF_OUTPUT_SVC_HAS(input_poll_p0)) {
        printf("\nSTOP: REQUIRED SDK SERVICE MISSING\n");
        fflush(stdout);
        while (1) usleep(100000);
    }

    unsigned mode = 0;
    uint32_t last = 0;
    for (;;) {
        of_input_poll_p0();
        uint32_t keys = _of_input_p0.buttons & 0xffffu;
        uint32_t edges = keys & ~last;
        last = keys;
        if (edges & OF_BTN_A) { mode = 1; break; }
        if (edges & OF_BTN_B) { mode = 2; break; }
        usleep(16000);
    }

    title58();
    printf("MODE %c RUNNING\n", mode == 1u ? 'A' : 'B');
    printf("OPEN 0192; SNAPSHOT; THEN DATA I/O\n");
    printf("DO NOT QUIT OR POWER OFF\n");
    fflush(stdout);
    struct result58 r;
    run58(mode, &r);
    unsigned page = 0;
    result58(&r, page);
    last = 0;
    for (;;) {
        of_input_poll_p0();
        uint32_t keys = _of_input_p0.buttons & 0xffffu;
        uint32_t edges = keys & ~last;
        last = keys;
        unsigned previous = page;
        if (edges & (OF_BTN_LEFT | OF_BTN_RIGHT))
            page ^= 1u;
        if (page != previous)
            result58(&r, page);
        usleep(16000);
    }
#endif
}
