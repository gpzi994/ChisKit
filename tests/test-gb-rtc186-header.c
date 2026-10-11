#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../app/ui98/gb_rtc186_header.h"
static unsigned calls,fail;
static int read_block(void *ctx,uint32_t a,void *p,uint32_t n){
 (void)ctx;assert(n==32);assert(a==0x100+32*calls);assert(!(a&31));
 calls++;if(calls==fail)return -13;
 memset(p,(int)calls,n);return 0;
}
int main(void){
 struct {uint8_t before,h[96],after;} b;
 for(fail=0;fail<=3;fail++){
  memset(&b,0xa5,sizeof b);calls=0;
  assert(cb_gbrtc186_header(b.h,read_block,NULL)==(fail?-13:0));
  assert(calls==(fail?fail:3));assert(b.before==0xa5&&b.after==0xa5);
  if(!fail)for(unsigned i=0;i<96;i++)assert(b.h[i]==1+i/32);
 }
 puts("GB RTC header: aligned 32-byte reads, 96-byte bounds and early errors PASS");
}
