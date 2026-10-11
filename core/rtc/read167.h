/* Read-only S-3511 protocol, following FlashGBX Mapper.AGB_GPIO.
 * Only 63/65 serial commands; GPIO writes are not RTC register writes.
 * Addresses here are GBA BYTE offsets, not FlashGBX reader word addresses. */
#ifndef CB_RTC167_READ_H
#define CB_RTC167_READ_H
#include <string.h>
#include "of_rtc167_data.h"
struct cb_rtc167_ops {
 void *ctx;
 int (*read)(void*,uint32_t,uint16_t*);
 int (*write)(void*,uint32_t,uint16_t);
 int (*live)(void*);
};
static int rtc167_wr(const struct cb_rtc167_ops *o,uint32_t a,uint16_t v){
 return o->live(o->ctx)?o->write(o->ctx,a,v):-4;
}
static int rtc167_rd(const struct cb_rtc167_ops *o,uint32_t a,uint16_t *v){
 return o->live(o->ctx)?o->read(o->ctx,a,v):-4;
}
static int rtc167_end(const struct cb_rtc167_ops *o){
 int first=0,r;const uint32_t a[]={0xc4,0xc4,0xc8};const uint16_t v[]={1,1,0};
 for(unsigned i=0;i<3;i++){r=rtc167_wr(o,a[i],v[i]);if(r&&!first)first=r;}
 return first;
}
static int rtc167_command(const struct cb_rtc167_ops *o,unsigned cmd,uint8_t *out,unsigned n){
 if(!((cmd==0x63&&n==1)||(cmd==0x65&&n==7)))return -1;
 int rc=rtc167_wr(o,0xc8,1);
 if(!rc)rc=rtc167_wr(o,0xc4,1);
 if(!rc)rc=rtc167_wr(o,0xc4,5);
 if(!rc)rc=rtc167_wr(o,0xc6,7);
 for(unsigned i=0;i<8&&!rc;i++){
  unsigned v=4|(((cmd>>(7-i))&1)<<1);
  for(unsigned j=0;j<3&&!rc;j++)rc=rtc167_wr(o,0xc4,v);
  if(!rc)rc=rtc167_wr(o,0xc4,v|1);
 }
 if(!rc)rc=rtc167_wr(o,0xc6,5);
 for(unsigned i=0;i<n&&!rc;i++){
  out[i]=0;
  for(unsigned bit=0;bit<8&&!rc;bit++){
   for(unsigned j=0;j<5&&!rc;j++)rc=rtc167_wr(o,0xc4,4);
   if(!rc)rc=rtc167_wr(o,0xc4,5);
   uint16_t v=0;if(!rc)rc=rtc167_rd(o,0xc4,&v);
   if(!rc)out[i]|=((v>>1)&1)<<bit;
  }
 }
 return rc;
}
static int rtc167_bcd(unsigned b){return (b&15)>9||(b>>4)>9?-1:(int)((b>>4)*10+(b&15));}
static int rtc167_valid(const of_rtc167_report *r){
 /* First release reports 12h raw rather than guessing clone PM encoding. */
 if(!(r->status&0x40)||(r->status&0x95))return 0;
 int y=rtc167_bcd(r->date[0]),m=rtc167_bcd(r->date[1]),d=rtc167_bcd(r->date[2]);
 int h=rtc167_bcd(r->date[4]&0x3f),mi=rtc167_bcd(r->date[5]),s=rtc167_bcd(r->date[6]);
 static const unsigned days[]={31,28,31,30,31,30,31,31,30,31,30,31};
 if(y<0||m<1||m>12||d<1||r->date[3]>6||h<0||h>23||mi<0||mi>59||s<0||s>59)return 0;
 return (unsigned)d<=days[m-1]+(m==2&&y%4==0);
}
static int cb_rtc167_capture(const struct cb_rtc167_ops *o,of_rtc167_report *r){
 memset(r,0,sizeof(*r));r->version=167;r->stage=1;
 int rc=rtc167_command(o,0x63,&r->status,1),have_rom=0;uint16_t verify[3];
 /* FlashGBX reads status first, then checks for a GPIO mapping change. */
 {int clean=rtc167_end(o);if(!rc)rc=clean;}
 if(!rc)rc=rtc167_wr(o,0xc8,0);
 for(unsigned i=0;i<3&&!rc;i++)rc=rtc167_rd(o,0xc4+2*i,&r->rom[i]);
 for(unsigned i=0;i<3&&!rc;i++)rc=rtc167_rd(o,0xc4+2*i,&verify[i]);
 if(!rc&&memcmp(verify,r->rom,sizeof verify))rc=-3;
 if(!rc)have_rom=1;
 r->stage=2;
 if(!rc)rc=rtc167_wr(o,0xc8,1);
 for(unsigned i=0;i<3&&!rc;i++)rc=rtc167_rd(o,0xc4+2*i,&r->mapped[i]);
 if(!rc&&memcmp(r->rom,r->mapped,sizeof r->rom))r->flags|=OF_RTC167_GPIO;
 if(!rc&&(r->flags&OF_RTC167_GPIO)){
  if(r->status&0x80)r->flags|=OF_RTC167_POWER_FLAG;
  else {
   r->stage=3;rc=rtc167_command(o,0x65,r->date,7);
   if(!rc&&rtc167_valid(r))r->flags|=OF_RTC167_TIME_OK;
  }
 }
 r->cleanup_rc=rtc167_end(o);
 if(have_rom&&!r->cleanup_rc){
  for(unsigned i=0;i<3&&!r->cleanup_rc;i++)r->cleanup_rc=rtc167_rd(o,0xc4+2*i,&verify[i]);
  if(!r->cleanup_rc&&memcmp(verify,r->rom,sizeof verify))r->cleanup_rc=-3;
 }
 r->rc=rc?rc:r->cleanup_rc;
 if(r->rc)r->flags&=~OF_RTC167_TIME_OK;
 return r->rc;
}
#endif
