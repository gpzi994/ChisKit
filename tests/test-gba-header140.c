#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
struct cb_probe_ops {void *ctx;int(*read)(void*,unsigned,uint32_t,void*,unsigned);int(*safe)(void*);void(*progress)(void*,const char*);};
#include "../app/ui98/gba_header140.h"
static unsigned calls,safes,fail;
static int rd(void*x,unsigned bus,uint32_t a,void*p,unsigned n){
 (void)x;assert(bus==2&&n==32&&a<192);calls++;
 if(calls==fail)return -9;
 for(unsigned i=0;i<n;i++)((uint8_t*)p)[i]=(uint8_t)(a+i);
 return 0;
}
static int safe(void*x){(void)x;safes++;return 0;}
int main(void){struct cb_probe_ops o={0,rd,safe,0};struct cb_gba_header140 r;
 cb_gba_header140_read(&r,&o);assert(calls==28&&safes==2);
 for(unsigned p=0;p<4;p++){assert(!r.rc[p]&&r.blocks[p]==6);for(unsigned i=0;i<192;i++)assert(r.data[p][i]==i);}
 for(fail=1;fail<=28;fail++){
  calls=safes=0;cb_gba_header140_read(&r,&o);assert(calls==fail&&safes==2);
  assert(r.rc[(fail-1)/7]==-9);
  for(unsigned p=(fail-1)/7+1;p<4;p++)assert(r.rc[p]==-100&&!r.blocks[p]);
 }
 puts("PASS GBA header diagnostic: 4 complete snapshots, GBA-only reads, all 28 read faults, cleanup and incomplete guards");
}
