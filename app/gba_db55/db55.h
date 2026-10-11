#ifndef CHISBURNER_GBA_DB55_H
#define CHISBURNER_GBA_DB55_H

#include <stddef.h>
#include <stdint.h>

#define DB55_ENTRY_COUNT_EXPECTED 2923u
#define DB55_UNKNOWN_ROM_SIZE     0x02000000u
#define DB55_HEADER_BYTES         0x180u
#define DB55_HEADER_SHA1_BYTES    20u

#define DB55_FLAG_HAS_3D 0x01u
#define DB55_FLAG_3D     0x02u
#define DB55_FLAG_HAS_RV 0x04u

/* Kept naturally aligned: the RISC-V target must not perform unaligned words. */
struct db55_entry {
    uint8_t header_sha1[DB55_HEADER_SHA1_BYTES];
    uint32_t rom_size;
    uint32_t rom_crc32;
    uint32_t save_size;
    uint8_t game_code[4];
    uint8_t save_type;
    uint8_t flags;
    uint8_t reserved[2];
};

struct db55_result {
    uint8_t header_sha1[DB55_HEADER_SHA1_BYTES];
    const struct db55_entry *entry;
    uint32_t dump_size;
    uint8_t hash_bytes;
    uint8_t fixed96_ok;
    uint8_t checksum_ok;
    uint8_t logo_ok;
};

extern const struct db55_entry db55_entries[];
extern const uint32_t db55_entry_count;
extern const char db55_source_sha256[];
extern const char db55_source_commit[];

const struct db55_entry *db55_lookup(const uint8_t sha1[20]);
int db55_validate_index(void);
void db55_identify_header(const uint8_t header[DB55_HEADER_BYTES],
                          struct db55_result *out);
uint8_t db55_header_checksum(const uint8_t header[DB55_HEADER_BYTES]);

#endif
