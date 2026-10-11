#define _GNU_SOURCE
#define GB186_TEST_MAIN rtc186_core_tests
#define alive core_alive
#include "test-gb-rtc186.c"
#undef alive
#include <stdbool.h>
#include <sys/mman.h>
enum {CB_NOR_ARGUMENT=-1,CB_NOR_UNSUPPORTED=-3};
static bool busy,private_owned;
static unsigned mapper_kind,job_token,CART_BUS,CART_CAPS=0x10300;
static struct {bool authorized;}job;
static void fence(void){}
static int alive(void *p){(void)p;return private_owned&&!m.removed;}
static uint32_t now_ms(void *p){(void)p;static uint32_t n;return n++;}
static int enter(unsigned bus){assert(bus==1);int rc=acquire(&m);if(!rc)private_owned=true;return rc;}
static int leave(void){int rc=release(&m);private_owned=false;return rc;}
static int transaction(uint32_t a,uint16_t *v,bool wr){
 assert(CART_BUS==8&&private_owned&&!m.removed);
 if(wr)return ctl(&m,a,*v);
 if(a>=0x100&&a<0x150){*v=m.h[a-0x100];return 0;}
 assert(a>=0xa880&&a<0xa980&&m.bank>=8&&m.bank<=12&&m.enabled&&latched);
 *v=regs[m.bank-8];return 0;
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/gb_save175.h"
#include "../.integration/openfpgaos/src/firmware/os/kernel/gb_rtc186.h"
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
 munmap(p,4096);puts("GBRTC186 OS PASS: bus8 only, control whitelist, no data writes, SDRAM/caps/overlap/busy, ABI, UI formatting");
}
