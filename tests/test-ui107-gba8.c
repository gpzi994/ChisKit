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
static uint8_t *file_image; static const uint32_t dump_bytes=0x800000;
static unsigned byte_at(uint32_t a){a%=dump_bytes;return ((a*13u)^(a>>8)^(a>>16)^(a>>24)^0x5a)&255;}
static uint32_t clock_ms(void){return ms;}
static long file_size(const char*p){assert(!strcmp(p,"slot:4")||!strcmp(p,"slot:5"));return dump_bytes;}
static int sleep_us(unsigned n){ms+=n/1000;return 0;}
static void poll(void){
 uint32_t t=ms-keys_at;assert(t<15000);
 _of_input_p0.buttons=cancel?OF_BTN_B:t<200?OF_BTN_A:t<400?0:OF_BTN_A;
}
static int get_caps(of_cart_caps_t*c){memset(c,0,sizeof(*c));c->cart_caps_raw=0x02313f03;c->bus_mask=3;c->operation_mask=63;c->gba_space_mask=c->gb_gbc_space_mask=17;c->writes_enabled=1;return 0;}
static int get_flash(of_cart_flash_caps_t*f){memset(f,0,sizeof(*f));f->struct_size=sizeof(*f);f->version=OF_CART_FLASH_CAPS_VERSION;f->id_prefix=0x227e0001;f->cfi_signature=0x00520051;f->command_set=OF_CART_FLASH_CMDSET_AMD;f->profile_status=1;f->flash_size_bytes=0x2000000;f->sector_size_bytes=0x20000;f->program_buffer_bytes=512;return 0;}
static int safe(void){return 0;}
static int erase(uint32_t a,uint32_t t){(void)a;(void)t;assert(0);return -1;}
static int program(uint32_t a,const void*p,uint32_t n,uint32_t t){(void)a;(void)p;(void)n;(void)t;assert(0);return -1;}
static int read_cart(of_cart_bus_t b,uint32_t a,void*p,uint32_t n,uint32_t t){assert(b==2&&n==32&&t==50000&&a+n<=0x2000000);reads++;if(read_error)return -9;for(unsigned i=0;i<n;i++)((uint8_t*)p)[i]=byte_at(a+i);return 0;}
static int open_output(const char*p,uint32_t n){assert(!strncmp(p,"/Saves/chisburner/common/cb102-",29)&&n==dump_bytes);opens++;return output_error?-9:0;}
static int write_output(uint32_t a,const void*p,uint32_t n){assert(n==65536&&a+n<=dump_bytes);writes++;if(write_error)return -2;memcpy(file_image+a,p,n);return 0;}
static int async_busy(void){return 0;}
static int async_poll(void){return 0;}
static int async_read(uint32_t slot,uint32_t a,void*p,uint32_t n,void(*cb)(int,int)){
 assert(slot==5&&(n==65536||(n==32&&a==dump_bytes-32))&&a+n<=dump_bytes);source_reads++;memcpy(p,file_image+a,n);
 if(corrupt_file&&a==0)((uint8_t*)p)[0]^=1;cb(42,0);return 42;
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
 *u=(struct cb_ui){.cart={.platform=CB_GBA,.result=CB_READY},.analysis={.matched=1,.writable=1}};
 ms=100;keys_at=ms;opens=writes=reads=source_reads=0;paints=0;
 cb_transfer(u,2,paint_test);assert(u->finished&&!u->busy&&!u->analysis.writable&&!u->analysis.matched&&paints);
}
int main(void){
 file_image=malloc(0x2000000);assert(file_image);svc.timer_get_ms=clock_ms;svc.file_size=file_size;svc.output_open=open_output;svc.output_write=write_output;
 struct cb_ui u;cancel=1;run(&u);assert(u.task_rc&&!opens&&!writes&&reads==31);
 cancel=0;output_error=1;run(&u);assert(u.task_rc&&opens==1&&!writes&&reads==31);
 output_error=0;read_error=1;run(&u);assert(u.task_rc&&!opens&&!writes&&reads==1);
 read_error=0;write_error=1;run(&u);assert(u.task_rc==-2&&writes==1&&source_reads==1&&u.done==0&&u.total==dump_bytes);
 assert(strstr(u.io_diag,"00000000")&&strstr(u.failure_phase,"Writing backup output"));
 write_error=0;run(&u);assert(!u.task_rc&&writes==128&&reads==262144+31&&source_reads==129&&strlen(u.hash)==32&&!u.io_diag[0]);
 corrupt_file=1;run(&u);assert(u.task_rc==OF_ERR_IO&&writes==128&&source_reads==129);
 u.cart.platform=CB_GB;opens=0;cb_transfer(&u,2,paint_test);assert(!opens);
 free(file_image);puts("PASS: two-A confirmation, production backup orchestration, cancel, open/read failure, 8 MiB mirrored ROM reread MD5+CRC, corruption and lockout");
}

