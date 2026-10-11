#ifndef CB_GB_RTC186_UI_H
#define CB_GB_RTC186_UI_H
#include "of_gbrtc186.h"
#include "gb_rtc186_header.h"
#include <stdio.h>
static inline int cb_gbrtc186_read_block(void *ctx,uint32_t address,void *out,uint32_t n){
 (void)ctx;return of_cart_read(OF_CART_BUS_GB_GBC,address,out,n,50000);
}
static inline void cb_gbrtc186_format(char *dst,size_t n,int rc,const of_gb186_report *r){
 if(rc||!r->supported){snprintf(dst,n,rc==-2?"RTC not confirmed":"RTC read failed %d",rc);return;}
 snprintf(dst,n,"%lud %02lu:%02lu:%02lu%s%s",(unsigned long)r->days,(unsigned long)r->hours,
  (unsigned long)r->minutes,(unsigned long)r->seconds,r->halt?" H":"",r->carry?" C":"");
}
#endif
