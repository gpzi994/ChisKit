#include "../../core/gb_save175/rtc189.h"
static struct gb189_value gb189_saved;
static uint8_t gb189_tail[48],gb189_target[5];
static struct gb189_report gb189_result;
static int gb189_has_tail;
static void gb190_log(int rc){
 char path[128],body[384];
 snprintf(path,sizeof path,"/Saves/chiskit/common/gbrtc190-%08lx.txt",(unsigned long)now());
 int n=snprintf(body,sizeof body,"GB RTC restore rc=%d detail=%ld stage=%u reg=%u written=%lu verified=%lu cleanup=%ld\n%s\n%s\n",rc,(long)gb189_result.detail,gb189_result.reserved[0],gb189_result.reserved[1],(unsigned long)gb189_result.written,(unsigned long)gb189_result.verified,(long)gb189_result.cleanup_rc,ui->io_diag,ui->io_regs);
 if(n>0&&(unsigned)n<sizeof body&&of_output_open(path,(uint32_t)n)>=0)(void)of_output_write(0,body,(uint32_t)n);
}
static int gb189_target_now(void){
 struct gb189_value v=gb189_saved;
 if(ui->rtc_restore_choice==2){
  struct timespec ts;if(clock_gettime(CLOCK_REALTIME,&ts)||ts.tv_sec<28800)return OF_ERR_IO;
  if(gb189_advance(&v,(uint64_t)ts.tv_sec-28800))return OF_ERR_BAD_RANGE;
 }
 memcpy(gb189_target,v.raw,5);return 0;
}
static int gb189_prepare_restore(uint32_t n){
 ui->rtc_restore_pending=ui->rtc_restore_choice=0;gb189_has_tail=0;
 long size=OF_SVC->file_size("slot:6");if(size==(long)n)return 0;
 if(size!=(long)n+48||(ui->gb_save175_header[0x47]!=0x0f&&ui->gb_save175_header[0x47]!=0x10))return OF_ERR_BAD_RANGE;
 uint8_t check[48];int rc=file_read(6,n,gb189_tail,48);if(rc)return rc;
 rc=file_read(6,n,check,48);if(rc)return rc;
 if(memcmp(check,gb189_tail,48)||gb189_parse(gb189_tail,&gb189_saved))return OF_ERR_BAD_RANGE;
 gb189_has_tail=1;ui->phase="Checking GB RTC";paint();
 rc=gb187_current();if(rc==OF_ERR_NOT_SUPPORTED)return 0;if(rc)return rc;
 if(!gb187_rtc.supported)return 0;
 ui->confirm=3;ui->phase="RTC data found in selected SAV";paint();
 int released=0;
 for(;;){
  of_input_poll_p0();unsigned keys=_of_input_p0.buttons;
  if(!keys)released=1;
  else if(released){released=0;
   if(keys&OF_BTN_B){rc=OF_ERR_DENIED;break;}
   if(keys&OF_BTN_UP)ui->rtc_restore_choice=(ui->rtc_restore_choice+3)%4;
   else if(keys&OF_BTN_DOWN)ui->rtc_restore_choice=(ui->rtc_restore_choice+1)%4;
   else if(keys&OF_BTN_A){if(ui->rtc_restore_choice==3)rc=OF_ERR_DENIED;break;}
   paint();
  }usleep(16000);
 }
 do{of_input_poll_p0();if(!_of_input_p0.buttons)break;usleep(16000);}while(1);
 ui->confirm=0;if(rc||!ui->rtc_restore_choice)return rc;
 if(!OF_CART_SVC_HAS(gb189_write))return OF_ERR_NOT_SUPPORTED;
 rc=gb189_target_now();if(rc)return rc;
 ui->rtc_restore_pending=1;
 /* Preserve the current clock separately without changing the established fixed RAM backup size. */
 rc=gb187_prepare();if(rc)return rc;
 cb_backup_name(ui->path,sizeof ui->path,&ui->cart,"sav",0);
 size_t len=strlen(ui->path);if(len<13||strcmp(ui->path+len-13,"-00000000.sav"))return OF_ERR_BAD_RANGE;
 strcpy(ui->path+len-13,"-rtc-backup.bin");
 ui->phase="Open original GB RTC backup";paint();
 rc=of_output_open(ui->path,48);if(rc<0)return rc;
 if(OF_SVC->file_size("slot:5")!=48)return OF_ERR_BAD_RANGE;
 ui->phase="Write original GB RTC backup";paint();
 rc=of_output_write(0,gb187_footer,48);if(rc)return rc;
 ui->phase="Verify original GB RTC backup";paint();
 rc=backup_read(0,check,48);if(rc)return rc;
 return memcmp(check,gb187_footer,48)?OF_ERR_IO:0;
}
static int gb189_finish_restore(uint32_t n){
 if(!ui->rtc_restore_pending)return 0;
 ui->phase="Restore GB RTC and verify";paint();
 uint8_t check[48];
 if(!gb189_has_tail||OF_SVC->file_size("slot:6")!=(long)n+48)return OF_ERR_BAD_RANGE;
 int rc=file_read(6,n,check,48);if(rc)return rc;
 if(memcmp(check,gb189_tail,48))return OF_ERR_IO;
 rc=gb189_target_now();if(rc)return rc;
 memset(&gb189_result,0,sizeof gb189_result);
 rc=OF_SVC->gb189_write(ui->gb_save175_header,gb189_target,&gb189_result,sizeof gb189_result);
 snprintf(ui->io_diag,sizeof ui->io_diag,"RTC %d S%u W%lu C%ld",rc,gb189_result.reserved[0],(unsigned long)gb189_result.written,(long)gb189_result.cleanup_rc);
 snprintf(ui->io_regs,sizeof ui->io_regs,"T%02X%02X%02X%02X%02X R%02X%02X%02X%02X%02X",gb189_target[0],gb189_target[1],gb189_target[2],gb189_target[3],gb189_target[4],gb189_result.raw[0],gb189_result.raw[1],gb189_result.raw[2],gb189_result.raw[3],gb189_result.raw[4]);
 gb190_log(rc);
 if(rc||gb189_result.rc||gb189_result.cleanup_rc||!gb189_result.verified)return rc?rc:OF_ERR_IO;
 ui->notice="GB SAV and RTC restored / verified";return 0;
}
