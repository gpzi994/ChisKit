#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifndef PROGRAM65_BUILD_LABEL
#define PROGRAM65_BUILD_LABEL "smoke.66"
#endif
#ifndef PROGRAM65_PC_TITLE
#define PROGRAM65_PC_TITLE "POKEMON EMER"
#endif
#ifndef PROGRAM65_PC_GAME_CODE
#define PROGRAM65_PC_GAME_CODE "BPEE"
#endif
#ifndef PROGRAM65_PROTOCOL_PREFLIGHT
#define PROGRAM65_PROTOCOL_PREFLIGHT 0
#endif
#ifndef PROGRAM65_RUNTIME_SLOT
#define PROGRAM65_RUNTIME_SLOT 0
#endif

#include "program65.h"
#include "of.h"
#include "../storage_stream52/gate.h"

#define PROGRAM65_SLOT 4

static struct guard51 gate65;
static struct program65_source source65;
static struct program65_result result65;
static unsigned source_reads65;

static void title65(void) {
    printf("\033[2J\033[H GBA PROGRAM %s\n", PROGRAM65_BUILD_LABEL);
    printf("TARGET S29GL256S 32 MiB\n");
}

static const char *stage65(enum program65_stage stage) {
    static const char *names[] = {
        "SOURCE TF SCAN", "IDENTIFY TARGET", "ERASE 128 KiB",
        "PROGRAM BUFFER", "FINAL CART MD5", "COMPLETE"
    };
    return names[stage];
}

static void hash65(const uint8_t value[16]) {
    char text[33];
    md5_hex53(value, text);
    printf("%s\n", text);
}

static void gate_text65(void) {
    switch (gate65.state) {
    case G_QUIET: printf("LOCKED: RELEASE ALL 0.5 SEC\n"); break;
    case G_LOCKED: printf("HOLD L+R FOR 2 SECONDS\n"); break;
    case G_HOLD_LR: printf("KEEP L+R HELD 2 SECONDS\n"); break;
    case G_RELEASE_LR: printf("RELEASE ALL 0.5 SECOND\n"); break;
    case G_CHOOSE: printf("A: ERASE+PROGRAM  B: CANCEL/RELOCK\n"); break;
    case G_RELEASE_MODE: printf("RELEASE ALL 0.5 SECOND\n"); break;
    case G_CONFIRM: printf("HOLD START (+) 1 SECOND\n"); break;
    case G_HOLD_START: printf("KEEP START HELD 1 SECOND\n"); break;
    case G_DONE: printf("CONFIRMED; ONE SHOT ONLY\n"); break;
    }
}

static void show_ready65(const of_cart_flash_caps_t *flash) {
    title65();
    printf("SOURCE %s %s\n", source65.title, source65.game_code);
    printf("SIZE %lu B HEADER %s\n",
           (unsigned long)source65.bytes,
           source65.logo_ok && source65.checksum_ok && source65.fixed96_ok ?
               "PASS" : "FAIL");
    printf("SOURCE MD5\n");
    hash65(source65.md5);
    printf("TARGET AMD ID %08lX\n", (unsigned long)flash->id_prefix);
    printf("IMAGE %lu B\n", (unsigned long)PROGRAM65_ROM_BYTES);
    printf("FULL ERASE %u SECTORS\n", PROGRAM65_SECTORS);
    printf("128 KiB SECTOR / %u B PROGRAM\n", PROGRAM65_BUFFER_BYTES);
    printf("WARNING: ERASE IS DESTRUCTIVE\n");
    gate_text65();
    fflush(stdout);
}

