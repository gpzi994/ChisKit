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
static uint8_t *file_image;
static unsigned byte_at(uint32_t a){return ((a*13u)^(a>>8)^(a>>16)^(a>>24)^0x5a)&255;}
static uint32_t clock_ms(void){return ms;}
static long input_size=0x2000000;
static long file_size(const char*p){assert(!strcmp(p,"slot:20")||!strcmp(p,"slot:5"));return !strcmp(p,"slot:20")?input_size:0x2000000;}
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
static int open_output(const char*p,uint32_t n){assert(!strncmp(p,"/Saves/chisburner/common/GBA-",27)&&n==0x2000000);opens++;return output_error?-9:0;}
static int write_output(uint32_t a,const void*p,uint32_t n){assert(n==65536&&a+n<=0x2000000);writes++;if(write_error)return -2;memcpy(file_image+a,p,n);return 0;}
static int async_busy(void){return 0;}
static int async_poll(void){return 0;}
static int async_read(uint32_t slot,uint32_t a,void*p,uint32_t n,void(*cb)(int,int)){
 assert((slot==5||slot==20)&&n<=32768&&a+n<=0x2000000);source_reads++;memcpy(p,file_image+a,n);
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
 char named[96];struct cb_cart cart={.platform=CB_GBA,.title="A/B:TEST",.code="ABCD"};
 cb_backup_name(named,sizeof(named),&cart,"sav",0x1234);
 assert(!strcmp(named,"/Saves/chisburner/common/GBA-A_B_TEST-ABCD-00001234.sav"));
 cart.title[0]=cart.code[0]=0;cb_backup_name(named,sizeof(named),&cart,"gba",0);
 assert(strstr(named,"GBA-UNKNOWN-00000000.gba"));
 file_image=malloc(0x2000000);assert(file_image);svc.timer_get_ms=clock_ms;svc.file_size=file_size;svc.output_open=open_output;svc.output_write=write_output;
 struct cb_ui u;cancel=1;run(&u);assert(u.task_rc&&!opens&&!writes&&reads==27);
 cancel=0;output_error=1;run(&u);assert(u.task_rc&&opens==1&&!writes&&reads==27);
 output_error=0;read_error=1;run(&u);assert(u.task_rc&&!opens&&!writes&&reads==1);
 read_error=0;write_error=1;run(&u);assert(u.task_rc==-2&&writes==1&&source_reads==1&&u.done==0&&u.total==0x2000000);
 assert(strstr(u.io_diag,"00000000")&&strstr(u.failure_phase,"Writing backup output"));
 write_error=0;run(&u);assert(!u.task_rc&&writes==512&&reads==1048576+27&&source_reads==1025&&strlen(u.hash)==32&&!u.io_diag[0]);
 corrupt_file=1;run(&u);assert(u.task_rc==OF_ERR_IO&&writes==512&&source_reads==1025);
 u.cart.platform=CB_GB;opens=0;cb_transfer(&u,2,paint_test);assert(!opens);
 corrupt_file=0;uint8_t sample[32];memset(file_image,0x5a,sizeof(sample));
 assert(!source_read(0,sample,sizeof(sample))&&!memcmp(sample,file_image,sizeof(sample)));
 u=(struct cb_ui){.cart={.platform=CB_GBA,.result=CB_READY},.analysis={.matched=1,.writable=1}};
 input_size=0;cb_transfer(&u,4,paint_test);assert(u.task_rc==OF_ERR_BAD_RANGE&&strstr(u.notice,"Saves ROM new"));
 u.analysis.matched=u.analysis.writable=1;
 input_size=-1;cb_transfer(&u,4,paint_test);assert(u.task_rc==OF_ERR_BAD_RANGE);
 uint8_t log[513];memset(log,0x31,sizeof log);memcpy(file_image,log,sizeof log);
 assert(!cb_output_verify161(log,sizeof log));
 memset(log,0xa5,sizeof log);memcpy(file_image,log,sizeof log);
 assert(!cb_output_verify161(log,sizeof log)); /* Same slot, new file content. */
 file_image[512]^=1;assert(cb_output_verify161(log,sizeof log)==OF_ERR_IO);
 assert(cb_output_verify161(NULL,1)==OF_ERR_INVALID_PARAM);
 free(file_image);puts("PASS: dynamic output verification, Saves slot 20 read, missing selection guard, backup, cancellation and corruption lockout");
}
