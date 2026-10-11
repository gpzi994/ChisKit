#define main rtc_read_regression
#include "test-rtc167.c"
#undef main
#include "../core/rtc/write172.h"
static int identity(void *ctx){return *(int*)ctx;}
int main(void){
 assert(!rtc_read_regression());
 struct mock m=init();m.allow_write=1;
 struct cb_rtc167_ops o={&m,rd,wr,live};of_rtc172_report r;
 uint8_t target[]={0x26,0x09,0x27,0,0x14,0x03,0x10,0x40};int id=0;
 assert(!cb_rtc172_write(&o,target,identity,&id,&r));
 assert(r.verified&&r.attempted&&m.register_writes==64&&!m.re);
 assert(m.date[4]==0x94&&rtc172_same(target,m.date));unsigned calls=m.calls;
 m=init();m.allow_write=1;m.absent=1;
 assert(cb_rtc172_write(&o,target,identity,&id,&r)&&!r.attempted&&!m.register_writes);
 m=init();m.allow_write=1;id=-13;
 assert(cb_rtc172_write(&o,target,identity,&id,&r)==-13&&!r.attempted);id=0;
 m=init();m.allow_write=1;target[7]=0x80;
 assert(cb_rtc172_write(&o,target,identity,&id,&r)&&!m.calls);target[7]=0x40;
 m=init();m.allow_write=1;m.corrupt=1;
 assert(cb_rtc172_write(&o,target,identity,&id,&r)&&r.attempted&&!r.verified&&!m.re);
 m=init();m.allow_write=1;m.corrupt=2;
 assert(!cb_rtc172_write(&o,target,identity,&id,&r)&&r.verified);
 for(unsigned i=1;i<=calls;i++){
  m=init();m.allow_write=1;m.fail=i;
  assert(cb_rtc172_write(&o,target,identity,&id,&r)&&!r.verified);
  m=init();m.allow_write=1;m.drop=i;
  assert(cb_rtc172_write(&o,target,identity,&id,&r)&&!r.verified&&m.calls==i);
 }
 puts("RTC172 protocol PASS: 64/62 commands, LSB payload, MSB commands, identity/support guards, readback mismatch, every bus fault/removal; mock hardware");
}