static void progress65(enum program65_stage stage, uint32_t completed,
                       uint32_t total) {
    title65();
    printf("%s\n", stage65(stage));
    printf("%lu/%lu\n", (unsigned long)completed, (unsigned long)total);
    if (stage == P65_ERASE)
        printf("SECTOR %lu/%lu (128 KiB)\n",
               (unsigned long)completed, (unsigned long)total);
    else if (stage == P65_PROGRAM)
        printf("SOURCE 64 KiB; FLASH %u B\n", PROGRAM65_BUFFER_BYTES);
    else if (stage == P65_VERIFY)
        printf("CART READ %u B; MD5 ONLY\n", PROGRAM65_READ_BYTES);
    printf("KEEP POWER ON; DO NOT REMOVE CART\n");
    fflush(stdout);
}

static void show_result65(const struct program65_result *result) {
    title65();
    printf("RESULT PAGE 1/2\n");
    printf("PROGRAM %s RC %d\n", result->rc ? "FAIL" : "PASS", result->rc);
    printf("STAGE %s\n", stage65(result->stage));
    printf("ERASE %lu/%u\n", (unsigned long)result->erased_sectors,
           PROGRAM65_SECTORS);
    printf("PROGRAM %lu/%lu\n", (unsigned long)result->programmed_blocks,
           (unsigned long)PROGRAM65_PROGRAM_BLOCKS);
    printf("VERIFY %lu/%lu\n", (unsigned long)result->verified_bytes,
           (unsigned long)PROGRAM65_ROM_BYTES);
    if (result->stop_offset == PROGRAM65_ROM_BYTES)
        printf("STOP OFFSET: NONE\n");
    else
        printf("STOP OFFSET %08lX\n", (unsigned long)result->stop_offset);
    printf("SOURCE MD5\n"); hash65(result->source_md5);
    printf("CART MD5\n"); hash65(result->cart_md5);
    printf("DISARM %d SAFE %d\n", result->disarm_rc, result->safe_rc);
    printf(result->rc ? "STOP; DO NOT RETRY; SEND SCREEN\n" :
                        "SUCCESS; COLD BOOT AND TEST GAME\n");
    printf("LEFT/RIGHT: TIMING PAGE 2\n");
    fflush(stdout);
}

static void show_timing65(const struct program65_result *result) {
    title65();
    printf("TIMING PAGE 2/2 - MILLISECONDS\n");
    printf("SOURCE SCAN %lu\n", (unsigned long)result->source_ms);
    printf("IDENTIFY    %lu\n", (unsigned long)result->identify_ms);
    printf("ERASE       %lu\n", (unsigned long)result->erase_ms);
    printf("PROGRAM     %lu\n", (unsigned long)result->program_ms);
    printf("FINAL MD5   %lu\n", (unsigned long)result->verify_ms);
    printf("WRITE TOTAL %lu\n", (unsigned long)result->total_ms);
    printf("LAST WR %08lX +%lu\n",
           (unsigned long)result->last_program_address,
           (unsigned long)result->last_program_length);
    printf("LAST RD %08lX +%lu\n",
           (unsigned long)result->last_read_address,
           (unsigned long)result->last_read_length);
    printf("END 32 BYTE MATCH %s\n",
           result->terminal_match ? "PASS" : "FAIL");
    printf("TF READS %u + PROGRAM PASS\n", source_reads65);
    printf("NO CHIP ERASE; NO SAVE/RTC WRITE\n");
    printf("LEFT/RIGHT OR A: RESULT PAGE 1\n");
    fflush(stdout);
}

#ifndef OF_PC
static volatile unsigned callback_count65;
static volatile int callback_token65;
static volatile int callback_rc65;

static uint32_t now65(void) {
    return OF_SVC->timer_get_ms();
}

static void callback65(int token, int rc) {
    if (callback_count65 == 0) {
        callback_token65 = token;
        callback_rc65 = rc;
    }
    __asm__ volatile("" ::: "memory");
    callback_count65++;
}

