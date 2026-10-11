// SPDX-License-Identifier: AGPL-3.0-only
// Adapted from MoriBunrnner-for-ESP32-S3; see PROVENANCE.md.
#include "chisburner_nor_db.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "chisburner_nor_db_data.h"

/* Lookup and metadata helpers layered on top of the standalone NOR database. */

static bool chisburner_nor_id_match(const uint8_t *id, const chisburner_nor_entry_t *entry)
{
    size_t i;

    if (id == NULL || entry == NULL) {
        return false;
    }
    for (i = 0u; i < entry->id_len; ++i) {
        if ((id[i] & entry->id_mask[i]) != (entry->id[i] & entry->id_mask[i])) {
            return false;
        }
    }
    return true;
}

static const chisburner_nor_entry_t *chisburner_nor_db_lookup(
    chisburner_nor_idbus_t bus,
    const uint8_t *id,
    size_t id_len)
{
    const chisburner_nor_entry_t *table = NULL;
    size_t table_count = 0u;
    size_t i;

    if (id == NULL) {
        return NULL;
    }

    switch (bus) {
        case CHISBURNER_NOR_IDBUS_GBA:
            table = g_chisburner_nor_db_gba;
            table_count = g_chisburner_nor_db_gba_count;
            break;
        case CHISBURNER_NOR_IDBUS_MBC5:
            table = g_chisburner_nor_db_mbc5;
            table_count = g_chisburner_nor_db_mbc5_count;
            break;
        default:
            return NULL;
    }

    for (i = 0u; i < table_count; ++i) {
        if (table[i].id_len != id_len) {
            continue;
        }
        if (chisburner_nor_id_match(id, &table[i])) {
            return &table[i];
        }
    }
    return NULL;
}

static uint16_t chisburner_nor_family_buffer_write_bytes(const chisburner_nor_entry_t *entry)
{
    if (entry == NULL || entry->family == NULL) {
        return 0u;
    }

    switch (entry->bus) {
        case CHISBURNER_NOR_IDBUS_GBA:
            return entry->family->buffer_write_bytes_gba;
        case CHISBURNER_NOR_IDBUS_MBC5:
            return entry->family->buffer_write_bytes_mbc5;
        default:
            return 0u;
    }
}

const chisburner_nor_entry_t *chisburner_nor_db_lookup_gba(const uint8_t id[8])
{
    return chisburner_nor_db_lookup(CHISBURNER_NOR_IDBUS_GBA, id, 8u);
}

const chisburner_nor_entry_t *chisburner_nor_db_lookup_mbc5(const uint8_t id[4])
{
    return chisburner_nor_db_lookup(CHISBURNER_NOR_IDBUS_MBC5, id, 4u);
}

const chisburner_nor_family_t *chisburner_gba_family_from_id(const uint8_t id[8])
{
    const chisburner_nor_entry_t *entry = chisburner_nor_db_lookup_gba(id);
    return (entry != NULL) ? entry->family : NULL;
}

const chisburner_nor_family_t *chisburner_mbc5_family_from_id(const uint8_t id[4])
{
    const chisburner_nor_entry_t *entry = chisburner_nor_db_lookup_mbc5(id);
    return (entry != NULL) ? entry->family : NULL;
}

const char *chisburner_nor_entry_name(const chisburner_nor_entry_t *entry)
{
    if (entry == NULL) {
        return "unknown";
    }
    if (entry->name != NULL) {
        return entry->name;
    }
    if (entry->family != NULL && entry->family->name != NULL) {
        return entry->family->name;
    }
    return "unknown";
}

chisburner_nor_cmdset_t chisburner_nor_entry_cmdset(const chisburner_nor_entry_t *entry)
{
    if (entry == NULL) {
        return CHISBURNER_NOR_CMDSET_UNKNOWN;
    }
    if (entry->cmdset != CHISBURNER_NOR_CMDSET_UNKNOWN) {
        return entry->cmdset;
    }
    if (entry->family != NULL) {
        return entry->family->cmdset;
    }
    return CHISBURNER_NOR_CMDSET_UNKNOWN;
}

