#define _GNU_SOURCE
#define CART_GB_RTC189 1
#define GB186_TEST_MAIN rtc186_core_tests
#define alive core_alive
#include "test-gb-rtc186.c"
#undef alive
#include <stdbool.h>
#include <sys/mman.h>
enum {CB_NOR_ARGUMENT=-1,CB_NOR_UNSUPPORTED=-3};
static bool busy,private_owned;
static unsigned mapper_kind,job_token,CART_BUS,CART_CAPS=0x12300;
static unsigned CART_ARM,CART_COMMAND,clock_bursts,clock_credit,clock_failure;
static uint8_t snapshot193[5];
static int wait_idle(unsigned ms,bool done){assert(ms==20&&done&&CART_BUS==12&&CART_COMMAND==8&&CART_ARM==0x43424e32u);clock_bursts++;clock_credit=1;return clock_failure?-88:0;}
static struct {bool authorized;}job;
static void fence(void){}
static int alive(void *p){(void)p;return private_owned&&!m.removed;}
static uint32_t now_ms(void *p){(void)p;static uint32_t n;return n++;}
static int enter(unsigned bus){assert(bus==1);int rc=acquire(&m);if(!rc)private_owned=true;return rc;}
static int leave(void){int rc=release(&m);private_owned=false;return rc;}
static int transaction(uint32_t a,uint16_t *v,bool wr){
 assert((CART_BUS==8||CART_BUS==10)&&private_owned&&!m.removed);
 if(wr||a>=0xa000){assert(clock_credit);clock_credit=0;}
 if(wr&&a==0xa000){assert(CART_BUS==10&&m.bank>=8&&m.bank<=12);regs[m.bank-8]=(uint8_t)*v;return 0;}
 if(wr){unsigned previous=latched;int rc=ctl(&m,a,*v);
  if(!rc&&a==0x6000&&*v==1&&previous==0)memcpy(snapshot193,regs,5);
  return rc;
 }
 if(a>=0x100&&a<0x150){*v=m.h[a-0x100];return 0;}
 assert(a>=0xa880&&a<0xa980&&m.bank>=8&&m.bank<=12&&m.enabled&&latched);
 *v=snapshot193[m.bank-8];return 0;
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/gb_save175.h"
#include "../.integration/openfpgaos/src/firmware/os/kernel/gb_rtc186.h"
#include "../.integration/openfpgaos/src/firmware/os/kernel/gb_rtc189.h"
#include "../.integration/openfpgaos/src/firmware/api/of_gbrtc186.h"
#include "../app/ui98/gb_rtc186_ui.h"
int main(void){
 _Static_assert(sizeof(of_gb186_report)==sizeof(struct gb186_report),"RTC ABI");
 uint8_t *p=mmap((void*)0x11000000,4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);assert(p!=MAP_FAILED);
 struct gb186_report *out=(void*)(p+128);init();memcpy(p,m.h,80);mapper_kind=5;
 assert(!gb186_read_service(p,out,sizeof *out)&&out->supported&&out->days==2&&!busy&&!private_owned&&!m.enabled&&!m.bank&&!mapper_kind);
 for(unsigned a=0;a<65536;a++)if(a!=0&&a!=0x4000&&a!=0x6000)assert(gb186_control(NULL,a,0)==CB_NOR_ARGUMENT);
 assert(gb186_control(NULL,0x4000,7)==CB_NOR_ARGUMENT&&gb186_control(NULL,0x4000,13)==CB_NOR_ARGUMENT);
 assert(gb186_control(NULL,0x6000,2)==CB_NOR_ARGUMENT);
 assert(gb186_window(NULL,0xa000,p,256)==CB_NOR_ARGUMENT);
 assert(gb186_read_service((void*)0x20000000,out,sizeof *out)==CB_NOR_ARGUMENT);
 assert(gb186_read_service(p,p+1,sizeof *out)==CB_NOR_ARGUMENT);
 unsigned calls=m.acquires;busy=true;assert(gb186_read_service(p,out,sizeof *out)==GB175_IO&&m.acquires==calls);busy=false;
 CART_CAPS=0x10100;assert(gb186_read_service(p,out,sizeof *out)==GB175_IO&&!private_owned);
 char text[28];of_gb186_report ui={0};ui.supported=1;ui.days=511;ui.hours=23;ui.minutes=59;ui.seconds=59;ui.halt=ui.carry=1;
 cb_gbrtc186_format(text,sizeof text,0,&ui);assert(!strcmp(text,"511d 23:59:59 H C"));
 cb_gbrtc186_format(text,sizeof text,-2,&ui);assert(!strcmp(text,"RTC not confirmed"));
 cb_gbrtc186_format(text,sizeof text,-4,&ui);assert(strstr(text,"failed"));
 init();CART_CAPS=0x12b00;memcpy(p,m.h,80);
 uint8_t *target=p+240;uint8_t raw189[]={12,34,5,8,0};memcpy(target,raw189,5);
 struct gb189_report *report189=(void*)(p+256);
 assert(!gb189_write_service(p,target,report189,sizeof *report189)&&report189->verified&&!memcmp(regs,target,5)&&!private_owned&&!busy);
 assert(clock_bursts>6000);
 /* Same inserted cartridge: emulate the live clock advancing, then read via
  * the normal read-only service repeatedly without resetting the mock. */
 for(unsigned i=0;i<3;i++){
  regs[0]++;unsigned pulses=clock_bursts;
  assert(!gb186_read_service(p,out,sizeof *out)&&out->seconds==regs[0]);
  assert(clock_bursts>pulses+2500&&CART_BUS==8&&!private_owned&&!m.bank&&!m.enabled);
 }
 init();CART_CAPS=0x10b00;assert(gb189_write_service(p,target,report189,sizeof *report189)!=0&&!private_owned);
 init();CART_CAPS=0x12b00;clock_failure=1;uint8_t saved[5];memcpy(saved,regs,5);
 assert(gb189_write_service(p,target,report189,sizeof *report189)!=0&&!private_owned&&!memcmp(saved,regs,5));clock_failure=0;
 assert(gb189_write_service(p,target,p+1,sizeof *report189)==CB_NOR_ARGUMENT);
 assert(gb189_write_service(p,(void*)0x20000000,report189,sizeof *report189)==CB_NOR_ARGUMENT);
 assert(gb189_data(NULL,0)==CB_NOR_ARGUMENT);
 init();CART_CAPS=0x10300;memcpy(p,m.h,80);
 assert(gb189_write_service(p,target,report189,sizeof *report189)!=0&&!private_owned);
 puts("GB189 OS PASS: exact A000 write, RTC bank guard, SDRAM/overlap, missing write caps, cleanup");
 munmap(p,4096);puts("GBRTC186 OS PASS: bus8 only, control whitelist, no data writes, SDRAM/caps/overlap/busy, ABI, UI formatting");
}
