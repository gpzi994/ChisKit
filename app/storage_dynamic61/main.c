#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "of_output.h"
#include "dynamic61.h"

static uint8_t expected61[D61_SIZE] __attribute__((aligned(64)));
#ifndef OF_PC
static uint8_t actual61[D61_SIZE] __attribute__((aligned(64)));
#endif

struct result61 {
    unsigned create_mode;
    int audit_rc;
    int open_rc;
    long slot_size;
    int snapshot_rc;
    unsigned snapshot_events;
    struct d60_trace trace;
    int write_rc;
    unsigned write_blocks;
    int read_rc;
    unsigned read_bytes;
    unsigned matches;
    uint32_t first_bad;
    uint32_t actual_crc;
};

static void title61(void) {
    printf("\033[2J\033[H TF CREATE smoke.61\n");
    printf("NO CARTRIDGE ACCESS\n");
}

static const char *host_result61(unsigned result) {
    switch (result) {
    case 0: return "OPENED";
    case 1: return "CREATED";
    case 2: return "SLOT UNDEFINED";
    case 3: return "FILE NOT FOUND";
    case 4: return "MALFORMED PATH";
    case 5: return "GENERAL ERROR";
    default: return "NO STATUS";
    }
}

static void initial61(void) {
    title61();
    printf("0192 NUMERIC-WORD ENDIAN FIX\n");
    printf("TARGET dyn61-endian.bin 8192\n");
    printf("TARGET IS ABSENT FROM PACKAGE\n");
    printf("A: CREATE + WRITE + READBACK\n");
    printf("B: EXISTING READ-ONLY VERIFY\n");
    printf("\nFIRST RUN: PRESS A\n");
    printf("AFTER PASS: QUIT + COLD BOOT\n");
    printf("SECOND RUN: PRESS B\n");
    printf("NO 0188 FLUSH IN THIS BUILD\n");
    fflush(stdout);
}

static int result_pass61(const struct result61 *r) {
    if (r->audit_rc || r->open_rc || r->snapshot_rc ||
        r->slot_size != (long)D61_SIZE ||
        !d61_open_trace_ok(&r->trace, r->create_mode) ||
        r->write_rc || r->read_rc || r->read_bytes != D61_SIZE ||
        r->matches != D61_SIZE || r->first_bad != D58_NO_BAD ||
        r->actual_crc != D61_EXPECTED_CRC)
        return 0;
    return !r->create_mode || r->write_blocks == D58_BLOCKS;
}

