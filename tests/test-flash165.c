#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define OF_ERR_NOT_SUPPORTED -1
#define OF_ERR_IO -2
#define OF_ERR_DENIED -3
#define OF_ERR_BAD_RANGE -4
#define CB_READY 1
#define OF_CART_BUS_GBA 2
#define OF_CART_SPACE_SAVE_PARALLEL 1
#define OF_CART_SAVE_ARRAY64 0x40000000u
#define OF_CART_SAVE_FLASH128 0x20000000u
struct cb_cart {int result;char code[16],title[32];};
static struct {int save_rc,save_detected_type;unsigned save_detected_bytes,total,done,elapsed_ms;
 const char *task,*phase,*notice;char path[256],hash[40];struct cb_cart cart;} state,*ui=&state;
static uint8_t sram137_header[192],save138_snapshots[2][131072],buffer[32768],data[131072],disk[131072];
static unsigned began,reads,opens,restores;static int fault,unstable,corrupt,cancel,cleanup_error;
static int open_rc;static unsigned file_writes,file_reads;
static unsigned save169_file_size(unsigned n){return n;}
static void paint(void){} static unsigned now(void){return 0;}
static int of_cart_enter_safe_idle(void){return 0;}
static int cart_read(unsigned a,void*p,unsigned n,unsigned t){(void)a;(void)t;memset(p,0,n);return 0;}
static int cb_parse_gba(struct cb_cart*c,const void*h){(void)h;*c=ui->cart;return 1;}
static int sram137_identity(void){return 0;}
static int sram137_cancel(int x){(void)x;return cancel;}
static int sram137_restore(int x){(void)x;restores++;return cleanup_error;}
static int of_cart_read_space(int bus,int space,unsigned a,void*p,unsigned n,unsigned t){
 assert(bus==2&&space==1);
 if(state.save_detected_type==5){assert((a&0xfffe0000u)==OF_CART_SAVE_FLASH128&&n==32768&&t==10000000);a&=131071;}
 else {assert((a&0xffff0000u)==OF_CART_SAVE_ARRAY64);a&=65535;assert(n==32&&a+n<=65536&&t==50000);}
 reads++;if(fault&&reads==7)return -77;memcpy(p,data+a,n);
 if(unstable&&reads>(state.save_detected_type==5?4u:2048u))((uint8_t*)p)[0]^=1;return 0;
}
static void cb_backup_name(char*p,unsigned n,const struct cb_cart*c,const char*ext,unsigned t){(void)c;(void)ext;(void)t;snprintf(p,n,"flash.sav");}
static int of_output_open(const char*p,unsigned n){(void)p;assert(n==state.save_detected_bytes);opens++;return open_rc;}
static long file_size(const char*p){(void)p;return state.save_detected_bytes;}
static struct {long(*file_size)(const char*);} svc={file_size};
#define OF_SVC (&svc)
static int of_output_write(unsigned a,const void*p,unsigned n){file_writes++;memcpy(disk+a,p,n);return 0;}
static int backup_read(unsigned a,void*p,unsigned n){file_reads++;memcpy(p,disk+a,n);if(corrupt)((uint8_t*)p)[0]^=1;return 0;}
struct md5_53 {int x;};
static void md5_init53(struct md5_53*m){(void)m;}
static void md5_add53(struct md5_53*m,const void*p,unsigned n){(void)m;(void)p;(void)n;}
static void md5_end53(struct md5_53*m,void*p){(void)m;memset(p,0,16);}
static void md5_hex53(const void*p,char*h){(void)p;h[0]=0;}
#include "../app/ui98/flash165_transfer.h"
int main(void){
 state.cart.result=CB_READY;state.save_detected_type=4;state.save_detected_bytes=65536;
 for(unsigned i=0;i<65536;i++)data[i]=(uint8_t)(i*31+i/257);
 assert(!backup_flash165());assert(reads==4096&&opens==1&&restores==1&&!memcmp(data,disk,65536));
 for(int mode=1;mode<=4;mode++){
  reads=opens=restores=0;fault=mode==1;unstable=mode==2;cancel=mode==3;cleanup_error=mode==4?-88:0;
  assert(backup_flash165()!=0);assert(opens==0&&restores==1);
 }
 fault=unstable=cancel=cleanup_error=0;corrupt=1;assert(backup_flash165()==OF_ERR_IO);corrupt=0;
 reads=0;state.save_detected_type=5;state.save_detected_bytes=131072;
#ifdef CB_FLASH166
 for(unsigned i=0;i<131072;i++)data[i]=(uint8_t)(i*31+i/257+(i>>16)*97);
 assert(!backup_flash165());assert(reads==8&&!memcmp(data,disk,131072));
 for(int mode=1;mode<=4;mode++){
  reads=opens=restores=0;fault=mode==1;unstable=mode==2;cancel=mode==3;cleanup_error=mode==4?-88:0;
  assert(backup_flash165()!=0);assert(!opens&&restores==1);
 }
 fault=unstable=cancel=cleanup_error=0;corrupt=1;assert(backup_flash165()==OF_ERR_IO);corrupt=0;reads=0;
#else
 assert(backup_flash165()==OF_ERR_NOT_SUPPORTED&&reads==0);
#endif
 for(unsigned type=4;type<=5;type++){
#ifndef CB_FLASH166
  if(type==5)break;
#endif
  state.save_detected_type=type;state.save_detected_bytes=type==4?65536:131072;
  for(unsigned repeat=0;repeat<3;repeat++){
   open_rc=repeat?1:0;reads=file_writes=file_reads=0;
   data[17]^=0x35; /* A reused file must receive the new original snapshot. */
   assert(!backup_flash165());
   assert(file_writes==state.save_detected_bytes/32768&&file_reads==file_writes);
   assert(!memcmp(data,disk,state.save_detected_bytes));
  }
  open_rc=1;corrupt=1;assert(backup_flash165()==OF_ERR_IO);corrupt=0;
  open_rc=-71;file_writes=file_reads=0;assert(backup_flash165()==-71);
  assert(!file_writes&&!file_reads);
 }
 open_rc=0;reads=0;state.save_detected_type=4;state.save_detected_bytes=65536;state.save_rc=-3;
 assert(backup_flash165()==OF_ERR_NOT_SUPPORTED&&reads==0);
 puts("Flash backup PASS: capacities, new/reused file 0/+1, repeated update/readback, negative open and corrupt readback rejection; no cartridge write API");
}
