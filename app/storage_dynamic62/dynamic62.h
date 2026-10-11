#ifndef CHISBURNER_DYNAMIC62_H
#define CHISBURNER_DYNAMIC62_H

#include "../storage_dynamic61/dynamic61.h"

#define D62_PATH              "/Saves/chisburner/common/dyn62-direct.bin"
#define D62_SLOT              4u
#define D62_SIZE              D61_SIZE
#define D62_CHUNK             D58_CHUNK
#define D62_BLOCKS            D58_BLOCKS
#define D62_EXPECTED_CRC      D61_EXPECTED_CRC
#define D62_CREATE_FLAGS_WIRE 0x00000003u
#define D62_CREATE_SIZE_WIRE  0x00002000u
#define D62_EXIST_FLAGS_WIRE  0x00000000u
#define D62_EXIST_SIZE_WIRE   0x00000000u

static inline int d62_trace_ok(const struct d60_trace *t,
                               unsigned create_mode) {
    return t->command_0192_tx == 1u &&
           t->host_result == (create_mode ? 1u : 0u) &&
           t->param_unique == D60_PARAM_WORDS &&
           t->flags == (create_mode ? D62_CREATE_FLAGS_WIRE :
                                     D62_EXIST_FLAGS_WIRE) &&
           t->size == (create_mode ? D62_CREATE_SIZE_WIRE :
                                    D62_EXIST_SIZE_WIRE);
}

#endif
