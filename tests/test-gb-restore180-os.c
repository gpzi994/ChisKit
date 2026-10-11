#define _GNU_SOURCE
#define CART_GB_RESTORE180 1
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
 assert(CART_BUS==8||CART_BUS==10);
 if(wr&&a>=0xa000){
  assert(CART_BUS==10&&a<0xc000&&m.enabled&&*v<=255);
  unsigned off=m.bank*8192+a-0xa000;assert(off<m.bytes);m.writes++;
  m.ram[off]=(uint8_t)*v;return 0;
 }
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
#include "../.integration/openfpgaos/src/firmware/os/kernel/gb_restore180.h"
int main(void){
 unsigned char *p=mmap((void*)0x11000000,393216,PROT_READ|PROT_WRITE,
  MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);assert(p!=MAP_FAILED);
 unsigned char *source=p+131072,*h=p+262144;
 struct gb175_report *r=(void*)(p+262240);
 setup(0x1b,4);memcpy(h,m.h,80);memcpy(p,m.ram,131072);memset(source,0x96,131072);
 CART_CAPS=0x107bf;
 assert(gb180_restore_service(h,p,source,131072,r,sizeof *r)==GB175_IO&&!m.writes&&!private_owned);
 CART_CAPS|=0x800;
 assert(!gb180_restore_service(h,p,source,131072,r,sizeof *r));
 assert(m.writes==131072&&r->verified==131072&&!memcmp(m.ram,source,131072));
 assert(!private_owned&&!busy&&!gb180_active&&!gb177_active&&!m.bank&&!m.enabled);
 setup(0x1b,4);memcpy(p,m.ram,131072);
 assert(gb180_restore_service(h,(void*)0x20000000,source,131072,r,sizeof *r)==CB_NOR_ARGUMENT&&!m.acquires);
 assert(gb180_restore_service(h,p,p+1,131072,r,sizeof *r)==CB_NOR_ARGUMENT&&!m.acquires);
 assert(gb180_restore_service(h,p,source,131072,p,sizeof *r)==CB_NOR_ARGUMENT&&!m.acquires);
 busy=true;assert(gb180_restore_service(h,p,source,131072,r,sizeof *r)==GB175_IO&&!m.acquires);busy=false;
 m.ram[131071]^=1;
 assert(gb180_restore_service(h,p,source,131072,r,sizeof *r)==GB175_CHANGED&&!m.writes&&!busy&&!private_owned);
 setup(0x13,3);memcpy(h,m.h,80);
 assert(gb180_restore_service(h,p,source,131072,r,sizeof *r)==GB175_UNSUPPORTED&&!m.acquires);
 setup(6,0);memcpy(h,m.h,80);
 m.bytes=512;memcpy(p,m.ram,512);memset(source,0xf9,512);
 assert(!gb180_restore_service(h,p,source,512,r,sizeof *r)&&r->verified==512&&m.writes==512);
 for(unsigned i=0;i<512;i++)assert(m.ram[i]==9);
 assert(!m.enabled&&!private_owned&&!busy);
 assert(gb180_write(NULL,0xa000,p,1)==CB_NOR_ARGUMENT);
 gb180_active=1;assert(gb180_write(NULL,0x9fff,p,1)==CB_NOR_ARGUMENT);
 assert(gb180_write(NULL,0xbfff,p,2)==CB_NOR_ARGUMENT);gb180_active=0;
 gb180_active=1;gb175_active_plan.mapper=GB178_MBC2;
 assert(gb180_write(NULL,0xa200,p,1)==CB_NOR_ARGUMENT);
 assert(gb180_write(NULL,0xa1ff,p,2)==CB_NOR_ARGUMENT);
 assert(gb175_control(NULL,0x4000,0)==CB_NOR_ARGUMENT);
 assert(gb175_control(NULL,0x2100,1)==CB_NOR_ARGUMENT);gb180_active=0;
 gb175_active_plan.mapper=GB175_MBC3;gb175_active_plan.banks=4;gb177_active=1;
 assert(gb175_control(NULL,0x4000,4)==CB_NOR_ARGUMENT);
 assert(gb175_control(NULL,0x4000,8)==CB_NOR_ARGUMENT);
 assert(gb175_control(NULL,0x6000,1)==CB_NOR_ARGUMENT);
 gb175_active_plan.mapper=GB175_MBC30;gb175_active_plan.banks=8;
 assert(gb175_control(NULL,0x4000,8)==CB_NOR_ARGUMENT);
 gb175_active_plan.mapper=GB177_PLAIN;
 assert(gb175_control(NULL,0,10)==CB_NOR_ARGUMENT);gb177_active=0;
 munmap(p,393216);
 puts("PASS GB180 production OS adapter: byte transactions, capability, SDRAM, overlap, busy, exclusions, changed original, cleanup");
 return 0;
}
