#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "dynamic64.h"

static uint32_t wire_word(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | p[3];
}

int main(void) {
    uint8_t p[D64_PARAM_BYTES];
    d64_make_open_param(p, 0u, 33554432u);
    assert(!strcmp((const char *)p, D64_PATH));
    assert(wire_word(p + 256u) == 0u);
    assert(wire_word(p + 260u) == 0u);
    for (unsigned i = sizeof(D64_PATH); i < D64_PARAM_BYTES; ++i)
        assert(p[i] == 0u);

    d64_make_open_param(p, 1u, 33554432u);
    assert(!strcmp((const char *)p, D64_PATH));
    assert(wire_word(p + 256u) == 0x00000003u);
    assert(wire_word(p + 260u) == 0x02000000u);
    assert(p[256] == 0x00 && p[257] == 0x00 &&
           p[258] == 0x00 && p[259] == 0x03);
    assert(p[260] == 0x02 && p[261] == 0x00 &&
           p[262] == 0x00 && p[263] == 0x00);
    puts("dynamic64 0192 flags/32MiB size wire serialization PASS");
    return 0;
}
