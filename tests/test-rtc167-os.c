#define _GNU_SOURCE
#define main protocol_tests_main
#include "test-rtc167.c"
#undef main
#include <stdbool.h>
#include <sys/mman.h>
enum {CB_NOR_ARGUMENT=-1,CB_NOR_IO=-2,CB_NOR_UNSUPPORTED=-3,CB_NOR_CANCELLED=-4};
static bool busy,private_owned;static unsigned job_token,mapper_kind,CART_BUS,CART_CAPS=0x100bf;
static struct {bool authorized;}job;
static struct mock card;
#define CPU_FREQ_HZ 1000000u
static uint64_t read_cycles(void){static uint64_t c;return ++c;}
static void fence(void){}
static int alive(void *p){(void)p;return private_owned&&card.live;}
static int enter(unsigned bus){assert(bus==2);private_owned=true;return card.live?0:-4;}
static int leave(void){private_owned=false;return 0;}
static int transaction(uint32_t a,uint16_t *v,bool write){
 assert(CART_BUS==2&&alive(NULL));return write?wr(&card,a,*v):rd(&card,a,v);
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/rtc167.h"
int main(void){
 of_rtc167_report *r=mmap((void*)0x11000000,4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);assert(r!=MAP_FAILED);
 card=init();assert(!rtc167_read(0,r,sizeof(*r),1000000));assert(r->flags&OF_RTC167_TIME_OK);assert(!private_owned&&!busy&&!card.re);
 card=init();card.write_only_control=1;assert(!rtc167_read(0,r,sizeof(*r),1000000));assert(r->flags&OF_RTC167_TIME_OK);
 card=init();assert(rtc167_read(1,r,sizeof(*r),1000000)==CB_NOR_ARGUMENT);
 assert(rtc167_read(0,(void*)0x20000000,sizeof(*r),1000000)==CB_NOR_ARGUMENT);
 assert(rtc167_read(0,r,1,1000000)==CB_NOR_ARGUMENT);
 assert(rtc167_read(0,r,sizeof(*r),0)==CB_NOR_ARGUMENT);assert(!card.calls);
 assert(rtc167_bus_write(NULL,0x5555,0xaa)==CB_NOR_ARGUMENT);
 assert(rtc167_bus_write(NULL,0xc4,8)==CB_NOR_ARGUMENT);assert(!card.calls);
 busy=true;assert(rtc167_read(0,r,sizeof(*r),1000000)==CB_NOR_IO);busy=false;
 CART_CAPS=0x200bf;assert(rtc167_read(0,r,sizeof(*r),1000000)==CB_NOR_UNSUPPORTED);CART_CAPS=0x100bf;assert(!card.calls&&!private_owned);
 card=init();card.fail=30;assert(rtc167_read(0,r,sizeof(*r),1000000));assert(!private_owned&&!busy&&!(r->flags&OF_RTC167_TIME_OK));
 card=init();card.drop=30;assert(rtc167_read(0,r,sizeof(*r),1000000));assert(card.calls==30&&!private_owned&&!busy);
 munmap(r,4096);puts("RTC167 OS wrapper PASS: production adapter, SDRAM-only, strict request/write address guards, caps, ownership, fault and removal cleanup (mock bus)");
}
