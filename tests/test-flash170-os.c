#define _GNU_SOURCE
#define main flash170_core_tests
#include "test-flash170.c"
#undef main
#include <sys/mman.h>
#include <stdbool.h>
#include "../core/save/probe139.h"
enum{CB_NOR_ARGUMENT=-1,CB_NOR_IO=-2,CB_NOR_UNSUPPORTED=-3,CB_NOR_CANCELLED=-4};
static bool busy,private_owned;static unsigned job_token,mapper_kind;
static struct{bool authorized;}job;
static unsigned CART_CAPS=0x100bf,CPU_FREQ_HZ=1000000;
static int file_error,header_error,probe_error;static long file_bytes;
static uint64_t read_cycles(void){return tick++;}
static int alive(void*c){(void)c;return !dead;}
static int enter(int bus){assert(bus==2);private_owned=true;return 0;}
static int leave(void){private_owned=false;return 0;}
static uint32_t now_ms(void*c){return ms(c);}
static int save139_read(void*c,uint32_t a,uint8_t*v){return rd(c,a,v);}
static int save139_write(void*c,uint32_t a,uint8_t v){return wr(c,a,v);}
static int save158_write(void*c,uint32_t a,uint8_t v){return wr(c,a,v);}
static int save139_alive(void*c){return alive(c);}
static int save139_delay(void*c,unsigned t){return delay(c,t*1000);}
static int eeprom162_header(const uint8_t*p){(void)p;return header_error;}
static int save158_sdram_range(const void*p,uint32_t n){uintptr_t a=(uintptr_t)p;return a>=0x10000000&&a<0x14000000&&n<=0x14000000-a;}
static int save158_overlaps(const void*a,unsigned n,const void*b,unsigned m){return (uintptr_t)a<(uintptr_t)b+m&&(uintptr_t)b<(uintptr_t)a+n;}
static long of_file_size(unsigned slot){assert(slot==5);return file_bytes;}
static int of_file_read(unsigned slot,unsigned off,void*p,unsigned n){assert(slot==5&&off+n<=131072);if(file_error)return file_error;memcpy(p,original+off,n);return 0;}
struct cb_save139_flash_report cb_save139_probe_flash(const struct cb_save139_flash_ops*o){
 (void)o;struct cb_save139_flash_report p={0};p.id=device;p.id_read=1;p.exit_verified=1;p.rc=probe_error;return p;
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/flash170.h"
int main(void){
 uint8_t*p=mmap((void*)0x11000000,0x60000,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);assert(p!=MAP_FAILED);
 uint8_t*src=p,*backup=p+0x20000,*id=p+0x40000;struct cb_flash170_report*r=(void*)(p+0x40100);
 reset(0xc209);file_bytes=131072;memcpy(src,source,131072);src[65536]^=1;
 assert(!flash170_restore_service(src,backup,131072,id,r,sizeof *r)&&r->verified==131072&&!bank&&!private_owned&&!busy);
 assert(!memcmp(mem,src,131072));
 reset(0x1f3d);file_bytes=65536;memcpy(src,source,65536);src[128]^=1;
 assert(!flash170_restore_service(src,backup,65536,id,r,sizeof *r)&&r->verified==65536&&!erase_count);
 reset(0xc209);assert(flash170_restore_service(src,backup,65536,id,r,sizeof *r)==CB_NOR_UNSUPPORTED&&!writes);
 reset(0x1f3d);header_error=-2;assert(flash170_restore_service(src,backup,65536,id,r,sizeof *r)==-2&&!writes);header_error=0;
 file_error=-7;assert(flash170_restore_service(src,backup,65536,id,r,sizeof *r)==-7&&!writes);file_error=0;
 probe_error=-8;assert(flash170_restore_service(src,backup,65536,id,r,sizeof *r)==-8&&!writes);probe_error=0;
 assert(flash170_restore_service(src,src,65536,id,r,sizeof *r)==-1&&!writes);
 assert(flash170_restore_service((void*)0x20000000,backup,65536,id,r,sizeof *r)==-1&&!writes);
 file_bytes=65552;assert(flash170_restore_service(src,backup,65536,id,r,sizeof *r)==-1&&!writes);
 munmap(p,0x60000);puts("Flash170 OS PASS: SDRAM guards, persistent backup, identity/ID, sizes, both algorithms, cleanup and file faults (mock bus)");
}