static int source_read65(uint32_t offset, void *dst, uint32_t length) {
    int busy = of_file_async_busy();
    if (busy)
        return busy < 0 ? busy : OF_ERR_BUSY;
    callback_count65 = 0;
    callback_token65 = OF_ERR_INVALID_STATE;
    callback_rc65 = OF_ERR_INVALID_STATE;
    uint32_t begin = now65();
    int token = of_file_read_async(PROGRAM65_SLOT, offset, dst, length,
                                   callback65);
    if (token < 0)
        return token;
    while (callback_count65 == 0) {
        int rc = of_file_async_poll();
        if (rc < 0)
            return rc;
        if ((uint32_t)(now65() - begin) >= 10000u)
            return OF_ERR_TIMEOUT;
        usleep(1000);
    }
    __asm__ volatile("" ::: "memory");
    if (callback_count65 != 1 || callback_token65 != token)
        return OF_ERR_FAILED;
    if (callback_rc65 != OF_OK)
        return callback_rc65;
    busy = of_file_async_busy();
    if (busy)
        return busy < 0 ? busy : OF_ERR_BUSY;
    source_reads65++;
    return OF_OK;
}

static int cart_read65(uint32_t address, void *dst, uint32_t length,
                       uint32_t timeout_us) {
    return of_cart_read(OF_CART_BUS_GBA, address, dst, length, timeout_us);
}

static void halt65(void) {
    of_cart_flash_disarm();
    of_cart_enter_safe_idle();
    fflush(stdout);
    while (1)
        usleep(100000);
}

static long wait_source_slot65(void) {
    long size = OF_SVC->file_size("slot:4");
#if PROGRAM65_RUNTIME_SLOT
    while (size != (long)PROGRAM65_ROM_BYTES) {
        title65();
        printf("WAITING FOR PROGRAM ROM\n");
        printf("OPEN POCKET MENU > CORE SETTINGS\n");
        printf("LOAD PROGRAM ROM (SLOT 4)\n");
        printf("SIZE %ld EXPECT %u\n", size, PROGRAM65_ROM_BYTES);
        printf("NO CARTRIDGE ERASE OR WRITE YET\n");
        fflush(stdout);
        usleep(250000);
        size = OF_SVC->file_size("slot:4");
    }
#endif
    return size;
}

#if PROGRAM65_PROTOCOL_PREFLIGHT
#ifndef PROGRAM65_READY_RETRIES
#define PROGRAM65_READY_RETRIES 1u
#endif
#ifndef PROGRAM65_READY_STABLE_SAMPLES
#define PROGRAM65_READY_STABLE_SAMPLES 1u
#endif
#ifndef PROGRAM65_READY_RETRY_US
#define PROGRAM65_READY_RETRY_US 10000u
#endif
static int preflight_cart_rc65 = OF_ERR_INVALID_STATE;
static uint32_t preflight_cart_mask65;
static uint32_t preflight_flash_mask65;
static uint32_t preflight_life_first65;
static uint32_t preflight_life_last65;
static uint32_t preflight_life_tries65;
static uint32_t preflight_life_stable65;

static uint32_t preflight_cart_mask_value65(const of_cart_caps_t *cart) {
    return
        ((cart->struct_size == sizeof(*cart) &&
          cart->version == OF_CART_CAPS_VERSION) ? 1u : 0u) |
        ((cart->bus_mask == OF_CART_BUS_MASK_GBA) ? 2u : 0u) |
        ((cart->operation_mask ==
          (OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ | OF_CART_OP_ABORT |
           OF_CART_OP_QUERY | OF_CART_OP_ERASE_SECTOR |
           OF_CART_OP_PROGRAM)) ? 4u : 0u) |
        ((cart->max_read_bytes == PROGRAM65_READ_BYTES) ? 8u : 0u) |
        ((cart->writes_enabled == 1u) ? 0x10u : 0u) |
        ((cart->gb_gbc_space_mask == 0u &&
          cart->gba_space_mask ==
              (OF_CART_SPACE_MASK_ROM | OF_CART_SPACE_MASK_IDENTIFY)) ?
             0x20u : 0u) |
        (((cart->lifecycle_status & 0x80000307u) == 0x80000303u) ?
             0x40u : 0u);
}

