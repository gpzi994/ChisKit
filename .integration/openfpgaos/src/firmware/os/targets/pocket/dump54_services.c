/* SDK/OS service implementation ONLY. The application uses of_cart/of_output.
 * Fixed-layout smoke53 extension: preserve boot, storage transfers, BSS and
 * all existing service pointer addresses. Cartridge writes are never issued. */
#include <stdint.h>
#include "of_cart.h"
#define TOTAL54 8705280u
#define MAGIC54 0x53543534
#ifndef DUMP54_TEST
#define RD54(i) (*(volatile uint32_t *)(uintptr_t)(0x400001a0u+4u*(i)))
#define WR54(i,v) (RD54(i)=(v))
#define FEATURES54 (*(volatile uint32_t *)(uintptr_t)0x40000098u)
#endif
extern int of_file_async_busy(void);
extern long of_file_size(uint32_t);
extern volatile int output_is_open;
extern volatile uint32_t output_open_size,output_open_slot;
#define SECTION54(s) __attribute__((section(s),noinline))
static inline int present54(void){return (FEATURES54&32u) && RD54(0)==0x31424746u;}
SECTION54(".caps54") int caps54(void *out,uint32_t n) {
    if(!out || n<sizeof(of_cart_caps_t))return OF_ERR_INVALID_PARAM;
    of_cart_caps_t *c=out;
    uint32_t *p=out;for(unsigned i=0;i<11;++i)p[i]=0;
    c->struct_size=sizeof(*c);c->version=4;
    if(!present54() || RD54(1)!=0x00100f02u)return OF_ERR_NOT_SUPPORTED;
    c->bus_mask=2;c->operation_mask=15;c->max_read_bytes=32;c->gba_space_mask=1;
    c->lifecycle_status=RD54(11);c->host_b1_raw=RD54(12);c->host_diag=RD54(13);
    return 0;
}
SECTION54(".read54") int read54(uint32_t bus,uint32_t off,void *dst,uint32_t n,uint32_t us) {
    if(!dst || !us)return OF_ERR_INVALID_PARAM;
    if(bus!=2 || n!=32 || (off&31u) || off>TOTAL54-32 || us!=50000)return OF_ERR_BAD_RANGE;
    if(!present54() || RD54(1)!=0x00100f02u)return OF_ERR_NOT_SUPPORTED;
    if((RD54(11)&0x80000307u)!=0x80000303u)return OF_ERR_INVALID_STATE;
    if(RD54(2)&2u)return OF_ERR_BUSY;
    WR54(16,0);WR54(4,2);WR54(5,off);WR54(6,32);WR54(7,4500000);WR54(3,2);
    unsigned wait=1000000;uint32_t st;
    do {st=RD54(2);if(!(st&2u))break;}while(--wait);
    if(!wait){WR54(3,3);return OF_ERR_TIMEOUT;}
    if(st&32u)return OF_ERR_TIMEOUT;
    if(st&16u)return OF_ERR_INVALID_STATE;
    if((st&8u) || !(st&4u) || RD54(9)!=32)return OF_ERR_IO;
    uint8_t *p=dst;
    for(unsigned i=0;i<8;++i){
        uint32_t w=RD54(8);
        for(unsigned j=0;j<4;++j)*p++=(uint8_t)(w>>(8*j));
    }
    return 0;
}
/* The original LTO image inlined the old read stub here. Updating only
 * cart_read cannot update cart_read_space: replace this bounded entry too. */
SECTION54(".space54") int space54(uint32_t bus,uint32_t space,uint32_t off,void *dst,uint32_t n,uint32_t us) {
    if(space!=OF_CART_SPACE_ROM)return OF_ERR_NOT_SUPPORTED;
    return read54(bus,off,dst,n,us);
}
SECTION54(".bind54") int bind54(const char *name,uint32_t size) {
    static const char expected[]="dump54.gba";
    if(!name)return size==0?MAGIC54:OF_ERR_INVALID_PARAM;
    if(size!=TOTAL54)return OF_ERR_INVALID_PARAM;
    for(unsigned i=0;i<sizeof(expected);++i)if(name[i]!=expected[i])return OF_ERR_INVALID_PARAM;
    int busy=of_file_async_busy();if(busy)return busy<0?busy:OF_ERR_BUSY;
    if(of_file_size(4)!=TOTAL54)return OF_ERR_BAD_RANGE;
    output_open_slot=4;output_open_size=TOTAL54;output_is_open=1;return 0;
}
