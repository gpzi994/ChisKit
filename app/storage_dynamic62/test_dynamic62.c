#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "dynamic62.h"

int main(void) {
    uint8_t data[D62_SIZE];
    d58_fill(data, 0, sizeof(data));
    if (d58_crc32(data, sizeof(data)) != D62_EXPECTED_CRC)
        return 1;

    struct d60_trace t;
    memset(&t, 0, sizeof(t));
    t.command_0192_tx = 1;
    t.param_unique = D60_PARAM_WORDS;
    t.host_result = 1;
    t.flags = D62_CREATE_FLAGS_WIRE;
    t.size = D62_CREATE_SIZE_WIRE;
    if (!d62_trace_ok(&t, 1))
        return 2;
    t.host_result = 0;
    t.flags = D62_EXIST_FLAGS_WIRE;
    t.size = D62_EXIST_SIZE_WIRE;
    if (!d62_trace_ok(&t, 0))
        return 3;

    puts("Dynamic62 direct-open logic: PASS");
    return 0;
}
