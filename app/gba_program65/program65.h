#ifndef CHISBURNER_GBA_PROGRAM65_H
#define CHISBURNER_GBA_PROGRAM65_H

#include <stdint.h>
#include <string.h>

#include "of_cart.h"
#include "of_error.h"
#include "../gba_dump53/md5.h"
#include "../gba_db55/sha1_55.h"

#ifndef PROGRAM65_IMAGE_BYTES
#define PROGRAM65_IMAGE_BYTES     0x02000000u
#endif

#define PROGRAM65_FLASH_BYTES     0x02000000u
#define PROGRAM65_ROM_BYTES       PROGRAM65_IMAGE_BYTES
#define PROGRAM65_SOURCE_CHUNK    0x00010000u
#define PROGRAM65_SECTOR_BYTES    0x00020000u
#define PROGRAM65_READ_BYTES      32u
#ifndef PROGRAM65_BUFFER_BYTES
#define PROGRAM65_BUFFER_BYTES    32u
#endif
#define PROGRAM65_SOURCE_BLOCKS   \
    ((PROGRAM65_ROM_BYTES + PROGRAM65_SOURCE_CHUNK - 1u) / \
     PROGRAM65_SOURCE_CHUNK)
#define PROGRAM65_SECTORS         \
    (PROGRAM65_FLASH_BYTES / PROGRAM65_SECTOR_BYTES)
#define PROGRAM65_PROGRAM_BLOCKS  \
    ((PROGRAM65_ROM_BYTES + PROGRAM65_BUFFER_BYTES - 1u) / \
     PROGRAM65_BUFFER_BYTES)

_Static_assert(PROGRAM65_ROM_BYTES >= 0xC0u,
               "GBA image must contain a complete header");
_Static_assert((PROGRAM65_ROM_BYTES & 1u) == 0u,
               "GBA image length must be even for the 16-bit flash bus");
_Static_assert((PROGRAM65_ROM_BYTES % PROGRAM65_SOURCE_CHUNK) == 0u ||
               (PROGRAM65_ROM_BYTES % PROGRAM65_SOURCE_CHUNK) >=
                   PROGRAM65_READ_BYTES,
               "Final source chunk must contain the 32-byte end sentinel");
_Static_assert(PROGRAM65_BUFFER_BYTES >= 2u &&
               PROGRAM65_BUFFER_BYTES <= 512u &&
               (PROGRAM65_BUFFER_BYTES & 1u) == 0u &&
               (PROGRAM65_BUFFER_BYTES & (PROGRAM65_BUFFER_BYTES - 1u)) == 0u,
               "Flash program buffer must be an even power of two, 2..512");
_Static_assert(PROGRAM65_ROM_BYTES <= PROGRAM65_FLASH_BYTES,
               "GBA image must fit the target S29GL256S");
_Static_assert((PROGRAM65_FLASH_BYTES % PROGRAM65_SECTOR_BYTES) == 0u,
               "Target flash geometry must contain whole sectors");

#define PROGRAM65_ID_PREFIX       0x227e0001u
#define PROGRAM65_CFI_PREFIX      0x00520051u

#define PROGRAM65_ERASE_TIMEOUT_US   20000000u
#define PROGRAM65_PROGRAM_TIMEOUT_US   250000u
#define PROGRAM65_READ_TIMEOUT_US       50000u

enum program65_stage {
    P65_SOURCE_SCAN,
    P65_IDENTIFY,
    P65_ERASE,
    P65_PROGRAM,
    P65_VERIFY,
    P65_COMPLETE
};

struct program65_ops {
    int (*source_read)(uint32_t offset, void *dst, uint32_t length);
    int (*cart_caps)(of_cart_caps_t *caps);
    int (*flash_caps)(of_cart_flash_caps_t *caps);
    int (*safe_idle)(void);
    int (*erase_sector)(uint32_t address, uint32_t timeout_us);
    int (*program)(uint32_t address, const void *src, uint32_t length,
                   uint32_t timeout_us);
    int (*cart_read)(uint32_t address, void *dst, uint32_t length,
                     uint32_t timeout_us);
    int (*disarm)(void);
    uint32_t (*now)(void);
    void (*progress)(enum program65_stage stage, uint32_t completed,
                     uint32_t total);
};

struct program65_source {
    int rc;
    uint32_t bytes;
    uint32_t blocks;
    uint32_t ms;
    uint8_t logo_ok;
    uint8_t checksum_ok;
    uint8_t fixed96_ok;
    uint8_t reserved;
    char title[13];
    char game_code[5];
    uint8_t md5[16];
    uint8_t terminal_bytes[PROGRAM65_READ_BYTES];
};

