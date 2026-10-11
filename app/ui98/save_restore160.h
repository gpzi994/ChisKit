/* Optional, explicitly built SRAM-only restore. No Flash/EEPROM path. */
static of_save160_report save160_report;
static int restore_save160(void){
 if(!OF_CART_SVC_HAS(sram160_restore)){
  ui->notice="SRAM write service unavailable";return OF_ERR_NOT_SUPPORTED;
 }
 int rc=prepare_save159();if(rc)return rc;
 const uint32_t n=ui->save_detected_bytes;
 ui->task="Write SRAM SAV";
 ui->phase="Original SAV verified; confirm write";
 ui->notice="Backup file retained in Saves";paint();
 if(!confirm()){ui->notice="SAV write cancelled; backup retained";return OF_ERR_DENIED;}
 ui->phase="OS rechecks file and SRAM hardware";ui->done=0;paint();
 memset(&save160_report,0,sizeof save160_report);
 rc=of_save160_restore(save138_snapshots[0],save138_snapshots[1],n,
  save159_expected_id,&save160_report);
 ui->done=save160_report.verified;ui->total=n;
 ui->elapsed_ms=now()-began;paint();
 if(rc||save160_report.rc||save160_report.verified!=n){
  if(save160_report.attempted&&!save160_report.restore_verified)
   ui->notice="SAV may be damaged; keep original backup";
  else if(save160_report.restore_attempted&&save160_report.restore_verified)
   ui->notice="Write failed; original restored and verified";
  else ui->notice="Write blocked; original backup retained";
  return rc?rc:save160_report.rc?save160_report.rc:OF_ERR_IO;
 }
 ui->notice="SAV restored; full cartridge readback PASS";
 return 0;
}
