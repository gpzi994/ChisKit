#ifndef REFERENCE53_H
#define REFERENCE53_H
#include <stdint.h>
#define TOTAL53 8705280u
#define CHUNK53 4096u
#define BLOCKS53 ((TOTAL53+CHUNK53-1)/CHUNK53)
#define MAGIC53 0x53543533
#define NAME53 "dump53.gba"
#define MD5_TEXT53 "07E851B1DC49009EBD00982514EBFE92"
static const uint8_t expected53[16]={0x07,0xe8,0x51,0xb1,0xdc,0x49,0x00,0x9e,0xbd,0x00,0x98,0x25,0x14,0xeb,0xfe,0x92};
static const uint32_t sample_off53[5]={0,160,4194304,8388608,TOTAL53-32};
static const uint32_t sample_crc53[5]={0x07e38ac2,0x03b1960f,0xbce2266a,0xc4babcbc,0x190a55ad};
static unsigned length53(unsigned off){return TOTAL53-off<CHUNK53?TOTAL53-off:CHUNK53;}
static uint32_t crc53(const uint8_t *p,unsigned n){
    uint32_t c=~0u;while(n--){c^=*p++;for(unsigned b=0;b<8;++b)c=(c>>1)^(0xedb88320u&(0u-(c&1)));}return ~c;
}
#endif
