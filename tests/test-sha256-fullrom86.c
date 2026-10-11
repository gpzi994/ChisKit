#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../app/gb_mbc5_fullrom86/sha256.h"

int main(int argc, char **argv) {
    static const uint8_t expected[32] = {
        0x8b,0xa5,0x33,0xbe,0x59,0x08,0x30,0x5a,
        0x0f,0x2f,0x04,0x11,0x7a,0x85,0x8a,0x91,
        0xd4,0x09,0x8e,0xd8,0xd4,0xd2,0xbe,0x6e,
        0x48,0xc3,0xc3,0xe9,0xdf,0x18,0x85,0xe7
    };
    if (argc != 2) return 2;
    FILE *f = fopen(argv[1], "rb");
    if (!f) return 3;
    sha256_ctx_t ctx;
    uint8_t buf[777], digest[32];
    sha256_init(&ctx);
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) != 0)
        sha256_update(&ctx, buf, n);
    fclose(f);
    sha256_final(&ctx, digest);
    if (memcmp(digest, expected, 32) != 0) return 4;
    puts("fullrom86 SHA-256 implementation/reference ROM PASS");
    return 0;
}