static void result61(const struct result61 *r, unsigned page) {
    title61();
    printf("MODE %c  PAGE %u/2\n", r->create_mode ? 'A' : 'B', page + 1u);
    if (page == 0u) {
        printf("OPEN RC %d SIZE %ld/8192\n", r->open_rc, r->slot_size);
        if (r->trace.host_result == D60_NO_RESULT)
            printf("HOST RESULT: NONE\n");
        else
            printf("HOST %u: %s\n", r->trace.host_result,
                   host_result61(r->trace.host_result));
        printf("FLAGS WIRE %08lX\n", (unsigned long)r->trace.flags);
        printf("SIZE  WIRE %08lX\n", (unsigned long)r->trace.size);
        printf("WRITE %u/%u RC %d\n", r->write_blocks,
               r->create_mode ? D58_BLOCKS : 0u, r->write_rc);
        printf("READ %u/8192 RC %d\n", r->read_bytes, r->read_rc);
        printf("MATCH %u/8192\n", r->matches);
        if (r->first_bad == D58_NO_BAD)
            printf("FIRST BAD: NONE\n");
        else
            printf("FIRST BAD: %lu\n", (unsigned long)r->first_bad);
        printf("CRC %08lX EXP %08lX\n", (unsigned long)r->actual_crc,
               (unsigned long)D61_EXPECTED_CRC);
        printf("RESULT: %s\n", result_pass61(r) ? "PASS" : "FAIL");
    } else {
        printf("OPEN TRACE SNAP RC %d\n", r->snapshot_rc);
        printf("EVENTS %u PARAM %u/%u\n", r->snapshot_events,
               r->trace.param_unique, D60_PARAM_WORDS);
        printf("0192 TX %u BUSY %u OK %u\n", r->trace.command_0192_tx,
               r->trace.host_busy_writes, r->trace.host_ok_writes);
        printf("HOST RESULT %lu EXPECT %u\n",
               (unsigned long)r->trace.host_result,
               r->create_mode ? 1u : 0u);
        printf("FLAGS EXPECT 00000003\n");
        printf("SIZE  EXPECT 00002000\n");
        printf("PARAM WIRE: %s\n",
               d61_param_wire_ok(&r->trace) ? "PASS" : "FAIL");
        printf("AUDIT ARM RC %d\n", r->audit_rc);
        printf("0184 DATA WRITE: %s\n",
               r->create_mode ? "8 X 1024" : "NONE");
        printf("APF 0188 FLUSH: NOT CALLED\n");
        if (result_pass61(r) && r->create_mode)
            printf("QUIT; COLD BOOT; RUN 61; B\n");
        else if (result_pass61(r))
            printf("DYNAMIC PERSISTENCE: PASS\n");
        else
            printf("STOP; SEND BOTH PAGES\n");
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

static struct forensic_snapshot open_snapshot61;

static int read_slot61(uint8_t *out, unsigned *read_bytes) {
    FILE *f = fopen(D61_SLOT_PATH, "rb");
    if (!f)
        return -1;
    *read_bytes = (unsigned)fread(out, 1, D61_SIZE, f);
    int close_rc = fclose(f);
    return (*read_bytes == D61_SIZE && close_rc == 0) ? 0 : -1;
}

static void run61(struct result61 *r, unsigned create_mode) {
    memset(r, 0, sizeof(*r));
    r->create_mode = create_mode;
    r->slot_size = -1;
    r->read_rc = -1;
    r->first_bad = D58_NO_BAD;
    r->trace.host_result = D60_NO_RESULT;
    d58_fill(expected61, 0, D61_SIZE);

    r->audit_rc = of_bridge_audit_begin_with(of_bridge_trace_read);
    if (r->audit_rc)
        return;
    r->open_rc = of_output_open(D61_PATH, D61_SIZE);
    r->snapshot_rc = forensic_snapshot_read(&open_snapshot61,
                                             of_bridge_trace_read);
    r->snapshot_events = open_snapshot61.count_valid ?
                         (open_snapshot61.flags & 0xffffu) : 0u;
    if (!r->snapshot_rc) {
        r->trace = d60_parse_trace(
            open_snapshot61.raw + OF_BRIDGE_AUDIT_HEADER_WORDS,
            r->snapshot_events, D61_PATH);
    }
    if (r->open_rc)
        return;

    r->slot_size = OF_SVC->file_size(D61_SLOT_PATH);
    if (r->slot_size != (long)D61_SIZE)
        return;

    if (create_mode) {
        for (unsigned block = 0; block < D58_BLOCKS; ++block) {
            uint32_t offset = block * D58_CHUNK;
            r->write_rc = of_output_write(offset, expected61 + offset,
                                           D58_CHUNK);
            if (r->write_rc)
                return;
            r->write_blocks = block + 1u;
        }
    }

    r->read_rc = read_slot61(actual61, &r->read_bytes);
    if (r->read_rc)
        return;
    r->actual_crc = d58_crc32(actual61, D61_SIZE);
    r->matches = d58_compare(actual61, expected61, D61_SIZE,
                             &r->first_bad);
}
#endif

int main(int argc, char **argv) {
#ifdef OF_PC
    d58_fill(expected61, 0, D61_SIZE);
    if (argc > 1 && !strcmp(argv[1], "--demo")) {
        struct result61 r;
        memset(&r, 0, sizeof(r));
        r.create_mode = 1; r.open_rc = 0; r.slot_size = D61_SIZE;
        r.snapshot_events = 165; r.trace.command_0192_tx = 1;
        r.trace.host_busy_writes = 1; r.trace.host_ok_writes = 1;
        r.trace.host_result = 1; r.trace.param_unique = D60_PARAM_WORDS;
        r.trace.flags = D61_FLAGS_WIRE; r.trace.size = D61_SIZE_WIRE;
        r.write_blocks = D58_BLOCKS; r.read_bytes = D61_SIZE;
        r.matches = D61_SIZE; r.first_bad = D58_NO_BAD;
        r.actual_crc = D61_EXPECTED_CRC;
        initial61(); result61(&r, 0); result61(&r, 1);
        r.create_mode = 0; r.trace.host_result = 0; r.write_blocks = 0;
        result61(&r, 0); result61(&r, 1);
        return 0;
    }
    int pass = d58_crc32(expected61, D61_SIZE) == D61_EXPECTED_CRC &&
               of_output_open(D61_PATH, D61_SIZE) == OF_ERR_NOT_SUPPORTED;
    printf("Dynamic61 PC fallback: %s\n", pass ? "PASS" : "FAIL");
    return pass ? 0 : 1;
#else
    (void)argc; (void)argv;
    initial61();
    if (!of_output_available() || !OF_OUTPUT_SVC_HAS(file_size) ||
        !OF_OUTPUT_SVC_HAS(input_poll_p0)) {
        printf("\nSTOP: REQUIRED SDK SERVICE MISSING\n");
        fflush(stdout);
        while (1) usleep(100000);
    }
    uint32_t last = 0;
    unsigned create_mode = 0;
    for (;;) {
        of_input_poll_p0();
        uint32_t keys = _of_input_p0.buttons & 0xffffu;
        uint32_t edges = keys & ~last;
        last = keys;
        if (edges & OF_BTN_A) { create_mode = 1; break; }
        if (edges & OF_BTN_B) { create_mode = 0; break; }
        usleep(16000);
    }
    title61();
    printf("MODE %c RUNNING\n", create_mode ? 'A' : 'B');
    printf("0192; SNAPSHOT; THEN DATA I/O\n");
    printf("DO NOT QUIT OR POWER OFF\n");
    fflush(stdout);
    struct result61 r;
    run61(&r, create_mode);
    unsigned page = 0;
    result61(&r, page);
    last = 0;
    for (;;) {
        of_input_poll_p0();
        uint32_t keys = _of_input_p0.buttons & 0xffffu;
        uint32_t edges = keys & ~last;
        last = keys;
        if (edges & (OF_BTN_LEFT | OF_BTN_RIGHT)) {
            page ^= 1u;
            result61(&r, page);
        }
        usleep(16000);
    }
#endif
}
