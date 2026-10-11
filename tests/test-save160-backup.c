#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define OF_ERR_DENIED -1
#define OF_ERR_IO -2
#define OF_ERR_BAD_RANGE -3
#define OF_CART_BUS_GBA 1
#define OF_CART_SPACE_SAVE_PARALLEL 2
#define OF_CART_SAVE_BANKED_1M 0x1000000u
#define CB_READY 1
#define CB_GBA 1
struct cb_cart {char title[17],code[9];int platform,result;};
static struct {uint32_t save_detected_bytes,total,done,elapsed_ms;const char *task,*phase,*notice;char path[256];struct cb_cart cart;} state,*ui=&state;
static uint8_t buffer[65536],save138_snapshots[2][131072],cart_data[131072],disk[131072];
static uint32_t began,bytes;
static unsigned disk_exists,writes,creates;
static int corrupt_readback,write_failure;
static uint32_t now(void){return 1;}
static void paint(void){}
static int check_save157(void){return 0;}
static int sram137_cancel(int x){(void)x;return 0;}
static int of_cart_enter_safe_idle(void){return 0;}
static int of_cart_read_space(int bus,int space,uint32_t a,void *p,unsigned n,unsigned timeout){
 (void)bus;(void)space;(void)timeout;memcpy(p,cart_data+(a&0xffffff),n);return 0;
}
static int cart_read(unsigned a,void *p,unsigned n,unsigned timeout){(void)a;(void)timeout;memset(p,0,n);return 0;}
static int cb_parse_gba(struct cb_cart *c,const uint8_t *h){(void)h;*c=ui->cart;return 1;}
static void cb_name_part(char *p,unsigned n,const char *s){snprintf(p,n,"%s",s);}
static int of_output_open(const char *p,unsigned n){
 assert(strstr(p,"TEST-CODE-")&&strstr(p,"KiB-backup.sav"));bytes=n;
 if(disk_exists)return 1;
 disk_exists=1;creates++;return 0;
}
static long file_size(const char *p){assert(!strcmp(p,"slot:5"));return bytes;}
static struct {long (*file_size)(const char *);} services={file_size};
#define OF_SVC (&services)
static int of_output_write(unsigned off,const void *p,unsigned n){
 if(write_failure)return -77;
 memcpy(disk+off,p,n);writes++;return 0;
}
static int backup_read(unsigned off,void *p,unsigned n){memcpy(p,disk+off,n);if(corrupt_readback)((uint8_t*)p)[0]^=1;return 0;}
#include "../app/ui98/save_backup159.h"
int main(void){
 strcpy(ui->cart.title,"TEST");strcpy(ui->cart.code,"CODE");ui->cart.result=CB_READY;
 for(unsigned n=32768;n<=131072;n*=4){
  ui->save_detected_bytes=n;disk_exists=writes=creates=0;
  memset(cart_data,0x5a,n);assert(!prepare_save159());
  assert(creates==1&&writes==n/32768&&!memcmp(cart_data,disk,n));
  unsigned previous=writes;assert(!prepare_save159());assert(writes==previous&&creates==1);
  cart_data[n-1]^=1;assert(!prepare_save159());
  assert(writes==previous+n/32768&&creates==1&&!memcmp(cart_data,disk,n));
  corrupt_readback=1;assert(prepare_save159()==OF_ERR_IO);corrupt_readback=0;
  cart_data[0]^=1;write_failure=1;assert(prepare_save159()==-77);write_failure=0;
 }
 puts("PASS backup preparation: 32/128 KiB, single file, identical reuse, changed overwrite, readback/write failures");
}
