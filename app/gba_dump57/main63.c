#define main main57_reference
#include "main.c"
#undef main
#include "dynamic63.h"

static void title63(void) {
    printf("\033[2J\033[H GBA DYNAMIC DUMP smoke.63\n");
    printf("IDENTIFY FIRST; CHISFLASH READ ONLY\n");
}

static void progress63(enum stage57 phase, unsigned blocks) {
    title63();
    printf("%s\n", phase57(phase));
    printf("BYTES %lu/%lu\n", (unsigned long)blocks * CHUNK57,
           (unsigned long)TOTAL57);
    printf("BLOCKS %u/%u\n", blocks, BLOCKS57);
    printf("CART READ32 %u/%u IDENT %u/1\n", cart_reads,
           CART_READS57, ident_reads);
    printf("TF WRITE %u READ %u BIND %u\n", writes, reads, binds);
    printf("DYNAMIC FILE; NO 32 MiB SEED\n");
    printf("FLUSH NOT USED; SAVE NOT ACCESSED\n");
    printf("PLEASE WAIT; KEEP POWER ON\n");
    fflush(stdout);
}

static const char *tf_state63(unsigned created) {
    if (created) return "NEW FILE - SCAN SKIPPED";
    if (boot.kind == ZERO57 && !boot.rc) return "EXISTING ZERO";
    if (boot.kind == ROM57 && !boot.rc) return "ROM VERIFIED";
    return "INVALID - NO WRITE";
}

static void show63(unsigned created, long size, unsigned host) {
    title63();
    identity57();
    printf("OPEN HOST %u  TF SIZE %ld\n", host, size);
    printf("TF STATE: %s\n", tf_state63(created));
    printf("/Saves/chisburner/common/\n");
    printf("dump57-32m.gba\n");
    if (boot.kind == ROM57 && !boot.rc) {
        printf("TF MD5\n"); hash57(boot.md5);
        printf("NO REWRITE; DYNAMIC PERSISTENCE PASS\n");
    } else if (created || (boot.kind == ZERO57 && !boot.rc)) {
        gate_text57();
    } else {
        printf("TF RC %d BYTES %u/%u\n", boot.rc, boot.bytes, TOTAL57);
        printf("DELETE BAD OUTPUT ON PC TO RETRY\n");
    }
    printf("CART ROM/SAVE NEVER WRITTEN\n");
    printf("APF 0188 FLUSH NOT CALLED\n");
    fflush(stdout);
}

static void result63(const struct job57 *r) {
    title63();
    printf("RESULT PAGE 1/2\nROM DUMP %s RC %d\n",
           r->rc ? "FAIL" : "PASS", r->rc);
    printf("STAGE %s\n", phase57(r->stage));
    printf("BYTES %u/%u\n", r->bytes, TOTAL57);
    printf("BLOCKS %u/%u\n", r->blocks, BLOCKS57);
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
                   "QUIT; COLD BOOT; RUN AGAIN\n");
    printf("LEFT/RIGHT: TIMING PAGE 2\n");
    fflush(stdout);
}

static void timing63(const struct job57 *r) {
    const struct times57 *t = &r->time;
    uint32_t accounted = accounted57(t);
    title63();
    printf("TIMING PAGE 2/2 - MILLISECONDS\n");
    printf("CART READ       %lu\n", (unsigned long)t->cart);
    printf("TF WRITE        %lu\n", (unsigned long)t->write);
    printf("BLOCK READBACK  %lu\n", (unsigned long)t->readback);
    printf("HASH+COMPARE    %lu\n", (unsigned long)t->hash);
    printf("SAFE IDLE       %lu\n", (unsigned long)t->safe);
    printf("FINAL TF VERIFY %lu\n", (unsigned long)t->verify);
    printf("OTHER / UI      %lu\n",
           (unsigned long)(r->ms >= accounted ? r->ms - accounted : 0));
    printf("DUMP TOTAL      %lu\n", (unsigned long)r->ms);
    printf("TF W %u R %u CART %u\n", writes, reads, cart_reads);
    printf("TF 32 KiB; CART 32 B\n");
    printf("APF 0188 FLUSH NOT CALLED\n");
    printf("LEFT/RIGHT OR A: RESULT PAGE 1\n");
    fflush(stdout);
}

