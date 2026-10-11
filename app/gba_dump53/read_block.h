#ifndef READ_BLOCK53_H
#define READ_BLOCK53_H
#include "reference.h"
#include "of_error.h"
#include <string.h>
struct read_ops53 {
    int (*start)(int,uint32_t,void *,uint32_t,void (*)(int,int));
    int (*poll)(void);int (*busy)(void);uint32_t (*now)(void);void (*idle)(void);
};
/* Single flight; persistent DMA storage survives timeout. Never reuse a
 * poisoned request. Canaries include all bytes beyond the short final block. */
static uint8_t dma53[CHUNK53+128] __attribute__((aligned(64)));
static volatile unsigned cb_count53;
static volatile int cb_token53,cb_rc53;
static int poisoned53;
static void callback53(int token,int rc){
    if(!cb_count53){cb_token53=token;cb_rc53=rc;}
    __asm__ volatile("" ::: "memory");++cb_count53;
}
static int read_block53(const struct read_ops53 *o,uint32_t off,uint8_t *out,uint32_t n){
    if(poisoned53)return OF_ERR_INVALID_STATE;
    if(!out || off>=TOTAL53 || off%CHUNK53 || n!=length53(off))return OF_ERR_BAD_RANGE;
    int busy=o->busy();if(busy)return busy<0?busy:OF_ERR_BUSY;
    memset(dma53,0xcc,sizeof(dma53));cb_count53=0;cb_token53=cb_rc53=OF_ERR_INVALID_STATE;
    uint32_t begin=o->now();int token=o->start(4,off,dma53+64,n,callback53);
    if(token<0){poisoned53=1;return token;}
    unsigned polls=0;
    while(!cb_count53){
        int rc=o->poll();if(rc<0){poisoned53=1;return rc;}
        if(cb_count53)break;
        if((uint32_t)(o->now()-begin)>=5000u || ++polls>=10000u){poisoned53=1;return OF_ERR_TIMEOUT;}
        o->idle();
    }
    __asm__ volatile("" ::: "memory");
    if(cb_count53!=1 || cb_token53!=token){poisoned53=1;return OF_ERR_FAILED;}
    if(cb_rc53){poisoned53=1;return cb_rc53;}
    busy=o->busy();if(busy){poisoned53=1;return busy<0?busy:OF_ERR_BUSY;}
    for(unsigned i=0;i<sizeof(dma53);++i)if((i<64 || i>=64+n) && dma53[i]!=0xcc){poisoned53=1;return OF_ERR_BAD_RANGE;}
    memcpy(out,dma53+64,n);return 0;
}
#endif
