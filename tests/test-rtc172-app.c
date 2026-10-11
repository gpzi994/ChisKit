#define OF_PC
#define OF_CART_SVC_HAS(member) 0
#define CB_RTC172 1
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../app/ui98/ui.h"
#include "of_input.h"
#include "of_rtc172.h"
#include "of_flash170.h"
#include "../app/ui98/rtc167_ui.h"
#include "../app/ui98/rtc168_policy.h"
#define CB_RTC169_UTC_OFFSET_MINUTES 480
static struct cb_ui state,*ui=&state;
static uint8_t buffer[65536],save138_snapshots[2][131072],source[131088],sram137_header[192];
static unsigned n,size,reads,tail_reads,backups,flash_calls,rtc_calls,polls;
static int flash_fail,rtc_fail,read_fail,confirm_ok,capable,rtc_capable,changed,no_op,bad_clock;
static unsigned keys[32];
static of_rtc167_report rtc168_capture;
static int flash170_original_backup;static unsigned rtc169_extra;
of_input_state_t _of_input_p0;
static long mock_size(const char *p){assert(!strcmp(p,"slot:6"));return size;}
static struct {long (*file_size)(const char *);} svc={mock_size};
#undef OF_SVC
#define OF_SVC (&svc)
#undef OF_CART_SVC_HAS
#define OF_CART_SVC_HAS(member) (strcmp(#member,"rtc172_write")?capable:rtc_capable)
static void paint(void){}
static int file_read(unsigned slot,uint32_t off,void *dst,uint32_t len){
 assert(slot==6&&off+len<=size);reads++;memcpy(dst,source+off,len);
 if(off==n){tail_reads++;if(changed&&tail_reads==3)((uint8_t*)dst)[0]^=1;}
 return 0;
}
static int rtc168_read_current(void){return read_fail;}
static void poll(void){assert(polls<32);_of_input_p0.buttons=keys[polls++];}
static int sleep_mock(unsigned t){(void)t;return 0;}
static int clock_mock(clockid_t c,struct timespec *t){(void)c;t->tv_sec=1700028860;t->tv_nsec=0;return bad_clock;}
static int backup_flash165(void){assert(flash170_original_backup&&!rtc169_extra);backups++;memset(sram137_header,0xa5,sizeof sram137_header);return 0;}
static int confirm(void){return confirm_ok;}
static int flash_mock(const void *src,void *backup,uint32_t bytes,const uint8_t *id,of_flash170_report *r){
 (void)backup;assert(backups&&bytes==n&&id[0]==0xa5&&!memcmp(src,source,n));flash_calls++;
 memset(r,0,sizeof *r);r->rc=flash_fail;r->verified=flash_fail?0:n;r->attempted=no_op?0:n;return flash_fail;
}
static int rtc_mock(const uint8_t *target,const uint8_t *id,of_rtc172_report *r){
 assert(flash_calls&&!flash_fail&&id[0]==0xa5);rtc_calls++;memset(r,0,sizeof *r);
 r->version=172;r->rc=rtc_fail;r->attempted=1;r->verified=!rtc_fail;
 r->after.version=167;r->after.flags=3;r->after.status=target[7];memcpy(r->after.date,target,7);
 return rtc_fail;
}
#define of_input_poll_p0 poll
#define usleep sleep_mock
#define clock_gettime clock_mock
#define of_rtc172_write rtc_mock
#define of_flash170_restore flash_mock
#include "../app/ui98/rtc172_transfer.h"
#include "../app/ui98/flash170_transfer.h"
static void reset(unsigned bytes,int tail,unsigned choice){
 memset(&state,0,sizeof state);memset(&rtc168_capture,0,sizeof rtc168_capture);
 n=bytes;size=n+(tail?16:0);state.save_detected_bytes=n;state.save_detected_type=n==65536?4:5;
 memset(source,0x5a,n);uint8_t f[]={0x24,1,1,1,0x10,0,0,0x40};memcpy(source+n,f,8);
 uint64_t stamp=1700000000;for(unsigned i=0;i<8;i++)source[n+8+i]=(uint8_t)(stamp>>(8*i));
 rtc168_capture.version=167;rtc168_capture.flags=3;rtc168_capture.status=0x40;
 reads=tail_reads=backups=flash_calls=rtc_calls=polls=0;
 flash_fail=rtc_fail=read_fail=changed=no_op=bad_clock=0;confirm_ok=capable=rtc_capable=1;
 memset(keys,0,sizeof keys);unsigned pos=1;
 for(unsigned i=0;i<choice;i++){keys[pos]=OF_BTN_DOWN;pos+=2;}
 keys[pos]=OF_BTN_A;
}
int main(void){
 for(unsigned bytes=65536;bytes<=131072;bytes*=2){
  reset(bytes,0,0);assert(!restore_flash170()&&flash_calls==1&&!rtc_calls&&!polls);
  reset(bytes,1,0);assert(!restore_flash170()&&flash_calls==1&&!rtc_calls&&polls);
  reset(bytes,1,1);assert(!restore_flash170()&&rtc_calls==1&&!strcmp(ui->cart.rtc,"2024-01-01 10:00:00"));
  reset(bytes,1,2);no_op=1;assert(!restore_flash170()&&rtc_calls==1&&!strcmp(ui->cart.rtc,"2024-01-01 10:01:00"));
  reset(bytes,1,3);assert(restore_flash170()==OF_ERR_DENIED&&!backups&&!flash_calls&&!rtc_calls);
  reset(bytes,1,1);rtc168_capture.flags=0;assert(!restore_flash170()&&!polls&&flash_calls==1&&!rtc_calls);
  reset(bytes,1,1);flash_fail=-13;assert(restore_flash170()==-13&&flash_calls==1&&!rtc_calls);
  reset(bytes,1,1);rtc_fail=-13;assert(restore_flash170()==-13&&flash_calls==1&&rtc_calls==1&&strstr(ui->notice,"SAV OK"));
  reset(bytes,1,1);confirm_ok=0;assert(restore_flash170()==OF_ERR_DENIED&&!flash_calls&&!rtc_calls);
  reset(bytes,1,1);changed=1;assert(restore_flash170()&&!backups&&!flash_calls&&!rtc_calls);
  reset(bytes,1,1);source[n+1]=0x13;assert(restore_flash170()&&!backups&&!polls);
  reset(bytes,1,1);read_fail=-13;assert(restore_flash170()==-13&&!backups&&!polls);
  reset(bytes,1,1);size--;assert(restore_flash170()&&!backups);
  reset(bytes,1,1);rtc_capable=0;assert(restore_flash170()==OF_ERR_NOT_SUPPORTED&&!backups&&!flash_calls);
  reset(bytes,1,2);bad_clock=1;assert(restore_flash170()&&!backups&&!flash_calls);
  reset(bytes,1,1);keys[1]=OF_BTN_B;assert(restore_flash170()==OF_ERR_DENIED&&!backups&&!flash_calls);
 }
 puts("RTC172 APP PASS: 64/128 KiB, pure/tail, four choices, unsupported RTC, Flash error/no-op, RTC error, cancel, changed tail, malformed data; mock services");
}
