#ifndef CB_GB_RTC187_FOOTER_H
#define CB_GB_RTC187_FOOTER_H
#include <stdint.h>
#include <string.h>
/* FlashGBX DMG_MBC3.ReadRTC: five LE32 registers, duplicate, LE64 Unix time.
 * Independent of the GBA seven-byte calendar/status footer. */
static inline int cb_gbrtc187_footer(uint8_t out[48],const uint8_t raw[5],int64_t local_seconds){
 int64_t utc=local_seconds-480LL*60; /* User's Pocket wall clock: UTC+8. */
 if(raw[0]>59||raw[1]>59||raw[2]>23||(raw[4]&0x3e)||utc<946684800LL||utc>4102444799LL)return -1;
 memset(out,0,48);
 for(unsigned i=0;i<5;i++)out[4*i]=out[20+4*i]=raw[i];
 for(unsigned i=0;i<8;i++)out[40+i]=(uint8_t)((uint64_t)utc>>(8*i));
 return 0;
}
#endif
