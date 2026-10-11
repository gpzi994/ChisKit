#include "of_save171.h"
static of_save171_report save171_report;
static uint8_t save171_id[16];
static int save171_identity(void){
 uint8_t header[192];struct cb_cart current={0};
 int rc=of_cart_enter_safe_idle();if(rc)return rc;
 rc=cart_read(0,header,32,50000);if(rc)return rc;
 for(unsigned off=0;off<sizeof header;off+=32){
  rc=cart_read(off,header+off,32,50000);if(rc)return rc;
 }
 if(!cb_parse_gba(&current,header)||current.result!=CB_READY||
    strcmp(current.code,ui->cart.code)||strcmp(current.title,ui->cart.title))return OF_ERR_IO;
 memcpy(save171_id,header+0xa0,16);return 0;
}
static int restore_save171(void){
 const struct cb_save171_selection s=ui->save_selection;
 if(!OF_CART_SVC_HAS(save171_restore)||s.io_blocked||s.conflict||
    !((s.type==1&&s.bytes==512)||(s.type==2&&s.bytes==8192)||
      (s.type==3&&s.bytes==32768)||(s.type==8&&s.bytes==131072)))return OF_ERR_NOT_SUPPORTED;
 const uint32_t n=s.bytes;
 ui->task="Write selected SAV";ui->total=n;ui->phase="Check selected SAV size";paint();
 if(OF_SVC->file_size("slot:6")!=(long)n){
  ui->notice="Select exact-size SAV (no RTC tail)";return OF_ERR_BAD_RANGE;
 }
 int rc;
 for(uint32_t off=0;off<n;off+=32768){
  uint32_t part=n-off<32768?n-off:32768;
  if(sram137_cancel(0))return OF_ERR_DENIED;
  rc=file_read(6,off,save138_snapshots[0]+off,part);if(rc)return rc;
  rc=file_read(6,off,buffer,part);if(rc)return rc;
  if(memcmp(buffer,save138_snapshots[0]+off,part))return OF_ERR_IO;
 }
 ui->phase="Check cartridge identity";paint();
 rc=save171_identity();if(rc)return rc;
 uint8_t before[16];memcpy(before,save171_id,16);
 ui->phase="Read original selected save twice";paint();
 if(s.type==1||s.type==2){
  memset(&ee161_capture,0,sizeof ee161_capture);
  rc=of_eeprom139_capture(&ee161_capture);if(rc)return rc;
  if(ee161_capture.rc||ee161_capture.cleanup_rc)return ee161_capture.rc?ee161_capture.rc:ee161_capture.cleanup_rc;
  unsigned mode=s.type==2?1:0;
  if(!(ee161_capture.stable_mask&(1u<<mode))||(ee161_capture.mismatch_mask&(1u<<mode)))return OF_ERR_IO;
  memcpy(save138_snapshots[1],mode?ee161_capture.large:ee161_capture.small,n);
 }else{
  for(uint32_t off=0;off<n;off+=32768){
   if(sram137_cancel(0))return OF_ERR_DENIED;
   for(unsigned pass=0;pass<2;pass++){
    uint8_t *dst=pass?buffer:save138_snapshots[1]+off;
    if(s.type==8)rc=of_cart_read_space(OF_CART_BUS_GBA,OF_CART_SPACE_SAVE_PARALLEL,
     OF_CART_SAVE_BANKED_1M|off,dst,32768,10000000);
    else for(unsigned b=0;b<32768;b+=32){
     rc=of_cart_read_space(OF_CART_BUS_GBA,OF_CART_SPACE_SAVE_PARALLEL,off+b,dst+b,32,50000);
     if(rc)break;
    }
    if(rc)return rc;
   }
   if(memcmp(buffer,save138_snapshots[1]+off,32768))return OF_ERR_IO;
  }
 }
 rc=save171_identity();if(rc)return rc;
 if(memcmp(before,save171_id,16))return OF_ERR_IO;
 char title[17],code[9];cb_name_part(title,sizeof title,ui->cart.title);
 cb_name_part(code,sizeof code,ui->cart.code);
 snprintf(ui->path,sizeof ui->path,"/Saves/chiskit/common/GBA-%s-%s-%luB-backup.sav",
  title[0]?title:"UNKNOWN",code,(unsigned long)n);
 ui->phase="Save original selected backup";paint();
 rc=of_output_open(ui->path,n);if(rc<0)return rc;
 if(OF_SVC->file_size("slot:5")!=(long)n)return OF_ERR_BAD_RANGE;
 for(uint32_t off=0;off<n;off+=32768){
  uint32_t part=n-off<32768?n-off:32768;
  rc=of_output_write(off,save138_snapshots[1]+off,part);if(rc)return rc;
  rc=backup_read(off,buffer,part);if(rc)return rc;
  if(memcmp(buffer,save138_snapshots[1]+off,part))return OF_ERR_IO;
 }
 ui->phase="Backup verified; confirm selected type";
 ui->notice=cb_save171_name(s.type);paint();
 if(!confirm())return OF_ERR_DENIED;
 ui->phase="OS validates selected save profile";paint();
 rc=of_save171_restore(s.type,save138_snapshots[0],save138_snapshots[1],n,save171_id,&save171_report);
 ui->done=save171_report.verified;ui->elapsed_ms=now()-began;
 if(rc||save171_report.version!=171||save171_report.rc||save171_report.verified!=n){
  ui->notice=save171_report.attempted&&save171_report.restore_verified!=n?
   "Save may be damaged; retain backup":save171_report.restore_verified==n?
   "Write failed; original restored":"Selected profile rejected; no write";
  return rc?rc:save171_report.rc?save171_report.rc:OF_ERR_IO;
 }
 ui->notice=save171_report.attempted?"Save restored and fully verified":"SAV identical; no save bytes written";
 return 0;
}
