#include <string.h>
#include "db55.h"
#include "sha1_55.h"

_Static_assert(sizeof(struct db55_entry) == 40, "db55 entry layout changed");

static int valid_code(const uint8_t code[4]) {
    for (unsigned i = 0; i < 4; ++i)
        if (code[i] < 0x20u || code[i] > 0x7eu) return 0;
    return 1;
}

uint8_t db55_header_checksum(const uint8_t header[DB55_HEADER_BYTES]) {
    uint8_t sum = 0;
    for (unsigned i = 0xa0; i <= 0xbc; ++i) sum = (uint8_t)(sum + header[i]);
    return (uint8_t)(0u - sum - 0x19u);
}

const struct db55_entry *db55_lookup(const uint8_t sha1[20]) {
    uint32_t lo = 0, hi = db55_entry_count;
    while (lo < hi) {
        uint32_t mid = lo + (hi - lo) / 2u;
        int order = memcmp(sha1, db55_entries[mid].header_sha1, 20);
        if (order == 0) return &db55_entries[mid];
        if (order < 0) hi = mid; else lo = mid + 1u;
    }
    return NULL;
}

int db55_validate_index(void) {
    static const uint32_t rom_sizes[] = {
        0x00100000u, 0x00200000u, 0x00400000u, 0x00800000u,
        0x01000000u, 0x02000000u, 0x04000000u
    };
    if (db55_entry_count != DB55_ENTRY_COUNT_EXPECTED) return 0;
    for (uint32_t i = 0; i < db55_entry_count; ++i) {
        const struct db55_entry *e = &db55_entries[i];
        int size_ok = 0;
        for (unsigned j = 0; j < sizeof(rom_sizes) / sizeof(rom_sizes[0]); ++j)
            if (e->rom_size == rom_sizes[j]) size_ok = 1;
        if (!size_ok || e->save_type > 6u || !valid_code(e->game_code)) return 0;
        if (i && memcmp(db55_entries[i - 1].header_sha1,
                        e->header_sha1, 20) >= 0) return 0;
        if (db55_lookup(e->header_sha1) != e) return 0;
    }
    return 1;
}

void db55_identify_header(const uint8_t header[DB55_HEADER_BYTES],
                          struct db55_result *out) {
    static const uint8_t logo_sha1[20] = {
        0x17,0xda,0xa0,0xfe,0xc0,0x2f,0xc3,0x3c,0x0f,0x6a,
        0xbb,0x54,0x9a,0x8b,0x80,0xb6,0x61,0x3b,0x48,0xee
    };
    uint8_t logo[20];
    memset(out, 0, sizeof(*out));
    out->hash_bytes = (header[0xac] == 'M' && valid_code(header + 0xac))
        ? 0x100u / 16u : 0x180u / 16u;
    uint32_t hash_len = out->hash_bytes == 0x10u ? 0x100u : 0x180u;
    sha1_55(header, hash_len, out->header_sha1);
    sha1_55(header + 4, 0x9cu, logo);
    out->logo_ok = (uint8_t)(memcmp(logo, logo_sha1, 20) == 0);
    out->fixed96_ok = (uint8_t)(header[0xb2] == 0x96u);
    out->checksum_ok = (uint8_t)(header[0xbd] == db55_header_checksum(header));
    out->entry = db55_lookup(out->header_sha1);
    out->dump_size = out->entry ? out->entry->rom_size : DB55_UNKNOWN_ROM_SIZE;
}
