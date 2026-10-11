#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "of_output.h"

#define OUTPUT_PATH   "/Saves/chisburner/common/storage-smoke.41.bin"
#define TOTAL_BYTES   (1024u * 1024u + 37u)
#define CHUNK_BYTES   (32u * 1024u)
#define EXPECTED_CRC  0xE3E64E77u

static uint8_t block[CHUNK_BYTES];

#ifndef OF_PC
static const char *open_result_name(int rc) {
    switch (rc) {
    case OF_OK:             return "OK";
    case -2:                return "SLOT";
    case -3:                return "NOFILE";
    case -4:                return "PATH";
    case -5:                return "GENERAL";
    case OF_ERR_TIMEOUT:    return "TIMEOUT";
    default:                return "OTHER";
    }
}
#endif

static uint32_t crc32_update(uint32_t crc, const uint8_t *data,
                             uint32_t size) {
    for (uint32_t i = 0; i < size; ++i) {
        crc ^= data[i];
        for (uint32_t bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ ((0u - (crc & 1u)) & 0xEDB88320u);
    }
    return crc;
}

static void fill_block(uint32_t offset, uint32_t size) {
    for (uint32_t i = 0; i < size; ++i)
        block[i] = (uint8_t)(((offset + i) * 73u + 19u) & 0xFFu);
}

#ifndef OF_PC
static void stage(const char *name, uint32_t done) {
    printf("\033[2J\033[H");
    printf("  ChisBurner\n");
    printf("  TF STORAGE smoke.41\n");
    printf("  ===================\n\n");
    printf("  NO CARTRIDGE ACCESS\n\n");
    printf("  Stage: %s\n", name);
    printf("  Bytes: %lu / %lu\n", (unsigned long)done,
           (unsigned long)TOTAL_BYTES);
    fflush(stdout);
}
#endif

int main(void) {
#ifdef OF_PC
    uint32_t crc = 0xFFFFFFFFu;
    uint32_t offset = 0;
    while (offset < TOTAL_BYTES) {
        uint32_t size = TOTAL_BYTES - offset;
        if (size > CHUNK_BYTES)
            size = CHUNK_BYTES;
        fill_block(offset, size);
        crc = crc32_update(crc, block, size);
        offset += size;
    }
    crc = ~crc;
    int pass = of_output_available() == 0 &&
               of_output_open(OUTPUT_PATH, TOTAL_BYTES) ==
                   OF_ERR_NOT_SUPPORTED &&
               crc == EXPECTED_CRC;
    printf("PC fallback: %s CRC=%08lx\n", pass ? "PASS" : "FAIL",
           (unsigned long)crc);
    return pass ? 0 : 1;
#else
    int failures = 0;
    int api_ok = of_output_available() == 1;
    stage("OUTPUT API", 0);
    if (!api_ok)
        failures++;

    int open_rc = api_ok ? of_output_open(OUTPUT_PATH, TOTAL_BYTES) :
                           OF_ERR_NOT_SUPPORTED;
    if (open_rc != OF_OK)
        failures++;

    uint32_t offset = 0;
    uint32_t chunks = 0;
    uint32_t crc_state = 0xFFFFFFFFu;
    int write_rc = open_rc;
    if (open_rc == OF_OK) {
        while (offset < TOTAL_BYTES) {
            uint32_t size = TOTAL_BYTES - offset;
            if (size > CHUNK_BYTES)
                size = CHUNK_BYTES;
            fill_block(offset, size);
            crc_state = crc32_update(crc_state, block, size);
            write_rc = of_output_write(offset, block, size);
            if (write_rc != OF_OK)
                break;
            offset += size;
            chunks++;
            stage("STREAM WRITE", offset);
        }
    }
    if (write_rc != OF_OK || offset != TOTAL_BYTES)
        failures++;

    uint32_t crc = ~crc_state;
    if (crc != EXPECTED_CRC)
        failures++;

    stage("EXPLICIT FLUSH", offset);
    int flush_rc = (write_rc == OF_OK && offset == TOTAL_BYTES) ?
                   of_output_flush() : OF_ERR_INVALID_STATE;
    if (flush_rc != OF_OK)
        failures++;

    /* Preserve the kernel's timeout-stage/status line on a failed FLUSH.
     * Clearing here made every distinct bridge failure look like RC=-12. */
    if (flush_rc == OF_OK)
        printf("\033[2J\033[H");
    else
        printf("\n  FLUSH DIAG ABOVE (DO NOT CLEAR)\n\n");
    printf("  ChisBurner\n");
    printf("  TF STORAGE smoke.41\n\n");
    printf("  OUTPUT API: %s\n", api_ok ? "PASS" : "FAIL");
    printf("  OPEN:  %s RC=%d\n", open_rc == OF_OK ? "PASS" : "FAIL",
           open_rc);
    printf("  OPEN HOST: %s\n", open_result_name(open_rc));
    printf("  WRITE: %s RC=%d\n",
           write_rc == OF_OK && offset == TOTAL_BYTES ? "PASS" : "FAIL",
           write_rc);
    printf("  BYTES: %lu / %lu\n", (unsigned long)offset,
           (unsigned long)TOTAL_BYTES);
    printf("  CHUNKS: %lu (LAST 37)\n", (unsigned long)chunks);
    printf("  CRC32: %08lx %s\n", (unsigned long)crc,
           crc == EXPECTED_CRC ? "PASS" : "FAIL");
    printf("  FLUSH: %s RC=%d\n\n", flush_rc == OF_OK ? "PASS" : "FAIL",
           flush_rc);
    printf("  STORAGE STREAM: %s", failures ? "FAIL" : "PASS");
    if (failures)
        printf(" (%d)", failures);
    printf("\n  DIAG COMPLETE\n");
    fflush(stdout);

    while (1)
        usleep(100000);
#endif
}
