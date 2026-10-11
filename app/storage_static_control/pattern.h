#ifndef STATIC50_PATTERN_H
#define STATIC50_PATTERN_H
#include <stdint.h>
#define STATIC50_BYTES 512u
enum static50_pattern { P_UNKNOWN=0, P_SEED=0x33, P_A=0xa5, P_B=0x5a };
static uint8_t pattern_byte(unsigned i, unsigned tag) {
    static const uint8_t magic[6]={'F','G','B','X','5','0'};
    if (i<6) return magic[i];
    if (i==6) return (uint8_t)tag;
    if (i==7) return (uint8_t)~tag;
    return (uint8_t)((i*73u+(i>>3)*19u) ^ (i>>8) ^ tag);
}
static void pattern_fill(uint8_t *p,unsigned tag) {
    for (unsigned i=0;i<STATIC50_BYTES;++i) p[i]=pattern_byte(i,tag);
}
static unsigned pattern_diff(const uint8_t *p,unsigned tag,unsigned *first) {
    unsigned count=0; *first=STATIC50_BYTES;
    for(unsigned i=0;i<STATIC50_BYTES;++i) if(p[i]!=pattern_byte(i,tag)) {
        if(!count) *first=i;
        ++count;
    }
    return count;
}
static unsigned pattern_identify(const uint8_t *p) {
    unsigned first;
    if(!pattern_diff(p,P_SEED,&first)) return P_SEED;
    if(!pattern_diff(p,P_A,&first)) return P_A;
    if(!pattern_diff(p,P_B,&first)) return P_B;
    return P_UNKNOWN;
}
#endif
