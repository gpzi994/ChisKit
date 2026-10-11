#include <stdio.h>
#include <unistd.h>
#include "of_output.h"
#include "dump57.h"
#include "read_block.h"

static struct guard51 gate;
static struct scan57 boot;
static struct db55_result content;
static struct ident56_hw hardware;
static of_cart_caps_t caps;
static unsigned reads, writes, cart_reads, ident_reads, binds;
static uint32_t dump_plan;

static const char *phase57(enum stage57 p) {
    static const char *n[] = {
        "IDENTIFY CART","BOOT TF SCAN","CART CAPABILITY","CART SAMPLES",
        "TF BIND","CART ROM -> TF","TF WRITE","BLOCK READBACK",
        "FULL TF VERIFY","COMPLETE"
    };
    return n[p];
}
static void title57(void) {
    printf("\033[2J\033[H GBA DUMP smoke.57\n32 MiB CHISFLASH READ ONLY\n");
}
static void hash57(const uint8_t *d) {
    char h[33]; md5_hex53(d, h); printf("%s\n", h);
}
static void identity57(void) {
    printf("CONTENT BPEE DB 16 MiB\n");
    printf("HARDWARE S29GL256N 32 MiB\n");
    printf("DUMP PLAN %lu MiB\n", (unsigned long)(dump_plan >> 20));
}
static void progress57(enum stage57 phase, unsigned blocks) {
    title57(); printf("%s\n", phase57(phase));
    printf("BYTES %lu/%lu BLOCKS %u/%u\n",
           (unsigned long)blocks * CHUNK57, (unsigned long)TOTAL57,
           blocks, BLOCKS57);
    printf("CART READ32 %u/%u IDENT %u/1\n", cart_reads, CART_READS57, ident_reads);
    printf("TF WRITE %u READ %u BIND %u\n", writes, reads, binds);
    printf("FLUSH NOT USED; SAVE NOT ACCESSED\n");
    printf("PLEASE WAIT; KEEP POWER ON\n"); fflush(stdout);
}
static const char *boot_kind57(void) {
    return boot.kind == ZERO57 ? "ZERO SEED" :
           boot.kind == ROM57 ? "ROM VERIFIED" : "UNKNOWN";
}
static void gate_text57(void) {
    switch (gate.state) {
    case G_QUIET: printf("LOCKED: RELEASE ALL 0.5 SEC\n"); break;
    case G_LOCKED: printf("HOLD L+R FOR 2 SECONDS\n"); break;
    case G_HOLD_LR: printf("KEEP L+R HELD 2 SECONDS\n"); break;
    case G_RELEASE_LR: printf("RELEASE ALL 0.5 SECOND\n"); break;
    case G_CHOOSE: printf("A: DUMP ROM  B: CANCEL\n"); break;
    case G_RELEASE_MODE: printf("RELEASE ALL 0.5 SECOND\n"); break;
    case G_CONFIRM: printf("HOLD START (+) 1 SECOND\n"); break;
    case G_HOLD_START: printf("KEEP START HELD 1 SECOND\n"); break;
    case G_DONE: printf("CONFIRMED; ONE SHOT ONLY\n"); break;
    }
}
static void show_boot57(void) {
    title57(); identity57();
    printf("OUTPUT %s\n", NAME57);
    printf("BOOT CONTENT: %s\n", boot_kind57());
    printf("TF RC %d BYTES %u/%u\n", boot.rc, boot.bytes, TOTAL57);
    if (boot.bytes == TOTAL57) { printf("TF MD5\n"); hash57(boot.md5); }
    printf("SOURCE PREFIX CRC %08lX\n", (unsigned long)boot.db_crc32);
    printf("EXPECT %08lX; DB REF %08lX\n",
           (unsigned long)SOURCE_PREFIX_CRC57,
           (unsigned long)DB_REFERENCE_CRC57);
    if (boot.kind == ZERO57 && !boot.rc) gate_text57();
    else if (boot.kind == ROM57)
        printf("WRITING DISABLED; NO REWRITE\n");
    else printf("STOP UNKNOWN FILE; NO OVERWRITE\n");
    printf("CART ROM/SAVE NEVER WRITTEN\nFLUSH NOT USED\n"); fflush(stdout);
}
static void result57(const struct job57 *r) {
    title57(); printf("RESULT PAGE 1/2\nROM DUMP %s RC %d\n",
                      r->rc ? "FAIL" : "PASS", r->rc);
    printf("STAGE %s\n", phase57(r->stage));
    printf("BYTES %u/%u BLOCKS %u/%u\n", r->bytes, TOTAL57, r->blocks, BLOCKS57);
    printf("SAMPLES %u/5 SAFE RC %d\n", r->samples, r->safe_rc);
    if (r->stop == TOTAL57) printf("STOP OFFSET: NONE\n");
    else printf("STOP OFFSET %08lX\n", (unsigned long)r->stop);
    printf("CART MD5\n"); hash57(r->cart_md5);
    printf("TF MD5\n"); hash57(r->verify.md5);
    printf("DB CRC CART %08lX TF %08lX\n",
           (unsigned long)r->cart_db_crc32,
           (unsigned long)r->verify.db_crc32);
    printf("SECONDS %lu; FLUSH NOT USED\n", (unsigned long)(r->ms / 1000));
    printf(r->rc ? "STOP; SEND SCREEN; NO RETRY\n" :
                   "QUIT; POWER CYCLE; RUN AGAIN\n");
    printf("LEFT/RIGHT: TIMING PAGE 2\n"); fflush(stdout);
}
static void timing57(const struct job57 *r) {
    const struct times57 *t = &r->time;
    uint32_t accounted = accounted57(t);
    title57(); printf("TIMING PAGE 2/2 - MILLISECONDS\n");
    printf("CART READ      %lu\n", (unsigned long)t->cart);
    printf("TF WRITE       %lu\n", (unsigned long)t->write);
    printf("BLOCK READBACK %lu\n", (unsigned long)t->readback);
    printf("HASH+COMPARE   %lu\n", (unsigned long)t->hash);
    printf("SAFE IDLE      %lu\n", (unsigned long)t->safe);
    printf("FINAL TF VERIFY %lu\n", (unsigned long)t->verify);
    printf("OTHER / UI     %lu\n",
           (unsigned long)(r->ms >= accounted ? r->ms - accounted : 0));
    printf("DUMP TOTAL     %lu\n", (unsigned long)r->ms);
    printf("BOOT TF SCAN   %lu\n", (unsigned long)boot.ms);
    printf("TF W %u R %u CART %u\n", writes, reads, cart_reads);
    printf("TF 32 KiB; CART 32 B\n");
    printf("FLUSH NOT USED; SAVE NOT ACCESSED\n");
    printf("LEFT/RIGHT OR A: RESULT PAGE 1\n"); fflush(stdout);
}

