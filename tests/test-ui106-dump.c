/* Execute the production transfer orchestration with mocked OS boundaries. */
#define OF_PC 1
#include "ui.h"
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
static uint32_t ms,keys_at,opens,writes,reads,source_reads;
static int cancel,output_error,read_error,corrupt_file,paints,write_error;
static int cancel_mid,header_corrupt,size_error,reread_error;
static uint8_t *file_image, *rom_image;
static unsigned next_read; static int cart_corrupt, safe_error;
static unsigned byte_at(uint32_t a){return ((a*13u)^(a>>8)^0x5a)&255;}
static uint32_t clock_ms(void){return ms;}
static long file_size(const char*p){assert(!strcmp(p,"slot:4")||!strcmp(p,"slot:5"));return size_error?0:0x200000;}
static int sleep_us(unsigned n){ms+=n/1000;return 0;}
static void poll(void){
 uint32_t t=ms-keys_at;assert(t<15000);
 _of_input_p0.buttons=(cancel||(cancel_mid&&writes))?OF_BTN_B:t<200?OF_BTN_A:t<400?0:OF_BTN_A;
}
static int get_caps(of_cart_caps_t*c){memset(c,0,sizeof(*c));c->cart_caps_raw=0x02313f03;c->bus_mask=3;c->operation_mask=63;c->gba_space_mask=c->gb_gbc_space_mask=17;c->writes_enabled=1;return 0;}
static int get_flash(of_cart_flash_caps_t*f){memset(f,0,sizeof(*f));f->struct_size=sizeof(*f);f->version=OF_CART_FLASH_CAPS_VERSION;f->id_prefix=0x227e0001;f->cfi_signature=0x00520051;f->command_set=OF_CART_FLASH_CMDSET_AMD;f->profile_status=1;f->flash_size_bytes=0x2000000;f->sector_size_bytes=0x20000;f->program_buffer_bytes=512;return 0;}
static int safe(void){next_read=0;return safe_error?-7:0;}
static int erase(uint32_t a,uint32_t t){(void)a;(void)t;assert(0);return -1;}
static int program(uint32_t a,const void*p,uint32_t n,uint32_t t){(void)a;(void)p;(void)n;(void)t;assert(0);return -1;}
static int read_cart(of_cart_bus_t b,uint32_t a,void*p,uint32_t n,uint32_t t){
 assert(b==1&&n==32&&t==50000&&a>=0x100&&a+n<=0x160);memcpy(p,rom_image+a,n);if(header_corrupt)((uint8_t*)p)[0]^=1;return 0;
}
static int read_space(of_cart_bus_t b,of_cart_space_t space,uint32_t a,void*p,uint32_t n,uint32_t t){
 assert(b==1&&space==OF_CART_SPACE_IDENTIFY&&a==0x200000+next_read&&n==32&&t==50000);
 assert(next_read+n<=0x200000);reads++;if(read_error)return -9;
 memcpy(p,rom_image+next_read,n);if(cart_corrupt&&next_read==0x1fffe0)((uint8_t*)p)[31]^=1;
 next_read+=32;return 0;
}static int open_output(const char*p,uint32_t n){assert(!strncmp(p,"/Saves/chisburner/common/cb106-",29)&&n==0x200000);opens++;return output_error?-9:0;}
static int write_output(uint32_t a,const void*p,uint32_t n){assert(n==65536&&a+n<=0x200000);writes++;if(write_error)return -2;memcpy(file_image+a,p,n);return 0;}
static int async_busy(void){return 0;}
static int async_poll(void){return 0;}
static int async_read(uint32_t slot,uint32_t a,void*p,uint32_t n,void(*cb)(int,int)){
 assert(slot==5&&(n==65536||(n==32&&a==0x200000-32))&&a+n<=0x200000);source_reads++;memcpy(p,file_image+a,n);
 if(corrupt_file&&a==0)((uint8_t*)p)[0]^=1;cb(42,reread_error?-2:0);return 42;
}
#define usleep sleep_us
#define of_input_poll_p0 poll
#define of_cart_get_caps get_caps
#define of_cart_flash_get_caps get_flash
#define of_cart_enter_safe_idle safe
#define of_cart_flash_disarm safe
#define of_cart_flash_erase_sector erase
#define of_cart_flash_program program
#define of_cart_read read_cart
#define of_cart_read_space read_space
#define of_output_open open_output
#define of_output_write write_output
#define of_file_async_busy async_busy
#define of_file_async_poll async_poll
#define of_file_read_async async_read
#include "../app/ui98/transfer.c"
int cb_program32(struct cb_ui*u,int(*c)(void)){(void)u;(void)c;assert(0);return -1;}
int cb_program8(struct cb_ui*u,int(*c)(void)){(void)u;(void)c;assert(0);return -1;}
static void paint_test(void){paints++;assert(ui->page==CB_TRANSFER);}
static void run(struct cb_ui*u){
 /* Independent test cases model a fresh core session. Async transport
  * failures intentionally poison the current production session. */
 poisoned=0;
 *u=(struct cb_ui){0};assert(cb_parse_gb(&u->cart,rom_image+0x100));
 ms=100;keys_at=ms;opens=writes=reads=source_reads=0;next_read=0;paints=0;
 cb_transfer(u,2,paint_test);assert(u->finished&&!u->busy&&!u->analysis.writable&&paints);
}
int main(int argc,char **argv){
 assert(argc==3);file_image=malloc(0x200000);rom_image=malloc(0x200000);assert(file_image&&rom_image);
 svc.timer_get_ms=clock_ms;svc.file_size=file_size;svc.output_open=open_output;svc.output_write=write_output;
 struct cb_ui u;
 for(unsigned f=1;f<=2;f++){
  FILE *in=fopen(argv[f],"rb");assert(in);assert(fread(rom_image,1,0x200000,in)==0x200000);assert(fgetc(in)==EOF);fclose(in);
  run(&u);assert(!u.task_rc&&writes==32&&reads==65536&&source_reads==33&&strlen(u.sha256)==64);
  assert(!memcmp(file_image,rom_image,0x200000));
  cancel=1;run(&u);assert(u.task_rc&&!opens&&!reads);cancel=0;
  cancel_mid=1;run(&u);assert(u.task_rc&&opens==1&&writes==1&&reads==2048&&strstr(u.failure_phase,"Cancelled"));cancel_mid=0;
  header_corrupt=1;run(&u);assert(u.task_rc&&!opens&&!reads);header_corrupt=0;
  size_error=1;run(&u);assert(u.task_rc==OF_ERR_BAD_RANGE&&opens==1&&!writes);size_error=0;
  reread_error=1;run(&u);assert(u.task_rc&&opens==1&&!writes);reread_error=0;
  read_error=1;run(&u);assert(u.task_rc&&!opens&&reads==1);read_error=0;
  output_error=1;run(&u);assert(u.task_rc&&opens==1&&!writes);output_error=0;
  write_error=1;run(&u);assert(u.task_rc&&writes==1);write_error=0;
  corrupt_file=1;run(&u);assert(u.task_rc==OF_ERR_IO&&strstr(u.failure_phase,"File differs"));corrupt_file=0;
  cart_corrupt=1;run(&u);assert(u.task_rc==OF_ERR_DENIED&&strstr(u.failure_phase,"ROM differs"));cart_corrupt=0;
  safe_error=1;run(&u);assert(u.task_rc&&!opens);safe_error=0;
 }
 free(file_image);free(rom_image);puts("PASS production GB dump: both real ROMs, exact file equality and SHA256, cancellation, read/output/safe failures, file and cart corruption, no erase/program");
}
