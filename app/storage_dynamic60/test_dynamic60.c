#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "dynamic60.h"

int main(void) {
    uint8_t param[D60_PARAM_BYTES];
    d60_make_param(param, D60_PATH_MISSING);
    assert(!strcmp((const char *)param, D60_PATH_MISSING));
    assert(d60_wire_word(param) == 0x2f536176u);
    assert(d60_wire_word(param + 4) == 0x65732f66u);
    assert(d60_wire_word(param + 0x100) == 0x03000000u);
    assert(d60_wire_word(param + 0x104) == 0x00200000u);

    uint32_t events[(D60_PARAM_WORDS + 8u) * 4u];
    unsigned n = 0;
#define EVENT(kind, addr, data) do { \
    events[n*4u] = (uint32_t)(kind) << 28; \
    events[n*4u+1u] = (addr); events[n*4u+2u] = (data); \
    events[n*4u+3u] = 0; ++n; \
} while (0)
    EVENT(2, 0xf8001000u, 0x636d0192u);
    EVENT(3, 0xf8001000u, 0x62750000u);
    for (unsigned i = 0; i < D60_PARAM_WORDS; ++i)
        EVENT(2, D60_PARAM_BASE + i * 4u, d60_wire_word(param + i * 4u));
    EVENT(2, D60_PARAM_BASE + 0x100u, 0x03000000u);
    unsigned conflict_event = n;
    EVENT(2, D60_PARAM_BASE + 0x104u, 0x00002000u);
    EVENT(3, 0xf8001000u, 0x6f6b0003u);
    struct d60_trace t = d60_parse_trace(events, n, D60_PATH_MISSING);
    assert(t.command_0192_tx == 1);
    assert(t.status_writes == 2 && t.host_busy_writes == 1);
    assert(t.host_ok_writes == 1 && t.host_result == 3);
    assert(t.param_tx == D60_PARAM_WORDS + 2u);
    assert(t.param_unique == D60_PARAM_WORDS && t.param_matches == D60_PARAM_WORDS);
    assert(t.param_duplicates == 2 && t.param_conflicts == 1);
    assert(t.duplicate_saved == 2);
    assert(t.duplicate[0].address == D60_PARAM_BASE + 0x100u);
    assert(t.duplicate[0].first_value == t.duplicate[0].repeat_value);
    assert(t.duplicate[1].event_index == conflict_event);
    assert(t.duplicate[1].address == D60_PARAM_BASE + 0x104u);
    assert(t.duplicate[1].first_value == 0x00200000u);
    assert(t.duplicate[1].repeat_value == 0x00002000u);
    printf("Dynamic60 control/duplicate parser: PASS\n");
    return 0;
}
