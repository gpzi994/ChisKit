/* Read-only restore preparation. Source and cartridge snapshots stay in SDRAM.
 * No save-data write is reachable from this entry. */
static uint8_t save159_expected_id[16];
static int prepare_save159(void){
 memset(save159_expected_id,0,sizeof save159_expected_id);
 int rc=check_save157();if(rc)return rc;
 const uint32_t n=ui->save_detected_bytes;
 const int banked=n==131072u;
 ui->task="Prepare SRAM restore";ui->total=n;ui->done=0;
 ui->phase="Read original save twice";paint();
 rc=of_cart_enter_safe_idle();if(rc)return rc;
 for(uint32_t off=0;off<n;off+=32768u){
  if(sram137_cancel(0))return OF_ERR_DENIED;
  for(unsigned pass=0;pass<2;pass++){
   uint8_t *dst=pass?buffer:save138_snapshots[1]+off;
   if(banked){
    rc=of_cart_read_space(OF_CART_BUS_GBA,OF_CART_SPACE_SAVE_PARALLEL,
     OF_CART_SAVE_BANKED_1M|off,dst,32768u,10000000u);
   }else{
    for(unsigned b=0;b<32768u&&!rc;b+=32u)
     rc=of_cart_read_space(OF_CART_BUS_GBA,OF_CART_SPACE_SAVE_PARALLEL,
      off+b,dst+b,32u,50000u);
   }
   if(rc)return rc;
  }
  if(memcmp(save138_snapshots[1]+off,buffer,32768u)){
   ui->notice="Original save changed between reads";return OF_ERR_IO;
  }
  ui->done=off+32768u;ui->elapsed_ms=now()-began;paint();
 }
 /* A removed cartridge must not become a plausible backup merely because
  * both save samples happened to agree. The banked adapter also restores
  * bank zero after each request; any restore error prevents file creation. */
 rc=of_cart_enter_safe_idle();if(rc)return rc;
 uint8_t h[192];struct cb_cart current={0};
 rc=cart_read(0,h,32,50000u);if(rc)return rc;
 for(unsigned off=0;off<sizeof h;off+=32u){
  rc=cart_read(off,h+off,32,50000u);if(rc)return rc;
 }
 if(!cb_parse_gba(&current,h)||current.result!=CB_READY||
    strcmp(current.code,ui->cart.code)||strcmp(current.title,ui->cart.title)){
  ui->notice="Cartridge changed during save backup";return OF_ERR_IO;
 }
 memcpy(save159_expected_id,h+0xa0,sizeof save159_expected_id);
 char title[17],code[9];
 cb_name_part(title,sizeof title,ui->cart.title);
 cb_name_part(code,sizeof code,ui->cart.code);
 snprintf(ui->path,sizeof ui->path,
  "/Saves/chiskit/common/GBA-%s-%s-%luKiB-backup.sav",
  title[0]?title:"UNKNOWN",code,(unsigned long)(n/1024u));
 ui->phase="Open original SAV backup";paint();
 rc=of_output_open(ui->path,n);if(rc<0)return rc;
 int reuse=rc==1;
 if(OF_SVC->file_size("slot:5")!=(long)n)return OF_ERR_BAD_RANGE;
 if(reuse){
  ui->phase="Compare existing backup";paint();
  for(uint32_t off=0;off<n;off+=32768u){
   rc=backup_read(off,buffer,32768u);if(rc)return rc;
   if(memcmp(buffer,save138_snapshots[1]+off,32768u))reuse=0;
  }
 }
 ui->phase="Write original SAV backup";ui->done=0;paint();
 for(uint32_t off=0;!reuse&&off<n;off+=32768u){
  rc=of_output_write(off,save138_snapshots[1]+off,32768u);if(rc)return rc;
  ui->done=off+32768u;ui->elapsed_ms=now()-began;paint();
 }
 ui->phase="Read back original SAV file";ui->done=0;paint();
 for(uint32_t off=0;off<n;off+=32768u){
  rc=backup_read(off,buffer,32768u);if(rc)return rc;
  if(memcmp(buffer,save138_snapshots[1]+off,32768u)){
   ui->notice="Original SAV file differs; write locked";return OF_ERR_IO;
  }
  ui->done=off+32768u;ui->elapsed_ms=now()-began;paint();
 }
 ui->notice="Original SAV file verified; no cartridge write";
 return 0;
}