struct program65_result {
    int rc;
    int disarm_rc;
    int safe_rc;
    enum program65_stage stage;
    uint32_t stop_offset;
    uint32_t erased_sectors;
    uint32_t programmed_blocks;
    uint32_t verified_bytes;
    uint32_t last_program_address;
    uint32_t last_program_length;
    uint32_t last_read_address;
    uint32_t last_read_length;
    uint32_t source_ms;
    uint32_t identify_ms;
    uint32_t erase_ms;
    uint32_t program_ms;
    uint32_t verify_ms;
    uint32_t total_ms;
    uint8_t source_md5[16];
    uint8_t stream_md5[16];
    uint8_t cart_md5[16];
    uint8_t terminal_match;
};

static uint8_t program65_chunk[PROGRAM65_SOURCE_CHUNK]
    __attribute__((aligned(64)));

static int program65_printable(const uint8_t *p, uint32_t n) {
    for (uint32_t i = 0; i < n; ++i)
        if (p[i] != 0 && p[i] != 0x20u && (p[i] < 0x21u || p[i] > 0x7eu))
            return 0;
    return 1;
}

static uint8_t program65_header_checksum(const uint8_t *header) {
    uint8_t sum = 0;
    for (uint32_t i = 0xa0u; i <= 0xbcu; ++i)
        sum = (uint8_t)(sum + header[i]);
    return (uint8_t)(0u - sum - 0x19u);
}

static int program65_header_valid(struct program65_source *source,
                                  const uint8_t *header) {
    static const uint8_t logo_sha1[20] = {
        0x17,0xda,0xa0,0xfe,0xc0,0x2f,0xc3,0x3c,0x0f,0x6a,
        0xbb,0x54,0x9a,0x8b,0x80,0xb6,0x61,0x3b,0x48,0xee
    };
    uint8_t actual_logo_sha1[20];
    sha1_55(header + 4, 0x9cu, actual_logo_sha1);
    source->logo_ok = (uint8_t)(memcmp(actual_logo_sha1, logo_sha1, 20) == 0);
    source->checksum_ok =
        (uint8_t)(header[0xbdu] == program65_header_checksum(header));
    source->fixed96_ok = (uint8_t)(header[0xb2u] == 0x96u);
    memcpy(source->title, header + 0xa0u, 12);
    source->title[12] = 0;
    memcpy(source->game_code, header + 0xacu, 4);
    source->game_code[4] = 0;
    return source->logo_ok && source->checksum_ok && source->fixed96_ok &&
           program65_printable(header + 0xa0u, 12) &&
           program65_printable(header + 0xacu, 4);
}

static int program65_cart_caps_valid(const of_cart_caps_t *caps) {
    const uint32_t required_ops = OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
        OF_CART_OP_ABORT | OF_CART_OP_QUERY | OF_CART_OP_ERASE_SECTOR |
        OF_CART_OP_PROGRAM;
    return caps->struct_size == sizeof(*caps) &&
           caps->version == OF_CART_CAPS_VERSION &&
           caps->bus_mask == OF_CART_BUS_MASK_GBA &&
           caps->operation_mask == required_ops &&
           caps->max_read_bytes == PROGRAM65_READ_BYTES &&
           caps->writes_enabled == 1u &&
           caps->gb_gbc_space_mask == 0u &&
           caps->gba_space_mask ==
               (OF_CART_SPACE_MASK_ROM | OF_CART_SPACE_MASK_IDENTIFY) &&
           (caps->lifecycle_status & 0x80000307u) == 0x80000303u;
}

static int program65_flash_caps_valid(const of_cart_flash_caps_t *caps) {
    return caps->struct_size == sizeof(*caps) &&
           caps->version == OF_CART_FLASH_CAPS_VERSION &&
           caps->command_set == OF_CART_FLASH_CMDSET_AMD &&
           caps->flash_size_bytes == PROGRAM65_FLASH_BYTES &&
           caps->sector_size_bytes == PROGRAM65_SECTOR_BYTES &&
           caps->program_buffer_bytes == PROGRAM65_BUFFER_BYTES &&
           caps->profile_status == OF_CART_FLASH_PROFILE_OK &&
           caps->id_prefix == PROGRAM65_ID_PREFIX &&
           caps->cfi_signature == PROGRAM65_CFI_PREFIX;
}

