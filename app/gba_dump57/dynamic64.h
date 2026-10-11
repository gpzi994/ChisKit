#ifndef CHISBURNER_DYNAMIC64_H
#define CHISBURNER_DYNAMIC64_H

#include <stdint.h>
#include <string.h>

#define D64_PATH "/Saves/chisburner/common/dump57-32m.gba"
#define D64_SLOT 4u
#define D64_PARAM_BYTES 264u

static inline void d64_put_wire_word(uint8_t *dst, uint32_t value) {
    dst[0] = (uint8_t)(value >> 24);
    dst[1] = (uint8_t)(value >> 16);
    dst[2] = (uint8_t)(value >> 8);
    dst[3] = (uint8_t)value;
}

static inline void d64_make_open_param(uint8_t out[D64_PARAM_BYTES],
                                       unsigned create, uint32_t size) {
    memset(out, 0, D64_PARAM_BYTES);
    memcpy(out, D64_PATH, sizeof(D64_PATH));
    d64_put_wire_word(out + 256u, create ? 3u : 0u);
    d64_put_wire_word(out + 260u, create ? size : 0u);
}

#endif
