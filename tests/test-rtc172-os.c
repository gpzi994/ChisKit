#define _GNU_SOURCE
#define main rtc_protocol_tests
#include "test-rtc167.c"
#undef main
#include <stdbool.h>
#include <sys/mman.h>
enum {CB_NOR_ARGUMENT=-1,CB_NOR_IO=-2,CB_NOR_UNSUPPORTED=-3,CB_NOR_CANCELLED=-4};
static bool busy,private_owned;static unsigned job_token,mapper_kind,CART_BUS,CART_CAPS=0x100bf;
static struct {bool authorized;}job;
static struct mock card;static uint8_t cart_id[16];static int leave_error;
#define CPU_FREQ_HZ 1000000u
static uint64_t read_cycles(void){static uint64_t c;return ++c;}
static void fence(void){}
static int alive(void *p){(void)p;return private_owned&&card.live;}
static int enter(unsigned bus){assert(bus==2);private_owned=true;return card.live?0:-4;}
static int leave(void){private_owned=false;return leave_error;}
static int transaction(uint32_t a,uint16_t *v,bool write){
 assert(CART_BUS==2&&alive(NULL));
 if(a>=0xa0&&a<0xb0){assert(!write);*v=cart_id[a-0xa0]|(cart_id[a-0xa0+1]<<8);return 0;}
 return write?wr(&card,a,*v):rd(&card,a,v);
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/rtc167.h"
#include "../.integration/openfpgaos/src/firmware/os/kernel/rtc172.h"
int main(void){
 uint8_t *p=mmap((void*)0x11000000,4096,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);assert(p!=MAP_FAILED);
 uint8_t target[]={0x26,0x09,0x27,0,0x14,0x03,0x10,0x40};memcpy(p,target,8);
 memset(cart_id,0x33,16);memcpy(p+32,cart_id,16);of_rtc172_report *r=(void*)(p+64);
 card=init();card.allow_write=1;
 assert(!rtc172_write_service(p,p+32,r,sizeof *r)&&r->verified&&!busy&&!private_owned);
 card=init();card.allow_write=1;
 assert(rtc172_write_service((void*)0x20000000,p+32,r,sizeof *r)==CB_NOR_ARGUMENT&&!card.calls);
 p[32]^=1;assert(rtc172_write_service(p,p+32,r,sizeof *r)&&!r->attempted&&!card.calls);p[32]^=1;
 busy=true;assert(rtc172_write_service(p,p+32,r,sizeof *r)&&busy&&!card.calls);busy=false;
 CART_CAPS=0;assert(rtc172_write_service(p,p+32,r,sizeof *r)&&!card.calls&&!busy&&!private_owned);CART_CAPS=0x100bf;
 card=init();card.allow_write=1;leave_error=-77;
 assert(rtc172_write_service(p,p+32,r,sizeof *r)==-77&&r->cleanup_rc==-77&&!r->verified&&!busy&&!private_owned);
 puts("RTC172 OS PASS: SDRAM-only, identity, busy/caps, ownership and cleanup failure; mock hardware");
}
