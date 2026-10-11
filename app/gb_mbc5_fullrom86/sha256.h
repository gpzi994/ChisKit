#ifndef CHISBURNER_SHA256_H
#define CHISBURNER_SHA256_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t state[8];
    uint64_t total_bytes;
    uint8_t block[64];
    uint32_t block_bytes;
} sha256_ctx_t;

void sha256_init(sha256_ctx_t *ctx);
void sha256_update(sha256_ctx_t *ctx, const uint8_t *data, size_t size);
void sha256_final(sha256_ctx_t *ctx, uint8_t digest[32]);

#endif
