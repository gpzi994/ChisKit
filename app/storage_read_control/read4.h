#ifndef STORAGE_READ4_H
#define STORAGE_READ4_H
#include <stdint.h>
#include "of_error.h"

struct read4_ops {
    int (*start)(int, uint32_t, void *, uint32_t, void (*)(int, int));
    int (*poll)(void);
    int (*busy)(void);
    uint32_t (*now_ms)(void);
    void (*idle)(void);
};
struct read4_result {
    int rc, start_attempted, token, callback_token, callback_rc;
    unsigned callbacks;
    uint32_t elapsed_ms, polls;
    unsigned char bytes[4];
};
/* Persistent storage is intentional: a timed-out APF command cannot be
 * canceled and could finish later. Never leave a DMA/callback stack pointer. */
static unsigned char read4_dma[32] __attribute__((aligned(32)));
static volatile unsigned read4_callbacks;
static volatile int read4_cb_token, read4_cb_rc;
static void read4_callback(int token, int result) {
    if (!read4_callbacks) { read4_cb_token=token; read4_cb_rc=result; }
    __asm__ volatile("" ::: "memory");
    ++read4_callbacks;
}
static struct read4_result read4_run(const struct read4_ops *ops) {
    struct read4_result r={0};
    r.rc=r.token=r.callback_token=r.callback_rc=OF_ERR_INVALID_STATE;
    int busy=ops->busy();
    if (busy) { r.rc=busy<0?busy:OF_ERR_BUSY; return r; }
    for (unsigned i=0;i<sizeof(read4_dma);++i) read4_dma[i]=0xa5;
    read4_callbacks=0; read4_cb_token=read4_cb_rc=OF_ERR_INVALID_STATE;
    uint32_t begin=ops->now_ms();
    r.start_attempted=1;
    r.token=ops->start(4,0,read4_dma,4,read4_callback);
    if (r.token<0) { r.rc=r.token; return r; }
    r.rc=OF_OK;
    while (!read4_callbacks) {
        int p=ops->poll();
        ++r.polls;
        if (p<0) { r.rc=p; break; }
        if (read4_callbacks) break;
        if ((uint32_t)(ops->now_ms()-begin)>=5000u || r.polls>=10000u) {
            r.rc=OF_ERR_TIMEOUT; break;
        }
        ops->idle();
    }
    r.elapsed_ms=ops->now_ms()-begin;
    r.callbacks=read4_callbacks;
    if (r.rc!=OF_OK) return r;
    __asm__ volatile("" ::: "memory");
    r.callback_token=read4_cb_token; r.callback_rc=read4_cb_rc;
    if (r.callbacks!=1 || r.callback_token!=r.token) { r.rc=OF_ERR_FAILED; return r; }
    if (r.callback_rc!=OF_OK) { r.rc=r.callback_rc; return r; }
    busy=ops->busy();
    if (busy) { r.rc=busy<0?busy:OF_ERR_BUSY; return r; }
    for (unsigned i=4;i<sizeof(read4_dma);++i)
        if (read4_dma[i]!=0xa5) { r.rc=OF_ERR_BAD_RANGE; return r; }
    for (unsigned i=0;i<4;++i) r.bytes[i]=read4_dma[i];
    r.rc=OF_OK;
    return r;
}
#endif
