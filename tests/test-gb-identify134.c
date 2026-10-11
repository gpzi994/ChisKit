#include "analysis.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned seq,reads,safes,probes;
static int fail_safe,corrupt,available=1;
static uint8_t sample[352];
static int safe(void*x){(void)x;seq=0;return ++safes==(unsigned)fail_safe?-7:0;}
static int read_sample(void*x,unsigned bus,unsigned space,uint32_t off,void*p,unsigned n){
 static const unsigned addr[]={0,32,0x4000,0x4020,0,32,0x4000,0x4020,0x100,0x120,0x140};
 (void)x;assert(seq<11&&bus==1&&space==4&&n==32&&off==0x10000+addr[seq]);
 memcpy(p,sample+32*seq,32);if(corrupt&&seq==4)((uint8_t*)p)[0]^=1;
 seq++;reads++;return 0;
}
static int caps(void*x,of_cart_caps_t*p){(void)x;(void)p;assert(0);return -1;}
static int flash(void*x,of_cart_flash_caps_t*p){(void)x;(void)p;assert(0);return -1;}
static int avail(void){return available;}
static int probe(uint32_t bus,of_nor_info_t*n){assert(bus==1);probes++;memset(n,0,sizeof(*n));
 n->version=OF_NOR_VERSION;n->struct_size=sizeof(*n);n->bus=1;n->flash_bytes=0x800000;
 n->flags=OF_NOR_ID_OK|OF_NOR_WRITE_OK;return 0;}
#define of_nor_available avail
#define of_nor_probe probe
#include "../app/ui98/nor108_analysis.h"
static struct cb_analysis run(unsigned type,int alias){
 struct cb_cart c={0};struct cb_analysis a;struct cb_analysis_ops ops={0,read_sample,safe,flash,caps};
 memset(sample,0,sizeof(sample));for(unsigned i=0;i<64;i++){sample[i]=i+3;sample[i+64]=(i+3)^(alias?0:0x55);}
 memcpy(sample+128,sample,128);uint8_t*h=sample+256;memcpy(h+4,cb_gb_logo,48);
 memcpy(h+0x34,"TEST",4);h[0x47]=type;h[0x48]=6;h[0x49]=type==0x10?5:3;
 for(unsigned i=0x34;i<=0x4c;i++)h[0x4d]-=h[i]+1;
 assert(cb_parse_gb(&c,h));c.result=CB_READY;seq=reads=safes=probes=0;
 cb_analyze(&a,&c,&ops);analyze_nor108(&a,&c);return a;
}
int main(void){
 struct cb_analysis a;
 for(unsigned i=0;i<3;i++){a=run(0x1b,1);assert(!a.rc&&reads==11&&probes==1&&a.mapper_ok);}
 a=run(0x10,1);assert(!a.rc&&reads==11&&probes==1&&a.mapper_warning);
 corrupt=1;a=run(0x10,1);assert(a.rc&&!probes&&!a.writable);corrupt=0;
 fail_safe=1;a=run(0x1b,1);assert(a.rc&&!reads&&!probes);fail_safe=2;
 a=run(0x1b,1);assert(a.rc&&!probes);fail_safe=0;
 available=0;a=run(0x10,1);assert(a.rc&&!probes&&!a.writable);available=1;
 a=run(3,0);assert(!a.rc&&!reads&&!probes&&!a.writable);
 puts("PASS GB134: preserved 11 samples, repeat mismatch, safe failures, missing OS, retail gate, repeated analysis; no engine1 query");
}
