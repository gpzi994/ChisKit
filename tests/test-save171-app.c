#define OF_PC
#include "ui.h"
#include "of_save139.h"
#include "of_save171.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static struct of_services_table svc;
#undef OF_SVC
#define OF_SVC (&svc)
#undef OF_CART_SVC_HAS
#define OF_CART_SVC_HAS(f) (available)
static struct cb_ui state,*ui=&state;
static uint8_t buffer[65536],save138_snapshots[2][131072],cart[131072],input[131072],disk[131072];
static of_eeprom139_snapshot ee161_capture;
static unsigned bytes,opened,restores,available,accept,corrupt,source_bad,read_count;
static uint32_t began;
static long size(const char *path){return !strcmp(path,"slot:6")?bytes:opened;}
static int file_read(unsigned slot,uint32_t off,void *dst,uint32_t n){
 assert(slot==6&&off+n<=bytes);memcpy(dst,input+off,n);
 if(source_bad&&++read_count==2)((uint8_t*)dst)[0]^=1;return 0;
}
static void paint(void){}
static uint32_t now(void){return 1;}
static int sram137_cancel(void *ctx){(void)ctx;return 0;}
static int safe(void){return 0;}
static int cart_read(uint32_t off,void *p,uint32_t n,uint32_t timeout){
 (void)timeout;memset(p,0,n);if(off==160)memcpy(p,"TEST SAVE   ABCJ",16);return 0;
}
static int parse(struct cb_cart *c,const uint8_t *h){(void)h;*c=ui->cart;return 1;}
static int capture(of_eeprom139_snapshot *r){
 memset(r,0,sizeof *r);r->stable_mask=3;
 memcpy(r->small,cart,512);memcpy(r->large,cart,8192);return 0;
}
static int read_space(unsigned bus,unsigned space,uint32_t off,void *dst,uint32_t n,unsigned timeout){
 (void)space;(void)timeout;assert(bus==OF_CART_BUS_GBA);
 off&=~OF_CART_SAVE_BANKED_1M;assert(off+n<=bytes);memcpy(dst,cart+off,n);return 0;
}
static void cb_name_part(char *dst,size_t n,const char *s){snprintf(dst,n,"%s",s);}
static int output_open(const char *path,uint32_t n){assert(strstr(path,"backup.sav"));opened=n;return 1;}
static int output_write(uint32_t off,const void *p,uint32_t n){memcpy(disk+off,p,n);return 0;}
static int backup_read(uint32_t off,void *p,uint32_t n){memcpy(p,disk+off,n);if(corrupt)((uint8_t*)p)[0]^=1;return 0;}
static int confirm(void){assert(opened==bytes&&!memcmp(disk,cart,bytes));return accept;}
static int restore(unsigned type,const void *src,void *backup,uint32_t n,const uint8_t id[16],of_save171_report *r){
 assert(type==(unsigned)ui->save_selection.type&&n==bytes);
 assert(!memcmp(backup,cart,n)&&!memcmp(disk,cart,n)&&!memcmp(src,input,n));
 assert(!memcmp(id,"TEST SAVE   ABCJ",16));restores++;
 memcpy(cart,src,n);*r=(of_save171_report){.version=171,.attempted=n,.verified=n};return 0;
}
#define of_cart_enter_safe_idle safe
#define cb_parse_gba parse
#define of_eeprom139_capture capture
#define of_cart_read_space read_space
#define of_output_open output_open
#define of_output_write output_write
#define of_save171_restore restore
#include "../app/ui98/save_restore171.h"
static void setup(unsigned type,unsigned n){
 memset(&state,0,sizeof state);state.cart.platform=CB_GBA;state.cart.result=CB_READY;
 strcpy(state.cart.title,"TEST");strcpy(state.cart.code,"ABCJ");
 state.save_selection=(struct cb_save171_selection){.type=(int)type,.bytes=n,.supported=1};
 bytes=n;opened=restores=corrupt=source_bad=read_count=0;available=accept=1;
 memset(cart,255,sizeof cart);for(unsigned i=0;i<n;i++)input[i]=(uint8_t)i;
}
int main(void){
 svc.file_size=size;unsigned types[]={1,2,3,8},sizes[]={512,8192,32768,131072};
 for(unsigned i=0;i<4;i++){
  setup(types[i],sizes[i]);assert(!restore_save171()&&restores==1&&!memcmp(input,cart,bytes));
  setup(types[i],sizes[i]);accept=0;assert(restore_save171()&&!restores&&opened);
  setup(types[i],sizes[i]);corrupt=1;assert(restore_save171()&&!restores);
  setup(types[i],sizes[i]);source_bad=1;assert(restore_save171()&&!restores&&!opened);
  setup(types[i],sizes[i]);bytes++;assert(restore_save171()&&!restores&&!opened);
  setup(types[i],sizes[i]);available=0;assert(restore_save171()&&!restores&&!opened);
 }
 puts("SAVE171 app PASS: four sizes, reused backup +1, source repeat, backup verify, exact size, cancellation, missing OS");
}
