#include "of_flash170.h"
static uint8_t flash170_source[131072] __attribute__((aligned(32)));
static of_flash170_report flash170_report;
static char flash170_notice[96];
static int restore_flash170(void){
 if(!OF_CART_SVC_HAS(flash170_restore))return OF_ERR_NOT_SUPPORTED;
 uint32_t n=ui->save_detected_bytes;
 if(ui->save_rc||!((ui->save_detected_type==4&&n==65536u)||
    (ui->save_detected_type==5&&n==131072u)))return OF_ERR_NOT_SUPPORTED;
 ui->task="Write Flash SAV";ui->phase="Validate selected SAV";paint();
 int rc;
#ifdef CB_RTC172
 rc=rtc172_prepare(n);if(rc)return rc;
#else
 if(OF_SVC->file_size("slot:6")!=(long)n){
  ui->notice="Select pure SAV; RTC restore not enabled";return OF_ERR_BAD_RANGE;
 }
#endif
 for(unsigned off=0;off<n;off+=32768){
  rc=file_read(6,off,flash170_source+off,32768);if(rc)return rc;
  rc=file_read(6,off,buffer,32768);if(rc)return rc;
  if(memcmp(buffer,flash170_source+off,32768))return OF_ERR_IO;
 }
#ifdef CB_RTC172
 rc=rtc172_recheck_file(n);if(rc)return rc;
#endif
 flash170_original_backup=1;rtc169_extra=0;
 rc=backup_flash165();flash170_original_backup=0;if(rc)return rc;
 uint8_t id[16];memcpy(id,sram137_header+0xa0,16);
 ui->task="Write Flash SAV";ui->phase="Original saved; confirm overwrite";paint();
 if(!confirm())return OF_ERR_DENIED;
 ui->phase="Flash erase/program/verify; please wait";ui->done=0;ui->total=n;paint();
 rc=of_flash170_restore(flash170_source,save138_snapshots[1],n,id,&flash170_report);
 ui->done=flash170_report.verified;
 if(rc||flash170_report.rc||flash170_report.cleanup_rc||ui->done!=n){
  snprintf(flash170_notice,sizeof flash170_notice,"Flash stop S%lu @%05lX; keep backup",(unsigned long)flash170_report.stage,(unsigned long)flash170_report.address);
  ui->notice=flash170_notice;
  return rc?rc:flash170_report.rc?flash170_report.rc:OF_ERR_IO;
 }
 ui->notice=flash170_report.attempted?"Flash SAV full verify PASS; RTC unchanged":"SAV identical; no erase/program; RTC unchanged";
#ifdef CB_RTC172
 rc=rtc172_finish(id);if(rc)return rc;
#endif
 return 0;
}
