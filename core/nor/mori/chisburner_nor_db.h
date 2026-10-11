// SPDX-License-Identifier: AGPL-3.0-only
// Adapted from MoriBunrnner-for-ESP32-S3; see PROVENANCE.md.
#ifndef CHISBURNER_NOR_DB_H
#define CHISBURNER_NOR_DB_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    CHISBURNER_NOR_CMDSET_UNKNOWN = 0,
    CHISBURNER_NOR_CMDSET_AMD,
    CHISBURNER_NOR_CMDSET_INTEL,
} chisburner_nor_cmdset_t;

typedef enum {
    CHISBURNER_NOR_IDBUS_GBA = 0,
    CHISBURNER_NOR_IDBUS_MBC5,
} chisburner_nor_idbus_t;

typedef enum {
    CHISBURNER_NOR_FLAG_NONE = 0u,
    CHISBURNER_NOR_FLAG_DUAL_DIE = 1u << 0,
    CHISBURNER_NOR_FLAG_LIMIT_BUFFER_TO_ID = 1u << 1,
    CHISBURNER_NOR_FLAG_INTEL_88B0 = 1u << 2,
    CHISBURNER_NOR_FLAG_RESET_EVERY_1MB = 1u << 3,
    CHISBURNER_NOR_FLAG_DISABLE_BUFFER_PROGRAM = 1u << 4,
} chisburner_nor_flags_t;

typedef struct {
    const char *name;
    const char *gba_profile;
    chisburner_nor_cmdset_t cmdset;
    uint32_t flags;
    uint32_t device_size;
    uint32_t sector_size;
    uint16_t buffer_write_bytes_gba;
    uint16_t buffer_write_bytes_mbc5;
} chisburner_nor_family_t;

typedef struct {
    chisburner_nor_idbus_t bus;
    const chisburner_nor_family_t *family;
    const char *name;
    const char *profile;
    chisburner_nor_cmdset_t cmdset;
    uint32_t device_size;
    uint32_t sector_size;
    uint16_t buffer_write_bytes;
    uint32_t flags;
    uint8_t id_len;
    uint8_t id[8];
    uint8_t id_mask[8];
} chisburner_nor_entry_t;

const chisburner_nor_entry_t *chisburner_nor_db_lookup_gba(const uint8_t id[8]);
const chisburner_nor_entry_t *chisburner_nor_db_lookup_mbc5(const uint8_t id[4]);
const char *chisburner_nor_cmdset_name(chisburner_nor_cmdset_t cmdset);
chisburner_nor_cmdset_t chisburner_nor_cmdset_from_cfi_primary_id(uint16_t primary_id);
void chisburner_nor_format_chip_name(
    char *buf,
    size_t buf_len,
    const char *known_name,
    chisburner_nor_cmdset_t cmdset,
    uint32_t device_size);
const chisburner_nor_family_t *chisburner_gba_family_from_id(const uint8_t id[8]);
const chisburner_nor_family_t *chisburner_mbc5_family_from_id(const uint8_t id[4]);
const char *chisburner_nor_entry_name(const chisburner_nor_entry_t *entry);
chisburner_nor_cmdset_t chisburner_nor_entry_cmdset(const chisburner_nor_entry_t *entry);
uint32_t chisburner_nor_entry_flags(const chisburner_nor_entry_t *entry);
uint32_t chisburner_nor_entry_device_size(const chisburner_nor_entry_t *entry);
uint32_t chisburner_nor_entry_sector_size(const chisburner_nor_entry_t *entry);
uint16_t chisburner_nor_entry_buffer_write_bytes(const chisburner_nor_entry_t *entry);

const char *chisburner_gba_chip_name(const uint8_t id[8]);
const char *chisburner_gba_profile_name(const uint8_t id[8]);
bool chisburner_gba_geometry_from_id(
    const uint8_t id[8],
    uint32_t *device_size,
    uint32_t *sector_size,
    uint16_t *buffer_write_bytes);
chisburner_nor_cmdset_t chisburner_gba_cmdset_from_id(const uint8_t id[8]);
uint32_t chisburner_gba_nor_flags_from_id(const uint8_t id[8]);
bool chisburner_gba_nor_has_flag(const uint8_t id[8], uint32_t flag);

const char *chisburner_mbc5_chip_name(const uint8_t id[4]);
bool chisburner_mbc5_geometry_from_id(
    const uint8_t id[4],
    uint32_t *device_size,
    uint32_t *sector_size,
    uint16_t *buffer_write_bytes);
chisburner_nor_cmdset_t chisburner_mbc5_cmdset_from_id(const uint8_t id[4]);
uint32_t chisburner_mbc5_nor_flags_from_id(const uint8_t id[4]);
bool chisburner_mbc5_nor_has_flag(const uint8_t id[4], uint32_t flag);

#endif
