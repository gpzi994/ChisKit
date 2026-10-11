/* Formal read-only backup using the existing OS repeated capture service.
 * Detection chooses capacity; stability alone never chooses a mode. SDRAM only. */
static of_eeprom139_snapshot ee161_capture;
static int backup_eeprom161(void){
 const int type=ui->save_detected_type;
 const uint32_t bytes=ui->save_detected_bytes;
 if(ui->save_rc||!cb_eeprom161_supported(type,bytes))return OF_ERR_NOT_SUPPORTED;
 ui->task="Backup EEPROM";ui->total=bytes;ui->phase="Check cartridge identity";paint();
 int rc=of_cart_enter_safe_idle();if(rc)return rc;
 rc=cart_read(0,sram137_header,32,50000);if(rc)return rc;
 for(unsigned i=0;i<sizeof sram137_header;i+=32){
  rc=cart_read(i,sram137_header+i,32,50000);if(rc)return rc;
 }
 struct cb_cart current;
 if(!cb_parse_gba(&current,sram137_header)||current.result!=CB_READY||
    strcmp(current.code,ui->cart.code)||strcmp(current.title,ui->cart.title))return OF_ERR_IO;
 if(sram137_cancel(0))return OF_ERR_DENIED;
 ui->phase="Read EEPROM twice";paint();
 memset(&ee161_capture,0,sizeof ee161_capture);
 rc=of_eeprom139_capture(&ee161_capture);if(rc)return rc;
 if(ee161_capture.rc)return ee161_capture.rc;
 if(ee161_capture.cleanup_rc)return ee161_capture.cleanup_rc;
 unsigned mode=type==2?1:0;
 if(!(ee161_capture.stable_mask&(1u<<mode))||
    (ee161_capture.mismatch_mask&(1u<<mode)))return OF_ERR_IO;
 rc=sram137_restore(0);if(rc)return rc;
 if(sram137_cancel(0))return OF_ERR_DENIED;
 const uint8_t *data=mode?ee161_capture.large:ee161_capture.small;
 cb_backup_name(ui->path,sizeof ui->path,&current,"sav",now());
 ui->phase="Create EEPROM SAV";paint();
 rc=of_output_open(ui->path,save169_file_size(bytes));if(rc)return rc;
 if(OF_SVC->file_size("slot:5")!=(long)save169_file_size(bytes))return OF_ERR_BAD_RANGE;
 ui->phase="SDRAM -> SAV";paint();
 rc=of_output_write(0,data,bytes);if(rc)return rc;
 ui->phase="Verify complete SAV";paint();
 rc=backup_read(0,buffer,bytes);if(rc)return rc;
 if(memcmp(buffer,data,bytes))return OF_ERR_IO;
 struct md5_53 md;uint8_t digest[16];md5_init53(&md);
 md5_add53(&md,data,bytes);md5_end53(&md,digest);md5_hex53(digest,ui->hash);
 ui->done=bytes;ui->elapsed_ms=now()-began;
 ui->notice="Repeated read + SAV compare PASS";
 return 0;
}