static struct program65_source
program65_scan_source(const struct program65_ops *ops) {
    struct program65_source source;
    memset(&source, 0, sizeof(source));
    source.rc = OF_ERR_INVALID_PARAM;
    if (!ops || !ops->source_read || !ops->now)
        return source;

    struct md5_53 md5;
    md5_init53(&md5);
    uint32_t begin = ops->now();
    for (uint32_t offset = 0; offset < PROGRAM65_ROM_BYTES;
         offset += PROGRAM65_SOURCE_CHUNK) {
        uint32_t length = PROGRAM65_ROM_BYTES - offset;
        if (length > PROGRAM65_SOURCE_CHUNK)
            length = PROGRAM65_SOURCE_CHUNK;
        if (ops->progress && (source.blocks & 7u) == 0)
            ops->progress(P65_SOURCE_SCAN, source.blocks,
                          PROGRAM65_SOURCE_BLOCKS);
        source.rc = ops->source_read(offset, program65_chunk,
                                    length);
        if (source.rc != OF_OK)
            break;
        if (offset == 0 && !program65_header_valid(&source, program65_chunk)) {
            source.rc = OF_ERR_DENIED;
            break;
        }
        md5_add53(&md5, program65_chunk, length);
        const uint32_t terminal_offset =
            PROGRAM65_ROM_BYTES - PROGRAM65_READ_BYTES;
        if (offset <= terminal_offset &&
            terminal_offset + PROGRAM65_READ_BYTES <= offset + length)
            memcpy(source.terminal_bytes,
                   program65_chunk + terminal_offset - offset,
                   PROGRAM65_READ_BYTES);
        source.bytes += length;
        source.blocks++;
    }
    if (source.rc == OF_OK && source.bytes != PROGRAM65_ROM_BYTES)
        source.rc = OF_ERR_BAD_RANGE;
    md5_end53(&md5, source.md5);
    source.ms = ops->now() - begin;
    if (ops->progress)
        ops->progress(P65_SOURCE_SCAN, source.blocks,
                      PROGRAM65_SOURCE_BLOCKS);
    return source;
}

static int program65_identify(const struct program65_ops *ops,
                              of_cart_caps_t *cart,
                              of_cart_flash_caps_t *flash) {
    memset(cart, 0, sizeof(*cart));
    memset(flash, 0, sizeof(*flash));
    int rc = ops->cart_caps(cart);
    if (rc != OF_OK)
        return rc;
    if (!program65_cart_caps_valid(cart))
        return OF_ERR_DENIED;
    rc = ops->flash_caps(flash);
    if (rc != OF_OK)
        return rc;
    return program65_flash_caps_valid(flash) ? OF_OK : OF_ERR_DENIED;
}

