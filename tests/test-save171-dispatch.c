#define _GNU_SOURCE
#define OF_PC
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <save/restore158.h>
#include "of_save162.h"
#define CB_NOR_ARGUMENT (-4)
#define CB_NOR_IO (-3)
static bool busy;
static unsigned calls,last_type;
static int save158_sdram_range(const void *p,uint32_t n){
 uintptr_t a=(uintptr_t)p;return p&&a>=0x10000000&&a<0x14000000&&n<=0x14000000-a;
}
static int save158_overlaps(const void *a,uint32_t an,const void *b,uint32_t bn){
 return (uintptr_t)a<(uintptr_t)b+bn&&(uintptr_t)b<(uintptr_t)a+an;
}
static int eeprom171_restore_selected(const void *s,void *b,uint32_t n,const uint8_t *id,void *out,uint32_t size,unsigned type){
 (void)s;(void)b;(void)id;assert(size==sizeof(of_save162_report));calls++;last_type=type;
 *(of_save162_report*)out=(of_save162_report){.attempted=8,.verified=n};return 0;
}
static int sram160_restore_impl(const void *s,void *b,uint32_t n,const uint8_t *id,void *out,uint32_t size,int selected){
 (void)s;(void)b;(void)id;assert(selected==1&&size==sizeof(struct cb_save158_report));calls++;last_type=n==32768?3:8;
 *(struct cb_save158_report*)out=(struct cb_save158_report){.attempted=1,.verified=n};return 0;
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/save171.h"
int main(void){
 uint8_t *m=mmap((void*)0x11000000,0x50000,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);
 assert(m!=MAP_FAILED);of_save171_report *r=(void*)(m+0x40100);
 unsigned types[]={1,2,3,8},sizes[]={512,8192,32768,131072};
 for(unsigned i=0;i<4;i++){
  calls=0;assert(!save171_restore_service(types[i],m,m+0x20000,sizes[i],m+0x40000,r,sizeof *r));
  assert(calls==1&&last_type==types[i]&&r->version==171&&r->verified==sizes[i]);
 }
 calls=0;
 assert(save171_restore_service(1,m,m+0x20000,8192,m+0x40000,r,sizeof *r));
 assert(save171_restore_service(7,m,m+0x20000,65536,m+0x40000,r,sizeof *r));
 assert(save171_restore_service(8,m,m+8,131072,m+0x40000,r,sizeof *r));
 assert(save171_restore_service(8,m,m+0x20000,131072,m,r,sizeof *r));
 assert(save171_restore_service(8,m,m+0x20000,131072,m+0x40000,m,sizeof *r));
 assert(save171_restore_service(8,(void*)0x20000000,m+0x20000,131072,m+0x40000,r,sizeof *r));
 busy=true;assert(save171_restore_service(1,m,m+0x20000,512,m+0x40000,r,sizeof *r)==-3);
 assert(!calls);puts("SAVE171 dispatcher PASS: four routes, type/size/SDRAM/overlap/busy guards");
}
