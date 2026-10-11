#define _GNU_SOURCE
#define OF_PC
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include "probe.h"
#include <save/detect139.h>
static bool busy,private_owned;
static uint8_t cart[8192],disk[8192],header[192];
static unsigned size_now,writes,reads,file_reads,fail_write,unstable,wrong_type;
#ifdef CB_SAVE171
static unsigned unknown,detect_error,serial_stage,dirty_cleanup;
static struct cb_save139_diag save_auto139_diag;
#endif
struct eeprom139_session {bool stuck;};
static int alive(void *ctx){(void)ctx;return private_owned;}
static struct cb_save139_result save_auto139_kernel(void){
 struct cb_save139_result r={.type=wrong_type?CB_SAVE139_SRAM32:
  size_now==512?CB_SAVE139_EEPROM512:CB_SAVE139_EEPROM8K,.bytes=size_now};
#ifdef CB_SAVE171
 memset(&save_auto139_diag,0,sizeof save_auto139_diag);
 save_auto139_diag.stage=serial_stage;save_auto139_diag.cleanup_rc=dirty_cleanup?-4:0;
 if(unknown){r.type=unknown==2?CB_SAVE139_NONE:CB_SAVE139_UNKNOWN;r.bytes=0;}
 if(detect_error)r.rc=-3;
#endif
 return r;
}
static int eeprom139_begin(void){assert(!busy);busy=private_owned=true;return 0;}
static int eeprom139_release_owned(void *ctx){(void)ctx;busy=private_owned=false;return 0;}
static int read_bus(void *ctx,uint32_t a,uint16_t *word){
 (void)ctx;assert(a+1<sizeof header);*word=header[a]|((uint16_t)header[a+1]<<8);return 0;
}
static int eeprom139_read_owned(void *ctx,unsigned bits,unsigned block,uint8_t *out){
 (void)ctx;assert(bits==(size_now==512?6u:14u)&&block*8<size_now);reads++;
 memcpy(out,cart+block*8,8);if(unstable&&reads==2)out[0]^=1;return 0;
}
static int eeprom139_write_owned(struct eeprom139_session *ctx,
 unsigned bits,unsigned block,const uint8_t in[8]){
 (void)ctx;assert(bits==(size_now==512?6u:14u)&&block*8<size_now);
 writes++;if(fail_write&&writes==fail_write)return CB_NOR_IO;
 memcpy(cart+block*8,in,8);return 0;
}
static long of_file_size(uint32_t slot){assert(slot==5);return size_now;}
static int of_file_read(uint32_t slot,uint32_t off,void *out,uint32_t n){
 assert(slot==5&&n==512&&off+n<=size_now);file_reads++;
 memcpy(out,disk+off,n);return 0;
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/eeprom162.h"
static void setup(unsigned n,uint8_t *source,uint8_t *id){
 size_now=n;writes=reads=file_reads=fail_write=unstable=wrong_type=0;
 busy=private_owned=false;
#ifdef CB_SAVE171
 unknown=detect_error=dirty_cleanup=0;serial_stage=9;
#endif
 for(unsigned i=0;i<n;i++)cart[i]=(uint8_t)(i*19u+(i>>3));
 memcpy(disk,cart,n);memcpy(source,cart,n);
 memcpy(header+0xa0,"EEPROM TEST ABEJ",16);
 memcpy(id,header+0xa0,16);
}
int main(void){
 uint8_t *memory=mmap((void *)0x11000000u,0x9000u,
  PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);
 assert(memory!=MAP_FAILED);
 uint8_t *source=memory,*backup=memory+0x3000,*id=memory+0x6000;
 of_save162_report *r=(void *)(memory+0x6100);
 for(unsigned n=512;n<=8192;n*=16){
  setup(n,source,id);
  assert(!eeprom162_restore_service(source,backup,n,id,r,sizeof *r));
  assert(!writes&&r->verified==n&&file_reads==n/512);
  source[0]^=0x13;source[n-1]^=0x24;
  assert(!eeprom162_restore_service(source,backup,n,id,r,sizeof *r));
  assert(r->attempted==16&&r->verified==n&&writes==2&&!memcmp(cart,source,n));
  setup(n,source,id);wrong_type=1;
  assert(eeprom162_restore_service(source,backup,n,id,r,sizeof *r)==CB_NOR_UNSUPPORTED);
  assert(!writes&&!file_reads);
  setup(n,source,id);disk[0]^=1;
  assert(eeprom162_restore_service(source,backup,n,id,r,sizeof *r)==CB_NOR_IO);
  assert(!writes);
  setup(n,source,id);unstable=1;
  assert(eeprom162_restore_service(source,backup,n,id,r,sizeof *r)==CB_NOR_IO);
  assert(!writes);
  setup(n,source,id);source[0]^=1;fail_write=1;
  assert(eeprom162_restore_service(source,backup,n,id,r,sizeof *r)==CB_NOR_IO);
  assert(r->restore_verified==n&&!memcmp(cart,disk,n));
#ifdef CB_SAVE171
  unsigned type=n==512?1:2;
  for(unsigned classification=1;classification<=2;classification++){
  for(unsigned fill=0;fill<=255;fill+=255){
   setup(n,source,id);unknown=classification;memset(cart,fill,n);memcpy(disk,cart,n);
   memset(source,fill,n);source[0]^=1;source[n-1]^=2;
   assert(eeprom162_restore_service(source,backup,n,id,r,sizeof *r)==CB_NOR_UNSUPPORTED);
   assert(!writes);
   assert(!eeprom171_restore_selected(source,backup,n,id,r,sizeof *r,type));
   assert(r->verified==n&&writes==2&&!memcmp(cart,source,n));
  }
  for(unsigned fault=0;fault<7;fault++){
   setup(n,source,id);unknown=classification;memset(cart,255,n);memcpy(disk,cart,n);
   if(fault==0)detect_error=1;
   if(fault==1)serial_stage=8;
   if(fault==2)dirty_cleanup=1;
   if(fault==3)cart[n-1]=0;
   if(fault==4)unstable=1;
   if(fault==5)id[0]^=1;
   if(fault==6){unknown=0;wrong_type=1;}
   assert(eeprom171_restore_selected(source,backup,n,id,r,sizeof *r,type));
   assert(!writes);
  }
  setup(n,source,id);unknown=classification;memset(cart,255,n);memcpy(disk,cart,n);
  assert(eeprom171_restore_selected(source,backup,n,id,r,sizeof *r,type==1?2:1));
  assert(!writes);
  fail_write=1;
  assert(eeprom171_restore_selected(source,backup,n,id,r,sizeof *r,type));
  assert(r->restore_verified==n&&!memcmp(cart,disk,n));
  }
#endif
 }
 puts("EEPROM162 service PASS: 512/8192, file and type guards, repeated read, write/readback, rollback");
#ifdef CB_SAVE171
 puts("EEPROM171 selected PASS: blank 00/FF both sizes, ordinary path locked, exclusion/cleanup/identity/instability/conflict guards, rollback");
#endif
}
