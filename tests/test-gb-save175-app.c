#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../app/ui98/ui.h"
#include "of_output.h"
#include "of_gb175.h"
#include "of_input.h"
#include "../app/ui98/backup_name.h"
static struct of_services_table services;
of_input_state_t _of_input_p0,_of_input_p1;
const struct of_services_table *_of_svc_ptr=&services;
static struct cb_ui state,*ui=&state;
static uint8_t buffer[65536],disk[131072];
static unsigned reads,creates,writes;
static int capture_error,output_error,file_error,changed;
static uint32_t began;
static uint32_t now(void){return 12345;}
static void paint(void){}
static void input(void *p){memset(p,0,sizeof(of_input_state_t));}
static long size(const char *p){assert(!strcmp(p,"slot:5"));return state.gb_save175_bytes;}
static int capture(const uint8_t *h,void *p,uint32_t n,void *out,uint32_t z){
 (void)h;assert(n==state.gb_save175_bytes&&z==sizeof(of_gb175_report));reads++;
 of_gb175_report *r=out;r->compared=n;r->rc=capture_error;memset(p,0x5a,n);return capture_error;
}
#ifdef CB_GB_SAVE176
#include "../app/ui98/gb_selection176.h"
static int capture176(const uint8_t *h,uint32_t selected,void *p,uint32_t n,void *out,uint32_t z){
 assert(selected==n);return capture(h,p,n,out,z);
}
#endif
static int open_file(const char *p,uint32_t n){assert(strstr(p,"/Saves/chisburner/common/GB-")==p&&n==state.gb_save175_bytes);creates++;return output_error;}
static int write_file(uint32_t off,const void *p,uint32_t n){assert(off+n<=sizeof disk);writes++;memcpy(disk+off,p,n);return file_error;}
static int backup_read(uint32_t off,void *p,uint32_t n){assert(off+n<=sizeof disk);memcpy(p,disk+off,n);if(changed)((uint8_t*)p)[0]^=1;return file_error;}
#include "../app/ui98/gb_save175_transfer.h"
static void init(void){
 memset(&state,0,sizeof state);state.cart.platform=CB_GB;state.cart.result=CB_READY;
 state.gb_save175_ready=1;state.gb_save175_bytes=32768;strcpy(state.cart.title,"TEST GAME");
 reads=creates=writes=0;capture_error=output_error=file_error=changed=0;
}
int main(void){
 services.magic=OF_SVC_MAGIC;services.count=1000;services.input_poll_p0=input;
 services.gb175_capture=capture;services.output_open=open_file;services.output_write=write_file;services.file_size=size;
#ifdef CB_GB_SAVE176
 services.gb176_capture=capture176;
#ifdef CB_GB_SAVE177
 services.gb177_capture=capture176;
#ifdef CB_GB_SAVE178
 services.gb178_capture=capture176;
#ifdef CB_GB_SAVE179
 services.gb179_capture=capture176;
#endif
#endif
#endif
#endif
 for(unsigned n=8192;n<=131072;n*=2){init();state.gb_save175_bytes=n;assert(!backup_gb175());assert(reads==1&&creates==1&&writes>0&&state.done==n);}
 init();state.cart.platform=CB_GBA;assert(backup_gb175()==OF_ERR_DENIED&&!reads&&!creates);
 init();state.gb_save175_ready=0;assert(backup_gb175()==OF_ERR_DENIED&&!reads);
 init();capture_error=-77;assert(backup_gb175()==-77&&!creates);
 init();output_error=-77;assert(backup_gb175()==-77&&!writes);
 init();file_error=-77;assert(backup_gb175()==-77);
 init();changed=1;assert(backup_gb175()==OF_ERR_IO);
 init();services.gb175_capture=NULL;
#ifdef CB_GB_SAVE176
 services.gb176_capture=NULL;
#ifdef CB_GB_SAVE177
 services.gb177_capture=NULL;
#ifdef CB_GB_SAVE178
 services.gb178_capture=NULL;
#ifdef CB_GB_SAVE179
 services.gb179_capture=NULL;
#endif
#endif
#endif
#endif
 assert(backup_gb175()==OF_ERR_NOT_SUPPORTED&&!creates);
 init();assert(cb_action_enabled(ui,3)&&!cb_action_enabled(ui,5));
 ui->busy=1;assert(!cb_action_enabled(ui,3));ui->busy=0;
 ui->gb_save175_ready=0;assert(!cb_action_enabled(ui,3));
 ui->gb_save175_ready=1;ui->cart.result=CB_BAD_HEADER;assert(!cb_action_enabled(ui,3));
 puts("PASS GB175 application: independent GB route, GBA rejected, sizes, capture/file errors, full SAV compare");
#ifdef CB_GB_SAVE176
 init();memcpy(state.gb_save175_header+4,cb_gb_logo,48);
 state.gb_save175_header[0x47]=0x1b;state.gb_save175_header[0x49]=3;
 for(unsigned i=0x34;i<=0x4c;i++)state.gb_save175_header[0x4d]-=state.gb_save175_header[i]+1;
 state.gb_detected_bytes=32768;gb176_selection(&state);assert(state.gb_save175_bytes==32768);
 state.gb_choice=7;gb176_selection(&state);assert(state.gb_save175_bytes==131072&&state.gb_save175_ready);
 state.gb_probe_rc=-4;gb176_selection(&state);assert(!state.gb_save175_ready);
 state.gb_probe_rc=0;
 for(unsigned c=1;c<10;c++){state.gb_choice=c;gb176_selection(&state);
#if defined(CB_GB_SAVE177) && !defined(CB_GB_SAVE179)
 assert(state.gb_save175_ready==(c>=3&&c<=7));
#else
 assert(state.gb_save175_ready==(c>=4&&c<=7));
#endif
 }
 state.options=1;state.settings_focus=0;cb_key(&state,OF_BTN_DOWN);assert(state.settings_focus==1);
 cb_key(&state,OF_BTN_A);assert(state.options==2);
 state.gb_choice_draft=7;cb_key(&state,OF_BTN_A);assert(!state.options&&state.gb_save175_bytes==131072);
 cb_reset164(&state);assert(!state.gb_choice&&!state.gb_save175_ready&&!state.gb_detected_bytes);
 puts("PASS GB176 UI: automatic/manual128, unsupported choices, I/O lock and reset");
#ifdef CB_GB_SAVE178
 init();memcpy(state.gb_save175_header+4,cb_gb_logo,48);
 state.gb_save175_header[0x47]=6;
 for(unsigned i=0x34;i<=0x4c;i++)state.gb_save175_header[0x4d]-=state.gb_save175_header[i]+1;
 state.gb_detected_bytes=512;gb176_selection(&state);
 assert(state.gb_save175_ready&&state.gb_save175_bytes==512&&strstr(state.cart.save,"512 bytes"));
 services.gb178_capture=capture176;
#ifdef CB_GB_SAVE179
 services.gb179_capture=capture176;
#endif
 assert(!backup_gb175()&&state.done==512&&creates==1);
 for(unsigned c=1;c<10;c++){state.gb_choice=c;gb176_selection(&state);assert(state.gb_save175_ready==(c==2));}
 state.gb_choice=2;state.gb_probe_rc=-4;gb176_selection(&state);assert(!state.gb_save175_ready);
 puts("PASS GB178 UI: MBC2 automatic/manual512, SAV output, wrong types and failed probe blocked");
#ifdef CB_GB_SAVE179
 init();memcpy(state.gb_save175_header+4,cb_gb_logo,48);
 memcpy(state.gb_save175_header+0x34,"KORO2 KIRBYKKKJ",15);
 state.gb_save175_header[0x47]=0x22;state.gb_save175_header[0x48]=5;
 for(unsigned i=0x34;i<=0x4c;i++)state.gb_save175_header[0x4d]-=state.gb_save175_header[i]+1;
 state.gb_detected_bytes=256;gb176_selection(&state);
 assert(state.gb_save175_ready&&state.gb_save175_bytes==256&&strstr(state.cart.save,"EEPROM 256"));
 assert(!backup_gb175()&&state.done==256&&creates==1);
 for(unsigned c=1;c<10;c++){state.gb_choice=c;gb176_selection(&state);assert(state.gb_save175_ready==(c==8));}
 state.gb_choice=8;state.gb_probe_rc=-4;gb176_selection(&state);assert(!state.gb_save175_ready);
 puts("PASS GB179 UI: EEPROM256 automatic/manual backup, wrong types and failure blocked");
#endif
#endif
#endif
 return 0;
}