/* Reproduce the read-only sequence proven on hardware by smoke.74 before
 * exposing the destructive confirmation gate.  QUERY/PING do not own the
 * cartridge pins; the final SAFE_IDLE leaves the target disarmed. */
static int preflight_target65(const struct program65_ops *ops,
                              of_cart_caps_t *cart,
                              of_cart_flash_caps_t *flash,
                              int *protocol_rc, int *ping_rc,
                              int *safe_rc) {
    memset(cart, 0, sizeof(*cart));
    memset(flash, 0, sizeof(*flash));
    of_cart_protocol_info_t protocol;
    memset(&protocol, 0, sizeof(protocol));
    uint32_t ping = 0;

    preflight_life_first65 = 0;
    preflight_life_last65 = 0;
    preflight_life_tries65 = 0;
    preflight_life_stable65 = 0;
    for (uint32_t attempt = 0; attempt < PROGRAM65_READY_RETRIES;
         ++attempt) {
        preflight_cart_rc65 = ops->cart_caps(cart);
        preflight_life_tries65 = attempt + 1u;
        if (preflight_cart_rc65 != OF_OK)
            return preflight_cart_rc65;
        if (attempt == 0)
            preflight_life_first65 = cart->lifecycle_status;
        preflight_life_last65 = cart->lifecycle_status;
        preflight_cart_mask65 = preflight_cart_mask_value65(cart);

        /* Bits 0..5 describe the immutable ABI/capability contract.  A
         * mismatch there is not a startup timing condition and fails at
         * once.  Only the lifecycle field is allowed a bounded read-only
         * settling interval, and it must be exact on consecutive samples. */
        if ((preflight_cart_mask65 & 0x3fu) != 0x3fu)
            return OF_ERR_DENIED;
        if (preflight_cart_mask65 == 0x7fu &&
            program65_cart_caps_valid(cart)) {
            preflight_life_stable65++;
            if (preflight_life_stable65 >=
                PROGRAM65_READY_STABLE_SAMPLES)
                break;
        } else {
            preflight_life_stable65 = 0;
        }
        if (attempt + 1u < PROGRAM65_READY_RETRIES)
            usleep(PROGRAM65_READY_RETRY_US);
    }
    if (preflight_life_stable65 < PROGRAM65_READY_STABLE_SAMPLES)
        return OF_ERR_DENIED;

    *protocol_rc = of_cart_query_info(&protocol);
    if (*protocol_rc != OF_OK)
        return *protocol_rc;
    if (protocol.magic != OF_CART_PROTOCOL_MAGIC ||
        protocol.version != OF_CART_PROTOCOL_VERSION ||
        (protocol.features & (OF_CART_PROTOCOL_QUERY | OF_CART_PROTOCOL_PING)) !=
            (OF_CART_PROTOCOL_QUERY | OF_CART_PROTOCOL_PING))
        return OF_ERR_DENIED;

    *ping_rc = of_cart_ping(0x13579BDFu, &ping);
    if (*ping_rc != OF_OK)
        return *ping_rc;
    if (ping != 0xECA86420u)
        return OF_ERR_DENIED;

    int rc = ops->flash_caps(flash);
    if (rc != OF_OK)
        return rc;
    preflight_flash_mask65 =
        ((flash->struct_size == sizeof(*flash) &&
          flash->version == OF_CART_FLASH_CAPS_VERSION) ? 1u : 0u) |
        ((flash->command_set == OF_CART_FLASH_CMDSET_AMD) ? 2u : 0u) |
        ((flash->flash_size_bytes == PROGRAM65_FLASH_BYTES &&
          flash->sector_size_bytes == PROGRAM65_SECTOR_BYTES) ? 4u : 0u) |
        ((flash->program_buffer_bytes == PROGRAM65_BUFFER_BYTES) ? 8u : 0u) |
        ((flash->profile_status == OF_CART_FLASH_PROFILE_OK) ? 0x10u : 0u) |
        ((flash->id_prefix == PROGRAM65_ID_PREFIX) ? 0x20u : 0u) |
        ((flash->cfi_signature == PROGRAM65_CFI_PREFIX) ? 0x40u : 0u);
    if (preflight_flash_mask65 != 0x7fu ||
        !program65_flash_caps_valid(flash))
        return OF_ERR_DENIED;

    *safe_rc = ops->safe_idle();
    return *safe_rc;
}
#endif
#endif

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    (void)fill51;
    (void)identify51;
    (void)crc51;
    (void)gate_tick52;
    (void)take51;
    (void)program65_scan_source;
    (void)program65_run;
    (void)progress65;