static struct program65_result
program65_run(const struct program65_ops *ops,
              const struct program65_source *source,
              unsigned authorization) {
    struct program65_result result;
    memset(&result, 0, sizeof(result));
    result.rc = OF_ERR_INVALID_PARAM;
    result.disarm_rc = OF_ERR_NOT_SUPPORTED;
    result.safe_rc = OF_ERR_NOT_SUPPORTED;
    result.stage = P65_IDENTIFY;
    result.stop_offset = PROGRAM65_ROM_BYTES;

    if (!ops || !source || !ops->source_read || !ops->cart_caps ||
        !ops->flash_caps || !ops->safe_idle || !ops->erase_sector ||
        !ops->program || !ops->cart_read || !ops->disarm || !ops->now)
        return result;
    if (!authorization || source->rc != OF_OK ||
        source->bytes != PROGRAM65_ROM_BYTES ||
        source->blocks != PROGRAM65_SOURCE_BLOCKS || !source->logo_ok ||
        !source->checksum_ok || !source->fixed96_ok) {
        result.rc = OF_ERR_DENIED;
        return result;
    }

    memcpy(result.source_md5, source->md5, 16);
    uint32_t begin = ops->now();
    uint32_t phase_begin = begin;
    of_cart_caps_t cart;
    of_cart_flash_caps_t flash;
    if (ops->progress)
        ops->progress(P65_IDENTIFY, 0, 1);
    result.rc = program65_identify(ops, &cart, &flash);
    result.identify_ms = ops->now() - phase_begin;
    if (result.rc != OF_OK)
        goto stop;
    if (ops->progress)
        ops->progress(P65_IDENTIFY, 1, 1);

    result.stage = P65_ERASE;
    phase_begin = ops->now();
    for (uint32_t sector = 0; sector < PROGRAM65_SECTORS; ++sector) {
        result.stop_offset = sector * PROGRAM65_SECTOR_BYTES;
        if (ops->progress)
            ops->progress(P65_ERASE, sector, PROGRAM65_SECTORS);
        result.rc = ops->erase_sector(result.stop_offset,
                                      PROGRAM65_ERASE_TIMEOUT_US);
        if (result.rc != OF_OK)
            goto stop;
        result.erased_sectors++;
    }
    result.erase_ms = ops->now() - phase_begin;
    if (ops->progress)
        ops->progress(P65_ERASE, PROGRAM65_SECTORS, PROGRAM65_SECTORS);

    result.stage = P65_PROGRAM;
    phase_begin = ops->now();
    struct md5_53 stream_md5;
    md5_init53(&stream_md5);
    for (uint32_t offset = 0; offset < PROGRAM65_ROM_BYTES;
         offset += PROGRAM65_SOURCE_CHUNK) {
        uint32_t length = PROGRAM65_ROM_BYTES - offset;
        if (length > PROGRAM65_SOURCE_CHUNK)
            length = PROGRAM65_SOURCE_CHUNK;
        if (ops->progress)
            ops->progress(P65_PROGRAM, offset / PROGRAM65_SOURCE_CHUNK,
                          PROGRAM65_SOURCE_BLOCKS);
        result.stop_offset = offset;
        result.rc = ops->source_read(offset, program65_chunk,
                                     length);
        if (result.rc != OF_OK)
            goto stop;
        md5_add53(&stream_md5, program65_chunk, length);
        for (uint32_t within = 0; within < length;
             within += PROGRAM65_BUFFER_BYTES) {
            uint32_t program_length = length - within;
            if (program_length > PROGRAM65_BUFFER_BYTES)
                program_length = PROGRAM65_BUFFER_BYTES;
            result.stop_offset = offset + within;
            result.last_program_address = result.stop_offset;
            result.last_program_length = program_length;
            result.rc = ops->program(result.stop_offset,
                                     program65_chunk + within,
                                     program_length,
                                     PROGRAM65_PROGRAM_TIMEOUT_US);
            if (result.rc != OF_OK)
                goto stop;
            result.programmed_blocks++;
        }
    }
    md5_end53(&stream_md5, result.stream_md5);
    result.program_ms = ops->now() - phase_begin;
    if (memcmp(result.stream_md5, result.source_md5, 16) != 0) {
        result.rc = OF_ERR_FAILED;
        goto stop;
    }
    if (ops->progress)
        ops->progress(P65_PROGRAM, PROGRAM65_SOURCE_BLOCKS,
                      PROGRAM65_SOURCE_BLOCKS);

    result.rc = ops->safe_idle();
    if (result.rc != OF_OK)
        goto stop;

    result.stage = P65_VERIFY;
    phase_begin = ops->now();
    struct md5_53 cart_md5;
    md5_init53(&cart_md5);
    for (uint32_t offset = 0; offset < PROGRAM65_ROM_BYTES;
         offset += PROGRAM65_READ_BYTES) {
        uint32_t read_length = PROGRAM65_ROM_BYTES - offset;
        if (read_length > PROGRAM65_READ_BYTES)
            read_length = PROGRAM65_READ_BYTES;
        if (ops->progress && (offset & 0xffffu) == 0)
            ops->progress(P65_VERIFY, offset / PROGRAM65_SOURCE_CHUNK,
                          PROGRAM65_SOURCE_BLOCKS);
        result.stop_offset = offset;
        result.last_read_address = offset;
        result.last_read_length = read_length;
        result.rc = ops->cart_read(offset, program65_chunk,
                                   read_length,
                                   PROGRAM65_READ_TIMEOUT_US);
        if (result.rc != OF_OK)
            goto stop;
        md5_add53(&cart_md5, program65_chunk, read_length);
        if (offset + read_length == PROGRAM65_ROM_BYTES)
            result.terminal_match = (uint8_t)(
                memcmp(program65_chunk, source->terminal_bytes,
                       PROGRAM65_READ_BYTES) == 0);
        result.verified_bytes += read_length;
    }
    md5_end53(&cart_md5, result.cart_md5);
    result.verify_ms = ops->now() - phase_begin;
    if (memcmp(result.cart_md5, result.source_md5, 16) != 0) {
        result.rc = OF_ERR_FAILED;
        goto stop;
    }
    if (!result.terminal_match) {
        result.rc = OF_ERR_FAILED;
        goto stop;
    }
    if (ops->progress)
        ops->progress(P65_VERIFY, PROGRAM65_SOURCE_BLOCKS,
                      PROGRAM65_SOURCE_BLOCKS);

    result.stage = P65_COMPLETE;
    result.stop_offset = PROGRAM65_ROM_BYTES;
    result.rc = OF_OK;

stop:
    result.disarm_rc = ops->disarm();
    result.safe_rc = ops->safe_idle();
    if (result.rc == OF_OK && result.disarm_rc != OF_OK)
        result.rc = result.disarm_rc;
    if (result.rc == OF_OK && result.safe_rc != OF_OK)
        result.rc = result.safe_rc;
    result.total_ms = ops->now() - begin;
    return result;
}

#endif