#ifndef OF_PC
#include "of_file.h"
#include "of_input.h"
static uint32_t now57(void) { return OF_SVC->timer_get_ms(); }
static void idle57(void) { usleep(1000); }
static int sdk_read57(uint32_t off, uint8_t *dst, uint32_t n) {
    const struct read_ops57 o = {
        of_file_read_async, of_file_async_poll, of_file_async_busy,
        now57, idle57
    };
    ++reads; return read_block57(&o, off, dst, n);
}
static int sdk_write57(uint32_t off, const void *src, uint32_t n) {
    ++writes; return of_output_write(off, src, n);
}
static int sdk_cart57(uint32_t space, uint32_t off, uint8_t *dst) {
    if (space == OF_CART_SPACE_ROM) ++cart_reads; else ++ident_reads;
    return of_cart_read_space(OF_CART_BUS_GBA, space, off, dst, 32, 50000);
}
static int sdk_bind57(void) { ++binds; return of_output_open(NAME57, TOTAL57); }
static void halt57(void) { fflush(stdout); while (1) usleep(100000); }

static int identify_cart57(void) {
    uint8_t a[DB55_HEADER_BYTES], b[DB55_HEADER_BYTES];
    uint8_t snap[IDENT56_SNAPSHOT_BYTES];
    int rc = of_cart_get_caps(&caps);
    if (rc || !caps_valid57(&caps)) return rc ? rc : OF_ERR_DENIED;
    rc = of_cart_enter_safe_idle(); if (rc) return rc;
    for (uint32_t off = 0; off < DB55_HEADER_BYTES; off += 32u) {
        rc = sdk_cart57(OF_CART_SPACE_ROM, off, a + off); if (rc) return rc;
    }
    for (uint32_t off = 0; off < DB55_HEADER_BYTES; off += 32u) {
        rc = sdk_cart57(OF_CART_SPACE_ROM, off, b + off); if (rc) return rc;
    }
    if (memcmp(a, b, sizeof(a)) || !db55_validate_index()) return OF_ERR_FAILED;
    db55_identify_header(a, &content);
    if (!content.entry || !content.logo_ok || !content.checksum_ok ||
        content.entry->rom_size != DB_PREFIX57 ||
        content.entry->rom_crc32 != DB_REFERENCE_CRC57 ||
        memcmp(content.entry->game_code, "BPEE", 4)) return OF_ERR_DENIED;
    rc = sdk_cart57(OF_CART_SPACE_IDENTIFY, 0, snap); if (rc) return rc;
    ident56_parse(snap, &hardware);
    dump_plan = ident56_dump_plan(&content, &hardware, 1);
    rc = of_cart_enter_safe_idle(); if (rc) return rc;
    if (!hardware.supported_nor || dump_plan != TOTAL57) return OF_ERR_BAD_RANGE;
    return 0;
}
#endif

