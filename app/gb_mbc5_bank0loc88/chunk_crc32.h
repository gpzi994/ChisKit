#ifndef CHUNK_CRC32_H
#define CHUNK_CRC32_H

#include <stddef.h>
#include <stdint.h>

#define BANK0_CHUNK_COUNT 512u

extern const uint32_t bank0_expected_chunk_crc32[BANK0_CHUNK_COUNT];
uint32_t chunk_crc32_update(uint32_t crc, const uint8_t *data, size_t length);

#endif