#ifndef OF_PC
#define D63_REG32(a) (*(volatile uint32_t *)(uintptr_t)(a))
#define D63_SYSREG_BASE       0x40000000u
#define D63_DS_SLOT_ID        D63_REG32(D63_SYSREG_BASE + 0x20u)
#define D63_DS_PARAM_ADDR     D63_REG32(D63_SYSREG_BASE + 0x30u)
#define D63_DS_COMMAND        D63_REG32(D63_SYSREG_BASE + 0x38u)
#define D63_DS_STATUS         D63_REG32(D63_SYSREG_BASE + 0x3cu)
#define D63_DS_ACK            (1u << 0)
#define D63_DS_DONE           (1u << 1)
#define D63_DS_ERR_MASK       0x1cu
#define D63_DS_ERR_SHIFT      2u
#define D63_DS_READY          (1u << 5)
#define D63_DS_WR_IDLE        (1u << 6)
#define D63_DS_IRQ_PENDING    (1u << 7)
#define D63_CRAM0_CPU         0x30400000u
#define D63_CRAM0_BRIDGE      0x20400000u
#define D63_CRAM0_MODE        D63_REG32(0x4e000000u)
#define D63_TIMEOUT           200000000u

static void d63_fence(void) {
    __asm__ volatile("fence" ::: "memory");
}

static int d63_wait_mask(uint32_t mask, uint32_t want) {
    uint32_t timeout = D63_TIMEOUT;
    while ((D63_DS_STATUS & mask) != want) {
        if (--timeout == 0u) return -1;
    }
    return 0;
}

static int d63_command(uint32_t success_mask, unsigned *host_result) {
    if (d63_wait_mask(D63_DS_READY | D63_DS_WR_IDLE,
                      D63_DS_READY | D63_DS_WR_IDLE)) return -10;
    D63_DS_STATUS = D63_DS_IRQ_PENDING;
    d63_fence();
    D63_DS_COMMAND = 3u;
    d63_fence();
    if (d63_wait_mask(D63_DS_ACK, D63_DS_ACK)) return -11;
    if (d63_wait_mask(D63_DS_DONE, D63_DS_DONE)) return -12;
    unsigned result = (D63_DS_STATUS & D63_DS_ERR_MASK) >> D63_DS_ERR_SHIFT;
    if (host_result) *host_result = result;
    if (d63_wait_mask(D63_DS_READY | D63_DS_WR_IDLE,
                      D63_DS_READY | D63_DS_WR_IDLE)) return -13;
    D63_DS_STATUS = D63_DS_IRQ_PENDING;
    return (success_mask & (1u << result)) ? 0 :
           (result ? -(int)result : -20);
}

static int d63_open(unsigned create, unsigned *host_result) {
    uint8_t *param = (uint8_t *)(uintptr_t)D63_CRAM0_CPU;
    D63_CRAM0_MODE = 1u;
    for (volatile unsigned i = 0; i < 8u; ++i) {}
    d63_make_open_param(param, create, TOTAL57);
    d63_fence();
    D63_CRAM0_MODE = 0u;
    for (volatile unsigned i = 0; i < 8u; ++i) {}
    D63_DS_SLOT_ID = D63_SLOT;
    D63_DS_PARAM_ADDR = D63_CRAM0_BRIDGE;
    return d63_command(create ? (1u << 1) : ((1u << 0) | (1u << 3)),
                       host_result);
}

static void halt63(void) {
    fflush(stdout);
    while (1) usleep(100000);
}
#endif

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    (void)progress63;
#ifdef OF_PC
    dump_plan = TOTAL57;
    title63();
    printf("IDENTIFY CART FIRST\nNO TF OPEN/CREATE/WRITE YET\n");
    boot = (struct scan57){.kind=ZERO57,.bytes=TOTAL57,.blocks=BLOCKS57};
    show63(1u, TOTAL57, 1u);
    boot = (struct scan57){.kind=ROM57,.bytes=TOTAL57,.blocks=BLOCKS57,
                           .db_crc32=SOURCE_PREFIX_CRC57};
    memcpy(boot.md5, expected57, sizeof(expected57));
    show63(0u, TOTAL57, 0u);
    struct job57 r = {.stage=DONE57,.bytes=TOTAL57,.blocks=BLOCKS57,
                      .samples=5,.stop=TOTAL57,.ms=390000,
                      .cart_db_crc32=SOURCE_PREFIX_CRC57};
    r.time = (struct times57){120000,42000,65000,20000,4000,70000};
    r.verify = boot;
    memcpy(r.cart_md5, expected57, sizeof(expected57));
    result63(&r); timing63(&r);
    return 0;
