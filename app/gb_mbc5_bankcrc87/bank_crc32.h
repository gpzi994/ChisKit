#ifndef BANK_CRC32_H
#define BANK_CRC32_H

#include <stddef.h>
#include <stdint.h>

#define MBC5_DIAG_BANK_COUNT 128u

extern const uint32_t mbc5_expected_bank_crc32[MBC5_DIAG_BANK_COUNT];

uint32_t bank_crc32_update(uint32_t crc, const uint8_t *data, size_t length);

#endif
