/* Production capacity and ROM backup, using the user's expanded Emerald ROM. */
#define OF_PC 1
#include "ui.h"
#include "capacity164.h"
#include "../app/gba_db55/db55.h"
#include "of_file.h"
#include "of_output.h"
#include "of_input.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
static struct of_services_table svc;
#undef OF_SVC
#define OF_SVC (&svc)
#undef OF_CART_SVC_HAS
#define OF_CART_SVC_HAS(f) (svc.f!=0)
static of_input_state_t _of_input_p0;
static uint8_t *rom,*output;
static uint32_t tick,keys_at,file_bytes,opened,written,read_count,probes;
static int probe_result,safe_error,missing_service,change_after,read_error;
static uint32_t physical=0x2000000u,accessible=0x2000000u;
static uint32_t clock_ms(void){return tick;}
static int sleep_us(unsigned n){tick+=n/1000;return 0;}
static void poll(void){_of_input_p0.buttons=tick-keys_at<10?0:OF_BTN_A;}
static int safe(void){return safe_error;}
static int safe_ctx(void *ctx){(void)ctx;return safe();}
static int nor(void *ctx,of_nor_info_t *n){
 (void)ctx;probes++;memset(n,0,sizeof *n);
 if(missing_service)return -2;
 n->struct_size=sizeof *n;n->version=OF_NOR_VERSION;n->bus=2;n->probe_rc=probe_result;
 if(!probe_result){n->flags=OF_NOR_ID_OK;n->flash_bytes=physical;n->accessible_bytes=accessible;strcpy(n->model,"S29GL256S");}
 return probe_result;
}
static int probe_nor(uint32_t bus,of_nor_info_t *n){assert(bus==2);return nor(0,n);}
static int read_cart(of_cart_bus_t bus,uint32_t a,void *p,uint32_t n,uint32_t timeout){
 assert(bus==2&&n==32&&timeout==50000&&a+n<=0x2000000u);read_count++;
 if(read_error)return -9;
 memcpy(p,rom+a,n);
 if(change_after&&probes&&a==0)((uint8_t*)p)[0]^=1;
 return 0;
}
static int read_ctx(void *ctx,unsigned bus,uint32_t a,void *p,unsigned n){(void)ctx;return read_cart(bus,a,p,n,50000);}
static int initial_header(void *ctx,unsigned bus,uint32_t a,void *p,unsigned n){
 (void)ctx;if(bus==1){memset(p,0,n);return 0;}
 assert(a+n<=192);return read_ctx(0,bus,a,p,n);
}
static const struct cb_capacity164_ops ops={{0,read_ctx,safe_ctx,0},nor};
static long size_file(const char *p){assert(!strcmp(p,"slot:5"));return file_bytes;}
static int open_output(const char *p,uint32_t n){
 assert(strstr(p,"POKEMON_EMER")&&strstr(p,".gba"));file_bytes=n;opened++;return 0;
}
static int write_output(uint32_t off,const void *p,uint32_t n){
 assert(n==65536&&off+n<=file_bytes);memcpy(output+off,p,n);written+=n;return 0;
}
static int async_busy(void){return 0;}
static int async_poll(void){return 0;}
static int async_read(uint32_t slot,uint32_t off,void *p,uint32_t n,void(*cb)(int,int)){
 assert(slot==5&&n<=32768&&off+n<=file_bytes);memcpy(p,output+off,n);cb(42,0);return 42;
}
static int caps(of_cart_caps_t *c){(void)c;assert(0);return -1;}
static int flash(of_cart_flash_caps_t *f){(void)f;assert(0);return -1;}
static int erase(uint32_t a,uint32_t t){(void)a;(void)t;assert(0);return -1;}
static int program(uint32_t a,const void *p,uint32_t n,uint32_t t){(void)a;(void)p;(void)n;(void)t;assert(0);return -1;}
#define usleep sleep_us
#define of_input_poll_p0 poll
#define of_cart_get_caps caps
#define of_cart_flash_get_caps flash
#define of_cart_enter_safe_idle safe
#define of_cart_flash_disarm safe
#define of_cart_flash_erase_sector erase
#define of_cart_flash_program program
#define of_cart_read read_cart
#define of_nor_probe probe_nor
#define of_output_open open_output
#define of_output_write write_output
#define of_file_async_busy async_busy
#define of_file_async_poll async_poll
#define of_file_read_async async_read
#include "../app/ui98/transfer.c"
int cb_program32(struct cb_ui *u,int(*c)(void)){(void)u;(void)c;assert(0);return -1;}
int cb_program8(struct cb_ui *u,int(*c)(void)){(void)u;(void)c;assert(0);return -1;}
static void render(void){}
static int analyze(struct cb_ui *u){
 memset(u,0,sizeof *u);assert(cb_parse_gba(&u->cart,rom));u->cart.header_valid=1;probes=0;
 return cb_gba_analyze164(&u->cart,&u->analysis,&ops);
}
static void dump(struct cb_ui *u,uint32_t want){
 opened=written=0;tick=keys_at=0;
 cb_transfer(u,2,render);
 assert(u->task_rc==0&&opened==1&&file_bytes==want&&written==want);
 assert(!memcmp(output,rom,want));
}
int main(int argc,char **argv){
 assert(argc==2);rom=malloc(0x2000000);output=malloc(0x2000000);assert(rom&&output);
 FILE *f=fopen(argv[1],"rb");assert(f);assert(fread(rom,1,0x2000000,f)==0x2000000);fclose(f);
 struct db55_result db;db55_identify_header(rom,&db);
 assert(db55_validate_index()&&db.entry&&db.entry->rom_size==0x1000000);
 assert(db.logo_ok&&db.fixed96_ok&&db.checksum_ok);
 svc.timer_get_ms=clock_ms;svc.file_size=size_file;svc.output_open=open_output;svc.output_write=write_output;
 struct cb_ui u;
 const struct cb_probe_ops headops={0,initial_header,safe_ctx,0};
 cb_probe(&u.cart,&headops);assert(u.cart.result==CB_READY&&u.cart.header_valid&&!u.cart.rom_bytes);
 {const uint8_t key[20]={0xb6,0x12,0xd5,0x35,0x90,0x3c,0x3f,0x9c,0xc1,0x42,0x22,0xdc,0xec,0x62,0x3a,0x55,0x72,0xa9,0xeb,0x04};
 const struct db55_entry *minish=db55_lookup(key);assert(minish&&minish->rom_size==0x1000000&&memcmp(minish->game_code,"BZMJ",4)==0);}
 assert(!analyze(&u)&&u.cart.rom_source==1&&u.cart.rom_bytes==0x2000000);
 assert(!u.analysis.writable); /* Read does not require programming authority. */
 dump(&u,0x2000000);
 puts("PASS actual expanded Emerald: DB 16 MiB + NOR 32 MiB => full 32 MiB backup, byte equal");
 probe_result=-2;assert(!analyze(&u)&&u.cart.rom_source==2&&u.cart.rom_bytes==0x1000000);
 dump(&u,0x1000000);
 puts("PASS completed NOR miss: matching database => 16 MiB backup");
 rom[0x170]^=1;assert(!analyze(&u)&&u.cart.rom_source==3&&u.cart.rom_bytes==0x2000000);rom[0x170]^=1;
 probe_result=-4;assert(analyze(&u)==-4&&!u.cart.rom_bytes&&!u.cart.rom_source);
 probe_result=-2;missing_service=1;assert(analyze(&u)&&!u.cart.rom_bytes);missing_service=0;
 probe_result=0;accessible=0;assert(analyze(&u)&&!u.cart.rom_bytes);
 accessible=physical=0x4000000;assert(analyze(&u)&&!u.cart.rom_bytes);
 accessible=0x2000000;assert(!analyze(&u)&&u.cart.rom_bytes==accessible);physical=accessible;
 change_after=1;assert(analyze(&u)&&!u.cart.rom_bytes);change_after=0;
 read_error=1;assert(analyze(&u)==-9);read_error=0;
 safe_error=-9;assert(analyze(&u)==-9);safe_error=0;
 assert(!analyze(&u));probe_result=-2;opened=0;cb_transfer(&u,2,render);assert(u.task_rc&&!opened);
 probe_result=0;assert(!analyze(&u));rom[0x170]^=1;opened=0;cb_transfer(&u,2,render);assert(u.task_rc&&!opened);rom[0x170]^=1;
 memset(&u,0x5a,sizeof u);cb_reset164(&u);
 assert(!u.cart.title[0]&&!u.cart.rom_bytes&&!u.analysis.matched&&!u.analysis.writable);
 assert(!u.analysis.nor.generation&&u.save_detected_type==-1&&!u.save_detected_bytes&&!u.save_profile);
 assert(!u.path[0]&&!u.hash[0]&&!u.sha256[0]&&!u.source_title[0]&&!u.save_rom_hint[0]);
 for(unsigned i=2;i<=5;i++)assert(!cb_action_enabled(&u,i));
 /* GB/GBC headers still govern dump length, independently of NOR size. */
 for(unsigned kind=0;kind<5;kind++){
  static const uint8_t types[]={0,1,0x10,0x1b,0x22};
  uint8_t h[96]={0};memcpy(h+4,cb_gb_logo,48);memcpy(h+0x34,"GB_TEST",7);
  h[0x47]=types[kind];h[0x48]=kind?6:0;h[0x49]=kind==2?5:0;
  for(unsigned i=0x34;i<=0x4c;i++)h[0x4d]-=h[i]+1;
  memset(&u,0,sizeof u);assert(cb_parse_gb(&u.cart,h));
  u.analysis.bytes=0x800000;
  assert(u.cart.rom_bytes==(kind?0x200000u:0x8000u));
  if(kind==2)assert(!strcmp(u.cart.mapper,"MBC30"));
 }
 puts("PASS no-match fallback, transport/geometry errors, changed card, refresh clears metadata and actions");
 puts("PASS GB/GBC header capacities retained");
 free(rom);free(output);return 0;
}
