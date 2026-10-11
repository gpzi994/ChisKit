#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "identify56.h"

static uint8_t header_a[DB55_HEADER_BYTES], header_b[DB55_HEADER_BYTES];
static uint8_t snapshot[IDENT56_SNAPSHOT_BYTES];
static struct db55_result content;
static struct ident56_hw hardware;
static int caps_rc, safe_rc, read_rc, probe_rc, repeat_ok, index_ok;
static unsigned cart_reads, probe_complete;

static void title(unsigned page) {
    printf("\033[2J\033[H GBA IDENT smoke.56 %u/4\n", page);
}

static void ascii(char *out, const uint8_t *in, unsigned n) {
    for (unsigned i = 0; i < n; ++i)
        out[i] = (in[i] >= 0x20u && in[i] <= 0x7eu) ? (char)in[i] : '.';
    out[n] = 0;
}

static void hex8(const uint8_t *p) {
    static const char d[] = "0123456789ABCDEF";
    for (unsigned i = 0; i < 8; ++i) {
        putchar(d[p[i] >> 4]); putchar(d[p[i] & 15u]);
        if (i != 7) putchar(' ');
    }
    putchar('\n');
}

static uint32_t plan(void) {
    return ident56_dump_plan(&content, &hardware, probe_complete && probe_rc == 0);
}

static void page1(void) {
    char name[13], code[5];
    ascii(name, header_a + 0xa0, 12); ascii(code, header_a + 0xac, 4);
    title(1); printf("CONTENT IDENTIFICATION\n");
    printf("HEADER %u/24 RC %d REPEAT %s\n", cart_reads, read_rc,
           repeat_ok ? "PASS" : "FAIL");
    printf("TITLE %.12s\nCODE %.4s REV %02X\n", name, code,
           (unsigned)header_a[0xbc]);
    printf("LOGO %s CHECKSUM %s\n", content.logo_ok ? "PASS" : "FAIL",
           content.checksum_ok ? "PASS" : "FAIL");
    printf("DATABASE %s\n", content.entry ? "MATCH" : "NOT FOUND");
    printf("CAPS RC %d  DB INDEX %s\n", caps_rc, index_ok ? "PASS" : "FAIL");
    if (content.entry) {
        printf("DB ROM %lu MiB CRC %08lX\n",
               (unsigned long)(content.entry->rom_size >> 20),
               (unsigned long)content.entry->rom_crc32);
        printf("SAVE TYPE %u SIZE %lu\n", content.entry->save_type,
               (unsigned long)content.entry->save_size);
    } else printf("DB SIZE UNKNOWN\n");
    printf("RIGHT: NEXT  A: HARDWARE PROBE\n"); fflush(stdout);
}

static void page2(void) {
    title(2); printf("BOUNDED HARDWARE PROBE\n");
    printf("ONE FIXED SEQUENCE ONLY\n");
    printf("F0 > BASE > ID > F0 > CFI > F0\n");
    printf("NO PROGRAM OR ERASE OPCODES\n");
    printf("NO APP WRITE ADDRESS OR DATA\n");
    printf("WRITES_ENABLED REMAINS 0\n");
    if (!probe_complete) {
        printf("KEEP CART INSERTED; PRESS A ONCE\n");
        printf("DO NOT POWER OFF DURING PROBE\n");
    } else printf("PROBE COMPLETE RC %d SAFE RC %d\n", probe_rc, safe_rc);
    printf("LEFT/RIGHT: PAGE  A: RUN ONCE\n"); fflush(stdout);
}

static void page3(void) {
    title(3); printf("HARDWARE RESULT\n");
    if (!probe_complete) {
        printf("NOT TESTED; PRESS A\n");
    } else if (probe_rc) {
        printf("PROBE FAILED RC %d\n", probe_rc);
    } else {
        printf("BASE "); hex8(hardware.baseline);
        printf("ID   "); hex8(hardware.jedec);
        printf("CFI  "); hex8(hardware.cfi_qry);
        printf("SIZE "); hex8(hardware.cfi_size);
        printf("ID CHANGE %s  QRY %s\n", hardware.id_changed ? "YES" : "NO",
               hardware.qry_ok ? "PASS" : "NO");
        printf("CFI EXP %u  PHYS %lu MiB\n", hardware.size_exponent,
               (unsigned long)(hardware.physical_size >> 20));
        printf("SUPPORTED NOR %s\n", hardware.supported_nor ? "YES" : "NO");
    }
    printf("LEFT/RIGHT: PAGE  A: RUN ONCE\n"); fflush(stdout);
}