int main(int argc, char **argv) {
    (void)argc; (void)argv; (void)fill51; (void)identify51; (void)crc51;
    (void)content; (void)hardware; (void)caps; (void)progress57;
    (void)run57; (void)read_block57; (void)gate_tick52;
#ifdef OF_PC
    dump_plan = TOTAL57;
    boot = (struct scan57){.kind=ZERO57,.bytes=TOTAL57,.blocks=BLOCKS57,
                           .db_crc32=SOURCE_PREFIX_CRC57};
    for (unsigned i = G_QUIET; i <= G_DONE; ++i) {
        gate.state = i; show_boot57();
    }
    boot.kind = ROM57; memcpy(boot.md5, expected57, 16); show_boot57();
    struct job57 r = {.stage=DONE57,.bytes=TOTAL57,.blocks=BLOCKS57,
                      .samples=5,.stop=TOTAL57,.ms=390000,
                      .cart_db_crc32=SOURCE_PREFIX_CRC57};
    r.time = (struct times57){120000,42000,65000,20000,4000,70000};
    r.verify = (struct scan57){.kind=ROM57,.bytes=TOTAL57,
                               .db_crc32=SOURCE_PREFIX_CRC57};
    memcpy(r.cart_md5, expected57, 16); memcpy(r.verify.md5, expected57, 16);
    result57(&r); timing57(&r); return 0;
#else
    title57(); printf("IDENTIFYING CART; NO WRITE YET\n"); fflush(stdout);
    if (!OF_OUTPUT_SVC_HAS(output_open) || !OF_OUTPUT_SVC_HAS(output_write) ||
        !OF_OUTPUT_SVC_HAS(file_size) || !OF_OUTPUT_SVC_HAS(timer_get_ms) ||
        !OF_OUTPUT_SVC_HAS(input_poll_p0)) {
        printf("STOP API MISSING\n"); halt57();
    }
    int rc = of_output_open(NULL, 0);
    if (rc != MAGIC57) {
        printf("STOP WRONG OS %08lX\nCOPY WHOLE PACKAGE; POWER CYCLE\n",
               (unsigned long)rc); halt57();
    }
    rc = identify_cart57();
    if (rc) {
        title57(); printf("IDENTIFICATION STOP RC %d\n", rc);
        printf("DB %s DB SIZE %lu MiB\n", content.entry ? "MATCH" : "MISS",
               content.entry ? (unsigned long)(content.entry->rom_size >> 20) : 0ul);
        printf("NOR %s PHYS %lu MiB PLAN %lu MiB\n",
               hardware.supported_nor ? "YES" : "NO",
               (unsigned long)(hardware.physical_size >> 20),
               (unsigned long)(dump_plan >> 20));
        printf("THIS BUILD REQUIRES BPEE + 32 MiB NOR\n");
        printf("NO TF OR CARTRIDGE WRITE OCCURRED\n"); halt57();
    }
    long size = OF_SVC->file_size("slot:4");
    if (size != TOTAL57) {
        printf("STOP SLOT4 SIZE %ld EXPECT %u\n", size, TOTAL57); halt57();
    }
    const struct ops57 o = {
        of_cart_get_caps, sdk_cart57, of_cart_enter_safe_idle, of_cart_abort,
        sdk_bind57, sdk_write57, sdk_read57, now57, progress57
    };
    boot = scan57(&o, BOOT57);
    show_boot57();
    if (boot.kind != ZERO57 || boot.rc) halt57();
    unsigned previous = ~0u;
    while (1) {
        of_input_poll_p0(); unsigned held = _of_input_p0.buttons;
        enum gate51 old = gate.state;
        if (gate_tick52(&gate, held, now57())) break;
        if (old != gate.state) show_boot57();
        previous = held; (void)previous; usleep(16000);
    }
    struct job57 r = run57(&gate, &boot, &o); result57(&r);
    unsigned page = 0; previous = ~0u;
    while (1) {
        of_input_poll_p0(); unsigned held = _of_input_p0.buttons;
        unsigned edge = held & ~previous; previous = held;
        if (edge & (OF_BTN_LEFT | OF_BTN_RIGHT | OF_BTN_A)) {
            if (edge & OF_BTN_A) page = 0; else page ^= 1u;
            if (page) timing57(&r); else result57(&r);
        }
        usleep(16000);
    }
#endif
}
