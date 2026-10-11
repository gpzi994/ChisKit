/* SDK/OS service implementation ONLY. The application uses of_cart/of_output.
 * Fixed-layout smoke52 extension: preserve boot, storage transfers, BSS and
 * all existing service pointer addresses. Cartridge writes are never issued. */
#include <stdint.h>
#include "of_cart.h"
#define TOTAL53 8705280u
#define MAGIC53 0x53543533
#ifndef DUMP53_TEST
#define RD53(i) (*(volatile uint32_t *)(uintptr_t)(0x400001a0u+4u*(i)))
#define WR53(i,v) (RD53(i)=(v))
#define FEATURES53 (*(volatile uint32_t *)(uintptr_t)0x40000098u)
#endif
extern int of_file_async_busy(void);
extern long of_file_size(uint32_t);
extern volatile int output_is_open;
extern volatile uint32_t output_open_size,output_open_slot;
#define SECTION53(s) __attribute__((section(s),noinline))
static inline int present53(void){return (FEATURES53&32u) && RD53(0)==0x31424746u;}
SECTION53(".caps53") int caps53(void *out,uint32_t n) {
    if(!out || n<sizeof(of_cart_caps_t))return OF_ERR_INVALID_PARAM;
    of_cart_caps_t *c=out;
    uint32_t *p=out;for(unsigned i=0;i<11;++i)p[i]=0;
    c->struct_size=sizeof(*c);c->version=4;
    if(!present53() || RD53(1)!=0x00100f02u)return OF_ERR_NOT_SUPPORTED;
    c->bus_mask=2;c->operation_mask=15;c->max_read_bytes=32;c->gba_space_mask=1;
    c->lifecycle_status=RD53(11);c->host_b1_raw=RD53(12);c->host_diag=RD53(13);
    return 0;
}
SECTION53(".read53") int read53(uint32_t bus,uint32_t off,void *dst,uint32_t n,uint32_t us) {
    if(!dst || !us)return OF_ERR_INVALID_PARAM;
    if(bus!=2 || n!=32 || (off&31u) || off>TOTAL53-32 || us!=50000)return OF_ERR_BAD_RANGE;
    if(!present53() || RD53(1)!=0x00100f02u)return OF_ERR_NOT_SUPPORTED;
    if((RD53(11)&0x80000307u)!=0x80000303u)return OF_ERR_INVALID_STATE;
    if(RD53(2)&2u)return OF_ERR_BUSY;
    WR53(16,0);WR53(4,2);WR53(5,off);WR53(6,32);WR53(7,4500000);WR53(3,2);
    unsigned wait=1000000;uint32_t st;
    do {st=RD53(2);if(!(st&2u))break;}while(--wait);
    if(!wait){WR53(3,3);return OF_ERR_TIMEOUT;}
    if(st&32u)return OF_ERR_TIMEOUT;
    if(st&16u)return OF_ERR_INVALID_STATE;
    if((st&8u) || !(st&4u) || RD53(9)!=32)return OF_ERR_IO;
    uint8_t *p=dst;
    for(unsigned i=0;i<8;++i){
        uint32_t w=RD53(8);
        for(unsigned j=0;j<4;++j)*p++=(uint8_t)(w>>(8*j));
    }
    return 0;
}
/* The original LTO image inlined the old read stub here. Updating only
 * cart_read cannot update cart_read_space: replace this bounded entry too. */
SECTION53(".space53") int space53(uint32_t bus,uint32_t space,uint32_t off,void *dst,uint32_t n,uint32_t us) {
    if(space!=OF_CART_SPACE_ROM)return OF_ERR_NOT_SUPPORTED;
    return read53(bus,off,dst,n,us);
}
SECTION53(".bind53") int bind53(const char *name,uint32_t size) {
    static const char expected[]="dump53.gba";
    if(!name)return size==0?MAGIC53:OF_ERR_INVALID_PARAM;
    if(size!=TOTAL53)return OF_ERR_INVALID_PARAM;
    for(unsigned i=0;i<sizeof(expected);++i)if(name[i]!=expected[i])return OF_ERR_INVALID_PARAM;
    int busy=of_file_async_busy();if(busy)return busy<0?busy:OF_ERR_BUSY;
    if(of_file_size(4)!=TOTAL53)return OF_ERR_BAD_RANGE;
    output_open_slot=4;output_open_size=TOTAL53;output_is_open=1;return 0;
}
