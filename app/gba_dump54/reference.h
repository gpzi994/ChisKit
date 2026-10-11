#ifndef REFERENCE54_H
#define REFERENCE54_H
#include <stdint.h>
#define TOTAL54 8705280u
#define CHUNK54 32768u
#define BLOCKS54 ((TOTAL54+CHUNK54-1)/CHUNK54)
#define MAGIC54 0x53543534
#define NAME54 "dump54.gba"
#define MD5_TEXT54 "07E851B1DC49009EBD00982514EBFE92"
static const uint8_t expected54[16]={0x07,0xe8,0x51,0xb1,0xdc,0x49,0x00,0x9e,0xbd,0x00,0x98,0x25,0x14,0xeb,0xfe,0x92};
static const uint32_t sample_off54[5]={0,160,4194304,8388608,TOTAL54-32};
static const uint32_t sample_crc54[5]={0x07e38ac2,0x03b1960f,0xbce2266a,0xc4babcbc,0x190a55ad};
static unsigned length54(unsigned off){return TOTAL54-off<CHUNK54?TOTAL54-off:CHUNK54;}
static uint32_t crc54(const uint8_t *p,unsigned n){
    uint32_t c=~0u;while(n--){c^=*p++;for(unsigned b=0;b<8;++b)c=(c>>1)^(0xedb88320u&(0u-(c&1)));}return ~c;
}
#endif
