#define _GNU_SOURCE
#define main old_main175
#define alive mock_alive175
#include "test-gb-save175.c"
#undef main
#undef alive
#include <stdbool.h>
#include <sys/mman.h>
#include "../core/gb_save175/mbc7_restore185.h"
static struct {unsigned pins,bits,cmd,payload,pos,word,enabled,gate,write_enable,ops,fail,commits,corrupt,stuck,capacity,reads,ewen,ewds;uint8_t data[512];} ee;
static unsigned tick185,cancel185,remove185;
static uint8_t original185[256],source185[256];
static void init185(void){
 setup(0x22,0);memcpy(m.h+0x34,"KORO2 KIRBYKKKJ",15);m.h[0x48]=5;m.h[0x4d]=0;
 for(unsigned i=0x34;i<=0x4c;i++)m.h[0x4d]-=m.h[i]+1;
 memset(&ee,0,sizeof ee);ee.capacity=256;tick185=cancel185=remove185=0;
 for(unsigned i=0;i<256;i++){original185[i]=(uint8_t)(i*7);source185[i]=original185[i]^0x96;ee.data[i]=original185[i];}
}
static int live185(void *ctx){return mock_alive175(ctx);}
static int cancelled185(void *ctx){
 if(cancel185&&ee.ops>=cancel185)m.cancel=1;
 if(remove185&&ee.ops>=remove185)m.removed=1;
 return m.removed||cancelled(ctx);
}
static uint32_t now185(void *ctx){(void)ctx;return tick185++;}
static int control185(void *ctx,uint16_t a,uint8_t d){
 (void)ctx;assert(m.owned&&!m.removed);if(++ee.ops==ee.fail)return -77;
 if(a==0){assert(d==0||d==10);ee.enabled=d;return 0;}
 if(a==0x4000){assert(d==0||d==64);ee.gate=d;return 0;}
 assert(a==0xa080&&(d==0||d==0x80||d==0x82||d==0xc0||d==0xc2));
 if(d)assert(ee.enabled&&ee.gate);
 if(!(d&128)){
  if(ee.bits==11&&ee.cmd==0x4c0){ee.write_enable=1;ee.ewen++;}
  if(ee.bits==11&&ee.cmd==0x400){ee.write_enable=0;ee.ewds++;}
  if(ee.bits==27&&(ee.cmd>>8)==5){
   assert(ee.write_enable&&(ee.cmd&255)<128);unsigned off=(ee.cmd&127)*2;
   ee.data[off]=ee.payload>>8;ee.data[off+1]=ee.payload;ee.commits++;
   if(ee.commits==ee.corrupt)ee.data[off]^=1;
  }
  ee.bits=ee.cmd=ee.payload=ee.pos=0;
 }else if((d&64)&&!(ee.pins&64)){
  if(ee.bits<11){
   ee.cmd=(ee.cmd<<1)|((d>>1)&1);ee.bits++;
   if(ee.bits==11){
    assert((ee.cmd>>8)==6||(ee.cmd>=0x500&&ee.cmd<0x580)||ee.cmd==0x400||ee.cmd==0x4c0);
    if((ee.cmd>>8)==6){unsigned off=((ee.cmd&255)*2)%ee.capacity;ee.word=(ee.data[off]<<8)|ee.data[off+1];ee.reads++;}
   }
  }else if((ee.cmd>>8)==6){assert(!(d&2));ee.pos++;assert(ee.pos<=16);}
  else {assert((ee.cmd>>8)==5&&ee.bits<27);ee.payload=(ee.payload<<1)|((d>>1)&1);ee.bits++;}
 }
 ee.pins=d;return 0;
}
static int read185(void *ctx,uint16_t a,void *out,uint32_t n){
 (void)ctx;assert(m.owned&&!m.removed&&a==0xa080&&n==1&&ee.enabled&&ee.gate);
 if(++ee.ops==ee.fail)return -77;
 if(!ee.bits)*(uint8_t*)out=ee.stuck?0:1;
 else {assert((ee.cmd>>8)==6);*(uint8_t*)out=ee.pos?(ee.word>>(16-ee.pos))&1:0;}
 return 0;
}
static int run185(void){
 const struct gb185_bus b={{&m,acquire,live185,cancelled185,header,control185,read185,release},now185};
 return gb185_restore(&b,m.h,original185,source185,256,&report);
}
/* Production OS routing: MBC7 restore must use bus 11, never SRAM bus 10. */
#define CART_GB_RESTORE180 1
#define CART_GB_RESTORE185 1
enum {CB_NOR_ARGUMENT=-1,CB_NOR_UNSUPPORTED=-3};
static bool busy,private_owned;
static unsigned job_token,mapper_kind,CART_BUS,CART_CAPS=0x11fbf;
static struct {bool authorized;}job;
static void fence(void){}
static uint32_t now_ms(void *p){return now185(p);}
static int alive(void *p){(void)p;return private_owned&&!m.removed;}
static int enter(unsigned b){assert(b==1);int rc=acquire(&m);if(!rc)private_owned=true;return rc;}
static int leave(void){int rc=release(&m);private_owned=false;return rc;}
static int transaction(uint32_t a,uint16_t *v,bool wr){
 assert(CART_BUS==11);
 if(wr)return control185(&m,a,*v);
 if(a>=0x100&&a<0x150){*v=m.h[a-0x100];return 0;}
 uint8_t x=0;int rc=read185(&m,a,&x,1);*v=x;return rc;
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/gb_save175.h"
#include "../.integration/openfpgaos/src/firmware/os/kernel/gb_restore180.h"
int main(int argc,char **argv){
 init185();assert(!run185()&&report.compared==256&&report.written==256&&report.verified==256);
 assert(!memcmp(ee.data,source185,256)&&!m.owned&&!ee.pins&&!ee.enabled&&!ee.gate&&!ee.write_enable);
 unsigned ops=ee.ops;
 init185();memcpy(source185,original185,256);assert(!run185()&&!ee.ewen&&!ee.commits&&report.verified==256);
 for(unsigned fill=0;fill<2;fill++){init185();memset(ee.data,fill?255:0,256);memcpy(original185,ee.data,256);assert(!run185());}
 for(unsigned i=1;i<=128;i++){
  init185();ee.corrupt=i;assert(run185()==GB175_CHANGED&&!m.owned&&!ee.write_enable);
 }
 for(unsigned i=1;i<=ops;i+=137){init185();ee.fail=i;assert(run185()!=0&&!m.owned);}
 for(unsigned i=1;i<=4;i++){init185();m.change_header=i;assert(run185()==GB175_IDENTITY&&!m.owned);if(i<=2)assert(!ee.commits);}
 init185();ee.stuck=1;assert(run185()==GB175_IO&&ee.commits==1&&!m.owned&&!ee.write_enable);
 init185();ee.data[255]^=1;assert(run185()==GB175_CHANGED&&!ee.ewen&&!ee.commits);
 init185();ee.capacity=512;assert(run185()==GB175_UNSUPPORTED&&!ee.ewen&&!ee.commits);
 for(unsigned at=1;at<ops;at+=997){
  init185();cancel185=at;assert(run185()==GB175_CANCELLED&&!m.owned&&!ee.write_enable);
  init185();remove185=at;assert(run185()==GB175_CANCELLED&&!m.owned&&m.removed&&ee.ops>=at&&ee.ops<=at+2);
 }
 init185();m.release_error=1;assert(run185()==GB175_CLEANUP);
 if(argc==2){init185();FILE *f=fopen(argv[1],"rb");assert(f&&fread(source185,1,256,f)==256&&fgetc(f)==EOF);fclose(f);assert(!run185()&&!memcmp(ee.data,source185,256));}
 unsigned char *p=mmap((void*)0x11000000,4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);assert(p!=MAP_FAILED);
 uint8_t *h=p+1024;struct gb175_report *r=(void*)(p+1280);
 init185();memcpy(h,m.h,80);memcpy(p,original185,256);memcpy(p+256,source185,256);
 CART_CAPS&=~0x1000;assert(gb180_restore_service(h,p,p+256,256,r,sizeof *r)==GB175_IO&&!ee.ops&&!m.owned);CART_CAPS|=0x1000;
 assert(!gb180_restore_service(h,p,p+256,256,r,sizeof *r)&&r->verified==256&&!memcmp(ee.data,p+256,256));
 assert(!private_owned&&!busy&&!gb180_active&&!ee.write_enable);
 assert(gb180_restore_service(h,(void*)0x20000000,p+256,256,r,sizeof *r)==CB_NOR_ARGUMENT);
 assert(gb180_restore_service(h,p,p+1,256,r,sizeof *r)==CB_NOR_ARGUMENT);
 assert(gb180_restore_service(h,p,p+512,512,r,sizeof *r)==GB175_UNSUPPORTED);
 gb180_active=1;gb175_active_plan.mapper=GB179_MBC7;assert(gb180_write(NULL,0xa080,p,1)==CB_NOR_ARGUMENT);gb180_active=0;
 munmap(p,4096);
 puts("GB185 PASS: independent MBC7 wire model, EWEN/WRITE/EWDS, BE format, full readback, blank/no-op, mirror, timeout, fault/removal cleanup, OS bus11 capability/SDRAM and 512-byte exclusion");
 return 0;
}
