#ifndef READ_BLOCK52_H
#define READ_BLOCK52_H
#include <string.h>
#include "of_error.h"
#include "pattern.h"
struct read_ops52 {
    int (*start)(int,uint32_t,void *,uint32_t,void (*)(int,int));
    int (*poll)(void);
    int (*busy)(void);
    uint32_t (*now)(void);
    void (*idle)(void);
};
/* Generalization of smoke51's accepted read512_run: same single-flight
 * callback/token/canary/poison policy, now a bounded block at a checked offset.
 * Persistent storage stays alive even if a timeout leaves a late DMA pending. */
static uint8_t dma52[CHUNK52+128] __attribute__((aligned(64)));
static volatile unsigned cb_count52;
static volatile int cb_token52,cb_rc52;
static int poisoned52;
static void callback52(int token,int rc) {
    if(!cb_count52){cb_token52=token;cb_rc52=rc;}
    __asm__ volatile("" ::: "memory");++cb_count52;
}
static int read_block52(const struct read_ops52 *o,uint32_t off,uint8_t *out) {
    if(poisoned52)return OF_ERR_INVALID_STATE;
    if(!out || off>TOTAL52-CHUNK52 || off%CHUNK52)return OF_ERR_BAD_RANGE;
    int busy=o->busy();if(busy)return busy<0?busy:OF_ERR_BUSY;
    memset(dma52,0xcc,sizeof(dma52));cb_count52=0;
    cb_token52=cb_rc52=OF_ERR_INVALID_STATE;
    uint32_t begin=o->now();
    int token=o->start(4,off,dma52+64,CHUNK52,callback52);
    if(token<0){poisoned52=1;return token;}
    unsigned polls=0;
    while(!cb_count52) {
        int rc=o->poll();if(rc<0){poisoned52=1;return rc;}
        if(cb_count52)break;
        if((uint32_t)(o->now()-begin)>=5000u || ++polls>=10000u) {
            poisoned52=1;return OF_ERR_TIMEOUT;
        }
        o->idle();
    }
    __asm__ volatile("" ::: "memory");
    if(cb_count52!=1 || cb_token52!=token){poisoned52=1;return OF_ERR_FAILED;}
    if(cb_rc52){poisoned52=1;return cb_rc52;}
    busy=o->busy();if(busy){poisoned52=1;return busy<0?busy:OF_ERR_BUSY;}
    for(unsigned i=0;i<64;++i)if(dma52[i]!=0xcc || dma52[CHUNK52+64+i]!=0xcc) {
        poisoned52=1;return OF_ERR_BAD_RANGE;
    }
    memcpy(out,dma52+64,CHUNK52);return 0;
}
#endif
