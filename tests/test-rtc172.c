#include <assert.h>
#include <stdio.h>
#include "../core/rtc/restore172.h"
int main(void){
 uint8_t b[16]={0x24,0x02,0x28,3,0xA3,0x59,0x59,0x40};
 uint64_t stamp=1700000000;
 for(unsigned i=0;i<8;i++)b[8+i]=(uint8_t)(stamp>>(8*i));
 struct cb_rtc172_value v,original;
 for(unsigned n=65536;n<=131072;n*=2){
  assert(cb_rtc172_parse(NULL,n,n,&v)==0);
  assert(cb_rtc172_parse(b,n+15,n,&v)<0);
  assert(cb_rtc172_parse(b,n+16,n,&v)==1);
  original=v;assert(!cb_rtc172_advance(&v,stamp-1)&&!memcmp(&v,&original,sizeof v));
  assert(!cb_rtc172_advance(&v,stamp+1));
  assert(v.date[2]==0x29&&v.date[3]==4&&v.date[4]==0&&v.date[6]==0);
  b[7]=1;assert(cb_rtc172_parse(b,n+16,n,&v)==1&&v.status==0x40);b[7]=0x40;
  b[1]=0x13;assert(cb_rtc172_parse(b,n+16,n,&v)<0);b[1]=2;
  b[7]=0x80;assert(cb_rtc172_parse(b,n+16,n,&v)<0);b[7]=0x40;
 }
 assert(cb_rtc172_prompt(1,1)&&!cb_rtc172_prompt(0,1)&&!cb_rtc172_prompt(1,0)&&!cb_rtc172_prompt(1,-2));
 puts("RTC172 PASS: pure/tail size, BCD/status, legacy status, elapsed leap-day, backwards clock, prompt gates; no hardware writes");
}
