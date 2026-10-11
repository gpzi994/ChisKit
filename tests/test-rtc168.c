#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../app/gba_dump53/md5.h"
#include "ui.h"
#include "of_rtc167.h"
#include "of_input.h"
of_input_state_t _of_input_p0;
static struct cb_ui state,*ui=&state;
static void redraw(void){}
static void (*paint)(void)=redraw;
static of_rtc167_report report;
static int read_rc,identity_ok=1,output_rc,polls,reads,opens;
static unsigned keys[4];
static unsigned char output[131088],buffer[65536];static unsigned output_size;
static int clock_bad,corrupt;
static long fake_size(const char *p){(void)p;return output_size;}
static struct {long (*file_size)(const char *);} svc={fake_size};
static int fake_clock(clockid_t id,struct timespec *t){assert(id==CLOCK_REALTIME);t->tv_sec=clock_bad?0:1704096000;t->tv_nsec=0;return 0;}
static int backup_read(uint32_t off,void *p,uint32_t n){assert(off+n<=output_size);memcpy(p,output+off,n);if(corrupt&&n)((unsigned char*)p)[0]^=1;return 0;}
static int cart_read(uint32_t a,void *dst,uint32_t n,uint32_t timeout){(void)a;(void)timeout;memset(dst,0,n);return 0;}
static int fake_parse(struct cb_cart *c,const uint8_t *h){(void)h;*c=ui->cart;return identity_ok;}
static int fake_idle(void){return 0;}
static int fake_rtc(of_rtc167_report *r){reads++;*r=report;return read_rc;}
static void fake_poll(void){assert(polls<4);_of_input_p0.buttons=keys[polls++];}
static int fake_sleep(unsigned us){(void)us;return 0;}
static int fake_write(uint32_t off,const void *p,uint32_t n){assert(off==131072&&n==16);if(output_rc)return output_rc;memcpy(output+off,p,n);return 0;}
#define cb_parse_gba fake_parse
#define of_cart_enter_safe_idle fake_idle
#define of_rtc167_read fake_rtc
#define of_input_poll_p0 fake_poll
#define usleep fake_sleep
#define of_output_write fake_write
#undef OF_SVC
#define OF_SVC (&svc)
#define clock_gettime fake_clock
#include "../app/ui98/rtc168_transfer.h"
static void reset(void){
 memset(&state,0,sizeof state);memset(&report,0,sizeof report);
 strcpy(state.cart.title,"POKEMON EMER");strcpy(state.cart.code,"BPEE / 01");state.cart.result=CB_READY;
 strcpy(state.path,"/Saves/chisburner/common/emerald.sav");
 report.version=167;report.flags=3;report.status=0x6a;
 const uint8_t date[]={0x26,9,0x27,0,2,0x17,0x19};memcpy(report.date,date,7);
 read_rc=output_rc=polls=reads=opens=0;identity_ok=1;memset(keys,0,sizeof keys);
 clock_bad=corrupt=0;output_size=sizeof output;state.total=131072;memset(output,0xa5,sizeof output);
}
int main(void){
 int include;
 reset();keys[1]=OF_BTN_A;assert(!rtc168_choose(&include)&&include&&polls==3&&reads==1&&!state.confirm);
 assert(!rtc168_backup()&&reads==2&&!opens&&state.total==131088);
 assert(!memcmp(output+131072,report.date,7)&&output[131079]==0x6a);
 uint64_t epoch=0;for(unsigned i=0;i<8;i++)epoch|=(uint64_t)output[131080+i]<<(8*i);assert(epoch==1704067200);
 for(unsigned i=0;i<131072;i++)assert(output[i]==0xa5);
 reset();keys[1]=OF_BTN_B;assert(!rtc168_choose(&include)&&!include&&polls==3&&!opens);
 reset();report.flags=0;assert(!rtc168_choose(&include)&&!include&&!polls&&!opens);
 reset();report.flags=5;report.status=0xff;assert(!rtc168_choose(&include)&&!include&&!polls);
 reset();read_rc=-3;assert(rtc168_choose(&include)==-3&&!include&&!polls);
 reset();report.cleanup_rc=-4;assert(rtc168_choose(&include)==-4&&!polls);
 reset();identity_ok=0;assert(rtc168_choose(&include)&&!reads&&!polls);
 reset();report.flags=0;assert(rtc168_backup()&&!opens);
 reset();output_rc=-4;assert(rtc168_backup()==-4&&!opens);
 reset();clock_bad=1;assert(rtc168_backup());
 reset();corrupt=1;assert(rtc168_backup());
 reset();output_size--;assert(rtc168_backup());
 puts("RTC169 PASS: unchanged A/B and eligibility, fresh cart RTC, exact 16-byte footer, UTC+8 conversion, unchanged payload, readback and failure guards (mock services)");
}