#ifdef OF_PC
    source65 = (struct program65_source){
        .rc = OF_OK,
        .bytes = PROGRAM65_ROM_BYTES,
        .blocks = PROGRAM65_SOURCE_BLOCKS,
        .logo_ok = 1,
        .checksum_ok = 1,
        .fixed96_ok = 1
    };
    memcpy(source65.title, PROGRAM65_PC_TITLE, 12);
    memcpy(source65.game_code, PROGRAM65_PC_GAME_CODE, 5);
    of_cart_flash_caps_t flash = {
        .id_prefix = PROGRAM65_ID_PREFIX
    };
    for (unsigned state = G_QUIET; state <= G_DONE; ++state) {
        gate65.state = (enum gate51)state;
        show_ready65(&flash);
    }
    result65 = (struct program65_result){
        .stage = P65_COMPLETE,
        .stop_offset = PROGRAM65_ROM_BYTES,
        .erased_sectors = PROGRAM65_SECTORS,
        .programmed_blocks = PROGRAM65_PROGRAM_BLOCKS,
        .verified_bytes = PROGRAM65_ROM_BYTES,
        .last_program_address =
            ((PROGRAM65_ROM_BYTES - 1u) / PROGRAM65_BUFFER_BYTES) *
                PROGRAM65_BUFFER_BYTES,
        .last_program_length =
            ((PROGRAM65_ROM_BYTES - 1u) % PROGRAM65_BUFFER_BYTES) + 1u,
        .last_read_address = PROGRAM65_ROM_BYTES - PROGRAM65_READ_BYTES,
        .last_read_length = PROGRAM65_READ_BYTES,
        .terminal_match = 1,
        .disarm_rc = OF_OK,
        .safe_rc = OF_OK
    };
    show_result65(&result65);
    show_timing65(&result65);
    return 0;
#else
    title65();
    printf("PREFLIGHT ONLY; NO ERASE OR WRITE\n");
    fflush(stdout);
    if (!OF_CART_SVC_HAS(cart_flash_get_caps) ||
        !OF_CART_SVC_HAS(cart_flash_erase_sector) ||
        !OF_CART_SVC_HAS(cart_flash_program) ||
        !OF_CART_SVC_HAS(cart_flash_disarm) ||
        !OF_CART_SVC_HAS(file_size) ||
        !OF_CART_SVC_HAS(timer_get_ms) ||
        !OF_CART_SVC_HAS(input_poll_p0)) {
        printf("STOP: PROGRAM API MISSING\n");
        halt65();
    }
    const struct program65_ops ops = {
        source_read65,
        of_cart_get_caps,
        of_cart_flash_get_caps,
        of_cart_enter_safe_idle,
        of_cart_flash_erase_sector,
        of_cart_flash_program,
        cart_read65,
        of_cart_flash_disarm,
        now65,
        progress65
    };
    of_cart_caps_t cart;
    of_cart_flash_caps_t flash;
#if PROGRAM65_PROTOCOL_PREFLIGHT
    int protocol_rc = OF_ERR_INVALID_STATE;
    int ping_rc = OF_ERR_INVALID_STATE;
    int preflight_safe_rc = OF_ERR_INVALID_STATE;
    int rc = preflight_target65(&ops, &cart, &flash, &protocol_rc, &ping_rc,
                                &preflight_safe_rc);
