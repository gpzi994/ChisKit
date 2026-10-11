/* Read-only preflight for the native Saves picker. No cartridge data write. */
static char save_check_notice157[64];
static int check_save157(void){
 uint32_t n=ui->save_detected_bytes;
 ui->task="Check selected SRAM SAV";ui->phase="Check file size";paint();
 long size=OF_SVC->file_size("slot:6");
 if(ui->cart.platform!=CB_GBA||ui->cart.result!=CB_READY||ui->save_rc||
    !((ui->save_detected_type==3&&n==32768u)||
      (ui->save_detected_type==8&&n==131072u))){
  ui->notice="SRAM hardware not confirmed; refresh first";return OF_ERR_BAD_RANGE;}
 if(size<=0){ui->notice="Select SAV via Pocket: Saves SAV";return OF_ERR_BAD_RANGE;}
 if(size!=(long)n){
  snprintf(save_check_notice157,sizeof save_check_notice157,
   "SAV %ld B; cartridge needs %lu B",size,(unsigned long)n);
  ui->notice=save_check_notice157;return OF_ERR_BAD_RANGE;
 }
 ui->total=n;ui->phase="Read SAV twice into SDRAM";paint();
 for(uint32_t off=0;off<n;off+=32768u){
  int rc=file_read(6,off,save138_snapshots[0]+off,32768u);if(rc)return rc;
  rc=file_read(6,off,buffer,32768u);if(rc)return rc;
  if(memcmp(save138_snapshots[0]+off,buffer,32768u)){
   ui->notice="Selected SAV changed while reading";return OF_ERR_IO;}
  ui->done=off+32768u;ui->elapsed_ms=now()-began;paint();
 }
 ui->phase="Recheck cartridge identity";paint();
 int rc=of_cart_enter_safe_idle();if(rc)return rc;
 uint8_t h[192];struct cb_cart current={0};
 rc=cart_read(0,h,32,50000);if(rc)return rc;
 for(unsigned off=0;off<sizeof(h);off+=32){
  rc=cart_read(off,h+off,32,50000);if(rc)return rc;
 }
 if(!cb_parse_gba(&current,h)||current.result!=CB_READY||
    strcmp(current.code,ui->cart.code)||strcmp(current.title,ui->cart.title)){
  ui->notice="Cartridge changed; refresh first";return OF_ERR_IO;
 }
 ui->notice="SAV checked; cartridge unchanged; no write";return 0;
}
