#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic61.h"

int main(void) {
    uint8_t data[D61_SIZE];
    d58_fill(data, 0, D61_SIZE);
    assert(d58_crc32(data, D61_SIZE) == D61_EXPECTED_CRC);

    uint32_t events[(D60_PARAM_WORDS + 4u) * 4u];
    uint8_t param[D60_PARAM_BYTES];
    unsigned n = 0;
    memset(param, 0, sizeof(param));
    memcpy(param, D61_PATH, strlen(D61_PATH));
    param[0x103] = 3u;
    param[0x106] = 0x20u;
#define EVENT(kind, addr, value) do { \
    events[n * 4u] = (uint32_t)(kind) << 28; \
    events[n * 4u + 1u] = (addr); \
    events[n * 4u + 2u] = (value); \
    events[n * 4u + 3u] = 0; \
    ++n; \
} while (0)
    EVENT(2, 0xf8001000u, 0x636d0192u);
    EVENT(3, 0xf8001000u, 0x62750000u);
    for (unsigned i = 0; i < D60_PARAM_WORDS; ++i)
        EVENT(2, D60_PARAM_BASE + i * 4u,
              d60_wire_word(param + i * 4u));
    EVENT(3, 0xf8001000u, 0x6f6b0001u);
    struct d60_trace trace = d60_parse_trace(events, n, D61_PATH);
    assert(trace.flags == D61_FLAGS_WIRE);
    assert(trace.size == D61_SIZE_WIRE);
    assert(d61_param_wire_ok(&trace));
    assert(d61_open_trace_ok(&trace, 1));
    trace.host_result = 0;
    assert(d61_open_trace_ok(&trace, 0));
    trace.flags = 0x03000000u;
    assert(!d61_param_wire_ok(&trace));
    printf("Dynamic61 endian/trace/data logic: PASS\n");
    return 0;
}
