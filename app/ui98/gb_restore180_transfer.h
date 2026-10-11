#include "../../core/gb_save175/restore180.h"
/* Independent GB/GBC SDRAM buffers; neither GBA save state nor PSRAM. */
static uint8_t gb180_source[131072] __attribute__((aligned(64)));
#ifdef CB_GB_RTC189
#include "gb_rtc189_restore.h"
#endif
static int restore_gb180(void){
 struct gb175_plan plan;
 const uint32_t n=ui->gb_save175_bytes;
 if(ui->cart.platform!=CB_GB||ui->cart.result!=CB_READY||!ui->gb_save175_ready||
    gb_restore_plan(ui->gb_save175_header,n,&plan))return OF_ERR_NOT_SUPPORTED;
 if(!OF_CART_SVC_HAS(gb180_restore))return OF_ERR_NOT_SUPPORTED;
 ui->task=plan.mapper==GB179_MBC7?"Restore MBC7 EEPROM":"Restore GB/GBC SRAM";ui->total=n;ui->done=0;
 ui->phase="Check selected GB SAV size";paint();
#ifdef CB_GB_RTC189
 int rc=gb189_prepare_restore(n);if(rc)return rc;
#else
 if(OF_SVC->file_size("slot:6")!=(long)n){
  ui->notice="Select exact-size SAV in Pocket menu";return OF_ERR_BAD_RANGE;
 }
 int rc=0;
#endif
 rc=file_read(6,0,gb180_source,n);if(rc)return rc;
 ui->phase="Read selected GB SAV twice";paint();
 for(uint32_t off=0;off<n;){
  uint32_t part=n-off;if(part>32768)part=32768;
  rc=file_read(6,off,buffer,part);if(rc)return rc;
  if(memcmp(buffer,gb180_source+off,part))return OF_ERR_IO;
  off+=part;
 }
 if(gb175_user_cancel())return OF_ERR_DENIED;
 ui->phase=plan.mapper==GB179_MBC7?"Read original MBC7 EEPROM twice":"Read original GB RAM twice";paint();
 memset(&gb175_result,0,sizeof gb175_result);
 rc=of_gb179_capture(ui->gb_save175_header,n,gb175_snapshot,n,&gb175_result);
 if(rc)return rc;
 if(gb175_result.rc||gb175_result.cleanup_rc||gb175_result.compared!=n)return OF_ERR_IO;
 cb_backup_name(ui->path,sizeof ui->path,&ui->cart,"sav",0);
 size_t len=strlen(ui->path);
 if(len<13||strcmp(ui->path+len-13,"-00000000.sav"))return OF_ERR_BAD_RANGE;
 strcpy(ui->path+len-13,"-backup.sav");
 ui->phase="Open original GB backup";paint();
 rc=of_output_open(ui->path,n);if(rc<0)return rc; /* +1 means existing file reused. */
 int reuse=rc==1;
 if(OF_SVC->file_size("slot:5")!=(long)n)return OF_ERR_BAD_RANGE;
 if(reuse){
  ui->phase="Compare existing GB backup";paint();
  for(uint32_t off=0;off<n;){
   uint32_t part=n-off;if(part>32768)part=32768;
   rc=backup_read(off,buffer,part);if(rc)return rc;
   if(memcmp(buffer,gb175_snapshot+off,part))reuse=0;
   off+=part;
  }
 }
 ui->phase="Write original GB backup";paint();
 for(uint32_t off=0;!reuse&&off<n;){
  uint32_t part=n-off;if(part>32768)part=32768;
  if(gb175_user_cancel())return OF_ERR_DENIED;
  rc=of_output_write(off,gb175_snapshot+off,part);if(rc)return rc;
  off+=part;
 }
 ui->phase="Verify original GB backup";paint();
 for(uint32_t off=0;off<n;){
  uint32_t part=n-off;if(part>32768)part=32768;
  rc=backup_read(off,buffer,part);if(rc)return rc;
  if(memcmp(buffer,gb175_snapshot+off,part))return OF_ERR_IO;
  off+=part;
 }
 ui->phase="Original SAV verified; confirm write";
 ui->notice=ui->rtc_restore_pending?"Backup retained; RTC follows SAV":"Backup retained; ROM and RTC unchanged";paint();
 if(!confirm())return OF_ERR_DENIED;
 ui->phase=plan.mapper==GB179_MBC7?"Restore MBC7 EEPROM and compare":"Restore GB SRAM and full compare";paint();
 memset(&gb175_result,0,sizeof gb175_result);
 rc=of_gb180_restore(ui->gb_save175_header,gb175_snapshot,gb180_source,n,&gb175_result);
 ui->done=gb175_result.verified;ui->elapsed_ms=now()-began;
 snprintf(ui->io_diag,sizeof ui->io_diag,"GB rc %d detail %d clean %d",gb175_result.rc,gb175_result.detail,gb175_result.cleanup_rc);
 if(rc||gb175_result.rc||gb175_result.cleanup_rc||gb175_result.verified!=n){
  ui->notice=gb175_result.written?"SAV may be partial; keep original backup":"Write blocked; original backup retained";
  return rc?rc:gb175_result.rc?gb175_result.rc:OF_ERR_IO;
 }
 ui->notice=plan.mapper==GB179_MBC7?(gb175_result.written?"MBC7 restored; full compare PASS":"MBC7 unchanged; full compare PASS"):(gb175_result.written?"GB SRAM restored; full compare PASS":"GB SRAM unchanged; full compare PASS");
#ifdef CB_GB_RTC189
 rc=gb189_finish_restore(n);
 if(rc){ui->notice="SAV restored; GB RTC failed; keep backup";return rc;}
#endif
 return 0;
}
