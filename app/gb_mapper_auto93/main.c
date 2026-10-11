#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "of_cart.h"

#define BLOCK_BYTES 32u
#define HEADER_BYTES 96u
#define READ_TIMEOUT_US 50000u

enum mapper_family { MAP_UNKNOWN, MAP_MBC3, MAP_MBC5 };

static const uint32_t request_address[11] = {
    0x0000u, 0x0020u, 0x4000u, 0x4020u,
    0x0000u, 0x0020u, 0x4000u, 0x4020u,
    0x0100u, 0x0120u, 0x0140u
};

static const uint8_t nintendo_logo[48] = {
    0xce,0xed,0x66,0x66,0xcc,0x0d,0x00,0x0b,
    0x03,0x73,0x00,0x83,0x00,0x0c,0x00,0x0d,
    0x00,0x08,0x11,0x1f,0x88,0x89,0x00,0x0e,
    0xdc,0xcc,0x6e,0xe6,0xdd,0xdd,0xd9,0x99,
    0xbb,0xbb,0x67,0x63,0x6e,0x0e,0xec,0xcc,
    0xdd,0xdc,0x99,0x9f,0xbb,0xb9,0x33,0x3e
};

static uint8_t fixed1[64], switch1[64], fixed2[64], switch2[64];
static uint8_t header[HEADER_BYTES];
static of_cart_caps_t caps_before, caps_after;
static int caps_rc, safe_start_rc, read_rc, safe_end_rc;
static unsigned ran;

static enum mapper_family behavior_family, header_family;
static unsigned repeat_ok, relation_ok, logo_ok, checksum_ok;
static unsigned rom_size_ok, samples_ok, agreement_ok, all_ok;
static uint32_t rom_bytes, ram_bytes;

static void title(unsigned page) {
    printf("\033[2J\033[H GB/GBC MBC3+MBC5 AUTO smoke.93 %u/4\n", page);
}

static const char *family_name(enum mapper_family f) {
    return f == MAP_MBC3 ? "MBC3" : f == MAP_MBC5 ? "MBC5" : "UNKNOWN";
}

static unsigned caps_ok(const of_cart_caps_t *c) {
    return c->version == OF_CART_CAPS_VERSION &&
           c->bus_mask == OF_CART_BUS_MASK_GB_GBC &&
           c->operation_mask == (OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
                                 OF_CART_OP_ABORT | OF_CART_OP_QUERY) &&
           c->max_read_bytes == 32u && c->writes_enabled == 0u &&
           c->gb_gbc_space_mask == OF_CART_SPACE_MASK_ROM &&
           c->gba_space_mask == 0u;
}

static unsigned all_value(const uint8_t *p, unsigned n, uint8_t v) {
    for (unsigned i = 0; i < n; ++i) if (p[i] != v) return 0;
    return 1;
}

static uint8_t header_checksum(void) {
    uint8_t x = 0;
    for (unsigned i = 0x34u; i <= 0x4cu; ++i)
        x = (uint8_t)(x - header[i] - 1u);
    return x;
}

static enum mapper_family family_from_type(uint8_t type) {
    if (type >= 0x0fu && type <= 0x13u) return MAP_MBC3;
    if (type >= 0x19u && type <= 0x1eu) return MAP_MBC5;
    return MAP_UNKNOWN;
}

static uint32_t rom_size_from_code(uint8_t code) {
    if (code <= 8u) return 32768u << code;
    if (code == 0x52u) return 72u * 16384u;
    if (code == 0x53u) return 80u * 16384u;
    if (code == 0x54u) return 96u * 16384u;
    return 0;
}

static uint32_t ram_size_from_code(uint8_t code) {
    static const uint32_t sizes[6] = {0u,2048u,8192u,32768u,131072u,65536u};
    return code < 6u ? sizes[code] : 0u;
}

static void analyze(void) {
    unsigned equal1 = memcmp(fixed1, switch1, sizeof(fixed1)) == 0;
    unsigned equal2 = memcmp(fixed2, switch2, sizeof(fixed2)) == 0;
    behavior_family = equal1 ? MAP_MBC5 : MAP_MBC3;
    relation_ok = equal1 == equal2;
    repeat_ok = memcmp(fixed1, fixed2, sizeof(fixed1)) == 0 &&
                memcmp(switch1, switch2, sizeof(switch1)) == 0;
    samples_ok = !all_value(fixed1, sizeof(fixed1), 0xff) &&
                 !all_value(fixed1, sizeof(fixed1), 0x00) &&
                 !all_value(switch1, sizeof(switch1), 0xff) &&
                 !all_value(switch1, sizeof(switch1), 0x00);
    header_family = family_from_type(header[0x47]);
    logo_ok = memcmp(header + 4, nintendo_logo, sizeof(nintendo_logo)) == 0;
    checksum_ok = header_checksum() == header[0x4d];
    rom_bytes = rom_size_from_code(header[0x48]);
    ram_bytes = ram_size_from_code(header[0x49]);
    rom_size_ok = rom_bytes != 0 &&
                  !(header_family == MAP_MBC3 && rom_bytes > 2u*1024u*1024u);
    agreement_ok = header_family != MAP_UNKNOWN &&
                   behavior_family == header_family;
    all_ok = read_rc == 0 && repeat_ok && relation_ok && samples_ok &&
             logo_ok && checksum_ok && rom_size_ok && agreement_ok &&
             safe_end_rc == 0 && caps_ok(&caps_after);
}