uint32_t chisburner_nor_entry_flags(const chisburner_nor_entry_t *entry)
{
    uint32_t flags = CHISBURNER_NOR_FLAG_NONE;

    if (entry == NULL) {
        return CHISBURNER_NOR_FLAG_NONE;
    }
    if (entry->family != NULL) {
        flags |= entry->family->flags;
    }
    flags |= entry->flags;
    return flags;
}

uint32_t chisburner_nor_entry_device_size(const chisburner_nor_entry_t *entry)
{
    if (entry == NULL) {
        return 0u;
    }
    if (entry->device_size != 0u) {
        return entry->device_size;
    }
    if (entry->family != NULL) {
        return entry->family->device_size;
    }
    return 0u;
}

uint32_t chisburner_nor_entry_sector_size(const chisburner_nor_entry_t *entry)
{
    if (entry == NULL) {
        return 0u;
    }
    if (entry->sector_size != 0u) {
        return entry->sector_size;
    }
    if (entry->family != NULL) {
        return entry->family->sector_size;
    }
    return 0u;
}

uint16_t chisburner_nor_entry_buffer_write_bytes(const chisburner_nor_entry_t *entry)
{
    if (entry == NULL) {
        return 0u;
    }
    if (entry->buffer_write_bytes != 0u) {
        return entry->buffer_write_bytes;
    }
    return chisburner_nor_family_buffer_write_bytes(entry);
}

const char *chisburner_nor_cmdset_name(chisburner_nor_cmdset_t cmdset)
{
    switch (cmdset) {
        case CHISBURNER_NOR_CMDSET_AMD:
            return "amd";
        case CHISBURNER_NOR_CMDSET_INTEL:
            return "intel";
        case CHISBURNER_NOR_CMDSET_UNKNOWN:
        default:
            return "unknown";
    }
}

chisburner_nor_cmdset_t chisburner_nor_cmdset_from_cfi_primary_id(uint16_t primary_id)
{
    switch (primary_id) {
        case 0x0001u: /* Intel/Sharp extended */
        case 0x0003u: /* Intel standard */
            return CHISBURNER_NOR_CMDSET_INTEL;
        case 0x0002u: /* AMD/Fujitsu standard */
        case 0x0004u: /* AMD/Fujitsu extended */
            return CHISBURNER_NOR_CMDSET_AMD;
        default:
            return CHISBURNER_NOR_CMDSET_UNKNOWN;
    }
}

void chisburner_nor_format_chip_name(
    char *buf,
    size_t buf_len,
    const char *known_name,
    chisburner_nor_cmdset_t cmdset,
    uint32_t device_size)
{
    const char *cmdset_label = "NOR";

    if (buf == NULL || buf_len == 0u) {
        return;
    }

    if (known_name != NULL && known_name[0] != '\0' && strcmp(known_name, "unknown") != 0) {
        (void)snprintf(buf, buf_len, "%s", known_name);
        return;
    }

    switch (cmdset) {
        case CHISBURNER_NOR_CMDSET_AMD:
            cmdset_label = "AMD";
            break;
        case CHISBURNER_NOR_CMDSET_INTEL:
            cmdset_label = "INTEL";
            break;
        case CHISBURNER_NOR_CMDSET_UNKNOWN:
        default:
            cmdset_label = "NOR";
            break;
    }

    if (device_size >= (1024u * 1024u) && (device_size % (1024u * 1024u)) == 0u) {
        (void)snprintf(
            buf,
            buf_len,
            "CFI %s %" PRIu32 "MB",
            cmdset_label,
            (uint32_t)(device_size / (1024u * 1024u)));
    } else if (device_size >= 1024u && (device_size % 1024u) == 0u) {
        (void)snprintf(
            buf,
            buf_len,
            "CFI %s %" PRIu32 "KB",
            cmdset_label,
            (uint32_t)(device_size / 1024u));
    } else if (device_size > 0u) {
        (void)snprintf(buf, buf_len, "CFI %s %" PRIu32 "B", cmdset_label, device_size);
    } else {
        (void)snprintf(buf, buf_len, "CFI %s", cmdset_label);
    }
}

