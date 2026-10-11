#ifndef STORAGE_DYNAMIC58_H
#define STORAGE_DYNAMIC58_H

#include <stdint.h>
#include <string.h>

#define D58_BYTES 8192u
#define D58_CHUNK 1024u
#define D58_BLOCKS (D58_BYTES / D58_CHUNK)
#define D58_NO_BAD 0xffffffffu

struct d58_trace {
    unsigned command_0192_reads;
    unsigned host_busy_writes;
    unsigned host_ok0_writes;
    unsigned host_ok1_writes;
    unsigned other_status_writes;
};

static inline uint8_t d58_byte(uint32_t offset) {
    return (uint8_t)(((offset * 73u + 19u) ^ (offset >> 5)) & 0xffu);
}

static inline void d58_fill(uint8_t *out, uint32_t offset, uint32_t length) {
    for (uint32_t i = 0; i < length; ++i)
        out[i] = d58_byte(offset + i);
}

static inline uint32_t d58_crc32(const uint8_t *data, uint32_t length) {
    uint32_t crc = 0xffffffffu;
    for (uint32_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (unsigned bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ ((0u - (crc & 1u)) & 0xedb88320u);
    }
    return ~crc;
}

static inline unsigned d58_compare(const uint8_t *actual,
                                   const uint8_t *expected,
                                   uint32_t length, uint32_t *first_bad) {
    unsigned matches = 0;
    *first_bad = D58_NO_BAD;
    for (uint32_t i = 0; i < length; ++i) {
        if (actual[i] == expected[i]) {
            ++matches;
        } else if (*first_bad == D58_NO_BAD) {
            *first_bad = i;
        }
    }
    return matches;
}

/* Interpret only a fully validated smoke47/49 audit event stream. The caller
 * freezes immediately after OPEN, so every target-status event belongs to the
 * single 0x0192 command and cannot be confused with a later WRITE/READ. */
static inline struct d58_trace d58_parse_open_trace(const uint32_t *events,
                                                    unsigned count) {
    struct d58_trace out;
    memset(&out, 0, sizeof(out));
    for (unsigned i = 0; i < count; ++i) {
        const uint32_t *e = events + 4u * i;
        unsigned kind = e[0] >> 28;
        uint32_t addr = e[1];
        uint32_t data = e[2];
        if ((addr & 0xff00ffffu) != 0xf8001000u)
            continue;
        if (kind == 1u && data == 0x636d0192u)
            ++out.command_0192_reads;
        if (kind != 3u)
            continue;
        if (data == 0x62750192u)
            ++out.host_busy_writes;
        else if (data == 0x6f6b0000u)
            ++out.host_ok0_writes;
        else if (data == 0x6f6b0001u)
            ++out.host_ok1_writes;
        else
            ++out.other_status_writes;
    }
    return out;
}

#endif
