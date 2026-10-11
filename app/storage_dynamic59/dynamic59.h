#ifndef STORAGE_DYNAMIC59_H
#define STORAGE_DYNAMIC59_H

#include <stdint.h>
#include <string.h>

#define D59_PATH "/Saves/chisburner/common/dyn59-a7c9.bin"
#define D59_SIZE 8192u
#define D59_PARAM_BASE 0x20400000u
#define D59_PARAM_BYTES 264u
#define D59_PARAM_WORDS (D59_PARAM_BYTES / 4u)
#define D59_NO_RESULT 0xffffffffu

struct d59_trace {
    unsigned command_0192_tx;
    unsigned status_writes;
    unsigned host_busy_writes;
    unsigned host_ok_writes;
    unsigned host_result;
    uint32_t first_status;
    uint32_t last_status;
    unsigned param_reads;
    unsigned param_tx;
    unsigned param_unique;
    unsigned param_matches;
    unsigned param_duplicates;
    unsigned param_conflicts;
    uint32_t first_param_addr;
    uint32_t last_param_addr;
    uint32_t word0;
    uint32_t word1;
    uint32_t flags;
    uint32_t size;
};

static inline void d59_make_param(uint8_t out[D59_PARAM_BYTES]) {
    const char path[] = D59_PATH;
    memset(out, 0, D59_PARAM_BYTES);
    memcpy(out, path, sizeof(path) - 1u);
    out[0x100] = 3u;
    out[0x104] = (uint8_t)D59_SIZE;
    out[0x105] = (uint8_t)(D59_SIZE >> 8);
    out[0x106] = (uint8_t)(D59_SIZE >> 16);
    out[0x107] = (uint8_t)(D59_SIZE >> 24);
}

/* The bridge serializer's trace tap records the wire-order word. */
static inline uint32_t d59_wire_word(const uint8_t *bytes) {
    return ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16) |
           ((uint32_t)bytes[2] << 8) | bytes[3];
}

static inline struct d59_trace d59_parse_trace(const uint32_t *events,
                                                unsigned count) {
    struct d59_trace out;
    uint8_t expected[D59_PARAM_BYTES];
    uint32_t values[D59_PARAM_WORDS];
    uint32_t seen[3] = {0, 0, 0};
    memset(&out, 0, sizeof(out));
    memset(values, 0, sizeof(values));
    d59_make_param(expected);
    out.host_result = D59_NO_RESULT;
    out.first_param_addr = D59_NO_RESULT;

    for (unsigned i = 0; i < count; ++i) {
        const uint32_t *e = events + 4u * i;
        unsigned kind = e[0] >> 28;
        uint32_t addr = e[1];
        uint32_t data = e[2];
        if (kind == 2u && (addr & 0xff00ffffu) == 0xf8001000u &&
            data == 0x636d0192u)
            ++out.command_0192_tx;
        if (kind == 3u && (addr & 0xff00ffffu) == 0xf8001000u) {
            if (!out.status_writes)
                out.first_status = data;
            out.last_status = data;
            ++out.status_writes;
            if ((data >> 16) == 0x6275u)
                ++out.host_busy_writes;
            if ((data >> 16) == 0x6f6bu) {
                ++out.host_ok_writes;
                out.host_result = data & 0xffffu;
            }
        }
        if (addr < D59_PARAM_BASE || addr >= D59_PARAM_BASE + D59_PARAM_BYTES ||
            (addr & 3u))
            continue;
        unsigned index = (addr - D59_PARAM_BASE) >> 2;
        if (kind == 1u) {
            ++out.param_reads;
            continue;
        }
        if (kind != 2u)
            continue;
        ++out.param_tx;
        if (out.first_param_addr == D59_NO_RESULT)
            out.first_param_addr = addr;
        out.last_param_addr = addr;
        uint32_t bit = 1u << (index & 31u);
        unsigned bank = index >> 5;
        if (seen[bank] & bit) {
            ++out.param_duplicates;
            if (values[index] != data)
                ++out.param_conflicts;
            continue;
        }
        seen[bank] |= bit;
        values[index] = data;
        ++out.param_unique;
        if (data == d59_wire_word(expected + index * 4u))
            ++out.param_matches;
        if (index == 0u) out.word0 = data;
        if (index == 1u) out.word1 = data;
        if (index == 64u) out.flags = data;
        if (index == 65u) out.size = data;
    }
    return out;
}

#endif
