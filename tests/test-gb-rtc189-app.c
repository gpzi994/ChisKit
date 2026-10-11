#define CB_GB_RTC189 1
#define main old187main
#include "test-gb-rtc187.c"
#undef main
const uint8_t cb_gb_logo[48]={0xce,0xed,0x66,0x66,0xcc,0x0d,0,0x0b,3,0x73,0,0x83,0,0x0c,0,0x0d,
0,8,0x11,0x1f,0x88,0x89,0,0x0e,0xdc,0xcc,0x6e,0xe6,0xdd,0xdd,0xd9,0x99,
0xbb,0xbb,0x67,0x63,0x6e,0x0e,0xec,0xcc,0xdd,0xdc,0x99,0x9f,0xbb,0xb9,0x33,0x3e};
static uint8_t source_tail[48];
static int has_tail,select_choice,rtc_writes,ram_writes,restore_error,rtc_error,source_changed;
static unsigned press;
static int file_read(unsigned slot,uint32_t off,void *p,uint32_t n){
 assert(slot==6);
 if(off==ui->gb_save175_bytes&&n==48){memcpy(p,source_tail,n);if(source_changed)((uint8_t*)p)[0]^=1;}
 else {assert(off+n<=ui->gb_save175_bytes);memset(p,0x39,n);}return 0;
}
static int confirm(void){return 1;}
#include "../app/ui98/gb_restore180_transfer.h"
static long size189(const char *p){return !strcmp(p,"slot:6")?(long)ui->gb_save175_bytes+(has_tail?48:0):filesize;}
static int open189(const char *p,uint32_t n){(void)p;opens++;filesize=n;return 0;}
static void input189(void *p){of_input_state_t *s=p;memset(s,0,sizeof *s);
 if(ui->confirm==3){press++;if(!(press&1))s->buttons=press/2<=(unsigned)select_choice?OF_BTN_DOWN:OF_BTN_A;}}
static int restore189(const uint8_t *h,const void *original,const void *source,uint32_t n,void *out,uint32_t z){
 (void)h;(void)original;assert(z==sizeof(of_gb175_report)&&((const uint8_t*)source)[0]==0x39);
 ram_writes++;of_gb175_report *r=out;r->written=n;r->verified=restore_error?0:n;return restore_error;
}
static int rtc189(const uint8_t *h,const uint8_t *raw,void *out,uint32_t n){
 (void)h;assert(n==sizeof(struct gb189_report)&&ram_writes==1);rtc_writes++;
 if(select_choice==1)assert(raw[0]==18&&raw[1]==35&&raw[2]==14&&raw[3]==2);
 struct gb189_report *r=out;r->verified=!rtc_error;return rtc_error;
}
static void init189(void){
 init();memcpy(ui->gb_save175_header+4,cb_gb_logo,48);ui->gb_save175_header[0x49]=3;
 for(unsigned i=0x34;i<=0x4c;i++)ui->gb_save175_header[0x4d]-=ui->gb_save175_header[i]+1;
 uint8_t raw[]={18,35,14,2,0};assert(!cb_gbrtc187_footer(source_tail,raw,1700028790));
 has_tail=1;select_choice=1;rtc_writes=ram_writes=restore_error=rtc_error=source_changed=press=0;
 services.magic=OF_SVC_MAGIC;services.count=1000;services.input_poll_p0=input189;
 services.file_size=size189;services.output_open=open189;services.output_write=write_file;
 services.gb179_capture=capture;services.gb180_restore=restore189;services.gb186_read=rtc;services.gb189_write=rtc189;
}
int main(void){
 init189();assert(!restore_gb180()&&rtc_writes==1&&ram_writes==1&&opens==3);
 init189();select_choice=2;assert(!restore_gb180()&&rtc_writes==1);
 init189();select_choice=0;assert(!restore_gb180()&&!rtc_writes&&ram_writes==1&&opens==1);
 init189();select_choice=3;assert(restore_gb180()==OF_ERR_DENIED&&!rtc_writes&&!ram_writes&&!opens);
 init189();has_tail=0;assert(!restore_gb180()&&!rtc_writes&&!press);
 init189();unsupported=1;assert(!restore_gb180()&&!rtc_writes&&!press);
 init189();source_tail[1]=1;assert(restore_gb180()==OF_ERR_BAD_RANGE&&!opens&&!ram_writes);
 init189();restore_error=-88;assert(restore_gb180()==-88&&!rtc_writes);
 init189();rtc_error=-88;assert(restore_gb180()==-88&&ram_writes==1&&rtc_writes==1&&strstr(ui->notice,"RTC failed"));
 init189();assert(!gb189_prepare_restore(32768));source_changed=1;assert(gb189_finish_restore(32768)==OF_ERR_IO&&!rtc_writes);
 init189();ui->cart.platform=CB_GBA;assert(restore_gb180()==OF_ERR_NOT_SUPPORTED&&!opens&&!rtc_writes);
 puts("GB189 APP PASS: pure/tail, four choices, unsupported RTC, malformed/changed footer, fixed backups, RAM failure prevents RTC, RTC failure distinct, GBA rejected");
}
