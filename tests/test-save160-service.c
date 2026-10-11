#define _GNU_SOURCE
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <save/restore158.h>

#define CB_NOR_ARGUMENT (-4)
#define CB_NOR_UNSUPPORTED (-2)
static uint32_t caps=0,private_caps=0x100bf,CART_BUS;
#define CART_CAPS caps
#define fence() ((void)0)
static uint8_t cart[131072],disk[131072],header[192];
static uint32_t configured_size;
static unsigned bank,writes,file_reads,read_fail_at;
static int disk_bad,cart_alive=1,classified_sram=1;
static int alive(void *p){(void)p;return cart_alive;}
static int sram138_bank(unsigned b){assert(b<2);bank=b;return 0;}
static int save139_read(void *p,uint32_t a,uint8_t *v){
 (void)p;assert(a<65536);*v=cart[bank*65536+a];return 0;
}
static int transaction(uint32_t a,uint16_t *v,int write){
 assert(write&&CART_BUS==7&&a<65536);cart[bank*65536+a]=(uint8_t)*v;writes++;return 0;
}
typedef int (*save_auto139_sram_owned_fn)(const struct cb_save139_result *,
 const uint8_t *,const uint8_t *,void *);
static struct cb_save139_result save_auto139_kernel_owned(
 save_auto139_sram_owned_fn fn,void *ctx){
 struct cb_save139_result r={.type=classified_sram?
  (configured_size==32768?CB_SAVE139_SRAM32:CB_SAVE139_SRAM128):
  CB_SAVE139_FLASH128,.bytes=configured_size,.backup_allowed=classified_sram};
 uint32_t previous_caps=caps;caps=private_caps;
 r.rc=fn(&r,header,cart,ctx);caps=previous_caps;return r;
}
static long of_file_size(uint32_t slot){assert(slot==5);return configured_size;}
static int of_file_read(uint32_t slot,uint32_t off,void *dst,uint32_t n){
 assert(slot==5&&n==32768&&off+n<=configured_size);file_reads++;
 if(read_fail_at==off+1)return -77;
 memcpy(dst,disk+off,n);
 if(disk_bad&&off==0)((uint8_t *)dst)[0]^=1;
 return 0;
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/save158.h"

int main(void){
 uint8_t *memory=mmap((void *)0x11000000u,0x50000u,
  PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);
 assert(memory!=MAP_FAILED);
 uint8_t *source=memory,*backup=memory+0x20000,*id=memory+0x40000;
 struct cb_save158_report *report=(void *)(memory+0x40100);
 memcpy(header+0xa0,"POKEMON EMERBPEE",16);memcpy(id,header+0xa0,16);
 for(unsigned n=32768;n<=131072;n*=4){
  configured_size=n;
  for(unsigned i=0;i<n;i++)cart[i]=(uint8_t)(i*17u+(i>>16)*71u);
  memcpy(disk,cart,n);memcpy(source,cart,n);
  writes=file_reads=read_fail_at=0;disk_bad=0;classified_sram=1;
  caps=0;private_caps=0x100bf; /* Legacy read engine has no SAVE158 bit. */
  assert(!sram160_restore_service(source,backup,n,id,report,sizeof *report));
  assert(report->verified==n&&!writes&&file_reads==n/32768);
  source[0]^=1;source[n-1]^=2;
  assert(!sram160_restore_service(source,backup,n,id,report,sizeof *report));
  assert(report->attempted==2&&report->changed==2&&report->verified==n&&
   !memcmp(cart,source,n));
 }
 configured_size=131072;memcpy(disk,cart,configured_size);
 memcpy(source,cart,configured_size);writes=file_reads=0;
 assert(sram160_restore_service(source,backup,32768,id,report,sizeof *report)&&
  !writes&&!file_reads);
 assert(sram160_restore_service(source,source+8,131072,id,report,sizeof *report)&&
  !writes&&!file_reads);
 assert(sram160_restore_service(source,backup,131072,id,source+4,sizeof *report)&&
  !writes&&!file_reads);
 read_fail_at=1;assert(sram160_restore_service(source,backup,131072,id,report,sizeof *report)==-77&&
  !writes&&file_reads==1);
 read_fail_at=0;disk_bad=1;
 assert(sram160_restore_service(source,backup,131072,id,report,sizeof *report)&&!writes);
 disk_bad=0;classified_sram=0;
 assert(sram160_restore_service(source,backup,131072,id,report,sizeof *report)&&!writes);
 classified_sram=1;caps=0x100bf;private_caps=0x1003f;
 assert(sram160_restore_service(source,backup,131072,id,report,sizeof *report)&&!writes);
 puts("SAVE160 service PASS: OS file reread, 32/128 KiB, mismatch, file error, type and capability gates");
}
