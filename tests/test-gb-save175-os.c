#define _GNU_SOURCE
#define main core_tests175
#define alive mock_alive175
#include "test-gb-save175.c"
#undef main
#undef alive
#include <stdbool.h>
#include <sys/mman.h>
enum {CB_NOR_ARGUMENT=-1,CB_NOR_UNSUPPORTED=-3};
static bool busy,private_owned;
static unsigned job_token,mapper_kind,CART_BUS,CART_CAPS=0x101bf;
static struct {bool authorized;}job;
static void fence(void){}
static uint32_t now_ms(void *p){(void)p;static uint32_t tick;return tick++;}
static int alive(void *p){(void)p;return private_owned&&!m.removed;}
static int enter(unsigned b){assert(b==1);int rc=acquire(&m);if(!rc)private_owned=true;return rc;}
static int leave(void){int rc=release(&m);private_owned=false;return rc;}
static int transaction(uint32_t a,uint16_t *v,bool wr){
 if(!alive(NULL))return -77;
 assert(CART_BUS==8);
 if(wr){
  if(a==0x6000){assert((m.h[0x47]==2||m.h[0x47]==3)&&*v<=1);m.controls++;return 0;}
  return control(&m,(uint16_t)a,(uint8_t)*v);
 }
 if(a>=0x100&&a<0x150){
  if(a==0x100)m.headers++;
  if(m.header_error)return -77;
  *v=m.h[a-0x100];if(m.headers==m.change_header&&a==0x134)*v^=1;return 0;
 }
 if(m.h[0x47]==8||m.h[0x47]==9)m.enabled=1;
 uint8_t byte=0;int rc=ram(&m,(uint16_t)a,&byte,1);*v=byte;return rc;
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/gb_save175.h"
int main(void){
 unsigned char *p=mmap((void*)0x11000000,262144,PROT_READ|PROT_WRITE,
  MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);assert(p!=MAP_FAILED);
 struct gb175_report *r=(void*)(p+131200);unsigned char *h=p+131104;
 setup(0x1b,4);memcpy(h,m.h,80);
 assert(!gb175_capture_service(h,p,131072,r,sizeof *r));
 assert(r->compared==131072&&!memcmp(p,m.ram,131072)&&!private_owned&&!busy&&!m.enabled&&!m.bank);
 setup(0x13,3);memcpy(h,m.h,80);
 assert(gb175_capture_service(h,(void*)0x20000000,32768,r,sizeof *r)==CB_NOR_ARGUMENT&&!m.acquires);
 assert(gb175_capture_service(h,p,32768,p+32760,sizeof *r)==CB_NOR_ARGUMENT&&!m.acquires);
 assert(gb175_capture_service(h,p,8192,r,sizeof *r)==GB175_ARGUMENT&&!m.acquires);
 busy=true;assert(gb175_capture_service(h,p,32768,r,sizeof *r)==GB175_IO&&!m.acquires);busy=false;
 CART_CAPS=0x100bf;assert(gb175_capture_service(h,p,32768,r,sizeof *r)==GB175_IO&&!private_owned&&!m.reads&&!m.controls);CART_CAPS=0x101bf;
 setup(0x13,3);m.change_header=2;assert(gb175_capture_service(h,p,32768,r,sizeof *r)==GB175_IDENTITY&&!m.enabled&&!m.bank&&!private_owned);
 setup(0x13,3);m.remove_read=2;assert(gb175_capture_service(h,p,32768,r,sizeof *r)==GB175_IO&&!private_owned&&!busy);
 setup(0x13,3);m.release_error=1;assert(gb175_capture_service(h,p,32768,r,sizeof *r)==GB175_CLEANUP&&!busy&&!private_owned);
 puts("PASS GB175 production OS adapter: SDRAM/size/overlap/owner/capability/identity/removal/cleanup; mock bus");
 setup(0x1b,3);m.bytes=131072;memcpy(h,m.h,80);
 assert(!gb176_capture_service(h,0,p,131072,r,sizeof *r)&&r->compared==131072);
 assert(!memcmp(p,m.ram,131072));
 setup(0x1b,3);m.bytes=131072;memcpy(h,m.h,80);
 assert(!gb176_capture_service(h,131072,p,131072,r,sizeof *r));
 setup(0x13,3);memcpy(h,m.h,80);
 assert(gb176_capture_service(h,131072,p,131072,r,sizeof *r)==GB175_UNSUPPORTED&&!m.acquires);
 assert(gb176_capture_service(h,0,p,131072,r,sizeof *r)==GB175_ARGUMENT&&!m.acquires);
 assert(gb176_capture_service(h,0,(void*)0x20000000,32768,r,sizeof *r)==CB_NOR_ARGUMENT);
 puts("PASS GB176 OS: header32/read128, explicit128, MBC3 and SDRAM guards");
 setup(3,3);m.bytes=32768;memcpy(h,m.h,80);
 assert(gb177_capture_service(h,0,p,32768,r,sizeof *r)==GB175_IO&&!m.controls&&!m.reads);
 CART_CAPS|=0x200;setup(3,3);m.bytes=32768;memcpy(h,m.h,80);
 assert(!gb177_capture_service(h,0,p,32768,r,sizeof *r)&&r->compared==32768&&!m.enabled&&!m.bank);
 setup(9,1);m.bytes=2048;memcpy(h,m.h,80);
 assert(!gb177_capture_service(h,2048,p,2048,r,sizeof *r)&&r->compared==2048&&!m.controls);
 puts("PASS GB177 OS: old FPGA denied, MBC1 mode controls, plain 2KiB no mapper writes");
 setup(6,0);m.bytes=512;memcpy(h,m.h,80);
 assert(!gb178_capture_service(h,0,p,512,r,sizeof *r)&&r->compared==512&&!m.enabled&&!m.owned&&m.controls==3);
 for(unsigned i=0;i<512;i++)assert(p[i]==(m.ram[i]&15));
 assert(gb178_capture_service(h,8192,p,8192,r,sizeof *r)==GB175_UNSUPPORTED);
 assert(gb178_capture_service(h,0,(void*)0x20000000,512,r,sizeof *r)==CB_NOR_ARGUMENT);
 gb175_active_plan.mapper=GB178_MBC2;gb175_active_plan.banks=1;
 assert(gb175_control(NULL,0x4000,0)==CB_NOR_ARGUMENT);
 assert(gb175_control(NULL,0x6000,0)==CB_NOR_ARGUMENT);
 assert(gb175_ram(NULL,0xa200,p,1)==CB_NOR_ARGUMENT);
 assert(gb175_ram(NULL,0xa1ff,p,2)==CB_NOR_ARGUMENT);
 memset(&gb175_active_plan,0,sizeof gb175_active_plan);
 puts("PASS GB178 OS: MBC2 capture, low nibbles, SDRAM guard, bank/mode/address rejection");
 return 0;
}
