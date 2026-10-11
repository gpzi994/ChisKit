#define main old_main175
#include "test-gb-save175.c"
#undef main
#include "../core/gb_save175/rtc186.h"
static uint8_t regs[5];static unsigned latched;
static int ctl(void *ctx,uint16_t a,uint8_t v){
 (void)ctx;assert(m.owned&&!m.removed);m.controls++;
 assert((a==0&&(v==0||v==10))||(a==0x4000&&(v==0||(v>=8&&v<=12)))||(a==0x6000&&v<=1));
 if(m.controls==m.fail_control)return -77;
 if(a==0)m.enabled=v;else if(a==0x4000)m.bank=v;else latched=v;
 return 0;
}
static int rd(void *ctx,uint16_t a,void *p,uint32_t n){
 (void)ctx;assert(m.owned&&!m.removed&&m.enabled&&latched&&m.bank>=8&&m.bank<=12&&a==0xa880&&n==256);m.reads++;
 memset(p,regs[m.bank-8],n);
 if(m.reads==m.change_read)((uint8_t *)p)[3]^=1;
 if(m.reads==m.remove_read)m.removed=1;
 if(m.reads==m.cancel_read)m.cancel=1;
 return m.reads==m.fail_read?-77:0;
}
static struct gb186_report r;
static int run(void){const struct gb175_bus b={&m,acquire,alive,cancelled,header,ctl,rd,release};return gb186_read(&b,m.h,&r);}
static void init(void){setup(0x10,3);latched=0;uint8_t raw[]={40,29,3,2,0};memcpy(regs,raw,5);}
#ifndef GB186_TEST_MAIN
#define GB186_TEST_MAIN main
#endif
int GB186_TEST_MAIN(void){
 init();assert(!run()&&r.supported&&r.days==2&&r.hours==3&&r.minutes==29&&r.seconds==40&&!m.enabled&&!m.bank&&!m.owned);
 init();regs[3]=255;regs[4]=0xc1;assert(!run()&&r.days==511&&r.halt&&r.carry);
 init();memset(regs,0,5);assert(run()==GB175_UNSUPPORTED&&!r.supported);
 init();memset(regs,255,5);assert(run()==GB175_UNSUPPORTED&&!r.supported);
 for(unsigned i=0;i<5;i++){init();regs[i]=i<2?60:i==2?24:i==4?2:255;if(i==3)assert(!run());else assert(run()==GB175_CHANGED&&!r.supported);}
 for(unsigned i=1;i<=10;i++){init();m.fail_read=i;assert(run()!=0&&!m.owned&&!m.enabled&&!m.bank);init();m.change_read=i;assert(run()==GB175_CHANGED&&!r.supported);init();m.remove_read=i;assert(run()!=0&&!m.owned&&!r.supported);}
 for(unsigned i=1;i<=11;i++){init();m.fail_control=i;assert(run()!=0&&!m.owned&&!r.supported);}
 init();m.change_header=1;assert(run()==GB175_IDENTITY&&!m.controls);init();m.change_header=2;assert(run()==GB175_IDENTITY&&!r.supported);
 init();m.release_error=1;assert(run()==GB175_CLEANUP&&!r.supported);
 init();m.h[0x47]=0x1b;assert(run()==GB175_UNSUPPORTED&&!m.acquires);
 init();m.h[4]^=1;assert(run()==GB175_IDENTITY&&!m.acquires);
 init();m.h[0x47]=0x0f;m.h[0x49]=0;m.h[0x4d]=0;for(unsigned i=0x34;i<=0x4c;i++)m.h[0x4d]-=m.h[i]+1;assert(!run());
 puts("GBRTC186 PASS: MBC3 RTC, latch, alias/stability, days/halt/carry, all-zero/FF unknown, invalid values, every read/control fault, identity, removal, no data writes");
 return 0;
}
