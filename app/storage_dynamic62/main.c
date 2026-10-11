#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "of_output.h"
#include "dynamic62.h"

static uint8_t expected62[D62_SIZE] __attribute__((aligned(64)));
#ifndef OF_PC
static uint8_t actual62[D62_SIZE] __attribute__((aligned(64)));
#endif

struct result62 {
    unsigned create_mode;
    int audit_rc;
    int open_rc;
    unsigned open_host;
    long slot_size;
    int snapshot_rc;
    unsigned snapshot_events;
    struct d60_trace trace;
    int write_rc;
    unsigned write_blocks;
    int read_rc;
    unsigned read_blocks;
    unsigned matches;
    uint32_t first_bad;
    uint32_t actual_crc;
};

static void title62(void) {
    printf("\033[2J\033[H TF DIRECT CREATE smoke.62\n");
    printf("KNOWN-GOOD smoke.60 OS\n");
}

static const char *host_result62(unsigned result) {
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

static void initial62(void) {
    title62();
    printf("0192 DIRECT ENDIAN TEST\n");
    printf("TARGET dyn62-direct.bin 8192\n");
    printf("TARGET ABSENT FROM PACKAGE\n");
    printf("A: CREATE + WRITE + READBACK\n");
    printf("B: OPEN + READ PERSISTENCE\n");
    printf("\nFIRST RUN: PRESS A\n");
    printf("AFTER PASS: QUIT + COLD BOOT\n");
    printf("SECOND RUN: PRESS B\n");
    printf("NO CARTRIDGE; NO 0188 FLUSH\n");
    fflush(stdout);
}

static int result_pass62(const struct result62 *r) {
    if (r->audit_rc || r->open_rc || r->snapshot_rc ||
        r->open_host != (r->create_mode ? 1u : 0u) ||
        !d62_trace_ok(&r->trace, r->create_mode) ||
        r->write_rc || r->read_rc || r->read_blocks != D62_BLOCKS ||
        r->matches != D62_SIZE || r->first_bad != D58_NO_BAD ||
        r->actual_crc != D62_EXPECTED_CRC)
        return 0;
    return !r->create_mode || r->write_blocks == D62_BLOCKS;
}

static void result62(const struct result62 *r, unsigned page) {
    title62();
    printf("MODE %c  PAGE %u/2\n", r->create_mode ? 'A' : 'B', page + 1u);
    if (page == 0u) {
        printf("OPEN RC %d HOST %u %s\n", r->open_rc, r->open_host,
               host_result62(r->open_host));
        printf("SLOT SIZE %ld\n", r->slot_size);
        printf("FLAGS WIRE %08lX\n", (unsigned long)r->trace.flags);
        printf("SIZE  WIRE %08lX\n", (unsigned long)r->trace.size);
        printf("WRITE %u/%u RC %d\n", r->write_blocks,
               r->create_mode ? D62_BLOCKS : 0u, r->write_rc);
        printf("READ %u/%u RC %d\n", r->read_blocks, D62_BLOCKS,
               r->read_rc);
        printf("MATCH %u/8192\n", r->matches);
        if (r->first_bad == D58_NO_BAD)
            printf("FIRST BAD: NONE\n");
        else
            printf("FIRST BAD: %lu\n", (unsigned long)r->first_bad);
        printf("CRC %08lX EXP %08lX\n", (unsigned long)r->actual_crc,
               (unsigned long)D62_EXPECTED_CRC);
        printf("RESULT: %s\n", result_pass62(r) ? "PASS" : "FAIL");
    } else {
        printf("OPEN TRACE SNAP RC %d\n", r->snapshot_rc);
        printf("EVENTS %u PARAM %u/%u\n", r->snapshot_events,
               r->trace.param_unique, D60_PARAM_WORDS);
        printf("0192 TX %u BUSY %u OK %u\n", r->trace.command_0192_tx,
               r->trace.host_busy_writes, r->trace.host_ok_writes);
        printf("TRACE HOST %lu EXPECT %u\n",
               (unsigned long)r->trace.host_result,
               r->create_mode ? 1u : 0u);
        printf("PARAM WIRE: %s\n",
               d62_trace_ok(&r->trace, r->create_mode) ? "PASS" : "FAIL");
        printf("AUDIT ARM RC %d\n", r->audit_rc);
        printf("0184 WRITE: %s\n",
               r->create_mode ? "8 X 1024" : "NONE");
        printf("0180 READ: 8 X 1024\n");
        printf("APF 0188 FLUSH: NOT CALLED\n");
        if (result_pass62(r) && r->create_mode)
            printf("QUIT; COLD BOOT; RUN 62; B\n");
        else if (result_pass62(r))
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
#include "of_input.h"
#include "../storage_snapshot_forensics/snapshot.h"

#define D62_REG32(a) (*(volatile uint32_t *)(uintptr_t)(a))
#define D62_SYSREG_BASE       0x40000000u
#define D62_DS_SLOT_ID        D62_REG32(D62_SYSREG_BASE + 0x20u)
#define D62_DS_SLOT_OFFSET    D62_REG32(D62_SYSREG_BASE + 0x24u)
#define D62_DS_BRIDGE_ADDR    D62_REG32(D62_SYSREG_BASE + 0x28u)
#define D62_DS_LENGTH         D62_REG32(D62_SYSREG_BASE + 0x2cu)
#define D62_DS_PARAM_ADDR     D62_REG32(D62_SYSREG_BASE + 0x30u)
#define D62_DS_COMMAND        D62_REG32(D62_SYSREG_BASE + 0x38u)
#define D62_DS_STATUS         D62_REG32(D62_SYSREG_BASE + 0x3cu)
#define D62_DS_ACK            (1u << 0)
#define D62_DS_DONE           (1u << 1)
#define D62_DS_ERR_MASK       0x1cu
#define D62_DS_ERR_SHIFT      2u
#define D62_DS_READY          (1u << 5)
#define D62_DS_WR_IDLE        (1u << 6)
#define D62_DS_IRQ_PENDING    (1u << 7)
#define D62_CRAM0_CPU         0x30400000u
#define D62_CRAM0_BRIDGE      0x20400000u
#define D62_CRAM0_MODE        D62_REG32(0x4e000000u)
#define D62_TIMEOUT           200000000u

static struct forensic_snapshot open_snapshot62;

static void d62_fence(void) {
    __asm__ volatile("fence" ::: "memory");
}

static int d62_wait_mask(uint32_t mask, uint32_t want) {
    uint32_t timeout = D62_TIMEOUT;
    while ((D62_DS_STATUS & mask) != want) {
        if (--timeout == 0u)
            return -1;
    }
    return 0;
}

static int d62_command(uint32_t command, uint32_t success_mask,
                       unsigned *host_result) {
    if (d62_wait_mask(D62_DS_READY | D62_DS_WR_IDLE,
                      D62_DS_READY | D62_DS_WR_IDLE))
        return -10;
    D62_DS_STATUS = D62_DS_IRQ_PENDING;
    d62_fence();
    D62_DS_COMMAND = command;
    d62_fence();
    if (d62_wait_mask(D62_DS_ACK, D62_DS_ACK))
        return -11;
    if (d62_wait_mask(D62_DS_DONE, D62_DS_DONE))
        return -12;
    unsigned result = (D62_DS_STATUS & D62_DS_ERR_MASK) >> D62_DS_ERR_SHIFT;
    if (host_result)
        *host_result = result;
    if (d62_wait_mask(D62_DS_READY | D62_DS_WR_IDLE,
                      D62_DS_READY | D62_DS_WR_IDLE))
        return -13;
    D62_DS_STATUS = D62_DS_IRQ_PENDING;
    if (success_mask & (1u << result))
        return 0;
    return result ? -(int)result : -20;
}

static void d62_put_wire_word(uint8_t *dst, uint32_t value) {
    dst[0] = (uint8_t)(value >> 24);
    dst[1] = (uint8_t)(value >> 16);
    dst[2] = (uint8_t)(value >> 8);
    dst[3] = (uint8_t)value;
}

static int d62_open(unsigned create_mode, unsigned *host_result) {
    uint8_t *param = (uint8_t *)(uintptr_t)D62_CRAM0_CPU;
    D62_CRAM0_MODE = 1u;
    for (volatile unsigned i = 0; i < 8u; ++i) {}
    memset(param, 0, 264u);
    memcpy(param, D62_PATH, sizeof(D62_PATH));
    d62_put_wire_word(param + 256u, create_mode ? 3u : 0u);
    d62_put_wire_word(param + 260u, create_mode ? D62_SIZE : 0u);
    d62_fence();
    D62_CRAM0_MODE = 0u;
    for (volatile unsigned i = 0; i < 8u; ++i) {}
    D62_DS_SLOT_ID = D62_SLOT;
    D62_DS_PARAM_ADDR = D62_CRAM0_BRIDGE;
    return d62_command(3u, create_mode ? (1u << 1) : (1u << 0),
                       host_result);
}

static int d62_write_block(uint32_t offset, const uint8_t *src,
                           unsigned *host_result) {
    D62_CRAM0_MODE = 1u;
    for (volatile unsigned i = 0; i < 8u; ++i) {}
    memcpy((void *)(uintptr_t)D62_CRAM0_CPU, src, D62_CHUNK);
    d62_fence();
    D62_CRAM0_MODE = 0u;
    for (volatile unsigned i = 0; i < 8u; ++i) {}
    D62_DS_SLOT_ID = D62_SLOT;
    D62_DS_SLOT_OFFSET = offset;
    D62_DS_BRIDGE_ADDR = D62_CRAM0_BRIDGE;
    D62_DS_LENGTH = D62_CHUNK;
    return d62_command(2u, 1u << 0, host_result);
}

static int d62_read_block(uint32_t offset, uint8_t *dst,
                          unsigned *host_result) {
    D62_CRAM0_MODE = 0u;
    for (volatile unsigned i = 0; i < 8u; ++i) {}
    D62_DS_SLOT_ID = D62_SLOT;
    D62_DS_SLOT_OFFSET = offset;
    D62_DS_BRIDGE_ADDR = D62_CRAM0_BRIDGE;
    D62_DS_LENGTH = D62_CHUNK;
    int rc = d62_command(1u, 1u << 0, host_result);
    if (rc)
        return rc;
    D62_CRAM0_MODE = 1u;
    for (volatile unsigned i = 0; i < 8u; ++i) {}
    memcpy(dst, (const void *)(uintptr_t)D62_CRAM0_CPU, D62_CHUNK);
    D62_CRAM0_MODE = 0u;
    return 0;
}

static void run62(struct result62 *r, unsigned create_mode) {
    memset(r, 0, sizeof(*r));
    r->create_mode = create_mode;
    r->slot_size = -1;
    r->read_rc = -1;
    r->first_bad = D58_NO_BAD;
    r->trace.host_result = D60_NO_RESULT;
    r->open_host = D60_NO_RESULT;
    d58_fill(expected62, 0, D62_SIZE);

    r->audit_rc = of_bridge_audit_begin_with(of_bridge_trace_read);
    if (r->audit_rc)
        return;
    r->open_rc = d62_open(create_mode, &r->open_host);
    r->snapshot_rc = forensic_snapshot_read(&open_snapshot62,
                                             of_bridge_trace_read);
    r->snapshot_events = open_snapshot62.count_valid ?
                         (open_snapshot62.flags & 0xffffu) : 0u;
    if (!r->snapshot_rc) {
        r->trace = d60_parse_trace(
            open_snapshot62.raw + OF_BRIDGE_AUDIT_HEADER_WORDS,
            r->snapshot_events, D62_PATH);
    }
    if (r->open_rc)
        return;

    r->slot_size = OF_SVC->file_size("slot:4");
    unsigned host = 0;
    if (create_mode) {
        for (unsigned block = 0; block < D62_BLOCKS; ++block) {
            uint32_t offset = block * D62_CHUNK;
            r->write_rc = d62_write_block(offset, expected62 + offset,
                                           &host);
            if (r->write_rc)
                return;
            r->write_blocks = block + 1u;
        }
    }

    r->read_rc = 0;
    for (unsigned block = 0; block < D62_BLOCKS; ++block) {
        uint32_t offset = block * D62_CHUNK;
        r->read_rc = d62_read_block(offset, actual62 + offset, &host);
        if (r->read_rc)
            return;
        r->read_blocks = block + 1u;
    }
    r->actual_crc = d58_crc32(actual62, D62_SIZE);
    r->matches = d58_compare(actual62, expected62, D62_SIZE,
                             &r->first_bad);
}
#endif

int main(int argc, char **argv) {
#ifdef OF_PC
    d58_fill(expected62, 0, D62_SIZE);
    if (argc > 1 && !strcmp(argv[1], "--demo")) {
        struct result62 r;
        memset(&r, 0, sizeof(r));
        r.create_mode = 1; r.open_host = 1; r.slot_size = D62_SIZE;
        r.snapshot_events = 165; r.trace.command_0192_tx = 1;
        r.trace.host_busy_writes = 1; r.trace.host_ok_writes = 1;
        r.trace.host_result = 1; r.trace.param_unique = D60_PARAM_WORDS;
        r.trace.flags = D62_CREATE_FLAGS_WIRE;
        r.trace.size = D62_CREATE_SIZE_WIRE;
        r.write_blocks = D62_BLOCKS; r.read_blocks = D62_BLOCKS;
        r.matches = D62_SIZE; r.first_bad = D58_NO_BAD;
        r.actual_crc = D62_EXPECTED_CRC;
        initial62(); result62(&r, 0); result62(&r, 1);
        r.create_mode = 0; r.open_host = 0; r.write_blocks = 0;
        r.trace.host_result = 0; r.trace.flags = 0; r.trace.size = 0;
        result62(&r, 0); result62(&r, 1);
        return 0;
    }
    int pass = d58_crc32(expected62, D62_SIZE) == D62_EXPECTED_CRC &&
               of_output_open(D62_PATH, D62_SIZE) == OF_ERR_NOT_SUPPORTED;
    printf("Dynamic62 PC fallback: %s\n", pass ? "PASS" : "FAIL");
    return pass ? 0 : 1;
#else
    (void)argc; (void)argv;
    initial62();
    if (!OF_OUTPUT_SVC_HAS(file_size) || !OF_OUTPUT_SVC_HAS(input_poll_p0)) {
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
    title62();
    printf("MODE %c RUNNING\n", create_mode ? 'A' : 'B');
    printf("DIRECT 0192 THEN RAW 0184/0180\n");
    printf("DO NOT QUIT OR POWER OFF\n");
    fflush(stdout);
    struct result62 r;
    run62(&r, create_mode);
    unsigned page = 0;
    result62(&r, page);
    last = 0;
    for (;;) {
        of_input_poll_p0();
        uint32_t keys = _of_input_p0.buttons & 0xffffu;
        uint32_t edges = keys & ~last;
        last = keys;
        if (edges & (OF_BTN_LEFT | OF_BTN_RIGHT)) {
            page ^= 1u;
            result62(&r, page);
        }
        usleep(16000);
    }
#endif
}
