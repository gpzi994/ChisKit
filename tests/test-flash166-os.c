#define _GNU_SOURCE
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include "../core/save/probe139.h"
enum {CB_NOR_ARGUMENT=-1,CB_NOR_IO=-2,CB_NOR_UNSUPPORTED=-3,CB_NOR_CANCELLED=-4,CB_NOR_TIMEOUT=-5};
static bool busy,private_owned;static unsigned job_token,mapper_kind;
static struct {bool authorized;}job;
static unsigned CART_CAPS=0x100bf,tick,bank,seq,writes,reads,fail_write,fail_read,drop_read;
static int live=1,probe_error;static unsigned probe_id=0xc209;
static int alive(void*x){(void)x;return live;}
static unsigned now_ms(void*x){(void)x;return tick;}
static int enter(int x){assert(x==2);private_owned=true;return live?0:CB_NOR_CANCELLED;}
static int leave(void){private_owned=false;return 0;}
static int save139_alive(void*x){return alive(x);}
static int save139_delay(void*x,unsigned ms){(void)x;tick+=ms;return live?0:CB_NOR_CANCELLED;}
static int save139_write(void*x,uint32_t a,uint8_t v){
 (void)x;if(!live)return CB_NOR_CANCELLED;
 ++writes;if(writes==fail_write)return CB_NOR_IO;
 if(a==0&&v==0xf0){seq=0;return 0;}
 if(seq==0){assert(a==0x5555&&v==0xaa);seq=1;}
 else if(seq==1){assert(a==0x2aaa&&v==0x55);seq=2;}
 else if(seq==2){assert(a==0x5555&&v==0xb0);seq=3;}
 else{assert(a==0&&v<=1);bank=v;seq=0;}
 return 0;
}
static int save139_read(void*x,uint32_t a,uint8_t*v){
 (void)x;assert(a<65536&&!seq);reads++;
 if(reads==drop_read)live=0;
 if(!live)return CB_NOR_CANCELLED;
 if(reads==fail_read)return CB_NOR_IO;
 *v=(uint8_t)(a*31+a/257+bank*97);return 0;
}
struct cb_save139_flash_report cb_save139_probe_flash(const struct cb_save139_flash_ops *o){
 (void)o;struct cb_save139_flash_report r={0};r.id=probe_id;r.id_read=1;r.exit_verified=1;r.rc=probe_error;return r;
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/flash166.h"
static void reset(void){tick=bank=seq=writes=reads=fail_write=fail_read=drop_read=0;live=1;probe_error=0;probe_id=0xc209;}
int main(void){
 uint8_t *p=mmap((void*)0x11000000,32768,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);assert(p!=MAP_FAILED);
 for(unsigned id=0;id<2;id++)for(unsigned off=0;off<131072;off+=32768){
  reset();probe_id=id?0x6213:0xc209;
  assert(!flash166_read(0x20000000|off,p,32768,10000000));
  for(unsigned i=0;i<32768;i++){unsigned a=(off+i)&65535;assert(p[i]==(uint8_t)(a*31+a/257+((off+i)>>16)*97));}
  assert(!bank&&!busy&&!private_owned&&!seq);
 }
 reset();assert(!flash166_read(0x2000fff0,p,32,10000000));assert(p[16]==97&&!bank);
 for(unsigned f=1;f<=9;f++){
  reset();fail_write=f;assert(flash166_read(0x20010000,p,32,10000000)!=0);assert(!busy&&!private_owned);
  for(unsigned i=0;i<32;i++)assert(!p[i]);
 }
 reset();fail_read=17;assert(flash166_read(0x20010000,p,32,10000000)==CB_NOR_IO);assert(!bank);
 reset();drop_read=17;assert(flash166_read(0x20010000,p,32,10000000)==CB_NOR_CANCELLED);assert(writes==4);
 reset();assert(flash166_read(0x20010000,p,32,1000)==CB_NOR_TIMEOUT);assert(!bank);
 reset();probe_id=0x1f3d;assert(flash166_read(0x20000000,p,32,10000000)==CB_NOR_UNSUPPORTED);assert(!writes);
 reset();probe_error=-77;assert(flash166_read(0x20000000,p,32,10000000)==-77);assert(!writes);
 reset();assert(flash166_read(0x2001ffff,p,2,10000000)==CB_NOR_ARGUMENT);
 assert(flash166_read(0xa0000000,p,32,10000000)==CB_NOR_ARGUMENT);
 assert(flash166_read(0x60000000,p,32,10000000)==CB_NOR_ARGUMENT);
 assert(flash166_read(0x20000000,(void*)0x20000000,32,10000000)==CB_NOR_ARGUMENT);assert(!writes);
 munmap(p,32768);puts("Flash166 OS adapter PASS: both IDs/banks, boundary, cleanup/write/read/lifecycle/timeout faults, SDRAM and flag guards (mock bus/probe)");
}
