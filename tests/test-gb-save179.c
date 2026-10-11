#define _GNU_SOURCE
#define main old_main175
#define alive mock_alive175
#include "test-gb-save175.c"
#undef main
#undef alive
#include <stdbool.h>
#include <sys/mman.h>
static struct {unsigned enabled,gate,pins,bits,command,pos,word,commands,operations,fail,flip,capacity,floating;uint8_t data[512];} ee;
static unsigned stop_at179,remove179;
static int live179(void *v){return mock_alive175(v);}
static int cancel179(void *v){
 if(stop_at179&&ee.operations>=stop_at179){if(remove179)m.removed=1;else m.cancel=1;return 1;}
 return cancelled(v);
}
static void header_sum(void){m.h[0x4d]=0;for(unsigned i=0x34;i<=0x4c;i++)m.h[0x4d]-=m.h[i]+1;}
static void init179(void){
 setup(0x22,0);memcpy(m.h+0x34,"KORO2 KIRBYKKKJ",15);m.h[0x48]=5;header_sum();
 memset(&ee,0,sizeof ee);ee.capacity=256;
 stop_at179=remove179=0;
 for(unsigned i=0;i<512;i++)ee.data[i]=(uint8_t)(i*7+(i%256)/7);
}
static int control179(void *v,uint16_t a,uint8_t d){
 (void)v;assert(m.owned&&!m.removed);ee.operations++;
 if(ee.operations==ee.fail)return -77;
 if(a==0){assert(d==0||d==10);ee.enabled=d;return 0;}
 if(a==0x4000){assert(d==0||d==64);ee.gate=d;return 0;}
 assert(a==0xa080&&(d==0||(ee.enabled&&ee.gate))&&(d==0||d==0x80||d==0x82||d==0xc0||d==0xc2));
 if(!(d&128)){ee.bits=ee.command=ee.pos=0;}
 else if((d&64)&&!(ee.pins&64)){
  if(ee.bits<11){
   ee.command=(ee.command<<1)|((d>>1)&1);ee.bits++;
   if(ee.bits==11){assert((ee.command>>8)==6);ee.commands++;
    unsigned off=((ee.command&255)*2)%ee.capacity;
    ee.word=(ee.data[off]<<8)|ee.data[off+1];
    if(ee.flip&&ee.commands==257)ee.word^=1;
   }
  }else{assert(!(d&2));ee.pos++;assert(ee.pos<=16);}
 }
 ee.pins=d;return 0;
}
static int read179(void *v,uint16_t a,void *p,uint32_t n){
 (void)v;assert(m.owned&&a==0xa080&&n==1&&ee.enabled&&ee.gate&&ee.bits==11);
 ee.operations++;if(ee.operations==ee.fail)return -77;
 *(uint8_t*)p=ee.floating?1:ee.pos?(uint8_t)((ee.word>>(16-ee.pos))&1):0;
 return 0;
}
static int capture179(void){
 struct gb175_bus b={&m,acquire,live179,cancel179,header,control179,read179,release};
 return gb179_capture(&b,1,m.h,0,snapshot,sizeof snapshot,scratch,512,&report);
}
/* Exercise the actual OS wrapper using the same EEPROM wire model. */
enum {CB_NOR_ARGUMENT=-1,CB_NOR_UNSUPPORTED=-3};
static bool busy,private_owned;
static int alive(void *v){(void)v;return private_owned&&!m.removed;}
static unsigned job_token,mapper_kind,CART_BUS,CART_CAPS=0x107bf;
static struct {bool authorized;}job;
static void fence(void){}
static uint32_t ticks179;
static uint32_t now_ms(void *v){(void)v;return ticks179++;}
static int enter(unsigned b){assert(b==1);int rc=acquire(&m);if(!rc)private_owned=true;return rc;}
static int leave(void){int rc=release(&m);private_owned=false;return rc;}
static int transaction(uint32_t a,uint16_t *v,bool wr){
 assert(CART_BUS==9);
 if(wr)return control179(&m,(uint16_t)a,(uint8_t)*v);
 if(a>=0x100&&a<0x150){*v=m.h[a-0x100];return 0;}
 uint8_t value=0;int rc=read179(&m,(uint16_t)a,&value,1);*v=value;return rc;
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/gb_save175.h"
int main(void){
 struct gb175_plan p;
 init179();assert(!gb179_plan(1,m.h,0,&p)&&p.bytes==256&&p.mapper==7);
 assert(gb179_plan(2,m.h,0,&p)==GB175_UNSUPPORTED);
 assert(gb179_plan(1,m.h,512,&p)==GB175_UNSUPPORTED);
 assert(gb178_plan(1,m.h,0,&p)==GB175_UNSUPPORTED);
 m.h[4]^=1;assert(gb179_plan(1,m.h,0,&p)==GB175_IDENTITY);
 init179();memset(m.h+0x34,0,15);memcpy(m.h+0x34,"CMASTER_KCEJ",12);header_sum();
 assert(capture179()==GB175_UNSUPPORTED&&!m.acquires);
 init179();assert(!capture179()&&report.compared==256&&ee.commands==512&&!ee.pins&&!ee.enabled&&!ee.gate&&!m.owned);
 assert(!memcmp(snapshot,ee.data,256));
 init179();ee.data[0]=0x12;ee.data[1]=0x34;
 assert(!capture179()&&snapshot[0]==0x12&&snapshot[1]==0x34);
 for(unsigned fill=0;fill<2;fill++){init179();memset(ee.data,fill?255:0,512);assert(!capture179());}
 init179();ee.capacity=512;ee.data[256]^=1;assert(capture179()==GB175_UNSUPPORTED&&!report.compared&&!m.owned);
 init179();ee.flip=1;assert(capture179()==GB175_CHANGED&&!m.owned);
 init179();ee.floating=1;assert(capture179()==GB175_IO&&!m.owned&&!ee.enabled&&!ee.gate);
 for(unsigned i=1;i<=3;i++){init179();m.change_header=i;assert(capture179()==GB175_IDENTITY&&!m.owned);}
 const unsigned failures[]={1,2,3,4,10,20,40,60,70,100,1000,10000,30000};
 for(unsigned i=0;i<sizeof failures/sizeof *failures;i++){
  init179();ee.fail=failures[i];assert(capture179()!=0&&!m.owned);
 }
 for(unsigned removed=0;removed<2;removed++)for(unsigned at=1;at<100;at++){
  init179();stop_at179=at;remove179=removed;
  assert(capture179()==GB175_CANCELLED&&!m.owned);
  if(removed)assert(ee.operations==at);else assert(!ee.enabled&&!ee.gate&&!ee.pins);
 }
 unsigned char *a=mmap((void*)0x11000000,4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);assert(a!=MAP_FAILED);
 uint8_t *h=a+512;struct gb175_report *r=(void*)(a+1024);
 init179();memcpy(h,m.h,80);assert(!gb179_capture_service(h,0,a,256,r,sizeof *r)&&r->compared==256&&!memcmp(a,ee.data,256));
 assert(ticks179>=512*15);
 init179();CART_CAPS&=~0x400;assert(gb179_capture_service(h,0,a,256,r,sizeof *r)==GB175_IO&&!ee.operations&&!m.owned);CART_CAPS|=0x400;
 assert(gb179_capture_service(h,0,(void*)0x20000000,256,r,sizeof *r)==CB_NOR_ARGUMENT);
 gb175_active_plan.mapper=GB179_MBC7;
 assert(gb175_control(NULL,0xa000,0)==CB_NOR_ARGUMENT);
 assert(gb175_control(NULL,0x4000,1)==CB_NOR_ARGUMENT);
 assert(gb175_control(NULL,0xa080,0xff)==CB_NOR_ARGUMENT);
 assert(gb175_ram(NULL,0xa081,a,1)==CB_NOR_ARGUMENT);
 assert(gb175_ram(NULL,0xa080,a,2)==CB_NOR_ARGUMENT);
 puts("PASS GB179 MBC7: wire protocol, FlashGBX BE words, CS settle, dummy bit, mirror, double read, unsupported profiles, failures and OS guards");
 return 0;
}
