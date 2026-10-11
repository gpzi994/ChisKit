#include "sha256.h"

static uint32_t rotr(uint32_t x, unsigned n) {
    return (x >> n) | (x << (32u - n));
}

static const uint32_t k[64] = {
    0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,
    0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
    0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,
    0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
    0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,
    0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
    0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,
    0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
    0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,
    0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
    0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,
    0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
    0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,
    0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
    0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,
    0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u
};

static void transform(sha256_ctx_t *ctx, const uint8_t block[64]) {
    uint32_t w[64];
    for (unsigned i = 0; i < 16; ++i) {
        unsigned j = i * 4u;
        w[i] = ((uint32_t)block[j] << 24) |
               ((uint32_t)block[j + 1] << 16) |
               ((uint32_t)block[j + 2] << 8) | block[j + 3];
    }
    for (unsigned i = 16; i < 64; ++i) {
        uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^
                      (w[i - 15] >> 3);
        uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^
                      (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    uint32_t a = ctx->state[0], b = ctx->state[1];
    uint32_t c = ctx->state[2], d = ctx->state[3];
    uint32_t e = ctx->state[4], f = ctx->state[5];
    uint32_t g = ctx->state[6], h = ctx->state[7];
    for (unsigned i = 0; i < 64; ++i) {
        uint32_t s1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
        uint32_t ch = (e & f) ^ (~e & g);
        uint32_t t1 = h + s1 + ch + k[i] + w[i];
        uint32_t s0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t t2 = s0 + maj;
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }
    ctx->state[0] += a; ctx->state[1] += b;
    ctx->state[2] += c; ctx->state[3] += d;
    ctx->state[4] += e; ctx->state[5] += f;
    ctx->state[6] += g; ctx->state[7] += h;
}

void sha256_init(sha256_ctx_t *ctx) {
    static const uint32_t initial[8] = {
        0x6a09e667u,0xbb67ae85u,0x3c6ef372u,0xa54ff53au,
        0x510e527fu,0x9b05688cu,0x1f83d9abu,0x5be0cd19u
    };
    for (unsigned i = 0; i < 8; ++i)
        ctx->state[i] = initial[i];
    ctx->total_bytes = 0;
    ctx->block_bytes = 0;
}

void sha256_update(sha256_ctx_t *ctx, const uint8_t *data, size_t size) {
    ctx->total_bytes += size;
    while (size != 0) {
        size_t room = 64u - ctx->block_bytes;
        size_t take = size < room ? size : room;
        for (size_t i = 0; i < take; ++i)
            ctx->block[ctx->block_bytes + i] = data[i];
        ctx->block_bytes += (uint32_t)take;
        data += take;
        size -= take;
        if (ctx->block_bytes == 64u) {
            transform(ctx, ctx->block);
            ctx->block_bytes = 0;
        }
    }
}

void sha256_final(sha256_ctx_t *ctx, uint8_t digest[32]) {
    uint64_t bits = ctx->total_bytes * 8u;
    ctx->block[ctx->block_bytes++] = 0x80u;
    if (ctx->block_bytes > 56u) {
        while (ctx->block_bytes < 64u)
            ctx->block[ctx->block_bytes++] = 0;
        transform(ctx, ctx->block);
        ctx->block_bytes = 0;
    }
    while (ctx->block_bytes < 56u)
        ctx->block[ctx->block_bytes++] = 0;
    for (unsigned i = 0; i < 8; ++i)
        ctx->block[56u + i] = (uint8_t)(bits >> (56u - 8u * i));
    transform(ctx, ctx->block);
    for (unsigned i = 0; i < 8; ++i) {
        digest[i * 4u] = (uint8_t)(ctx->state[i] >> 24);
        digest[i * 4u + 1] = (uint8_t)(ctx->state[i] >> 16);
        digest[i * 4u + 2] = (uint8_t)(ctx->state[i] >> 8);
        digest[i * 4u + 3] = (uint8_t)ctx->state[i];
    }
}
