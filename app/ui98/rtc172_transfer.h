#include "of_rtc172.h"
#include "../../core/rtc/restore172.h"
static struct cb_rtc172_value rtc172_saved;
static uint8_t rtc172_footer[16];
static int rtc172_has_tail;
static of_rtc172_report rtc172_report;
static int rtc172_target(uint8_t target[8]){
 struct cb_rtc172_value value=rtc172_saved;
 if(ui->rtc_restore_choice==2){
  struct timespec t;
  if(clock_gettime(CLOCK_REALTIME,&t))return OF_ERR_IO;
  int64_t utc=(int64_t)t.tv_sec-CB_RTC169_UTC_OFFSET_MINUTES*60;
  if(utc<0||cb_rtc172_advance(&value,(uint64_t)utc))return OF_ERR_BAD_RANGE;
 }
 memcpy(target,value.date,7);target[7]=value.status;return 0;
}
static int rtc172_choose(void){
 ui->rtc_restore_choice=0;ui->confirm=3;
 ui->notice="RTC data stays outside save memory";
 ui->phase="RTC data found in selected SAV";paint();
 int released=0,rc=0;
 for(;;){
  of_input_poll_p0();unsigned keys=_of_input_p0.buttons;
  if(!keys)released=1;
  else if(released){
   released=0;
   if(keys&OF_BTN_B){rc=OF_ERR_DENIED;break;}
   if(keys&OF_BTN_UP)ui->rtc_restore_choice=(ui->rtc_restore_choice+3)%4;
   else if(keys&OF_BTN_DOWN)ui->rtc_restore_choice=(ui->rtc_restore_choice+1)%4;
   else if(keys&OF_BTN_A){if(ui->rtc_restore_choice==3)rc=OF_ERR_DENIED;break;}
   paint();
  }
  usleep(16000);
 }
 do{of_input_poll_p0();if(!_of_input_p0.buttons)break;usleep(16000);}while(1);
 ui->confirm=0;ui->notice=rc?"Save and RTC restore cancelled":0;return rc;
}
static int rtc172_prepare(uint32_t n){
 ui->rtc_restore_pending=ui->rtc_restore_choice=0;rtc172_has_tail=0;
 long size=OF_SVC->file_size("slot:6");
 if(size==(long)n)return 0;
 if(size!=(long)n+16){ui->notice="Select SAV or SAV + 16 RTC bytes";return OF_ERR_BAD_RANGE;}
 uint8_t again[16];int rc=file_read(6,n,rtc172_footer,16);if(rc)return rc;
 rc=file_read(6,n,again,16);if(rc)return rc;
 if(memcmp(again,rtc172_footer,16))return OF_ERR_IO;
 if(cb_rtc172_parse(rtc172_footer,(size_t)size,n,&rtc172_saved)!=1){
  ui->notice="Invalid RTC footer; no restore started";return OF_ERR_BAD_RANGE;
 }
 rtc172_has_tail=1;
 ui->phase="Check current cartridge RTC";paint();
 rc=rtc168_read_current();if(rc)return rc;
 if(!cb_rtc168_available(rc,&rtc168_capture))return 0;
 rc=rtc172_choose();if(rc)return rc;
 if(!ui->rtc_restore_choice)return 0;
 if(!OF_CART_SVC_HAS(rtc172_write)||rtc172_saved.status!=0x40||rtc168_capture.status!=0x40){
  ui->notice="RTC mode unavailable; choose SAV only";return OF_ERR_NOT_SUPPORTED;
 }
 uint8_t target[8];rc=rtc172_target(target);
 if(rc){ui->notice="Invalid backup/Pocket timestamp";return rc;}
 ui->rtc_restore_pending=1;return 0;
}
static int rtc172_recheck_file(uint32_t n){
 if(OF_SVC->file_size("slot:6")!=(long)(n+(rtc172_has_tail?16u:0u)))return OF_ERR_BAD_RANGE;
 if(rtc172_has_tail){
  uint8_t check[16];int rc=file_read(6,n,check,16);if(rc)return rc;
  if(memcmp(check,rtc172_footer,16))return OF_ERR_IO;
 }
 return 0;
}
static int rtc172_finish(const uint8_t id[16]){
 if(!ui->rtc_restore_pending)return 0;
 ui->phase="SAV verified; restoring RTC";paint();
 uint8_t target[8];int rc=rtc172_target(target);
 memset(&rtc172_report,0,sizeof rtc172_report);
 if(!rc)rc=of_rtc172_write(target,id,&rtc172_report);
 if(rc||rtc172_report.version!=172||rtc172_report.rc||rtc172_report.cleanup_rc||!rtc172_report.verified){
  cb_rtc167_format(ui->cart.rtc,sizeof ui->cart.rtc,OF_ERR_IO,&rtc172_report.after);
  ui->notice=rtc172_report.attempted?"SAV OK; RTC failed / may have changed":"SAV OK; RTC update blocked";
  return rc?rc:rtc172_report.rc?rtc172_report.rc:OF_ERR_IO;
 }
 cb_rtc167_format(ui->cart.rtc,sizeof ui->cart.rtc,0,&rtc172_report.after);
 ui->notice="SAV and RTC restored / verified";return 0;
}
