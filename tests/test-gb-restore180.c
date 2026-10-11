#define main old_main175
#include "test-gb-save175.c"
#undef main
#include "../core/gb_save175/restore180.h"
static unsigned mode177,plain177,bankmax177;
static int control177(void *v,uint16_t a,uint8_t d){
 struct mock *s=v;assert(!plain177&&s->owned&&!s->removed);
 if(a==0x6000){assert((s->h[0x47]==2||s->h[0x47]==3)&&d<=1);mode177=d;s->controls++;return s->controls==s->fail_control?-77:0;}
 if(a==0x4000)assert(d<=bankmax177);
 return control(v,a,d);
}
static int header177(void *v,uint8_t *h){assert(!mode177||!m.bank);return header(v,h);}
static int ram177(void *v,uint16_t a,void *p,uint32_t n){
 struct mock *s=v;
 if(plain177){assert(!s->controls&&!s->bank);s->enabled=1;}
 else if(s->h[0x47]==2||s->h[0x47]==3)assert(mode177==1);
 return ram(v,a,p,n);
}
static void init177(unsigned t,unsigned r,unsigned bytes){
 setup(t,r);m.bytes=bytes;mode177=0;bankmax177=(bytes+8191)/8192-1;plain177=t==8||t==9;
}
static uint8_t desired[131072];
static unsigned datawrites,failwrite,corruptwrite,remove_write;
static int writer(void *v,uint16_t a,const void *data,uint32_t n){
 struct mock *s=v;
 assert(s->owned&&!s->removed&&s->enabled);
 assert(a>=0xa000&&a<0xc000&&n&&n<=0xc000u-a);
 if(!plain177&&(s->h[0x47]==2||s->h[0x47]==3))assert(mode177==1);
 unsigned off=s->bank*8192+a-0xa000;assert(off+n<=s->bytes);
 datawrites++;memcpy(s->ram+off,data,n);
 if(datawrites==corruptwrite)s->ram[off]^=1;
 if(datawrites==remove_write)s->removed=1;
 return datawrites==failwrite?-77:0;
}
static void start180(unsigned t,unsigned size,unsigned bytes){
 init177(t,size,bytes);memcpy(snapshot,m.ram,bytes);
 for(unsigned i=0;i<bytes;i++)desired[i]=(uint8_t)(m.ram[i]^0xa5);
 datawrites=failwrite=corruptwrite=remove_write=0;
}
static int run180(unsigned chunk){
 struct gb180_bus b={{&m,acquire,alive,cancelled,header177,control177,ram177,release},writer};
 return gb180_restore(&b,m.h,snapshot,desired,m.bytes,scratch,chunk,&report);
}
#ifndef GB180_TEST_MAIN
#define GB180_TEST_MAIN main
#endif
int GB180_TEST_MAIN(void){
 struct gb175_plan p;
 const unsigned types[]={9,3,0x13,0x10,0x1b,0x1e};
 const unsigned sizes[]={2,3,3,5,4,5};
 const unsigned bytes[]={8192,32768,32768,65536,131072,65536};
 for(unsigned i=0;i<6;i++){
  start180(types[i],sizes[i],bytes[i]);
  assert(!run180(777)&&report.compared==bytes[i]&&report.verified==bytes[i]);
  assert(report.written==bytes[i]&&!memcmp(m.ram,desired,bytes[i]));
  assert(!m.owned&&!m.bank&&!mode177&&(!m.enabled||plain177));
  if(plain177)assert(!m.controls);
  start180(types[i],sizes[i],bytes[i]);memcpy(desired,snapshot,bytes[i]);
  assert(!run180(512)&&!datawrites&&!report.written&&report.verified==bytes[i]);
 }
 for(unsigned i=1;i<=512;i++){
  start180(0x1b,4,131072);m.fail_read=i;assert(run180(512)<0&&!m.owned&&!m.enabled&&!m.bank);
  if(i<=256)assert(!datawrites);
 }
 for(unsigned i=1;i<=256;i++){
  start180(0x1b,4,131072);failwrite=i;assert(run180(512)<0&&datawrites==i&&!m.owned&&!m.enabled&&!m.bank);
  start180(0x1b,4,131072);remove_write=i;assert(run180(512)==GB175_CANCELLED&&!m.owned);
 }
 for(unsigned i=1;i<=4;i++){
  start180(3,3,32768);m.change_header=i;assert(run180(512)==GB175_IDENTITY&&!m.owned);
  if(i<=2)assert(!datawrites);
 }
 start180(0x1b,4,131072);m.ram[131071]^=1;
 assert(run180(512)==GB175_CHANGED&&!datawrites&&!m.owned);
 for(unsigned fill=0;fill<2;fill++){
  start180(0x1b,4,131072);memset(m.ram,fill?255:0,131072);memcpy(snapshot,m.ram,131072);
  assert(!run180(512)&&!memcmp(m.ram,desired,131072));
 }
 start180(0x1b,4,131072);memcpy(desired,snapshot,131072);desired[131071]^=1;
 assert(!run180(512)&&datawrites==1&&report.written==512&&!memcmp(m.ram,desired,131072));
 start180(0x1b,4,131072);corruptwrite=256;assert(run180(512)==GB175_CHANGED&&report.verified==130560);
 start180(0x13,3,32768);m.cancel_read=64;assert(run180(512)==GB175_CANCELLED&&!datawrites&&!m.enabled);
 start180(3,3,32768);assert(!run180(512));unsigned controls=m.controls;
 for(unsigned i=1;i<=controls;i++){
  start180(3,3,32768);m.fail_control=i;assert(run180(512)<0&&!m.owned);
 }
 start180(3,3,32768);m.release_error=1;assert(run180(512)==GB175_CLEANUP);
 start180(0x1e,5,65536);assert(gb180_plan(m.h,131072,&p)<0);
 start180(3,3,32768);assert(gb180_plan(m.h,65536,&p)<0);
 start180(9,2,8192);assert(gb180_plan(m.h,32768,&p)<0);
 start180(6,0,512);assert(!gb180_plan(m.h,512,&p)&&p.mapper==GB178_MBC2);
 assert(gb180_plan(m.h,8192,&p)==GB175_UNSUPPORTED);
 start180(0x22,0,256);assert(run180(512)==GB175_UNSUPPORTED&&!m.acquires);
 start180(3,1,2048);assert(run180(512)==GB175_UNSUPPORTED&&!m.acquires);
 puts("PASS GB180 ordinary SRAM: all banks, no-op, changed original, identity, I/O failures, cleanup, exclusions");
 return 0;
}
