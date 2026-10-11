#ifndef READ_BLOCK54_H
#define READ_BLOCK54_H
#include "reference.h"
#include "of_error.h"
#include <string.h>
struct read_ops54 {
    int (*start)(int,uint32_t,void *,uint32_t,void (*)(int,int));
    int (*poll)(void);int (*busy)(void);uint32_t (*now)(void);void (*idle)(void);
};
/* Single flight; persistent DMA storage survives timeout. Never reuse a
 * poisoned request. Canaries include all bytes beyond the short final block. */
static uint8_t dma54[CHUNK54+128] __attribute__((aligned(64)));
static volatile unsigned cb_count54;
static volatile int cb_token54,cb_rc54;
static int poisoned54;
static void callback54(int token,int rc){
    if(!cb_count54){cb_token54=token;cb_rc54=rc;}
    __asm__ volatile("" ::: "memory");++cb_count54;
}
static int read_block54(const struct read_ops54 *o,uint32_t off,uint8_t *out,uint32_t n){
    if(poisoned54)return OF_ERR_INVALID_STATE;
    if(!out || off>=TOTAL54 || off%CHUNK54 || n!=length54(off))return OF_ERR_BAD_RANGE;
    int busy=o->busy();if(busy)return busy<0?busy:OF_ERR_BUSY;
    memset(dma54,0xcc,sizeof(dma54));cb_count54=0;cb_token54=cb_rc54=OF_ERR_INVALID_STATE;
    uint32_t begin=o->now();int token=o->start(4,off,dma54+64,n,callback54);
    if(token<0){poisoned54=1;return token;}
    unsigned polls=0;
    while(!cb_count54){
        int rc=o->poll();if(rc<0){poisoned54=1;return rc;}
        if(cb_count54)break;
        if((uint32_t)(o->now()-begin)>=5000u || ++polls>=10000u){poisoned54=1;return OF_ERR_TIMEOUT;}
        o->idle();
    }
    __asm__ volatile("" ::: "memory");
    if(cb_count54!=1 || cb_token54!=token){poisoned54=1;return OF_ERR_FAILED;}
    if(cb_rc54){poisoned54=1;return cb_rc54;}
    busy=o->busy();if(busy){poisoned54=1;return busy<0?busy:OF_ERR_BUSY;}
    for(unsigned i=0;i<sizeof(dma54);++i)if((i<64 || i>=64+n) && dma54[i]!=0xcc){poisoned54=1;return OF_ERR_BAD_RANGE;}
    memcpy(out,dma54+64,n);return 0;
}
#endif
