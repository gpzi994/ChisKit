#ifndef CB_GB_RTC186_HEADER_H
#define CB_GB_RTC186_HEADER_H
#include <stdint.h>
/* The shared GB ROM engine accepts only aligned 32-byte reads.
 * Read 96 bytes into owned storage; the RTC service consumes the first 80. */
static inline int cb_gbrtc186_header(uint8_t header[96],
 int (*read_block)(void *,uint32_t,void *,uint32_t),void *ctx){
 for(unsigned off=0;off<96;off+=32){
  int rc=read_block(ctx,0x100+off,header+off,32);
  if(rc)return rc;
 }
 return 0;
}
#endif
