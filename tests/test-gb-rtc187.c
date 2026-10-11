#define _POSIX_C_SOURCE 200809L
#define CB_GB_RTC187 1
#define CB_GB_SAVE179 1
#define CB_GB_SAVE175 1
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../app/ui98/ui.h"
#include "of_output.h"
#include "of_gb175.h"
#include "of_gbrtc186.h"
#include "of_input.h"
#include "../app/ui98/backup_name.h"
static struct of_services_table services;
const struct of_services_table *_of_svc_ptr=&services;
of_input_state_t _of_input_p0,_of_input_p1;
static struct cb_ui state,*ui=&state;
static uint8_t buffer[65536],disk[131120];
static uint32_t began,filesize;
static unsigned polls,reads,opens,writes;
static int choice,fail_read,bad_footer,clock_fail,unsupported;
static uint32_t now(void){return 10;}
static void paint(void){}
static int fake_sleep(unsigned n){(void)n;return 0;}
static int fake_clock(clockid_t id,struct timespec *t){(void)id;t->tv_sec=1700000000+28800;return clock_fail;}
#define usleep fake_sleep
#define clock_gettime fake_clock
static void input(void *p){of_input_state_t *s=p;memset(s,0,sizeof *s);if(ui->confirm==2){polls++;if(polls==2)s->buttons=choice<0?OF_BTN_B:choice?OF_BTN_DOWN:OF_BTN_A;if(polls==4&&choice==1)s->buttons=OF_BTN_A;}}
static long size(const char *p){assert(!strcmp(p,"slot:5"));return filesize;}
static int open_file(const char *p,uint32_t n){assert(strstr(p,".sav"));opens++;filesize=n;return 0;}
static int write_file(uint32_t off,const void *p,uint32_t n){assert(off+n<=filesize);memcpy(disk+off,p,n);writes++;return 0;}
static int backup_read(uint32_t off,void *p,uint32_t n){assert(off+n<=filesize);memcpy(p,disk+off,n);if(bad_footer&&n==48)((uint8_t*)p)[0]^=1;return 0;}
static int capture(const uint8_t *h,uint32_t selected,void *p,uint32_t n,void *out,uint32_t z){
 (void)h;assert(selected==n&&z==sizeof(of_gb175_report));
 of_gb175_report *r=out;r->compared=n;memset(p,0x5a,n);return 0;
}
static int rtc(const uint8_t *h,void *out,uint32_t z){
 assert(h[0x47]==0x10&&z==sizeof(of_gb186_report));reads++;
 if(unsupported)return OF_ERR_NOT_SUPPORTED;
 if(fail_read&&(int)reads==fail_read)return -12;
 of_gb186_report *r=out;memset(r,0,sizeof *r);r->supported=1;
 uint8_t raw[]={40,29,3,2,0};memcpy(r->raw,raw,5);r->days=2;r->hours=3;r->minutes=29;r->seconds=40;return 0;
}
#include "../app/ui98/gb_save175_transfer.h"
static void init(void){
 memset(&state,0,sizeof state);state.cart.platform=CB_GB;state.cart.result=CB_READY;
 state.gb_save175_ready=1;state.gb_save175_bytes=32768;state.gb_save175_header[0x47]=0x10;
 strcpy(state.cart.title,"POKEMON GOLD");polls=reads=opens=writes=filesize=0;
 choice=1;fail_read=bad_footer=clock_fail=unsupported=0;memset(disk,0,sizeof disk);
}
int main(void){
 services.magic=OF_SVC_MAGIC;services.count=1000;services.input_poll_p0=input;
 services.file_size=size;services.output_open=open_file;services.output_write=write_file;
 services.gb179_capture=capture;services.gb186_read=rtc;
 init();assert(!backup_gb175()&&filesize==32816&&reads==2);
 for(unsigned i=0;i<32768;i++)assert(disk[i]==0x5a);
 uint8_t expected[48]={40,0,0,0,29,0,0,0,3,0,0,0,2};
 memcpy(expected+20,expected,20);uint64_t ts=1700000000;
 for(unsigned i=0;i<8;i++)expected[40+i]=(uint8_t)(ts>>(8*i));
 assert(!memcmp(disk+32768,expected,48));assert(ui->done==32816);
 init();choice=0;assert(!backup_gb175()&&filesize==32768&&reads==1);
 init();choice=-1;assert(backup_gb175()==OF_ERR_DENIED&&!opens&&!writes&&ui->rtc_backup_cancelled);
 init();unsupported=1;assert(!backup_gb175()&&filesize==32768&&!polls&&reads==1);
 init();state.gb_save175_header[0x47]=0x1b;assert(!backup_gb175()&&!reads&&!polls&&filesize==32768);
 init();state.cart.platform=CB_GBA;assert(backup_gb175()==OF_ERR_DENIED&&!reads&&!opens);
 for(int i=1;i<=2;i++){init();fail_read=i;assert(backup_gb175()==-12&&!opens);}
 init();clock_fail=1;assert(backup_gb175()==OF_ERR_IO&&!opens);
 init();bad_footer=1;assert(backup_gb175()==OF_ERR_IO);
 uint8_t raw[]={59,59,23,255,193};assert(!cb_gbrtc187_footer(expected,raw,1700028800));
 raw[0]=60;assert(cb_gbrtc187_footer(expected,raw,1700028800));raw[0]=0;
 assert(cb_gbrtc187_footer(expected,raw,0));
 puts("GB187 PASS: FlashGBX 48 bytes, SAV unchanged, yes/no, no RTC mapper, GBA denied, both RTC read errors, timestamp/verification failure");
 return 0;
}
