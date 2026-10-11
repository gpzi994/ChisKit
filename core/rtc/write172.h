#ifndef CB_RTC_WRITE172_H
#define CB_RTC_WRITE172_H
#include "read167.h"
#include "restore172.h"
#include "of_rtc172_data.h"
/* FlashGBX AGB_GPIO: command MSB first; register payload LSB first.
 * No reset command, save-memory address or generic GPIO API is exposed. */
static int rtc172_send(const struct cb_rtc167_ops *o,unsigned cmd,const uint8_t *data,unsigned n){
 if(!((cmd==0x64&&n==7)||(cmd==0x62&&n==1)))return -1;
 int rc=rtc167_wr(o,0xc8,1);
 if(!rc)rc=rtc167_wr(o,0xc4,1);
 if(!rc)rc=rtc167_wr(o,0xc4,5);
 if(!rc)rc=rtc167_wr(o,0xc6,7);
 for(unsigned i=0;i<8&&!rc;i++){
  unsigned v=4|(((cmd>>(7-i))&1)<<1);
  for(unsigned k=0;k<3&&!rc;k++)rc=rtc167_wr(o,0xc4,v);
  if(!rc)rc=rtc167_wr(o,0xc4,v|1);
 }
 for(unsigned i=0;i<n&&!rc;i++)for(unsigned bit=0;bit<8&&!rc;bit++){
  unsigned v=4|(((data[i]>>bit)&1)<<1);
  for(unsigned k=0;k<3&&!rc;k++)rc=rtc167_wr(o,0xc4,v);
  if(!rc)rc=rtc167_wr(o,0xc4,v|1);
 }
 return rc;
}
static int rtc172_same(const uint8_t a[7],const uint8_t b[7]){
 for(unsigned i=0;i<7;i++)if((a[i]&(i==4?0x7f:0xff))!=(b[i]&(i==4?0x7f:0xff)))return 0;
 return 1;
}
static int cb_rtc172_write(const struct cb_rtc167_ops *o,const uint8_t target[8],
 int (*identity)(void *),void *identity_ctx,of_rtc172_report *r){
 memset(r,0,sizeof *r);r->version=172;r->stage=1;
 uint8_t footer[16]={0};memcpy(footer,target,8);
 struct cb_rtc172_value value;
 /* First writer supports the already validated 24-hour normal mode only. */
 if(cb_rtc172_parse(footer,65552,65536,&value)!=1||value.status!=0x40||!identity){r->rc=-1;return r->rc;}
 r->stage=2;int rc=cb_rtc167_capture(o,&r->before);
 if(!rc&&(!(r->before.flags&OF_RTC167_TIME_OK)||r->before.status!=0x40))rc=-2;
 if(!rc)rc=identity(identity_ctx);
 if(rc){r->cleanup_rc=r->before.cleanup_rc;r->rc=rc;return rc;}
 uint8_t date[7];memcpy(date,value.date,7);
 unsigned hour=(unsigned)cb_rtc172_bcd(date[4]&0x7f);
 date[4]=cb_rtc172_encode(hour)|(hour>=12?0x80:0);
 r->stage=3;r->attempted=1; /* May be partially written on any bus failure. */
 rc=rtc172_send(o,0x64,date,7);
 int clean=rtc167_end(o);if(clean)r->cleanup_rc=clean;if(!rc)rc=clean;
 if(!rc){
  r->stage=4;rc=rtc172_send(o,0x62,&value.status,1);
  clean=rtc167_end(o);if(clean)r->cleanup_rc=clean;if(!rc)rc=clean;
 }
 if(!rc){
  r->stage=5;rc=cb_rtc167_capture(o,&r->after);
  if(r->after.cleanup_rc)r->cleanup_rc=r->after.cleanup_rc;
  if(!rc&&(!(r->after.flags&OF_RTC167_TIME_OK)||r->after.status!=value.status||
     memcmp(r->before.rom,r->after.rom,sizeof r->before.rom)))rc=-3;
  if(!rc&&!rtc172_same(value.date,r->after.date)){
   /* A second tick during readback is legitimate; larger drift is not. */
   value.timestamp=946684800;
   if(cb_rtc172_advance(&value,946684801)||!rtc172_same(value.date,r->after.date))rc=-3;
  }
 }
 r->rc=rc;if(!rc&&!r->cleanup_rc){r->verified=1;r->stage=6;}
 return r->rc;
}
#endif
