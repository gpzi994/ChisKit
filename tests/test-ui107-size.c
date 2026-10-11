#include "dump_plan.h"
#include <assert.h>
#include <stdio.h>
#include "size107.h"
static unsigned mirror, calls, fail_at, unstable_at, fake_header;
static unsigned byte_at(uint32_t a){
 if(mirror)a%=mirror;
 if(!mirror&&a>=0x800000)return 255; /* 8 MiB software on 32 MiB NOR */
 return ((a*13u)^(a>>8)^(a>>16)^(a>>24)^0x5a)&255;
}
static int read_cart(void *ctx,unsigned bus,uint32_t a,void *out,unsigned n){
 (void)ctx;assert(bus==2&&n==32&&a+n<=0x2000000);calls++;
 if(calls==fail_at)return -9;
 if(fake_header&&a>=65536&&(a&65535)==0xa0)a=0xa0;
 for(unsigned i=0;i<n;i++)((uint8_t*)out)[i]=(uint8_t)byte_at(a+i);
 if(calls==unstable_at)((uint8_t*)out)[0]^=1;
 return 0;
}
static void check(unsigned m,unsigned expected){
 mirror=m;calls=0;uint32_t size=0;struct cb_probe_ops o={0,read_cart,0,0};
 assert(!cb_gba_size107(&o,&size)&&size==expected);
}
int main(void){
 check(65536,65536);check(0x800000,0x800000);check(0,0x2000000);
 fake_header=1;check(0,0x2000000);fake_header=0;
 struct cb_probe_ops o={0,read_cart,0,0};uint32_t size=0;
 fail_at=1;calls=0;assert(cb_gba_size107(&o,&size)==-9);fail_at=0;
 unstable_at=3;calls=0;assert(cb_gba_size107(&o,&size)==-13);
 unstable_at=11;calls=0;assert(!cb_gba_size107(&o,&size)&&size==0x2000000);
 fake_header=1;unstable_at=11;calls=0;assert(cb_gba_size107(&o,&size)==-13);
 fake_header=0;unstable_at=0;
 const unsigned types[]={0,1,5,0x10,0x10,0x1b,0x22,0xfc};
 const unsigned limits[]={32768,2097152,262144,2097152,4194304,8388608,4194304,1048576};
 for(unsigned k=0;k<8;k++){
  struct cb_cart c={.platform=CB_GB,.result=CB_READY,.type=types[k],.ram_bytes=k==4?65536:32768,.rom_bytes=limits[k]};
  assert(cb_dump_mode(&c)==(int)k);c.rom_bytes+=16384;assert(cb_dump_mode(&c)==-1);
  c.rom_bytes=0;assert(cb_dump_mode(&c)==-1);c.rom_bytes=32769;assert(cb_dump_mode(&c)==-1);
 }
 struct cb_cart c={.platform=CB_GB,.result=CB_READY,.type=1,.rom_bytes=1048576};
 strcpy(c.title,"MOMOCOL");assert(cb_dump_mode(&c)==-1);c.type=0x0b;assert(cb_dump_mode(&c)==-1);
 puts("PASS size policy: GBA mirrors, padded NOR, coincidental header, unstable reads; eight mapper bounds and multicart guard");
}
