#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "db55.h"

static uint8_t header_a[DB55_HEADER_BYTES], header_b[DB55_HEADER_BYTES];
static struct db55_result result;
static int read_rc, safe_rc, caps_rc, repeat_ok, index_ok;
static unsigned cart_reads;

static void title(unsigned page) {
    printf("\033[2J\033[H GBA DB smoke.55  %u/3\n", page);
}

static void hex(const uint8_t *p, unsigned n) {
    static const char d[] = "0123456789ABCDEF";
    for (unsigned i = 0; i < n; ++i) {
        putchar(d[p[i] >> 4]); putchar(d[p[i] & 15u]);
    }
}

static void ascii(char *out, const uint8_t *in, unsigned n) {
    for (unsigned i = 0; i < n; ++i)
        out[i] = (in[i] >= 0x20u && in[i] <= 0x7eu) ? (char)in[i] : '.';
    out[n] = 0;
}

static void page1(void) {
    char name[13], code[5], maker[3];
    ascii(name, header_a + 0xa0, 12); ascii(code, header_a + 0xac, 4);
    ascii(maker, header_a + 0xb0, 2);
    title(1); printf("READ-ONLY DATABASE IDENTIFY\n");
    printf("HEADER READ %u/24 RC %d REPEAT %s\n", cart_reads, read_rc,
           repeat_ok ? "PASS" : "FAIL");
    printf("TITLE %.12s\nCODE %.4s MAKER %.2s VER %02X\n", name, code, maker,
           (unsigned)header_a[0xbc]);
    printf("LOGO %s FIXED96 %s CHECKSUM %s\n",
           result.logo_ok ? "PASS" : "FAIL",
           result.fixed96_ok ? "PASS" : "FAIL",
           result.checksum_ok ? "PASS" : "FAIL");
    printf("HEADER SHA1 (%u BYTES)\n", result.hash_bytes * 16u);
    hex(result.header_sha1, 20); putchar('\n');
    printf("DATABASE: %s\n", result.entry ? "MATCH" : "NOT FOUND");
    printf("DUMP PLAN %lu BYTES (%lu MiB)\n",
           (unsigned long)result.dump_size,
           (unsigned long)(result.dump_size >> 20));
    printf("POLICY %s\n", result.entry ? "DB EXACT + CRC VERIFY" :
           "UNKNOWN => FULL 32 MiB");
    printf("SAFE RC %d; WRITES ABSENT\n", safe_rc);
    printf("LEFT/RIGHT: PAGE  A: PAGE 1\n"); fflush(stdout);
}

static void page2(void) {
    title(2); printf("CHISBURNER DB PINNED SNAPSHOT\n");
    printf("ENTRIES %lu / 2923\n", (unsigned long)db55_entry_count);
    printf("SOURCE COMMIT %.12s\n", db55_source_commit);
    printf("DB SHA256\n%.32s\n%.32s\n", db55_source_sha256,
           db55_source_sha256 + 32);
    printf("RUNTIME INDEX %s\n", index_ok ? "COMPLETE PASS" : "FAIL");
    printf("FIELDS SHA1 RS RC ST SS GC RV 3D\n");
    printf("CANONICAL JSON INCLUDED IN PACKAGE\n");
    printf("CURRENT %s PLAN %lu MiB\n", result.entry ? "MATCH" : "MISS",
           (unsigned long)(result.dump_size >> 20));
    printf("ROM/SAVE/TF WRITES: NONE\n");
    printf("LEFT/RIGHT: PAGE  A: PAGE 1\n"); fflush(stdout);
}

