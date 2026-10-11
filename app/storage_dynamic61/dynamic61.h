#ifndef STORAGE_DYNAMIC61_H
#define STORAGE_DYNAMIC61_H

#include <stdint.h>

#include "../storage_dynamic58/dynamic58.h"
#include "../storage_dynamic60/dynamic60.h"

#define D61_PATH "/Saves/chisburner/common/dyn61-endian.bin"
#define D61_SLOT_PATH "slot:4"
#define D61_SIZE D58_BYTES
#define D61_EXPECTED_CRC 0xd46fa793u
#define D61_FLAGS_WIRE 0x00000003u
#define D61_SIZE_WIRE 0x00002000u

static inline int d61_param_wire_ok(const struct d60_trace *trace) {
    return trace->flags == D61_FLAGS_WIRE &&
           trace->size == D61_SIZE_WIRE;
}

static inline int d61_open_trace_ok(const struct d60_trace *trace,
                                    unsigned create_mode) {
    return trace->command_0192_tx == 1u &&
           trace->host_busy_writes == 1u &&
           trace->host_ok_writes == 1u &&
           trace->host_result == (create_mode ? 1u : 0u) &&
           trace->param_unique == D60_PARAM_WORDS &&
           d61_param_wire_ok(trace);
}

#endif
