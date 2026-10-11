/* Standalone editor: diagnostic file only; no save writes or PSRAM staging. */
#include "rtc173_probe.h"
static int rtc173_probe_log(void){
 static char log[2048];static unsigned sequence;char path[112];
 int n=snprintf(log,sizeof log,"RTC173 entry r3 (post-ROM length=32)\nstage=%u rc=%d offset=%08lx blocks=%u parsed=%d identity=%d\nexpected=%.16s / %.15s\nactual=%.16s / %.15s\nRTC version=%lu rc=%ld flags=%lu stage=%lu cleanup=%ld status=%02X\n",
 rtc173_diag.stage,rtc173_diag.rc,(unsigned long)rtc173_diag.offset,rtc173_diag.blocks,rtc173_diag.parsed,rtc173_diag.identity,
 ui->cart.title,ui->cart.code,rtc173_diag.current.title,rtc173_diag.current.code,
 (unsigned long)rtc168_capture.version,(long)rtc168_capture.rc,(unsigned long)rtc168_capture.flags,(unsigned long)rtc168_capture.stage,(long)rtc168_capture.cleanup_rc,rtc168_capture.status);
 if(n<0||(unsigned)n>=sizeof log)return OF_ERR_IO;
 for(unsigned i=0;i<192;i++)n+=snprintf(log+n,sizeof log-(unsigned)n,"%02X%s",rtc173_diag.header[i],i%32==31?"\n":"");
 n+=snprintf(log+n,sizeof log-(unsigned)n,"post RTC id=");
 for(unsigned i=0;i<16;i++)n+=snprintf(log+n,sizeof log-(unsigned)n,"%02X",rtc173_diag.id[i]);
 n+=snprintf(log+n,sizeof log-(unsigned)n,"\nNo RTC date/status write attempted.\nEND RTC173\n");
 snprintf(path,sizeof path,"/Saves/chiskit/common/rtc173-%08lx-%04x.txt",(unsigned long)now(),++sequence);
 int rc=of_output_open(path,(uint32_t)n);if(rc<0)return rc;
 rc=of_output_write(0,log,(uint32_t)n);if(!rc)rc=cb_output_verify161(log,(uint32_t)n);return rc;
}
static int rtc173_pocket(void){
 struct timespec t;struct tm tm;
 if(clock_gettime(CLOCK_REALTIME,&t)||t.tv_sec<946684800LL||t.tv_sec>4102444799LL||!gmtime_r(&t.tv_sec,&tm))return OF_ERR_BAD_RANGE;
 /* APF epoch is LOCAL wall time already. gmtime decodes it without a
  * second timezone shift. tm_hour is 0..23 regardless of Pocket AM/PM UI. */
 ui->rtc_edit[0]=(unsigned)tm.tm_year+1900;ui->rtc_edit[1]=(unsigned)tm.tm_mon+1;
 ui->rtc_edit[2]=(unsigned)tm.tm_mday;ui->rtc_edit[3]=(unsigned)tm.tm_hour;
 ui->rtc_edit[4]=(unsigned)tm.tm_min;ui->rtc_edit[5]=(unsigned)tm.tm_sec;return 0;
}
static void rtc173_target(uint8_t target[8]){
 unsigned *v=ui->rtc_edit,days=0;
 for(unsigned y=2000;y<v[0];y++)days+=365+(y%4==0);
 for(unsigned m=1;m<v[1];m++)days+=cb_rtc172_days(v[0],m);
 days+=v[2]-1;
 target[0]=cb_rtc172_encode(v[0]-2000);target[1]=cb_rtc172_encode(v[1]);target[2]=cb_rtc172_encode(v[2]);
 target[3]=(uint8_t)((days+6)%7);
 target[4]=cb_rtc172_encode(v[3])|(v[3]>=12?0x80:0);
 target[5]=cb_rtc172_encode(v[4]);target[6]=cb_rtc172_encode(v[5]);target[7]=0x40;
}
static int rtc173_edit(void){
 ui->task="Set cartridge RTC";ui->phase="Check current cartridge RTC";paint();
 uint8_t id[16];int rc=rtc173_probe(id);
 int log_rc=rtc173_probe_log();
 if(rc){ui->notice=log_rc?"Entry failed; diagnostic log failed":"Entry failed; rtc173 log saved";return rc;}
 if(log_rc){ui->notice="Diagnostic log failed; no RTC write";return log_rc;}
 unsigned *v=ui->rtc_edit;
 v[0]=2000+cb_rtc172_bcd(rtc168_capture.date[0]);
 v[1]=cb_rtc172_bcd(rtc168_capture.date[1]);v[2]=cb_rtc172_bcd(rtc168_capture.date[2]);
 v[3]=cb_rtc172_bcd(rtc168_capture.date[4]&0x7f);
 v[4]=cb_rtc172_bcd(rtc168_capture.date[5]);v[5]=cb_rtc172_bcd(rtc168_capture.date[6]);
 ui->rtc_field=0;ui->confirm=4;ui->phase="Manual / Pocket time (24-hour)";
 ui->notice="Changes RTC only; SAV and ROM untouched";paint();
 int released=0;
 for(;;){
  of_input_poll_p0();unsigned k=_of_input_p0.buttons;
  if(!k)released=1;
  else if(released){
   released=0;
   if(k&OF_BTN_B){if(ui->confirm==5){ui->confirm=4;paint();continue;}ui->confirm=0;return OF_ERR_DENIED;}
   if(ui->confirm==5){if(k&OF_BTN_A)break;}
   else if(k&OF_BTN_A)ui->confirm=5;
   else if(k&OF_BTN_X){rc=rtc173_pocket();ui->notice=rc?"Pocket time invalid; not applied":"Pocket time loaded; review then A";}
   else if(k&OF_BTN_LEFT)ui->rtc_field=(ui->rtc_field+5)%6;
   else if(k&OF_BTN_RIGHT)ui->rtc_field=(ui->rtc_field+1)%6;
   else if(k&(OF_BTN_UP|OF_BTN_DOWN)){
    unsigned f=ui->rtc_field,lo=f==0?2000:f<3?1:0;
    unsigned hi=f==0?2099:f==1?12:f==2?cb_rtc172_days(v[0],v[1]):f==3?23:59;
    if(k&OF_BTN_UP)v[f]=v[f]==hi?lo:v[f]+1;else v[f]=v[f]==lo?hi:v[f]-1;
    unsigned last=cb_rtc172_days(v[0],v[1]);if(v[2]>last)v[2]=last;
   }
   paint();
  }
  usleep(16000);
 }
 ui->confirm=0;ui->phase="Write RTC and verify";paint();
 uint8_t target[8];rtc173_target(target);
 memset(&rtc172_report,0,sizeof rtc172_report);rc=of_rtc172_write(target,id,&rtc172_report);
 if(!rc&&(rtc172_report.version!=172||rtc172_report.rc||rtc172_report.cleanup_rc||!rtc172_report.verified))rc=OF_ERR_IO;
 cb_rtc167_format(ui->cart.rtc,sizeof ui->cart.rtc,rc,&rtc172_report.after);
 ui->notice=rc?(rtc172_report.attempted?"RTC failed; clock may have changed":"RTC blocked; analyze again"):"RTC updated and readback verified";
 return rc;
}
