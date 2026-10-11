#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "of_cart.h"

#define SNAP_BYTES 32u

static const uint8_t expect_id[4] = {0xc2,0x7e,0x10,0x00};
static const uint8_t expect_geom[8] = {0x07,0x00,0x20,0x00,
                                       0x7e,0x00,0x00,0x01};
static const uint8_t expect_pri[8] = {0x50,0x52,0x49,0x31,
                                      0x31,0x00,0x02,0x02};
static const uint8_t expect_meta[4] = {0x17,0x05,0x00,0x02};

static uint8_t snap[SNAP_BYTES];
static of_cart_caps_t caps_before, caps_after;
static int caps_rc, safe_start_rc, probe_rc, safe_end_rc;
static unsigned ran;

static void title(unsigned page) {
    printf("\033[2J\033[H GB/GBC MX29GL640EB GEOM smoke.92 %u/4\n", page);
}

static void bytes(const uint8_t *p, unsigned n) {
    for (unsigned i=0; i<n; ++i) {
        printf("%02x%s", p[i], i+1 == n ? "\n" : " ");
    }
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

static unsigned exact(const uint8_t *a, const uint8_t *b, unsigned n) {
    return memcmp(a,b,n) == 0;
}

static uint16_t le16(const uint8_t *p) {
    return (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

static void page1(void) {
    title(1);
    printf("READ ONLY; FIXED JEDEC/CFI COMMANDS\n");
    printf("REPEAT BOTH ERASE REGION RECORDS\n");
    printf("READ PRI BOOT ORIENTATION\n");
    printf("NO ERASE / PROGRAM / ROM / SRAM\n\n");
    printf("CAPS %s RC %d\n", caps_rc == 0 && caps_ok(&caps_before) ?
           "PASS" : "FAIL", caps_rc);
    printf("SAFE START %s RC %d\n", safe_start_rc == 0 ? "PASS" : "FAIL",
           safe_start_rc);
    printf("LEFT/RIGHT: PAGE  A: CAPTURE\n");
}

static void page2(void) {
    title(2);
    printf("EXPECTED FROM MACRONIX PM1494\n");
    printf("ID C2 7E 10 00 = BOTTOM BOOT\n");
    printf("REG0 8 x 8 KiB\n");
    printf("REG1 127 x 64 KiB\n");
    printf("TOTAL 8 MiB; BUFFER 32 BYTES\n");
    printf("PRI BOOT INDICATOR 02\n\n");
    printf("%s\n", ran ? "CAPTURE COMPLETE" : "PRESS A ONCE TO CAPTURE");
    printf("LEFT/RIGHT: PAGE  A: CAPTURE\n");
}

static void page3(void) {
    title(3);
    if (!ran) printf("NOT RUN\n");
    else if (probe_rc) printf("CAPTURE FAILED RC %d\n", probe_rc);
    else {
        printf("JEDEC 0000/02/1c/1e:\n"); bytes(snap,4);
        printf("GEOM PASS1 5a..68 EVEN:\n"); bytes(snap+4,8);
        printf("GEOM PASS2 5a..68 EVEN:\n"); bytes(snap+12,8);
        printf("PRI 80/82/84/86/88/8a/8c/9e:\n"); bytes(snap+20,8);
        printf("META 4e/54/56/58:\n"); bytes(snap+28,4);
    }
    printf("LEFT/RIGHT: PAGE\n");
}

static void page4(void) {
    title(4);
    if (!ran || probe_rc) printf("CAPTURE RESULT REQUIRED\n");
    else {
        uint16_t n0=(uint16_t)(le16(snap+4)+1u);
        uint16_t n1=(uint16_t)(le16(snap+8)+1u);
        uint32_t s0=(uint32_t)le16(snap+6)*256u;
        uint32_t s1=(uint32_t)le16(snap+10)*256u;
        uint32_t total=(uint32_t)n0*s0+(uint32_t)n1*s1;
        uint32_t profile=caps_after.cart_profile_raw;
        unsigned repeats=exact(snap+4,snap+12,8);
        unsigned all=exact(snap,expect_id,4) &&
                     exact(snap+4,expect_geom,8) && repeats &&
                     exact(snap+20,expect_pri,8) &&
                     exact(snap+28,expect_meta,4) &&
                     (profile & 0x13u) == 0x13u && safe_end_rc == 0;
        printf("JEDEC MX29GL640EB %s\n", exact(snap,expect_id,4)?"PASS":"FAIL");
        printf("GEOM REPEAT %s  EXPECT %s\n", repeats?"PASS":"FAIL",
               exact(snap+4,expect_geom,8)?"PASS":"FAIL");
        printf("REG0 %u x %lu KiB\n", n0, (unsigned long)(s0>>10));
        printf("REG1 %u x %lu KiB\n", n1, (unsigned long)(s1>>10));
        printf("TOTAL %lu KiB %s\n", (unsigned long)(total>>10),
               total == 8u*1024u*1024u ? "PASS":"FAIL");
        printf("PRI1.1 BOTTOM BOOT %s\n",
               exact(snap+20,expect_pri,8)?"PASS":"FAIL");
        printf("SIZE/BUFFER/REGIONS %s\n",
               exact(snap+28,expect_meta,4)?"PASS":"FAIL");
        printf("FINAL/SAFE %s  DECISION %s\n",
               ((profile&0x10u) && safe_end_rc==0)?"PASS":"FAIL",
               all?"PROFILE READY":"BLOCKED");
        printf("NO ERASE OR PROGRAM PERFORMED\n");
    }
    printf("LEFT/RIGHT: PAGE\n");
}

static void show(unsigned page) {
    if(page==1) page1(); else if(page==2) page2();
    else if(page==3) page3(); else page4();
    fflush(stdout);
}

#ifndef OF_PC
#include "of_input.h"
static void halt(void) { while (1) usleep(100000); }
#endif

int main(void) {
#ifdef OF_PC
    memset(&caps_before,0,sizeof(caps_before)); ran=1;
    probe_rc=OF_ERR_NOT_SUPPORTED; show(1); show(2); show(3); show(4);
    return 0;
#else
    caps_rc=of_cart_get_caps(&caps_before);
    safe_start_rc=of_cart_enter_safe_idle();
    if(caps_rc || !caps_ok(&caps_before) || safe_start_rc) {
        page1(); printf("STOP: INVALID SAFE GEOMETRY PROFILE\n"); halt();
    }
    unsigned page=1, previous=~0u; show(page);
    while(1) {
        of_input_poll_p0(); unsigned held=_of_input_p0.buttons;
        unsigned edge=held & ~previous; previous=held;
        if((edge&OF_BTN_A) && !ran) {
            title(2); printf("CAPTURING REPEATED CFI GEOMETRY\n"); fflush(stdout);
            probe_rc=of_cart_read_space(OF_CART_BUS_GB_GBC,
                                        OF_CART_SPACE_IDENTIFY,0,snap,
                                        sizeof(snap),50000);
            ran=1; safe_end_rc=of_cart_enter_safe_idle();
            memset(&caps_after,0,sizeof(caps_after));
            (void)of_cart_get_caps(&caps_after); page=3; show(page);
        } else if(edge&OF_BTN_RIGHT) { page=page==4?1:page+1; show(page); }
        else if(edge&OF_BTN_LEFT) { page=page==1?4:page-1; show(page); }
        usleep(16000);
    }
#endif
}
