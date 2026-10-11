#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "program65.h"

#ifndef PROGRAM65_EXPECTED_MD5
#define PROGRAM65_EXPECTED_MD5 "872A1A05E39006F3BEA69EFAA641B3A4"
#endif

static uint8_t *source_image;
static uint8_t *flash_image;
static uint32_t clock_ms;
static uint32_t erase_calls;
static uint32_t program_calls;
static uint32_t last_program_length;
static uint32_t disarm_calls;
static uint32_t safe_calls;
static int fail_program_at = -1;
static int corrupt_verify;
static int corrupt_terminal;
static int bad_profile;
static int profile_loaded;

static int source_read(uint32_t offset, void *dst, uint32_t length) {
    if (!dst || offset > PROGRAM65_ROM_BYTES ||
        length > PROGRAM65_ROM_BYTES - offset)
        return OF_ERR_BAD_RANGE;
    memcpy(dst, source_image + offset, length);
    clock_ms++;
    return OF_OK;
}

static int cart_caps(of_cart_caps_t *caps) {
    memset(caps, 0, sizeof(*caps));
    caps->struct_size = sizeof(*caps);
    caps->version = OF_CART_CAPS_VERSION;
    caps->bus_mask = OF_CART_BUS_MASK_GBA;
    caps->operation_mask = OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
        OF_CART_OP_ABORT | OF_CART_OP_QUERY | OF_CART_OP_ERASE_SECTOR |
        OF_CART_OP_PROGRAM;
    caps->max_read_bytes = 32;
    caps->writes_enabled = 1;
    caps->lifecycle_status = 0x80000303u;
    caps->gba_space_mask = OF_CART_SPACE_MASK_ROM |
                           OF_CART_SPACE_MASK_IDENTIFY;
    return OF_OK;
}

static int flash_caps(of_cart_flash_caps_t *caps) {
    memset(caps, 0, sizeof(*caps));
    caps->struct_size = sizeof(*caps);
    caps->version = OF_CART_FLASH_CAPS_VERSION;
    caps->command_set = OF_CART_FLASH_CMDSET_AMD;
    caps->flash_size_bytes = PROGRAM65_FLASH_BYTES;
    caps->sector_size_bytes = PROGRAM65_SECTOR_BYTES;
    caps->program_buffer_bytes = PROGRAM65_BUFFER_BYTES;
    caps->profile_status = OF_CART_FLASH_PROFILE_OK;
    caps->id_prefix = bad_profile ? 0u : PROGRAM65_ID_PREFIX;
    caps->cfi_signature = PROGRAM65_CFI_PREFIX;
    profile_loaded = 1;
    clock_ms++;
    return OF_OK;
}

static int safe_idle(void) {
    safe_calls++;
    profile_loaded = 0;
    return OF_OK;
}

static int erase_sector(uint32_t address, uint32_t timeout_us) {
    if (!profile_loaded)
        return OF_ERR_INVALID_STATE;
    assert(timeout_us == PROGRAM65_ERASE_TIMEOUT_US);
    assert((address & (PROGRAM65_SECTOR_BYTES - 1u)) == 0);
    memset(flash_image + address, 0xff, PROGRAM65_SECTOR_BYTES);
    erase_calls++;
    clock_ms += 2;
    return OF_OK;
}

static int program_block(uint32_t address, const void *src, uint32_t length,
                         uint32_t timeout_us) {
    if (!profile_loaded)
        return OF_ERR_INVALID_STATE;
    assert(timeout_us == PROGRAM65_PROGRAM_TIMEOUT_US);
    assert(length >= 2u && length <= PROGRAM65_BUFFER_BYTES &&
           (length & 1u) == 0u &&
           (address & (PROGRAM65_BUFFER_BYTES - 1u)) + length <=
               PROGRAM65_BUFFER_BYTES);
    if (fail_program_at >= 0 && program_calls == (uint32_t)fail_program_at)
        return OF_ERR_IO;
    const uint8_t *bytes = (const uint8_t *)src;
    for (uint32_t i = 0; i < length; ++i)
        flash_image[address + i] &= bytes[i];
    last_program_length = length;
    program_calls++;
    return OF_OK;
}

static int cart_read(uint32_t address, void *dst, uint32_t length,
                     uint32_t timeout_us) {
    assert(timeout_us == PROGRAM65_READ_TIMEOUT_US);
    assert(length <= PROGRAM65_READ_BYTES &&
           (address & (PROGRAM65_READ_BYTES - 1u)) == 0);
    memcpy(dst, flash_image + address, length);
    if (corrupt_verify && address == 0x1000u)
        ((uint8_t *)dst)[0] ^= 1u;
    if (corrupt_terminal && address + length == PROGRAM65_ROM_BYTES)
        ((uint8_t *)dst)[length - 1u] ^= 1u;
    return OF_OK;
}

static int disarm(void) {
    disarm_calls++;
    profile_loaded = 0;
    return OF_OK;
}

static uint32_t now_ms(void) {
    return clock_ms;
}

static void progress(enum program65_stage stage, uint32_t completed,
                     uint32_t total) {
    assert(stage <= P65_COMPLETE);
    assert(completed <= total);
}

static const struct program65_ops ops = {
    source_read, cart_caps, flash_caps, safe_idle, erase_sector,
    program_block, cart_read, disarm, now_ms, progress
};

