#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "dynamic58.h"

int main(void) {
    uint8_t expected[D58_BYTES], actual[D58_BYTES];
    d58_fill(expected, 0, D58_BYTES);
    for (unsigned i = 0; i < D58_BYTES; ++i)
        assert(expected[i] == d58_byte(i));
    assert(d58_crc32(expected, D58_BYTES) == 0xd46fa793u);
    memcpy(actual, expected, sizeof(actual));
    uint32_t first = 0;
    assert(d58_compare(actual, expected, D58_BYTES, &first) == D58_BYTES);
    assert(first == D58_NO_BAD);
    for (unsigned i = 0; i < D58_BYTES; ++i) {
        actual[i] ^= 1;
        assert(d58_compare(actual, expected, D58_BYTES, &first) == D58_BYTES - 1u);
        assert(first == i);
        actual[i] ^= 1;
    }

    uint32_t events[] = {
        0x10000000u, 0xf8001000u, 0x636d0192u, 0,
        0x30000000u, 0xf8001000u, 0x62750192u, 0,
        0x30000000u, 0xf8001000u, 0x6f6b0001u, 0,
        0x30000000u, 0xf8001020u, 0x6f6b0000u, 0,
        0x10000000u, 0xf8011000u, 0x636d0192u, 0
    };
    struct d58_trace t = d58_parse_open_trace(events, 5);
    assert(t.command_0192_reads == 2);
    assert(t.host_busy_writes == 1);
    assert(t.host_ok1_writes == 1);
    assert(t.host_ok0_writes == 0);
    assert(t.other_status_writes == 0);
    events[10] = 0x6f6b0000u;
    t = d58_parse_open_trace(events, 5);
    assert(t.host_ok0_writes == 1 && t.host_ok1_writes == 0);
    events[10] = 0x6f6b0005u;
    t = d58_parse_open_trace(events, 5);
    assert(t.other_status_writes == 1);
    printf("Dynamic58 pattern/CRC/trace parser: PASS\n");
    return 0;
}
