static of_eeprom139_snapshot ee_capture;
static int capture_eeprom139(void){
 ui->task="EEPROM read-only diagnostic";
 ui->notice="EEPROM cart only; B cancels";
 if(!confirm())return OF_ERR_DENIED;
 ui->phase="Read both modes twice";paint();
 memset(&ee_capture,0,sizeof ee_capture);
 ee_capture.first_difference=UINT32_MAX;
 int rc=of_eeprom139_capture(&ee_capture);
 if(rc){
  snprintf(ui->io_diag,sizeof ui->io_diag,"Bits %s pass %c block %lu",
   ee_capture.address_bits==6?"6":ee_capture.address_bits==14?"14":"?",
   ee_capture.pass<=2?'0'+(int)ee_capture.pass:'?',(unsigned long)ee_capture.block);
  snprintf(ui->io_regs,sizeof ui->io_regs,"S%c diff %08lx clean %d",
   ee_capture.stage<=5?'0'+(int)ee_capture.stage:'?',
   (unsigned long)ee_capture.first_difference,(int)ee_capture.cleanup_rc);
  ui->notice="Capture failed; no new files";
  return rc;
 }
 uint32_t stamp=now();
 snprintf(ui->io_diag,sizeof ui->io_diag,"6-bit %s; 14-bit %s",
  ee_capture.stable_mask&1?"stable":"unstable",
  ee_capture.stable_mask&2?"stable":"unstable");
 snprintf(ui->io_regs,sizeof ui->io_regs,"Diff6 %08lx Diff14 %08lx",
  (unsigned long)ee_capture.mode_difference[0],(unsigned long)ee_capture.mode_difference[1]);
 if(!ee_capture.stable_mask){ui->notice="No stable mode; no new files";return OF_ERR_IO;}
 ui->total=(ee_capture.stable_mask&1?512:0)+(ee_capture.stable_mask&2?8192:0);ui->done=0;
 for(unsigned mode=0;mode<2;mode++){
  if(!(ee_capture.stable_mask&(1u<<mode)))continue;
  uint32_t n=mode?8192:512;
  const uint8_t *data=mode?ee_capture.large:ee_capture.small;
  snprintf(ui->path,sizeof(ui->path),"/Saves/chiskit/common/ee139-%08lx-%s.bin",(unsigned long)stamp,mode?"large":"small");
  ui->phase="Create raw mode file";paint();
  rc=of_output_open(ui->path,n);if(rc)return rc;
  if(OF_SVC->file_size("slot:5")!=(long)n)return OF_ERR_BAD_RANGE;
  ui->phase="SDRAM -> TF raw data";paint();
  rc=of_output_write(0,data,n);if(rc)return rc;
  ui->phase="Read back TF file";paint();
  rc=backup_read(0,buffer,n);if(rc)return rc;
  if(memcmp(buffer,data,n))return OF_ERR_IO;
  ui->done+=n;ui->elapsed_ms=now()-began;paint();
 }
 ui->notice=ee_capture.stable_mask==3?"Files verified; compare on PC":"Stable mode saved; compare on PC";
 return 0;
}
