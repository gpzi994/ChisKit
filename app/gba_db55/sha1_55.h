#ifndef CHISBURNER_SHA1_55_H
#define CHISBURNER_SHA1_55_H

#include <stddef.h>
#include <stdint.h>

struct sha1_55_ctx {
    uint32_t h[5];
    uint64_t bytes;
    uint8_t block[64];
    uint32_t used;
};

static uint32_t sha1_55_rol(uint32_t v, unsigned n) {
    return (v << n) | (v >> (32u - n));
}

static void sha1_55_transform(struct sha1_55_ctx *c, const uint8_t b[64]) {
    uint32_t w[80];
    for (unsigned i = 0; i < 16; ++i) {
        unsigned p = i * 4u;
        w[i] = ((uint32_t)b[p] << 24) | ((uint32_t)b[p + 1] << 16) |
               ((uint32_t)b[p + 2] << 8) | b[p + 3];
    }
    for (unsigned i = 16; i < 80; ++i)
        w[i] = sha1_55_rol(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);

    uint32_t a = c->h[0], d = c->h[3], e = c->h[4];
    uint32_t bb = c->h[1], cc = c->h[2];
    for (unsigned i = 0; i < 80; ++i) {
        uint32_t f, k;
        if (i < 20) {
            f = (bb & cc) | ((~bb) & d); k = 0x5a827999u;
        } else if (i < 40) {
            f = bb ^ cc ^ d; k = 0x6ed9eba1u;
        } else if (i < 60) {
            f = (bb & cc) | (bb & d) | (cc & d); k = 0x8f1bbcdcu;
        } else {
            f = bb ^ cc ^ d; k = 0xca62c1d6u;
        }
        uint32_t t = sha1_55_rol(a, 5) + f + e + k + w[i];
        e = d; d = cc; cc = sha1_55_rol(bb, 30); bb = a; a = t;
    }
    c->h[0] += a; c->h[1] += bb; c->h[2] += cc;
    c->h[3] += d; c->h[4] += e;
}

static void sha1_55_init(struct sha1_55_ctx *c) {
    c->h[0] = 0x67452301u; c->h[1] = 0xefcdab89u;
    c->h[2] = 0x98badcfeu; c->h[3] = 0x10325476u;
    c->h[4] = 0xc3d2e1f0u; c->bytes = 0; c->used = 0;
}

static void sha1_55_update(struct sha1_55_ctx *c, const void *src, size_t n) {
    const uint8_t *p = (const uint8_t *)src;
    c->bytes += n;
    while (n) {
        size_t take = 64u - c->used;
        if (take > n) take = n;
        for (size_t i = 0; i < take; ++i) c->block[c->used + i] = p[i];
        c->used += (uint32_t)take; p += take; n -= take;
        if (c->used == 64u) { sha1_55_transform(c, c->block); c->used = 0; }
    }
}

static void sha1_55_final(struct sha1_55_ctx *c, uint8_t out[20]) {
    uint64_t bits = c->bytes * 8u;
    c->block[c->used++] = 0x80u;
    if (c->used > 56u) {
        while (c->used < 64u) c->block[c->used++] = 0;
        sha1_55_transform(c, c->block); c->used = 0;
    }
    while (c->used < 56u) c->block[c->used++] = 0;
    for (unsigned i = 0; i < 8; ++i)
        c->block[56u + i] = (uint8_t)(bits >> (56u - i * 8u));
    sha1_55_transform(c, c->block);
    for (unsigned i = 0; i < 5; ++i) {
        out[i * 4] = (uint8_t)(c->h[i] >> 24);
        out[i * 4 + 1] = (uint8_t)(c->h[i] >> 16);
        out[i * 4 + 2] = (uint8_t)(c->h[i] >> 8);
        out[i * 4 + 3] = (uint8_t)c->h[i];
    }
}

static void sha1_55(const void *src, size_t n, uint8_t out[20]) {
    struct sha1_55_ctx c; sha1_55_init(&c); sha1_55_update(&c, src, n);
    sha1_55_final(&c, out);
}

#endif
