#define main old187main
#include "test-gb-rtc187.c"
#undef main
#include "../core/gb_save175/rtc189.h"
#include "../app/ui98/gb_rtc192_edit.h"
static unsigned script[64],position,rtc_writes;
static uint8_t got[5],flags;
static int write_error;
static void input192(void *p){assert(position<64);of_input_state_t *s=p;memset(s,0,sizeof *s);s->buttons=script[position++];}
static int read192(const uint8_t *h,void *out,uint32_t n){int rc=rtc(h,out,n);((of_gb186_report*)out)->raw[4]=flags;return rc;}
static int write192(const uint8_t *h,const uint8_t *t,void *out,uint32_t n){
 assert(h[0x47]==0x10&&n==sizeof(struct gb189_report));rtc_writes++;memcpy(got,t,5);
 struct gb189_report *r=out;r->verified=!write_error;return write_error;
}
static void setup(void){init();memset(script,0,sizeof script);position=rtc_writes=0;flags=0;write_error=0;
 services.magic=OF_SVC_MAGIC;services.count=1000;services.input_poll_p0=input192;services.gb186_read=read192;services.gb189_write=write192;
 script[1]=OF_BTN_A;script[3]=OF_BTN_A;
}
int main(void){
 setup();assert(!gb192_edit()&&rtc_writes==1&&!opens&&!writes);assert(got[0]==40&&got[1]==29&&got[2]==3&&got[3]==2);
 setup();script[1]=OF_BTN_B;assert(gb192_edit()==OF_ERR_DENIED&&!rtc_writes);
 setup();script[3]=OF_BTN_B;script[5]=OF_BTN_B;assert(gb192_edit()==OF_ERR_DENIED&&!rtc_writes);
 setup();flags=0xc0;assert(!gb192_edit()&&got[4]==0xc0);
 setup();script[1]=OF_BTN_UP;script[3]=OF_BTN_RIGHT;script[5]=OF_BTN_DOWN;script[7]=OF_BTN_X;script[9]=OF_BTN_A;script[11]=OF_BTN_A;
 assert(!gb192_edit()&&got[3]==3&&got[2]==2&&got[1]==29&&got[0]==40); /* X ignored */
 setup();unsupported=1;assert(gb192_edit()==OF_ERR_NOT_SUPPORTED&&!rtc_writes);
 setup();ui->cart.platform=CB_GBA;assert(gb192_edit()==OF_ERR_DENIED&&!reads&&!rtc_writes);
 setup();ui->cart.result=CB_IO;assert(gb192_edit()==OF_ERR_DENIED&&!rtc_writes);
 setup();ui->gb_save175_header[0x47]=0x1b;assert(gb192_edit()==OF_ERR_NOT_SUPPORTED&&!reads);
 setup();write_error=-5;assert(gb192_edit()==-5&&!ui->gb_save175_ready);
 setup();fail_read=2;assert(gb192_edit()==-12&&!ui->gb_save175_ready);
 puts("GB192 PASS: editor, review/cancel, X ignored, flag preservation, platform/mapper guards, failure, no SAV writes");
 return 0;
}
