#ifndef PATTERN52_H
#define PATTERN52_H
#include <stdint.h>
#define TOTAL52 1048576u
#define CHUNK52 4096u
#define BLOCKS52 (TOTAL52/CHUNK52)
#define CRC_INITIAL52 0x502b18cdu
#define CRC_DATA52 0xcc3fa633u
enum pattern52 { UNKNOWN52=0, INITIAL52=0x3d, DATA52=0xa7 };
/* Every 4 KiB block has a version, tag and absolute block index/complement.
 * The rest depends on all absolute address bits, detecting offset aliasing. */
static uint8_t byte52(uint32_t i,unsigned tag) {
    const uint8_t magic[6]={'F','G','B','X','5','2'};
    unsigned p=i&(CHUNK52-1u);uint32_t block=i/CHUNK52;
    if(p<6)return magic[p];
    if(p==6)return (uint8_t)tag;
    if(p==7)return (uint8_t)~tag;
    if(p<12)return (uint8_t)(block>>(8u*(p-8u)));
    if(p<16)return (uint8_t)(~block>>(8u*(p-12u)));
    uint32_t x=i^(tag*0x01010101u)^0x5231a9cdu;
    x^=x>>16;x*=0x7feb352du;x^=x>>15;x*=0x846ca68bu;x^=x>>16;
    return (uint8_t)x;
}
static void fill52(uint8_t *p,uint32_t off,unsigned n,unsigned tag) {
    for(unsigned i=0;i<n;++i)p[i]=byte52(off+i,tag);
}
/* Incremental IEEE CRC32: start 0xffffffff, complement after last block. */
static uint32_t crc52(uint32_t c,const uint8_t *p,unsigned n) {
    for(unsigned i=0;i<n;++i) {
        c^=p[i];for(unsigned b=0;b<8;++b)c=(c>>1)^((c&1u)?0xedb88320u:0u);
    }
    return c;
}
#endif
