#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db55.h"
#include "sha1_55.h"

static void unhex(const char *s, uint8_t *out, unsigned n) {
    for (unsigned i = 0; i < n; ++i) {
        unsigned a = s[i * 2] <= '9' ? s[i * 2] - '0' : s[i * 2] - 'a' + 10;
        unsigned b = s[i * 2 + 1] <= '9' ? s[i * 2 + 1] - '0' : s[i * 2 + 1] - 'a' + 10;
        out[i] = (uint8_t)((a << 4) | b);
    }
}

static void expect_sha(const void *p, size_t n, const char *want) {
    uint8_t got[20], expected[20]; sha1_55(p, n, got); unhex(want, expected, 20);
    assert(memcmp(got, expected, 20) == 0);
}

int main(int argc, char **argv) {
    uint8_t key[20] = {0}, header[DB55_HEADER_BYTES];
    expect_sha("", 0, "da39a3ee5e6b4b0d3255bfef95601890afd80709");
    expect_sha("abc", 3, "a9993e364706816aba3e25717850c26c9cd0d89d");
    assert(sizeof(struct db55_entry) == 40);
    assert(db55_validate_index());
    assert(db55_lookup(key) == NULL);
    memset(key, 0xff, sizeof(key)); assert(db55_lookup(key) == NULL);

    unhex("bb54fbc9c80aed9c8b3fb1c1349e2aac35be8a91", key, 20);
    const struct db55_entry *aria = db55_lookup(key);
    assert(aria && aria->rom_size == 8388608u && aria->rom_crc32 == 894656899u);
    assert(aria->save_type == 3 && aria->save_size == 32768u);
    assert(memcmp(aria->game_code, "A2CE", 4) == 0);

    unhex("3e9fea08607861276888c0e902d2ef72fa6f40c6", key, 20);
    const struct db55_entry *video = db55_lookup(key);
    assert(video && video->rom_size == 67108864u);
    assert((video->flags & (DB55_FLAG_HAS_3D | DB55_FLAG_3D)) ==
           (DB55_FLAG_HAS_3D | DB55_FLAG_3D));

    if (argc != 2) { fprintf(stderr, "ROM path required\n"); return 2; }
    FILE *f = fopen(argv[1], "rb"); assert(f);
    assert(fread(header, 1, sizeof(header), f) == sizeof(header)); fclose(f);
    struct db55_result r; db55_identify_header(header, &r);
    unhex("ae78cb07cb1ca398cd34379a67cf7e01be79f0f4", key, 20);
    assert(memcmp(r.header_sha1, key, 20) == 0);
    assert(r.hash_bytes == 24 && r.entry == NULL);
    assert(r.dump_size == DB55_UNKNOWN_ROM_SIZE);
    assert(r.logo_ok && r.fixed96_ok && r.checksum_ok);
    puts("PASS db55: SHA-1 vectors, 2923-entry complete index, known/3D lookups, current ROM miss => 32 MiB");
    return 0;
}
