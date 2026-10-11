#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "dynamic59.h"

int main(void) {
    uint8_t param[D59_PARAM_BYTES];
    d59_make_param(param);
    assert(!strcmp((const char *)param, D59_PATH));
    assert(d59_wire_word(param) == 0x2f536176u);
    assert(d59_wire_word(param + 4) == 0x65732f66u);
    assert(d59_wire_word(param + 0x100) == 0x03000000u);
    assert(d59_wire_word(param + 0x104) == 0x00200000u);

    uint32_t events[(D59_PARAM_WORDS + 4u) * 4u];
    unsigned n = 0;
#define EVENT(kind, addr, data) do { \
    events[n*4u] = (uint32_t)(kind) << 28; \
    events[n*4u+1u] = (addr); events[n*4u+2u] = (data); \
    events[n*4u+3u] = 0; ++n; \
} while (0)
    EVENT(2, 0xf8001000u, 0x636d0192u);
    EVENT(3, 0xf8001000u, 0x62750000u);
    for (unsigned i = 0; i < D59_PARAM_WORDS; ++i)
        EVENT(2, D59_PARAM_BASE + i * 4u, d59_wire_word(param + i * 4u));
    EVENT(3, 0xf8001000u, 0x6f6b0003u);
    struct d59_trace t = d59_parse_trace(events, n);
    assert(t.command_0192_tx == 1);
    assert(t.status_writes == 2 && t.host_busy_writes == 1);
    assert(t.host_ok_writes == 1 && t.host_result == 3);
    assert(t.first_status == 0x62750000u && t.last_status == 0x6f6b0003u);
    assert(t.param_tx == D59_PARAM_WORDS);
    assert(t.param_unique == D59_PARAM_WORDS);
    assert(t.param_matches == D59_PARAM_WORDS);
    assert(t.param_duplicates == 0 && t.param_conflicts == 0);
    assert(t.word0 == 0x2f536176u && t.word1 == 0x65732f66u);
    assert(t.flags == 0x03000000u && t.size == 0x00200000u);
    EVENT(2, D59_PARAM_BASE, 0xdeadbeefu);
    t = d59_parse_trace(events, n);
    assert(t.param_duplicates == 1 && t.param_conflicts == 1);
    assert(t.param_unique == D59_PARAM_WORDS && t.param_matches == D59_PARAM_WORDS);
    printf("Dynamic59 parameter/trace parser: PASS\n");
    return 0;
}