static void page3(void) {
    title(3);
    if (result.entry) {
        printf("DATABASE MATCH DETAILS\nDB CODE ");
        for (unsigned i = 0; i < 4; ++i) putchar(result.entry->game_code[i]);
        printf("\nROM SIZE %lu\nROM CRC32 %08lX\n",
               (unsigned long)result.entry->rom_size,
               (unsigned long)result.entry->rom_crc32);
        printf("SAVE TYPE %u SIZE %lu\n", result.entry->save_type,
               (unsigned long)result.entry->save_size);
        printf("3D FIELD %s VALUE %s\n",
               (result.entry->flags & DB55_FLAG_HAS_3D) ? "YES" : "NO",
               (result.entry->flags & DB55_FLAG_3D) ? "TRUE" : "FALSE");
        printf("REV FIELD %s\n",
               (result.entry->flags & DB55_FLAG_HAS_RV) ? "YES" : "NO");
        printf("NEXT DUMP EXACT; VERIFY CRC32\n");
    } else {
        printf("DATABASE MISS DETAILS\nHEADER SHA1\n");
        hex(result.header_sha1, 20); putchar('\n');
        printf("EXACT SOURCE LENGTH UNKNOWN\n");
        printf("MIRROR PROBE IS NOT CAPACITY\n");
        printf("FALLBACK 33554432 BYTES\n");
        printf("SAVE TYPE/SIZE UNKNOWN\n");
        printf("NO DUMP PERFORMED IN smoke.55\n");
    }
    printf("CAPS RC %d HEADER RC %d\n", caps_rc, read_rc);
    printf("LEFT/RIGHT: PAGE  A: PAGE 1\n"); fflush(stdout);
}

static void show(unsigned page) {
    if (page == 1) page1(); else if (page == 2) page2(); else page3();
}

#ifndef OF_PC
#include "of_cart.h"
#include "of_input.h"

static int read_header(uint8_t *dst) {
    for (uint32_t off = 0; off < DB55_HEADER_BYTES; off += 32u) {
        int rc = of_cart_read_space(OF_CART_BUS_GBA, OF_CART_SPACE_ROM,
                                    off, dst + off, 32, 50000);
        ++cart_reads; if (rc) return rc;
    }
    return 0;
}

static void halt(void) { while (1) usleep(100000); }
#endif

int main(void) {
    index_ok = db55_validate_index();
#ifdef OF_PC
    for (unsigned i = 0; i < DB55_HEADER_BYTES; ++i) header_a[i] = (uint8_t)i;
    memcpy(header_b, header_a, sizeof(header_a)); cart_reads = 24;
    db55_identify_header(header_a, &result); repeat_ok = 1;
    show(1); show(2); show(3);
    return index_ok ? 0 : 1;
#else
    of_cart_caps_t caps;
    title(1); printf("READING GBA HEADER; PLEASE WAIT\nWRITES STRUCTURALLY ABSENT\n");
    fflush(stdout);
    caps_rc = of_cart_get_caps(&caps);
    if (caps_rc || caps.version != OF_CART_CAPS_VERSION ||
        caps.bus_mask != OF_CART_BUS_MASK_GBA || caps.max_read_bytes != 32 ||
        caps.writes_enabled || !(caps.gba_space_mask & OF_CART_SPACE_MASK_ROM)) {
        printf("STOP INVALID CART CAPS RC %d\n", caps_rc); halt();
    }
    safe_rc = of_cart_enter_safe_idle();
    if (safe_rc) { printf("STOP SAFE IDLE RC %d\n", safe_rc); halt(); }
    read_rc = read_header(header_a);
    if (!read_rc) read_rc = read_header(header_b);
    repeat_ok = !read_rc && memcmp(header_a, header_b, sizeof(header_a)) == 0;
    safe_rc = of_cart_enter_safe_idle();
    if (read_rc || !repeat_ok || safe_rc || !index_ok) {
        title(1); printf("STOP HEADER/DB VALIDATION\nREAD RC %d REPEAT %d SAFE %d DB %d\n",
                         read_rc, repeat_ok, safe_rc, index_ok); halt();
    }
    db55_identify_header(header_a, &result);
    unsigned page = 1, previous = ~0u; show(page);
    while (1) {
        of_input_poll_p0(); unsigned held = _of_input_p0.buttons;
        unsigned edge = held & ~previous; previous = held;
        if (edge & OF_BTN_A) { page = 1; show(page); }
        else if (edge & OF_BTN_RIGHT) { page = page == 3 ? 1 : page + 1; show(page); }
        else if (edge & OF_BTN_LEFT) { page = page == 1 ? 3 : page - 1; show(page); }
        usleep(16000);
    }
#endif
}
