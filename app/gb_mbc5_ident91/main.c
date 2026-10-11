#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "of_cart.h"

#define SNAP_BYTES 32u

static uint8_t snap[SNAP_BYTES];
static of_cart_caps_t caps_before, caps_after;
static int caps_rc, safe_start_rc, probe_rc, safe_end_rc;
static unsigned ran;

static void title(unsigned page) {
    printf("\033[2J\033[H GB/GBC MBC5 NOR IDENT smoke.91 %u/4\n", page);
}

static void bytes(const uint8_t *p, unsigned n) {
    for (unsigned i=0; i<n; ++i) {
        printf("%02x", p[i]);
        if (i+1 != n) putchar(' ');
    }
    putchar('\n');
}

static int caps_ok(const of_cart_caps_t *c) {
    return c->version == OF_CART_CAPS_VERSION &&
           c->bus_mask == OF_CART_BUS_MASK_GB_GBC &&
           c->operation_mask == (OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
                                 OF_CART_OP_ABORT | OF_CART_OP_QUERY) &&
           c->max_read_bytes == 32u && c->writes_enabled == 0u &&
           c->gb_gbc_space_mask == OF_CART_SPACE_MASK_IDENTIFY &&
           c->gba_space_mask == 0u;
}

static unsigned qry_ok(void) {
    return snap[8] == 0x51 && snap[9] == 0x52 && snap[10] == 0x59;
}

static unsigned id_changed(void) {
    return memcmp(snap, snap+4, 4) != 0;
}

static uint16_t cfi16(unsigned lo) {
    return (uint16_t)(snap[lo] | ((uint16_t)snap[lo+1] << 8));
}

static void page1(void) {
    title(1);
    printf("FIXED IDENTIFY COMMANDS ONLY\n");
    printf("MORI MBC5 ID + TWO CFI ENTRIES\n");
    printf("NO ERASE / PROGRAM / SRAM / ROM\n");
    printf("NO HOST WRITE ADDRESS OR DATA\n\n");
    printf("CAPS %s RC %d\n", caps_rc == 0 && caps_ok(&caps_before) ?
           "PASS" : "FAIL", caps_rc);
    printf("SAFE START %s RC %d\n", safe_start_rc == 0 ? "PASS" : "FAIL",
           safe_start_rc);
    printf("LEFT/RIGHT: PAGE  A: IDENTIFY\n");
}

static void page2(void) {
    title(2);
    printf("FIXED BUS SEQUENCE\n");
    printf("DISCARD FIRST READ\n");
    printf("F0 > BASE > AA/55/90 > ID > F0\n");
    printf("CFI 98@00AA; FALLBACK 98@0000\n");
    printf("MATCHED RESET; FINAL NORMAL READ\n\n");
    if (!ran) printf("KEEP CART INSERTED; PRESS A ONCE\n");
    else printf("IDENTIFY COMPLETE RC %d\n", probe_rc);
    printf("LEFT/RIGHT: PAGE  A: IDENTIFY\n");
}

static void page3(void) {
    title(3);
    if (!ran) {
        printf("NOT RUN; PRESS A ON PAGE 1 OR 2\n");
    } else if (probe_rc) {
        printf("IDENTIFY FAILED RC %d\n", probe_rc);
    } else {
        printf("BASE 0000/02/1c/1e: "); bytes(snap,4);
        printf("ID   0000/02/1c/1e: "); bytes(snap+4,4);
        printf("CFI  20..2e EVEN:\n"); bytes(snap+8,8);
        printf("CFI  30..3e EVEN:\n"); bytes(snap+16,8);
        printf("GEOM 4e/54/56/58/5a/5c/5e/60:\n"); bytes(snap+24,8);
    }
    printf("LEFT/RIGHT: PAGE\n");
}

static void page4(void) {
    title(4);
    if (!ran || probe_rc) {
        printf("IDENTIFY RESULT REQUIRED\n");
    } else {
        uint32_t profile=caps_after.cart_profile_raw;
        unsigned method=(profile >> 2) & 3u;
        uint16_t cmdset=cfi16(11), pri=cfi16(13);
        uint16_t buffer_exp=cfi16(25), sectors_m1=cfi16(28);
        uint32_t sector_bytes=(uint32_t)cfi16(30) * 256u;
        printf("ID CHANGED %s  QRY %s\n", id_changed()?"PASS":"FAIL",
               qry_ok()?"PASS":"FAIL");
        printf("PROFILE %08lx  CFI ENTRY %u\n",
               (unsigned long)profile, method);
        printf("CMDSET %04x PRI %04x\n", cmdset, pri);
        printf("SIZE EXP %u", snap[24]);
        if (snap[24] < 32u) printf(" = %lu MiB\n",
            (unsigned long)((1ul << snap[24]) >> 20));
        else printf(" = INVALID\n");
        printf("BUFFER EXP %u  REGIONS %u\n", buffer_exp, snap[27]);
        printf("REGION0 SECTORS %u SIZE %lu KiB\n",
               (unsigned)(sectors_m1+1u),
               (unsigned long)(sector_bytes >> 10));
        printf("FINAL NORMAL %s  SAFE %s\n",
               (profile & 0x10u)?"PASS":"FAIL",
               safe_end_rc==0?"PASS":"FAIL");
        printf("IDENTIFY ONLY; NO FLASH PROFILE YET\n");
    }
    printf("LEFT/RIGHT: PAGE\n");
}

static void show(unsigned page) {
    if (page==1) page1(); else if (page==2) page2();
    else if (page==3) page3(); else page4();
    fflush(stdout);
}

#ifndef OF_PC
#include "of_input.h"
static void halt(void) { while (1) usleep(100000); }
#endif

int main(void) {
#ifdef OF_PC
    memset(&caps_before,0,sizeof(caps_before));
    ran=1; probe_rc=OF_ERR_NOT_SUPPORTED;
    show(1); show(2); show(3); show(4);
    printf("PC fallback closed: %s\n", of_cart_available()==0?"PASS":"FAIL");
    return 0;
#else
    caps_rc=of_cart_get_caps(&caps_before);
    safe_start_rc=of_cart_enter_safe_idle();
    if (caps_rc || !caps_ok(&caps_before) || safe_start_rc) {
        page1(); printf("STOP: INVALID SAFE IDENTIFY PROFILE\n"); halt();
    }
    unsigned page=1, previous=~0u; show(page);
    while (1) {
        of_input_poll_p0(); unsigned held=_of_input_p0.buttons;
        unsigned edge=held & ~previous; previous=held;
        if ((edge & OF_BTN_A) && !ran) {
            title(2); printf("RUNNING FIXED ID/CFI; KEEP POWER ON\n");
            fflush(stdout);
            probe_rc=of_cart_read_space(OF_CART_BUS_GB_GBC,
                                        OF_CART_SPACE_IDENTIFY,0,snap,
                                        sizeof(snap),50000);
            ran=1; safe_end_rc=of_cart_enter_safe_idle();
            memset(&caps_after,0,sizeof(caps_after));
            (void)of_cart_get_caps(&caps_after);
            page=3; show(page);
        } else if (edge & OF_BTN_RIGHT) {
            page=page==4?1:page+1; show(page);
        } else if (edge & OF_BTN_LEFT) {
            page=page==1?4:page-1; show(page);
        }
        usleep(16000);
    }
#endif
}
