#ifndef STATIC50_READ512_H
#define STATIC50_READ512_H
#include <string.h>
#include "of_error.h"
#include <stdint.h>
struct read50_ops {
    int (*start)(int,uint32_t,void *,uint32_t,void (*)(int,int));
    int (*poll)(void);
    int (*busy)(void);
    uint32_t (*now_ms)(void);
    void (*idle)(void);
};
static uint8_t dma512[640] __attribute__((aligned(64)));
static volatile unsigned cb512_count;
static volatile int cb512_token,cb512_rc;
static int read512_poisoned;
static void callback512(int token,int rc) {
    if(!cb512_count) {cb512_token=token;cb512_rc=rc;}
    __asm__ volatile("" ::: "memory"); ++cb512_count;
}
static int read512_run(const struct read50_ops *o,uint8_t *out) {
    if(read512_poisoned) return OF_ERR_INVALID_STATE;
    int busy=o->busy(); if(busy) return busy<0?busy:OF_ERR_BUSY;
    memset(dma512,0xcc,sizeof(dma512));
    cb512_count=0;cb512_token=cb512_rc=OF_ERR_INVALID_STATE;
    uint32_t begin=o->now_ms();
    int token=o->start(4,0,dma512+64,512,callback512);
    if(token<0) return token;
    unsigned polls=0;
    while(!cb512_count) {
        int rc=o->poll();
        if(rc<0) {read512_poisoned=1;return rc;}
        if(cb512_count) break;
        if((uint32_t)(o->now_ms()-begin)>=5000 || ++polls>=10000) {
            read512_poisoned=1;return OF_ERR_TIMEOUT;
        }
        o->idle();
    }
    __asm__ volatile("" ::: "memory");
    if(cb512_count!=1 || cb512_token!=token) {read512_poisoned=1;return OF_ERR_FAILED;}
    if(cb512_rc) {read512_poisoned=1;return cb512_rc;}
    busy=o->busy(); if(busy) {read512_poisoned=1;return busy<0?busy:OF_ERR_BUSY;}
    for(unsigned i=0;i<64;++i)
        if(dma512[i]!=0xcc || dma512[576+i]!=0xcc) {
            read512_poisoned=1;return OF_ERR_BAD_RANGE;
        }
    memcpy(out,dma512+64,512);
    return 0;
}
#endif