static int capture(void) {
    uint8_t *dst[11] = {
        fixed1, fixed1+32, switch1, switch1+32,
        fixed2, fixed2+32, switch2, switch2+32,
        header, header+32, header+64
    };
    for (unsigned i = 0; i < 11; ++i) {
        int rc = of_cart_read_space(OF_CART_BUS_GB_GBC, OF_CART_SPACE_ROM,
                                    request_address[i], dst[i], BLOCK_BYTES,
                                    READ_TIMEOUT_US);
        if (rc != OF_OK) return rc;
    }
    return OF_OK;
}

static void print8(const uint8_t *p) {
    for (unsigned i = 0; i < 8; ++i) printf("%02x%s", p[i], i==7?"\n":" ");
}

static void page1(void) {
    title(1);
    printf("READ ONLY; MORI BEHAVIORAL METHOD\n");
    printf("BANK 0 REQUEST AT 2000H; REPEAT 2X\n");
    printf("EQUAL WINDOWS=MBC5; DIFFERENT=MBC3\n");
    printf("HEADER FAMILY MUST AGREE\n");
    printf("NO NOR / SRAM / SAVE COMMANDS\n\n");
    printf("CAPS %s RC %d\n", caps_rc==0 && caps_ok(&caps_before)?"PASS":"FAIL",caps_rc);
    printf("SAFE START %s RC %d\n", safe_start_rc==0?"PASS":"FAIL",safe_start_rc);
    printf("LEFT/RIGHT: PAGE  A: CAPTURE\n");
}

static void page2(void) {
    title(2);
    printf("TEST EACH CARTRIDGE SEPARATELY\n");
    printf("POWER OFF BEFORE SWAPPING CARTS\n\n");
    printf("EXPECTED: ONE MBC3 AND ONE MBC5\n");
    printf("BEHAVIOR + HEADER + REPEAT REQUIRED\n");
    printf("32-BYTE FIRST TRANSACTION DISCARDED\n\n");
    printf("%s\n", ran?"CAPTURE COMPLETE":"PRESS A ONCE TO CAPTURE");
    printf("LEFT/RIGHT: PAGE  A: CAPTURE\n");
}

static void page3(void) {
    title(3);
    if (!ran) printf("NOT RUN\n");
    else if (read_rc) printf("CAPTURE FAILED RC %d\n", read_rc);
    else {
        printf("FIXED PASS1: "); print8(fixed1);
        printf("SWITCH PASS1: "); print8(switch1);
        printf("FIXED PASS2: "); print8(fixed2);
        printf("SWITCH PASS2: "); print8(switch2);
        printf("RELATION %s  REPEAT %s\n", relation_ok?"PASS":"FAIL",repeat_ok?"PASS":"FAIL");
        printf("BEHAVIOR: %s\n", family_name(behavior_family));
        printf("HEADER TYPE %02x: %s\n", header[0x47],family_name(header_family));
        printf("HCHK %02x CALC %02x\n",header[0x4d],header_checksum());
    }
    printf("LEFT/RIGHT: PAGE\n");
}

static void page4(void) {
    title(4);
    if (!ran || read_rc) printf("CAPTURE RESULT REQUIRED\n");
    else {
        printf("BEHAVIOR %-7s %s\n",family_name(behavior_family),relation_ok?"PASS":"FAIL");
        printf("HEADER TYPE %02x %-7s %s\n",header[0x47],family_name(header_family),
               header_family!=MAP_UNKNOWN?"PASS":"FAIL");
        printf("MAPPER AGREEMENT %s\n",agreement_ok?"PASS":"FAIL");
        printf("LOGO/CHECKSUM %s/%s\n",logo_ok?"PASS":"FAIL",checksum_ok?"PASS":"FAIL");
        printf("ROM %lu KiB RAM %lu KiB %s\n",
               (unsigned long)(rom_bytes>>10),(unsigned long)(ram_bytes>>10),
               rom_size_ok?"PASS":"FAIL");
        printf("REPEAT/SAMPLES %s/%s\n",repeat_ok?"PASS":"FAIL",samples_ok?"PASS":"FAIL");
        printf("FINAL/SAFE %s  DECISION %s\n",safe_end_rc==0?"PASS":"FAIL",
               all_ok?"AUTO PROFILE READY":"BLOCKED");
        printf("NO NOR/SRAM COMMANDS\n");
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
    memset(&caps_before,0,sizeof(caps_before)); ran=1; read_rc=OF_ERR_NOT_SUPPORTED;
    show(1); show(2); show(3); show(4); return 0;
#else
    caps_rc=of_cart_get_caps(&caps_before);
    safe_start_rc=of_cart_enter_safe_idle();
    if(caps_rc || !caps_ok(&caps_before) || safe_start_rc) {
        page1(); printf("STOP: INVALID READ-ONLY PROFILE\n"); halt();
    }
    unsigned page=1, previous=~0u; show(page);
    while(1) {
        of_input_poll_p0(); unsigned held=_of_input_p0.buttons;
        unsigned edge=held & ~previous; previous=held;
        if((edge&OF_BTN_A) && !ran) {
            title(2); printf("CAPTURING MAPPER BEHAVIOR + HEADER\n"); fflush(stdout);
            read_rc=capture(); ran=1;
            safe_end_rc=of_cart_enter_safe_idle();
            memset(&caps_after,0,sizeof(caps_after));
            (void)of_cart_get_caps(&caps_after); analyze(); page=3; show(page);
        } else if(edge&OF_BTN_RIGHT) { page=page==4?1:page+1; show(page); }
        else if(edge&OF_BTN_LEFT) { page=page==1?4:page-1; show(page); }
        usleep(16000);
    }
#endif
}
