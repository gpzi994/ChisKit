#ifndef PATTERN51_H
#define PATTERN51_H
#include <stdint.h>
#define BYTES51 512u
enum pattern51 { UNKNOWN51=0, INITIAL51=0x3c, A51=0xa6, B51=0x59 };
static uint8_t byte51(unsigned i,unsigned tag) {
    static const uint8_t prefix[6]={'F','G','B','X','5','1'};
    if(i<6)return prefix[i];
    if(i==6)return (uint8_t)tag;
    if(i==7)return (uint8_t)~tag;
    return (uint8_t)((i*73u+(i>>3)*19u)^(i>>8)^tag^0x51u);
}
static void fill51(uint8_t *p,unsigned tag) {
    for(unsigned i=0;i<BYTES51;++i)p[i]=byte51(i,tag);
}
static unsigned diff51(const uint8_t *p,unsigned tag,unsigned *first) {
    unsigned n=0;*first=BYTES51;
    for(unsigned i=0;i<BYTES51;++i)if(p[i]!=byte51(i,tag)) {
        if(!n)*first=i;
        ++n;
    }
    return n;
}
static unsigned identify51(const uint8_t *p) {
    unsigned first;
    if(!diff51(p,INITIAL51,&first))return INITIAL51;
    if(!diff51(p,A51,&first))return A51;
    if(!diff51(p,B51,&first))return B51;
    return UNKNOWN51;
}
static uint32_t crc51(const uint8_t *p,unsigned n) {
    uint32_t c=0xffffffffu;
    for(unsigned i=0;i<n;++i) {
        c^=p[i];
        for(unsigned b=0;b<8;++b)c=(c>>1)^((c&1u)?0xedb88320u:0);
    }
    return ~c;
}
#endif
