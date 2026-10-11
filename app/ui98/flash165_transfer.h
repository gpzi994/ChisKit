/* Flash 64 KiB backup only. No ID, reset, program, erase or bank writes.
 * Analysis has already returned the device to array mode. Reuse SDRAM only. */
static int backup_flash165(void){
 unsigned size=65536u,flag=OF_CART_SAVE_ARRAY64,chunk=32u,timeout=50000u;
#ifdef CB_FLASH166
 if(ui->save_detected_type==5&&ui->save_detected_bytes==131072u){
  size=131072u;flag=OF_CART_SAVE_FLASH128;chunk=32768u;timeout=10000000u;
 }
#endif
 if(ui->save_rc||!((ui->save_detected_type==4&&ui->save_detected_bytes==65536u)||
                  (ui->save_detected_type==5&&size==131072u)))
  return OF_ERR_NOT_SUPPORTED;
 ui->task=size==65536?"Backup Flash 64 KiB":"Backup Flash 128 KiB";ui->total=size;ui->done=0;
 ui->phase="Check cartridge identity";paint();
 int rc=of_cart_enter_safe_idle();if(rc)return rc;
 rc=cart_read(0,sram137_header,32,50000);if(rc)return rc;
 for(unsigned off=0;off<sizeof sram137_header;off+=32){
  rc=cart_read(off,sram137_header+off,32,50000);if(rc)return rc;
 }
 struct cb_cart current;
 if(!cb_parse_gba(&current,sram137_header)||current.result!=CB_READY||
    strcmp(current.code,ui->cart.code)||strcmp(current.title,ui->cart.title))return OF_ERR_IO;
 for(unsigned pass=0;pass<2;pass++){
  rc=sram137_identity();if(rc)return rc;
  ui->phase=pass?"Read Flash / pass 2":"Read Flash / pass 1";paint();
  for(unsigned off=0;off<size;off+=chunk){
   if(!(off&1023u)&&sram137_cancel(0)){rc=OF_ERR_DENIED;goto cleanup;}
   rc=of_cart_read_space(OF_CART_BUS_GBA,OF_CART_SPACE_SAVE_PARALLEL,
    flag|off,save138_snapshots[pass]+off,chunk,timeout);
   if(rc)goto cleanup;
   if(!(off&4095u)){ui->done=off;ui->elapsed_ms=now()-began;paint();}
  }
 }
cleanup:
 {int clean=sram137_restore(0);if(clean)return clean;}
 if(rc)return rc;
 if(memcmp(save138_snapshots[0],save138_snapshots[1],size))return OF_ERR_IO;
 if(sram137_cancel(0))return OF_ERR_DENIED;
 cb_backup_name(ui->path,sizeof ui->path,&current,"sav",now());
#ifdef CB_FLASH170
 if(flash170_original_backup){
  char title[17],code[9];cb_name_part(title,sizeof title,current.title);cb_name_part(code,sizeof code,current.code);
  snprintf(ui->path,sizeof ui->path,"/Saves/chiskit/common/GBA-%s-%s-%luB-backup.sav",title,code,(unsigned long)size);
 }
#endif
 ui->phase="Create Flash SAV";paint();
 /* +1 means an existing same-size rotating backup was reopened successfully.
  * Still rewrite and verify the entire snapshot before allowing restore. */
 rc=of_output_open(ui->path,save169_file_size(size));if(rc<0)return rc;
 if(OF_SVC->file_size("slot:5")!=(long)save169_file_size(size))return OF_ERR_BAD_RANGE;
 for(unsigned off=0;off<size;off+=32768){
  ui->phase="SDRAM -> Flash SAV";paint();
  rc=of_output_write(off,save138_snapshots[0]+off,32768);if(rc)return rc;
 }
 ui->phase="Verify complete Flash SAV";paint();
 for(unsigned off=0;off<size;off+=32768){
  rc=backup_read(off,buffer,32768);if(rc)return rc;
  if(memcmp(buffer,save138_snapshots[0]+off,32768))return OF_ERR_IO;
 }
 struct md5_53 md;uint8_t digest[16];md5_init53(&md);
 md5_add53(&md,save138_snapshots[0],size);md5_end53(&md,digest);md5_hex53(digest,ui->hash);
 ui->done=size;ui->elapsed_ms=now()-began;
 ui->notice="Flash read twice + SAV compare PASS";
 return 0;
}
