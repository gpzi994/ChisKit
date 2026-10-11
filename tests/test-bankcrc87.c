#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../app/gb_mbc5_bankcrc87/bank_crc32.h"

int main(int argc, char **argv) {
    uint8_t block[16384];

    if (argc != 2) {
        fprintf(stderr, "usage: %s ROM\n", argv[0]);
        return 2;
    }
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("fopen");
        return 2;
    }
    for (unsigned bank = 0; bank < MBC5_DIAG_BANK_COUNT; ++bank) {
        if (fread(block, 1, sizeof(block), file) != sizeof(block)) {
            fprintf(stderr, "short ROM at bank %u\n", bank);
            return 1;
        }
        uint32_t crc = bank_crc32_update(0xffffffffu, block,
                                         sizeof(block)) ^ 0xffffffffu;
        if (crc != mbc5_expected_bank_crc32[bank]) {
            fprintf(stderr, "bank %u CRC mismatch: %08x != %08x\n",
                    bank, crc, mbc5_expected_bank_crc32[bank]);
            return 1;
        }
    }
    if (fgetc(file) != EOF) {
        fputs("ROM has trailing data\n", stderr);
        return 1;
    }
    fclose(file);
    puts("smoke.87 reference ROM per-bank CRC32 table PASS");
    return 0;
}
