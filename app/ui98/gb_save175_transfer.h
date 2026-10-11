#include "of_gb175.h"
/* Separate SDRAM snapshot; never borrow PSRAM or GBA save snapshots. */
static uint8_t gb175_snapshot[131072] __attribute__((aligned(64)));
static of_gb175_report gb175_result;
static int gb175_user_cancel(void){of_input_poll_p0();return (_of_input_p0.buttons&OF_BTN_B)!=0;}
#ifdef CB_GB_RTC187
#include "gb_rtc187_transfer.h"
#endif
static int backup_gb175(void){
 if(ui->cart.platform!=CB_GB||ui->cart.result!=CB_READY||!ui->gb_save175_ready||
    !ui->gb_save175_bytes||ui->gb_save175_bytes>sizeof gb175_snapshot)return OF_ERR_DENIED;
 uint32_t n=ui->gb_save175_bytes;
 ui->task="Backup GB/GBC RAM";ui->total=n;ui->done=0;
#ifdef CB_GB_RTC187
 int include_rtc=0;
 int choose_rc=gb187_choose(&include_rtc);if(choose_rc)return choose_rc;
#endif
 ui->phase="Read GB RAM twice";paint();
#ifdef CB_GB_SAVE179
 if(ui->gb_save175_header[0x47]==0x22){
  ui->task="Backup MBC7 EEPROM";ui->phase="Read MBC7 EEPROM twice";paint();
 }
#endif
 if(gb175_user_cancel())return OF_ERR_DENIED;
 memset(&gb175_result,0,sizeof gb175_result);
#ifdef CB_GB_SAVE179
 int rc=of_gb179_capture(ui->gb_save175_header,n,gb175_snapshot,n,&gb175_result);
#elif defined(CB_GB_SAVE178)
 int rc=of_gb178_capture(ui->gb_save175_header,n,gb175_snapshot,n,&gb175_result);
#elif defined(CB_GB_SAVE177)
 int rc=of_gb177_capture(ui->gb_save175_header,n,gb175_snapshot,n,&gb175_result);
#elif defined(CB_GB_SAVE176)
 int rc=of_gb176_capture(ui->gb_save175_header,n,gb175_snapshot,n,&gb175_result);
#else
 int rc=of_gb175_capture(ui->gb_save175_header,gb175_snapshot,n,&gb175_result);
#endif
 snprintf(ui->io_diag,sizeof ui->io_diag,"GB rc %d detail %d clean %d",gb175_result.rc,gb175_result.detail,gb175_result.cleanup_rc);
 if(rc)return rc;
 if(gb175_result.rc||gb175_result.cleanup_rc||gb175_result.compared!=n)return OF_ERR_IO;
 if(gb175_user_cancel())return OF_ERR_DENIED;
 cb_backup_name(ui->path,sizeof ui->path,&ui->cart,"sav",now());
 uint32_t file_bytes=n;
#ifdef CB_GB_RTC187
 if(include_rtc){rc=gb187_prepare();if(rc)return rc;file_bytes+=48;}
#endif
 ui->phase="Create GB SAV";paint();
 rc=of_output_open(ui->path,file_bytes);if(rc)return rc; /* Unique name: never reuse. */
 if(OF_SVC->file_size("slot:5")!=(long)file_bytes)return OF_ERR_BAD_RANGE;
 for(uint32_t off=0;off<n;){
  uint32_t part=n-off;if(part>32768)part=32768;
  if(gb175_user_cancel()){ui->notice="Incomplete SAV; do not use";return OF_ERR_DENIED;}
  ui->phase="SDRAM -> GB SAV";paint();
  rc=of_output_write(off,gb175_snapshot+off,part);if(rc){ui->notice="Incomplete SAV; do not use";return rc;}
  off+=part;
 }
 /* Host-completed output writes are committed by the existing Pocket API.
  * Read back slot 5 directly, never the POSIX slot-keyed cache. */
 ui->phase="Verify complete GB SAV";paint();
 for(uint32_t off=0;off<n;){
  uint32_t part=n-off;if(part>32768)part=32768;
  rc=backup_read(off,buffer,part);if(rc)return rc;
  if(memcmp(buffer,gb175_snapshot+off,part))return OF_ERR_IO;
  off+=part;ui->done=off;ui->elapsed_ms=now()-began;paint();
 }
 ui->notice="GB RAM read twice + SAV compare PASS";
#ifdef CB_GB_SAVE179
 if(ui->gb_save175_header[0x47]==0x22)ui->notice="MBC7 read twice + SAV compare PASS";
#endif
#ifdef CB_GB_RTC187
 if(include_rtc){
  ui->phase="Write and verify GB RTC footer";paint();
  rc=of_output_write(n,gb187_footer,48);if(rc)return rc;
  rc=backup_read(n,buffer,48);if(rc)return rc;
  if(memcmp(buffer,gb187_footer,48))return OF_ERR_IO;
  ui->done=ui->total=file_bytes;
  ui->notice="GB SAV + 48-byte RTC verified";
 }
#endif
 return 0;
}
