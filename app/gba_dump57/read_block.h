#ifndef CHISBURNER_DUMP57_READ_BLOCK_H
#define CHISBURNER_DUMP57_READ_BLOCK_H
#include <string.h>
#include "reference.h"
#include "of_error.h"

struct read_ops57 {
    int (*start)(int,uint32_t,void *,uint32_t,void (*)(int,int));
    int (*poll)(void);
    int (*busy)(void);
    uint32_t (*now)(void);
    void (*idle)(void);
};

static uint8_t dma57[CHUNK57 + 128] __attribute__((aligned(64)));
static volatile unsigned cb_count57;
static volatile int cb_token57, cb_rc57;
static int poisoned57;

static void callback57(int token, int rc) {
    if (!cb_count57) { cb_token57 = token; cb_rc57 = rc; }
    __asm__ volatile("" ::: "memory");
    ++cb_count57;
}

static int read_block57(const struct read_ops57 *o, uint32_t off,
                        uint8_t *out, uint32_t n) {
    if (poisoned57) return OF_ERR_INVALID_STATE;
    if (!out || off >= TOTAL57 || off % CHUNK57 || n != length57(off))
        return OF_ERR_BAD_RANGE;
    int busy = o->busy();
    if (busy) return busy < 0 ? busy : OF_ERR_BUSY;
    memset(dma57, 0xcc, sizeof(dma57));
    cb_count57 = 0; cb_token57 = cb_rc57 = OF_ERR_INVALID_STATE;
    uint32_t begin = o->now();
    int token = o->start(4, off, dma57 + 64, n, callback57);
    if (token < 0) { poisoned57 = 1; return token; }
    unsigned polls = 0;
    while (!cb_count57) {
        int rc = o->poll();
        if (rc < 0) { poisoned57 = 1; return rc; }
        if (cb_count57) break;
        if ((uint32_t)(o->now() - begin) >= 5000u || ++polls >= 10000u) {
            poisoned57 = 1; return OF_ERR_TIMEOUT;
        }
        o->idle();
    }
    __asm__ volatile("" ::: "memory");
    if (cb_count57 != 1 || cb_token57 != token) {
        poisoned57 = 1; return OF_ERR_FAILED;
    }
    if (cb_rc57) { poisoned57 = 1; return cb_rc57; }
    busy = o->busy();
    if (busy) { poisoned57 = 1; return busy < 0 ? busy : OF_ERR_BUSY; }
    for (unsigned i = 0; i < sizeof(dma57); ++i)
        if ((i < 64 || i >= 64 + n) && dma57[i] != 0xcc) {
            poisoned57 = 1; return OF_ERR_BAD_RANGE;
        }
    memcpy(out, dma57 + 64, n);
    return 0;
}
#endif
