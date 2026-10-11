#ifndef CB_RTC168_POLICY_H
#define CB_RTC168_POLICY_H
#include "of_rtc167_data.h"
#include <stdio.h>
static inline int cb_rtc168_available(int rc,const of_rtc167_report *r){
 return !rc&&!r->rc&&!r->cleanup_rc&&r->version==167&&
  (r->flags&(OF_RTC167_GPIO|OF_RTC167_TIME_OK|OF_RTC167_POWER_FLAG))==(OF_RTC167_GPIO|OF_RTC167_TIME_OK);
}
static inline int cb_rtc168_text(char *out,unsigned size,const char *title,const char *code,const of_rtc167_report *r){
 if(!cb_rtc168_available(0,r))return -1;
 int n=snprintf(out,size,"CHISBURNER_RTC_SIDECAR_V1\nROM=%.16s\nCODE=%.15s\n"
  "TIME=20%02X-%02X-%02X %02X:%02X:%02X\n"
  "RAW_DATE=%02X %02X %02X %02X %02X %02X %02X\nSTATUS=%02X\n"
  "Snapshot read after SAV backup. No host timestamp or elapsed-time adjustment.\nEND RTC\n",
  title,code,r->date[0],r->date[1],r->date[2],r->date[4]&0x3f,r->date[5],r->date[6],
  r->date[0],r->date[1],r->date[2],r->date[3],r->date[4],r->date[5],r->date[6],r->status);
 return n<0||(unsigned)n>=size?-1:n;
}
#endif
