#ifndef CB_RTC169_FOOTER_H
#define CB_RTC169_FOOTER_H
#include <stdint.h>
#include <string.h>
#include "rtc168_policy.h"
/* Pocket CLOCK_REALTIME is local wall time in the current OS. */
static inline int cb_rtc169_footer(uint8_t out[16],const of_rtc167_report *r,int64_t local_seconds,int offset_minutes){
 if(!cb_rtc168_available(0,r)||offset_minutes < -720||offset_minutes>840)return -1;
 int64_t utc=local_seconds-(int64_t)offset_minutes*60;
 if(utc<946684800LL||utc>4102444799LL)return -1;
 memcpy(out,r->date,7);out[7]=r->status;
 for(unsigned i=0;i<8;i++)out[8+i]=(uint8_t)((uint64_t)utc>>(8*i));
 return 0;
}
#endif
