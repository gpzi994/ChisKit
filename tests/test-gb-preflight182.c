#include <assert.h>
#include <stdio.h>
#include "../app/ui98/gb_preflight182.h"
static uint8_t h[96];static int safe_count,reads,fail_safe,fail_read;
static int safe(void *p){(void)p;return ++safe_count==fail_safe?-7:0;}
static int read_rom(void *p,uint32_t addr,void *out,unsigned n){
 (void)p;assert(n==32&&addr>=0x100&&addr<=0x140);
 if(++reads==fail_read)return -4;
 memcpy(out,h+addr-0x100,n);return 0;
}
static void init(unsigned type,unsigned ram){
 memset(h,0,sizeof h);memcpy(h+4,cb_gb_logo,48);memcpy(h+0x34,"TEST",4);
 h[0x47]=type;h[0x49]=ram;for(unsigned i=0x34;i<=0x4c;i++)h[0x4d]-=h[i]+1;
 safe_count=reads=fail_safe=fail_read=0;
}
int main(void){
 struct cb_cart expected;struct gb175_plan plan;struct gb182_trace t;
 const struct gb181_ops ops={0,read_rom,safe};
 unsigned types[]={0x13,3,0x1b,0x10};
 for(unsigned i=0;i<4;i++){
  init(types[i],i==3?5:3);assert(cb_parse_gb(&expected,h));
  assert(!gb182_preflight(&expected,&ops,&plan,&t));assert(t.stage==6&&t.blocks==3&&safe_count==2);
 }
 init(0x13,3);assert(cb_parse_gb(&expected,h));fail_safe=1;
 assert(gb182_preflight(&expected,&ops,&plan,&t)==-7&&t.stage==1&&!reads&&safe_count==2);
 for(unsigned i=1;i<=3;i++){
  init(0x13,3);fail_read=i;assert(gb182_preflight(&expected,&ops,&plan,&t)==-4);
  assert(t.stage==2&&t.blocks==i-1&&safe_count==2);
 }
 init(0x13,3);fail_safe=2;assert(gb182_preflight(&expected,&ops,&plan,&t)==-7&&t.stage==3);
 init(0x13,3);h[4]^=1;assert(gb182_preflight(&expected,&ops,&plan,&t)==-3&&t.stage==4);
 init(3,3);assert(gb182_preflight(&expected,&ops,&plan,&t)==-3&&t.stage==5&&(t.mismatch&2));
 init(0x20,4);assert(cb_parse_gb(&expected,h));
 assert(gb182_preflight(&expected,&ops,&plan,&t)==-2&&t.stage==6);
 puts("PASS GB182 actual preflight: MBC1/3/30/5, each read, entry/exit safe, bad header, changed identity, unsupported planner; no retry or relaxed guards");
}
