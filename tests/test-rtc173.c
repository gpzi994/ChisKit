#define OF_PC
#define OF_CART_SVC_HAS(x) 1
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../app/ui98/ui.h"
#include "of_input.h"
#include "of_rtc172.h"
#include "../app/ui98/rtc167_ui.h"
#include "../app/ui98/rtc168_policy.h"
#include "../core/rtc/restore172.h"
static struct cb_ui state,*ui=&state;
static of_rtc167_report rtc168_capture;
static of_rtc172_report rtc172_report;
of_input_state_t _of_input_p0;
static unsigned keys[32],pos,writes;
static int fail,probe_fail,parse_bad,identity_bad,post_identity_bad;
static unsigned read_count;
static time_t stamp;
static void paint(void){}
static int safe_mock(void){return probe_fail==1?-13:0;}
#define of_cart_enter_safe_idle safe_mock
static int cart_read(unsigned a,void *p,unsigned n,unsigned t){
 (void)t;read_count++;
 if(n!=32||(a&31))return -13; /* Match deployed FPGA read contract. */
 assert(a<=0xa0);
 if((probe_fail==2&&read_count==1)||(probe_fail==3&&read_count==3)||(probe_fail==9&&read_count==8))return -13;
 memset(p,0xa5,n);if(post_identity_bad&&read_count==8)((uint8_t*)p)[0]^=1;return 0;
}
int cb_parse_gba(struct cb_cart *c,const uint8_t *h){(void)h;memset(c,0,sizeof *c);c->result=CB_READY;if(identity_bad)strcpy(c->title,"changed");return !parse_bad;}
static of_rtc167_report capture_fixture;
static int of_rtc167_read(of_rtc167_report *r){*r=capture_fixture;return probe_fail==6?-13:0;}
static unsigned now(void){return 1;}
static int of_output_open(const char *p,uint32_t n){(void)p;(void)n;return 0;}
static int of_output_write(uint32_t o,const void *p,uint32_t n){(void)o;(void)p;(void)n;return 0;}
static int cb_output_verify161(const void *p,uint32_t n){(void)p;(void)n;return 0;}
static void poll(void){assert(pos<32);_of_input_p0.buttons=keys[pos++];}
static int sleep_mock(unsigned n){(void)n;return 0;}
static int clock_mock(clockid_t c,struct timespec *t){(void)c;t->tv_sec=stamp;return 0;}
static int write_mock(const uint8_t *t,const uint8_t *id,of_rtc172_report *r){
 assert(id[0]==0xa5);writes++;r->version=172;r->attempted=1;r->verified=!fail;
 r->after.version=167;r->after.flags=3;r->after.status=0x40;memcpy(r->after.date,t,7);return fail;
}
#define of_input_poll_p0 poll
#define of_rtc172_write write_mock
#define clock_gettime clock_mock
#define usleep sleep_mock
#include "../app/ui98/rtc173_transfer.h"
static void reset(void){
 memset(&state,0,sizeof state);memset(keys,0,sizeof keys);pos=writes=read_count=0;fail=probe_fail=parse_bad=identity_bad=post_identity_bad=0;
 rtc168_capture.version=167;rtc168_capture.flags=3;rtc168_capture.status=0x40;
 uint8_t d[]={0x24,2,0x29,4,0x10,0,0};memcpy(rtc168_capture.date,d,7);
 rtc168_capture.cleanup_rc=0;capture_fixture=rtc168_capture;
}
int main(void){
 reset();uint8_t short_read[16];assert(cart_read(0xa0,short_read,16,50000)==-13);
 reset();post_identity_bad=1;assert(rtc173_edit()==OF_ERR_IO&&!writes);
 reset();keys[1]=OF_BTN_A;keys[3]=OF_BTN_A;assert(!rtc173_edit()&&writes==1);
 reset();keys[1]=OF_BTN_B;assert(rtc173_edit()==OF_ERR_DENIED&&!writes);
 reset();capture_fixture.flags=0;assert(rtc173_edit()==OF_ERR_NOT_SUPPORTED&&!writes);
 reset();keys[1]=OF_BTN_A;keys[3]=OF_BTN_A;fail=-4;assert(rtc173_edit()==-4&&strstr(ui->notice,"may have changed"));
 reset();keys[1]=OF_BTN_UP;keys[3]=OF_BTN_A;keys[5]=OF_BTN_A;
 assert(!rtc173_edit()&&ui->rtc_edit[0]==2025&&ui->rtc_edit[2]==28);
 for(unsigned h=0;h<24;h++){
  stamp=1704067200+(time_t)h*3600;assert(!rtc173_pocket()&&ui->rtc_edit[3]==h);
  uint8_t t[8];rtc173_target(t);assert(cb_rtc172_bcd(t[4]&0x7f)==(int)h&&t[3]==1);
 }
 stamp=0;assert(rtc173_pocket());
 for(int stage=1;stage<=9;stage++){
  reset();probe_fail=stage;
  if(stage==4)parse_bad=1;
  if(stage==5)identity_bad=1;
  if(stage==7)capture_fixture.cleanup_rc=-13;
  if(stage==8)capture_fixture.flags=0;
  assert(rtc173_edit()<0&&!writes&&rtc173_diag.stage==(unsigned)stage);
 }
 puts("RTC173 PASS: aligned 32-byte contract, post-RTC identity, nine failures, manual, cancel, absent RTC, write failure, leap clamp, all 24 hours; mock services");
}