static void reset_target(void) {
    memset(flash_image, 0, PROGRAM65_FLASH_BYTES);
    clock_ms = erase_calls = program_calls = disarm_calls = safe_calls = 0;
    last_program_length = 0;
    fail_program_at = -1;
    corrupt_verify = 0;
    corrupt_terminal = 0;
    bad_profile = 0;
    profile_loaded = 0;
}

static void load_rom(const char *path) {
    FILE *file = fopen(path, "rb");
    assert(file);
    assert(fseek(file, 0, SEEK_END) == 0);
    assert(ftell(file) == (long)PROGRAM65_ROM_BYTES);
    assert(fseek(file, 0, SEEK_SET) == 0);
    assert(fread(source_image, 1, PROGRAM65_ROM_BYTES, file) ==
           PROGRAM65_ROM_BYTES);
    assert(fclose(file) == 0);
}

int main(int argc, char **argv) {
    assert(argc == 2);
    source_image = malloc(PROGRAM65_ROM_BYTES);
    flash_image = malloc(PROGRAM65_FLASH_BYTES);
    assert(source_image && flash_image);
    load_rom(argv[1]);

    reset_target();
    struct program65_source source = program65_scan_source(&ops);
    assert(source.rc == OF_OK && source.logo_ok && source.checksum_ok &&
           source.fixed96_ok && source.bytes == PROGRAM65_ROM_BYTES);
    char md5_text[33];
    md5_hex53(source.md5, md5_text);
    assert(strcmp(md5_text, PROGRAM65_EXPECTED_MD5) == 0);

    struct program65_result result = program65_run(&ops, &source, 0);
    assert(result.rc == OF_ERR_DENIED && erase_calls == 0 &&
           program_calls == 0 && disarm_calls == 0);

    bad_profile = 1;
    result = program65_run(&ops, &source, 1);
    assert(result.rc == OF_ERR_DENIED && result.stage == P65_IDENTIFY &&
           erase_calls == 0 && program_calls == 0 && disarm_calls == 1);

    reset_target();
    fail_program_at = 1234;
    result = program65_run(&ops, &source, 1);
    if (!(result.rc == OF_ERR_IO && result.stage == P65_PROGRAM &&
          result.stop_offset == 1234u * PROGRAM65_BUFFER_BYTES &&
          erase_calls == PROGRAM65_SECTORS && program_calls == 1234u &&
          disarm_calls == 1 && safe_calls == 1)) {
        fprintf(stderr,
                "program fault mismatch rc=%d stage=%d stop=%u erase=%u "
                "program=%u disarm=%u safe=%u\n",
                result.rc, result.stage, result.stop_offset, erase_calls,
                program_calls, disarm_calls, safe_calls);
    }
    assert(result.rc == OF_ERR_IO && result.stage == P65_PROGRAM &&
           result.stop_offset == 1234u * PROGRAM65_BUFFER_BYTES &&
           erase_calls == PROGRAM65_SECTORS && program_calls == 1234u &&
           disarm_calls == 1 && safe_calls == 1);

    reset_target();
    corrupt_verify = 1;
    result = program65_run(&ops, &source, 1);
    assert(result.rc == OF_ERR_FAILED && result.stage == P65_VERIFY &&
           erase_calls == PROGRAM65_SECTORS &&
           program_calls == PROGRAM65_PROGRAM_BLOCKS &&
           disarm_calls == 1);

    reset_target();
    corrupt_terminal = 1;
    result = program65_run(&ops, &source, 1);
    assert(result.rc == OF_ERR_FAILED && result.stage == P65_VERIFY &&
           result.last_read_address ==
               PROGRAM65_ROM_BYTES - PROGRAM65_READ_BYTES &&
           result.last_read_length == PROGRAM65_READ_BYTES &&
           !result.terminal_match && disarm_calls == 1);

    reset_target();
    result = program65_run(&ops, &source, 1);
    assert(result.rc == OF_OK && result.stage == P65_COMPLETE &&
           result.stop_offset == PROGRAM65_ROM_BYTES &&
           result.erased_sectors == PROGRAM65_SECTORS &&
           result.programmed_blocks == PROGRAM65_PROGRAM_BLOCKS &&
           last_program_length ==
               ((PROGRAM65_ROM_BYTES - 1u) % PROGRAM65_BUFFER_BYTES + 1u) &&
           result.last_program_address ==
               ((PROGRAM65_ROM_BYTES - 1u) / PROGRAM65_BUFFER_BYTES) *
                   PROGRAM65_BUFFER_BYTES &&
           result.last_program_length == last_program_length &&
           result.verified_bytes == PROGRAM65_ROM_BYTES &&
           result.last_read_address ==
               PROGRAM65_ROM_BYTES - PROGRAM65_READ_BYTES &&
           result.last_read_length == PROGRAM65_READ_BYTES &&
           result.terminal_match &&
           memcmp(result.source_md5, result.stream_md5, 16) == 0 &&
           memcmp(result.source_md5, result.cart_md5, 16) == 0 &&
           memcmp(source_image, flash_image, PROGRAM65_ROM_BYTES) == 0 &&
           disarm_calls == 1 && safe_calls == 2);

    free(flash_image);
    free(source_image);
    fprintf(stderr, "program65 app: preflight + authorization + profile gate + fail-stop + final MD5 PASS\n");
    return 0;
}