static void page4(void) {
    title(4); printf("CLASSIFICATION AND DUMP PLAN\n");
    if (!probe_complete || probe_rc) {
        printf("HARDWARE RESULT REQUIRED\n");
    } else if (hardware.supported_nor) {
        printf("HARDWARE: FLASHCART NOR\n");
        printf("PROFILE: S29GL256N COMPATIBLE\n");
        printf("PHYSICAL NOR: 32 MiB\n");
        printf("RULE: DUMP PHYSICAL CAPACITY\n");
    } else {
        printf("HARDWARE: GENERIC ROM PATH\n");
        printf("NO SUPPORTED NOR PROFILE FOUND\n");
        printf("NOT AN AUTHENTICITY CLAIM\n");
        printf("RULE: DB SIZE; MISS => 32 MiB\n");
    }
    printf("FINAL DUMP PLAN %lu BYTES\n", (unsigned long)plan());
    printf("FINAL DUMP PLAN %lu MiB\n", (unsigned long)(plan() >> 20));
    printf("THIS BUILD DOES NOT DUMP OR WRITE\n");
    printf("LEFT/RIGHT: PAGE  A: RUN ONCE\n"); fflush(stdout);
}

static void show(unsigned page) {
    if (page == 1) page1(); else if (page == 2) page2();
    else if (page == 3) page3(); else page4();
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
    memcpy(header_a + 0xa0, "WARIOLANDE  ", 12);
    memcpy(header_a + 0xac, "AWAC", 4); header_a[0xbc] = 0;
    memcpy(header_b, header_a, sizeof(header_a)); cart_reads = 24; repeat_ok = 1;
    const uint8_t awac[20] = {0xe3,0x49,0x7e,0xc9,0xcf,0x54,0xb0,0x86,
        0xa7,0x76,0x0c,0x34,0xc6,0x20,0x7b,0xc9,0xc1,0x37,0x07,0xfd};
    content.entry = db55_lookup(awac); probe_complete = 1;
    memcpy(snapshot, header_a, 8); memcpy(snapshot + 8, header_a, 8);
    ident56_parse(snapshot, &hardware);
    show(1); show(2); show(3); show(4); return index_ok ? 0 : 1;
#else
    of_cart_caps_t caps;
    title(1); printf("READING CONTENT HEADER\nNO WRITE OR DUMP IN THIS BUILD\n");
    fflush(stdout);
    caps_rc = of_cart_get_caps(&caps);
    if (caps_rc || caps.version != OF_CART_CAPS_VERSION ||
        caps.bus_mask != OF_CART_BUS_MASK_GBA || caps.max_read_bytes != 32 ||
        caps.writes_enabled ||
        (caps.gba_space_mask & (OF_CART_SPACE_MASK_ROM | OF_CART_SPACE_MASK_IDENTIFY)) !=
        (OF_CART_SPACE_MASK_ROM | OF_CART_SPACE_MASK_IDENTIFY)) {
        printf("STOP INVALID CART CAPS RC %d\n", caps_rc); halt();
    }
    safe_rc = of_cart_enter_safe_idle();
    if (safe_rc) { printf("STOP SAFE IDLE RC %d\n", safe_rc); halt(); }
    read_rc = read_header(header_a);
    if (!read_rc) read_rc = read_header(header_b);
    repeat_ok = !read_rc && memcmp(header_a, header_b, sizeof(header_a)) == 0;
    safe_rc = of_cart_enter_safe_idle();
    if (read_rc || !repeat_ok || safe_rc || !index_ok) {
        title(1); printf("STOP HEADER/DB VALIDATION\nREAD %d REPEAT %d SAFE %d DB %d\n",
                        read_rc, repeat_ok, safe_rc, index_ok); halt();
    }
    db55_identify_header(header_a, &content);
    unsigned page = 1, previous = ~0u; show(page);
    while (1) {
        of_input_poll_p0(); unsigned held = _of_input_p0.buttons;
        unsigned edge = held & ~previous; previous = held;
        if ((edge & OF_BTN_A) && !probe_complete) {
            title(2); printf("RUNNING FIXED PROBE; KEEP POWER ON\n"); fflush(stdout);
            probe_rc = of_cart_read_space(OF_CART_BUS_GBA, OF_CART_SPACE_IDENTIFY,
                                          0, snapshot, sizeof(snapshot), 50000);
            probe_complete = 1;
            if (!probe_rc) ident56_parse(snapshot, &hardware);
            safe_rc = of_cart_enter_safe_idle(); page = 3; show(page);
        } else if (edge & OF_BTN_RIGHT) {
            page = page == 4 ? 1 : page + 1; show(page);
        } else if (edge & OF_BTN_LEFT) {
            page = page == 1 ? 4 : page - 1; show(page);
        }
        usleep(16000);
    }
#endif
}