#else
    int rc = program65_identify(&ops, &cart, &flash);
#endif
    if (rc != OF_OK) {
        title65();
        printf("TARGET IDENTIFY FAIL RC %d\n", rc);
#if PROGRAM65_PROTOCOL_PREFLIGHT
        printf("PROTO %d PING %d SAFE %d\n", protocol_rc, ping_rc,
               preflight_safe_rc);
        printf("CART RC %d MASK %02lX\n", preflight_cart_rc65,
               (unsigned long)preflight_cart_mask65);
        printf("C ABI %lu/%lu BUS %08lX\n",
               (unsigned long)cart.struct_size,
               (unsigned long)cart.version,
               (unsigned long)cart.bus_mask);
        printf("C OPS %08lX WR %lu RD %lu\n",
               (unsigned long)cart.operation_mask,
               (unsigned long)cart.writes_enabled,
               (unsigned long)cart.max_read_bytes);
        printf("C SPACE %02lX/%02lX LIFE %08lX\n",
               (unsigned long)cart.gb_gbc_space_mask,
               (unsigned long)cart.gba_space_mask,
               (unsigned long)cart.lifecycle_status);
        printf("L TRY %lu STABLE %lu\n",
               (unsigned long)preflight_life_tries65,
               (unsigned long)preflight_life_stable65);
        printf("L FIRST %08lX LAST %08lX\n",
               (unsigned long)preflight_life_first65,
               (unsigned long)preflight_life_last65);
        printf("FLASH MASK %02lX\n",
               (unsigned long)preflight_flash_mask65);
#endif
        printf("NO CARTRIDGE ERASE OR WRITE\n");
        halt65();
    }
    long size = wait_source_slot65();
    if (size != (long)PROGRAM65_ROM_BYTES) {
        title65();
        printf("STOP SOURCE SIZE %ld EXPECT %u\n", size,
               PROGRAM65_ROM_BYTES);
        printf("NO CARTRIDGE ERASE OR WRITE\n");
        halt65();
    }
    title65();
    printf("TARGET IDENTIFIED; SCANNING ROM\n");
    printf("PREFLIGHT ONLY; NO ERASE OR WRITE\n");
    fflush(stdout);
    source65 = program65_scan_source(&ops);
    if (source65.rc != OF_OK) {
        title65();
        printf("SOURCE PREFLIGHT FAIL RC %d\n", source65.rc);
        printf("BYTES %lu/%u HEADER %u%u%u\n",
               (unsigned long)source65.bytes, PROGRAM65_ROM_BYTES,
               source65.logo_ok, source65.checksum_ok, source65.fixed96_ok);
        printf("NO CARTRIDGE ERASE OR WRITE\n");
        halt65();
    }
    show_ready65(&flash);
    while (1) {
        of_input_poll_p0();
        enum gate51 old = gate65.state;
        if (gate_tick52(&gate65, _of_input_p0.buttons, now65()))
            break;
        if (old != gate65.state)
            show_ready65(&flash);
        usleep(16000);
    }
    unsigned authorization = take51(&gate65) == A51;
    result65 = program65_run(&ops, &source65, authorization);
    result65.source_ms = source65.ms;
    show_result65(&result65);
    unsigned page = 0;
    unsigned previous = ~0u;
    while (1) {
        of_input_poll_p0();
        unsigned held = _of_input_p0.buttons;
        unsigned edge = held & ~previous;
        previous = held;
        if (edge & (OF_BTN_LEFT | OF_BTN_RIGHT | OF_BTN_A)) {
            if (edge & OF_BTN_A)
                page = 0;
            else
                page ^= 1u;
            if (page)
                show_timing65(&result65);
            else
                show_result65(&result65);
        }
        usleep(16000);
    }
#endif
}
