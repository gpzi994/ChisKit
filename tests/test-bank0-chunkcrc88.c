#include <stdint.h>
#include <stdio.h>

#include "../app/gb_mbc5_bank0loc88/chunk_crc32.h"

int main(int argc, char **argv) {
    uint8_t block[32];

    if (argc != 2) {
        fprintf(stderr, "usage: %s ROM\n", argv[0]);
        return 2;
    }
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("fopen");
        return 2;
    }
    for (unsigned chunk = 0; chunk < BANK0_CHUNK_COUNT; ++chunk) {
        if (fread(block, 1, sizeof(block), file) != sizeof(block)) {
            fprintf(stderr, "short ROM at chunk %u\n", chunk);
            return 1;
        }
        uint32_t crc = chunk_crc32_update(0xffffffffu, block,
                                          sizeof(block)) ^ 0xffffffffu;
        if (crc != bank0_expected_chunk_crc32[chunk]) {
            fprintf(stderr, "chunk %u CRC mismatch: %08x != %08x\n",
                    chunk, crc, bank0_expected_chunk_crc32[chunk]);
            return 1;
        }
    }
    fclose(file);
    puts("smoke.88 bank0 512 x 32-byte CRC32 table PASS");
    return 0;
}