const char *chisburner_gba_chip_name(const uint8_t id[8])
{
    const chisburner_nor_entry_t *entry = chisburner_nor_db_lookup_gba(id);
    return chisburner_nor_entry_name(entry);
}

const char *chisburner_gba_profile_name(const uint8_t id[8])
{
    const chisburner_nor_entry_t *entry = chisburner_nor_db_lookup_gba(id);

    if (entry != NULL && entry->profile != NULL) {
        return entry->profile;
    }
    if (entry != NULL && entry->family != NULL && entry->family->gba_profile != NULL) {
        return entry->family->gba_profile;
    }
    if (entry != NULL) {
        switch (chisburner_nor_entry_cmdset(entry)) {
            case CHISBURNER_NOR_CMDSET_AMD:
                return "AGB:AMD";
            case CHISBURNER_NOR_CMDSET_INTEL:
                return "AGB:INTEL";
            case CHISBURNER_NOR_CMDSET_UNKNOWN:
            default:
                break;
        }
    }
    return "unknown";
}

bool chisburner_gba_geometry_from_id(
    const uint8_t id[8],
    uint32_t *device_size,
    uint32_t *sector_size,
    uint16_t *buffer_write_bytes)
{
    const chisburner_nor_entry_t *entry = chisburner_nor_db_lookup_gba(id);

    if (entry == NULL || device_size == NULL || sector_size == NULL || buffer_write_bytes == NULL) {
        return false;
    }
    *device_size = chisburner_nor_entry_device_size(entry);
    *sector_size = chisburner_nor_entry_sector_size(entry);
    *buffer_write_bytes = chisburner_nor_entry_buffer_write_bytes(entry);
    return true;
}

chisburner_nor_cmdset_t chisburner_gba_cmdset_from_id(const uint8_t id[8])
{
    const chisburner_nor_entry_t *entry = chisburner_nor_db_lookup_gba(id);
    return chisburner_nor_entry_cmdset(entry);
}

uint32_t chisburner_gba_nor_flags_from_id(const uint8_t id[8])
{
    const chisburner_nor_entry_t *entry = chisburner_nor_db_lookup_gba(id);
    return chisburner_nor_entry_flags(entry);
}

bool chisburner_gba_nor_has_flag(const uint8_t id[8], uint32_t flag)
{
    return (chisburner_gba_nor_flags_from_id(id) & flag) != 0u;
}

const char *chisburner_mbc5_chip_name(const uint8_t id[4])
{
    const chisburner_nor_entry_t *entry = chisburner_nor_db_lookup_mbc5(id);
    return chisburner_nor_entry_name(entry);
}

bool chisburner_mbc5_geometry_from_id(
    const uint8_t id[4],
    uint32_t *device_size,
    uint32_t *sector_size,
    uint16_t *buffer_write_bytes)
{
    const chisburner_nor_entry_t *entry = chisburner_nor_db_lookup_mbc5(id);

    if (entry == NULL || device_size == NULL || sector_size == NULL || buffer_write_bytes == NULL) {
        return false;
    }
    *device_size = chisburner_nor_entry_device_size(entry);
    *sector_size = chisburner_nor_entry_sector_size(entry);
    *buffer_write_bytes = chisburner_nor_entry_buffer_write_bytes(entry);
    return true;
}

chisburner_nor_cmdset_t chisburner_mbc5_cmdset_from_id(const uint8_t id[4])
{
    const chisburner_nor_entry_t *entry = chisburner_nor_db_lookup_mbc5(id);
    return chisburner_nor_entry_cmdset(entry);
}

uint32_t chisburner_mbc5_nor_flags_from_id(const uint8_t id[4])
{
    const chisburner_nor_entry_t *entry = chisburner_nor_db_lookup_mbc5(id);
    return chisburner_nor_entry_flags(entry);
}

bool chisburner_mbc5_nor_has_flag(const uint8_t id[4], uint32_t flag)
{
    return (chisburner_mbc5_nor_flags_from_id(id) & flag) != 0u;
}