#else
    title63();
    printf("IDENTIFYING CART\n");
    printf("NO TF FILE OPEN/CREATE/WRITE YET\n");
    fflush(stdout);
    if (!OF_OUTPUT_SVC_HAS(output_open) || !OF_OUTPUT_SVC_HAS(output_write) ||
        !OF_OUTPUT_SVC_HAS(file_size) || !OF_OUTPUT_SVC_HAS(timer_get_ms) ||
        !OF_OUTPUT_SVC_HAS(input_poll_p0)) {
        printf("STOP API MISSING\n"); halt63();
    }
    int rc = of_output_open(NULL, 0);
    if (rc != MAGIC57) {
        printf("STOP WRONG OS %08lX\n", (unsigned long)rc);
        printf("COPY WHOLE PACKAGE; POWER CYCLE\n");
        halt63();
    }
    rc = identify_cart57();
    if (rc) {
        title63();
        printf("IDENTIFICATION STOP RC %d\n", rc);
        printf("DB %s DB SIZE %lu MiB\n", content.entry ? "MATCH" : "MISS",
               content.entry ?
               (unsigned long)(content.entry->rom_size >> 20) : 0ul);
        printf("NOR %s PHYS %lu MiB PLAN %lu MiB\n",
               hardware.supported_nor ? "YES" : "NO",
               (unsigned long)(hardware.physical_size >> 20),
               (unsigned long)(dump_plan >> 20));
        printf("NO TF FILE OPENED OR CREATED\n");
        printf("NO TF OR CARTRIDGE WRITE OCCURRED\n");
        halt63();
    }

    unsigned host = ~0u, created = 0u;
    rc = d63_open(0u, &host);
    if (!rc && host == 3u) {
        rc = d63_open(1u, &host);
        created = !rc && host == 1u;
    }
    if (rc || (!created && host != 0u)) {
        title63();
        printf("DYNAMIC OPEN STOP RC %d HOST %u\n", rc, host);
        printf("IDENTIFICATION PASSED; NO DATA WRITE\n");
        halt63();
    }
    long size = OF_SVC->file_size("slot:4");
    if (size != TOTAL57) {
        title63();
        printf("STOP SLOT4 SIZE %ld EXPECT %u\n", size, TOTAL57);
        printf("NO DATA WRITE\n");
        halt63();
    }

    const struct ops57 o = {
        of_cart_get_caps, sdk_cart57, of_cart_enter_safe_idle, of_cart_abort,
        sdk_bind57, sdk_write57, sdk_read57, now57, progress63
    };
    if (created) {
        boot = (struct scan57){.kind=ZERO57,.bytes=TOTAL57,
                               .blocks=BLOCKS57};
    } else {
        boot = scan57(&o, BOOT57);
    }
    show63(created, size, host);
    if (boot.kind == ROM57 && !boot.rc) halt63();
    if ((!created && boot.kind != ZERO57) || boot.rc) halt63();

    unsigned previous = ~0u;
    while (1) {
        of_input_poll_p0();
        unsigned held = _of_input_p0.buttons;
        enum gate51 old = gate.state;
        if (gate_tick52(&gate, held, now57())) break;
        if (old != gate.state) show63(created, size, host);
        previous = held; (void)previous;
        usleep(16000);
    }
    struct job57 r = run57(&gate, &boot, &o);
    result63(&r);
    unsigned page = 0u;
    previous = ~0u;
    while (1) {
        of_input_poll_p0();
        unsigned held = _of_input_p0.buttons;
        unsigned edge = held & ~previous;
        previous = held;
        if (edge & (OF_BTN_LEFT | OF_BTN_RIGHT | OF_BTN_A)) {
            if (edge & OF_BTN_A) page = 0u; else page ^= 1u;
            if (page) timing63(&r); else result63(&r);
        }
        usleep(16000);
    }
#endif
}
