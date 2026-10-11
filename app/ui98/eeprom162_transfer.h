/* EEPROM restore preflight. Both candidate and original snapshots are SDRAM,
 * never PSRAM. Hardware classification is repeated in the OS write service. */
static uint8_t ee162_source[8192] __attribute__((aligned(32)));
static uint8_t ee162_disk_backup[8192] __attribute__((aligned(32)));
static uint8_t ee162_expected_id[16] __attribute__((aligned(32)));
static of_save162_report ee162_report;
static int restore_eeprom162(void){
 const uint32_t n=ui->save_detected_bytes;
 const int type=ui->save_detected_type;
 if(ui->cart.platform!=CB_GBA||ui->cart.result!=CB_READY||ui->save_rc||
    !cb_eeprom161_supported(type,n))return OF_ERR_NOT_SUPPORTED;
 ui->task="Write EEPROM SAV";ui->total=n;ui->done=0;
 ui->phase="Check selected SAV";paint();
 if(OF_SVC->file_size("slot:6")!=(long)n){
  ui->notice="Select matching EEPROM SAV in Pocket";return OF_ERR_BAD_RANGE;
 }
 int rc=file_read(6,0,ee162_source,n);if(rc)return rc;
 rc=file_read(6,0,buffer,n);if(rc)return rc;
 if(memcmp(ee162_source,buffer,n)){
  ui->notice="Selected SAV changed during read";return OF_ERR_IO;
 }
 ui->phase="Recheck cartridge identity";paint();
 rc=of_cart_enter_safe_idle();if(rc)return rc;
 uint8_t header[192];struct cb_cart current={0};
 for(unsigned off=0;off<sizeof header;off+=32){
  rc=cart_read(off,header+off,32,50000);if(rc)return rc;
 }
 if(!cb_parse_gba(&current,header)||current.result!=CB_READY||
    strcmp(current.code,ui->cart.code)||strcmp(current.title,ui->cart.title)){
  ui->notice="Cartridge changed; refresh first";return OF_ERR_IO;
 }
 memcpy(ee162_expected_id,header+0xa0,sizeof ee162_expected_id);
 ui->phase="Read original EEPROM twice";paint();
 memset(&ee161_capture,0,sizeof ee161_capture);
 rc=of_eeprom139_capture(&ee161_capture);if(rc)return rc;
 if(ee161_capture.rc||ee161_capture.cleanup_rc)
  return ee161_capture.rc?ee161_capture.rc:ee161_capture.cleanup_rc;
 unsigned mode=n==8192u?1u:0u;
 if(!(ee161_capture.stable_mask&(1u<<mode))||
    (ee161_capture.mismatch_mask&(1u<<mode))){
  ui->notice="Original EEPROM read unstable";return OF_ERR_IO;
 }
 const uint8_t *original=mode?ee161_capture.large:ee161_capture.small;
 char title[17],code[9];
 cb_name_part(title,sizeof title,current.title);
 cb_name_part(code,sizeof code,current.code);
 snprintf(ui->path,sizeof ui->path,
  "/Saves/chiskit/common/GBA-%s-%s-%luB-backup.sav",
  title[0]?title:"UNKNOWN",code,(unsigned long)n);
 ui->phase="Save original EEPROM backup";paint();
 rc=of_output_open(ui->path,n);if(rc<0)return rc;
 if(OF_SVC->file_size("slot:5")!=(long)n)return OF_ERR_BAD_RANGE;
 rc=of_output_write(0,original,n);if(rc)return rc;
 ui->phase="Read back backup file";paint();
 rc=cb_output_verify161(original,n);if(rc){
  ui->notice="Original backup differs; write locked";return rc;
 }
 ui->phase="Original backed up; confirm write";
 ui->notice="Backup retained in Saves";paint();
 if(!confirm()){
  ui->notice="EEPROM write cancelled; backup retained";return OF_ERR_DENIED;
 }
 ui->phase="OS rechecks hardware and original";paint();
 memset(&ee162_report,0,sizeof ee162_report);
 rc=of_save162_restore(ee162_source,ee162_disk_backup,n,
  ee162_expected_id,&ee162_report);
 ui->done=ee162_report.verified;ui->elapsed_ms=now()-began;paint();
 if(rc||ee162_report.rc||ee162_report.verified!=n){
  if(ee162_report.attempted&&ee162_report.restore_verified!=n)
   ui->notice="EEPROM may be damaged; keep original backup";
  else if(ee162_report.restore_verified==n)
   ui->notice="Write failed; original restored and verified";
  else ui->notice="Write blocked; original backup retained";
  return rc?rc:ee162_report.rc?ee162_report.rc:OF_ERR_IO;
 }
 ui->notice=ee162_report.attempted?
  "EEPROM write and full readback PASS":
  "SAV identical; no EEPROM block written";
 return 0;
}
