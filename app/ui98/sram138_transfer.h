/* Parallel-save diagnostic: ROM hints never authorize or prevent reads.
 * Generic bank protocol, two independent snapshots, SDRAM only. No save writes. */
#include "../../core/save/hardware138.c"
static uint8_t save138_snapshots[2][131072] __attribute__((aligned(64)));
static int backup_sram138(void){
 ui->save_observed=0;ui->save_repeat=0;ui->save_rc=OF_ERR_IO;
 of_cart_caps_t caps;int rc=of_cart_get_caps(&caps);
 if(rc)return rc;
 if(caps.cart_caps_raw!=0x06713f03u||caps.gba_space_mask!=19)return OF_ERR_NOT_SUPPORTED;
 ui->task="Backup parallel save";ui->total=ui->save_profile==1?32768:131072;
 ui->phase="SRAM/FRAM only; no Flash/EEPROM";
 ui->notice="Confirm parallel SRAM/FRAM cartridge";
 if(!confirm())return OF_ERR_DENIED;
 rc=of_cart_enter_safe_idle();if(rc)return rc;
 /* Discard the first read after route changes, as in the proven ROM path. */
 rc=cart_read(0,sram137_header,32,50000);if(rc)return rc;
 for(unsigned i=0;i<192;i+=32){
  rc=cart_read(i,sram137_header+i,32,50000);if(rc)return rc;
 }
 struct cb_cart current;
 if(!cb_parse_gba(&current,sram137_header)||current.result!=CB_READY||
    strcmp(current.code,ui->cart.code)||strcmp(current.title,ui->cart.title))return OF_ERR_IO;
 unsigned profile=ui->save_profile;
 uint32_t capture=profile==1?32768:131072;
 ui->save_observed=0;ui->save_repeat=0;ui->save_rc=OF_ERR_IO;
 for(unsigned pass=0;pass<2;pass++){
  rc=sram137_identity();if(rc)return rc;
  ui->phase=pass?"Hardware read / pass 2":"Hardware read / pass 1";
  phase_began=now();
  for(uint32_t off=0;off<capture;off+=32768){
   if(sram137_cancel(0))return OF_ERR_DENIED;
   if(profile==1){
    for(unsigned i=0;i<32768;i+=32){
     if((i&1023u)==0&&sram137_cancel(0))return OF_ERR_DENIED;
     rc=of_cart_read_space(OF_CART_BUS_GBA,OF_CART_SPACE_SAVE_PARALLEL,
      off+i,save138_snapshots[pass]+off+i,32,50000);if(rc)return rc;
    }
   }else{
    rc=of_cart_read_space(OF_CART_BUS_GBA,OF_CART_SPACE_SAVE_PARALLEL,
     OF_CART_SAVE_BANKED_1M|off,save138_snapshots[pass]+off,32768,10000000);
    if(rc)return rc;
   }
   ui->done=off+32768;ui->phase_ms=now()-phase_began;ui->elapsed_ms=now()-began;paint();
  }
 }
 rc=sram137_restore(0);if(rc)return rc;
 struct cb_save_hw138_evidence evidence=cb_save_hw138_compare(
  profile==1?CB_SAVE_HW138_PARALLEL32_CONFIRMED:
  profile==2?CB_SAVE_HW138_CHISFLASH128_CONFIRMED:CB_SAVE_HW138_PARALLEL_PROBE,
  save138_snapshots[0],save138_snapshots[1],capture,profile!=1,profile!=1);
 ui->save_repeat=evidence.repeating_bytes;
 ui->save_observed=evidence.backup_bytes;
 ui->save_rc=evidence.state==CB_SAVE_HW138_STABLE||
  (evidence.state==CB_SAVE_HW138_UNIFORM&&evidence.backup_bytes)?0:OF_ERR_IO;
 if(ui->save_rc){ui->notice="Uncertain data; select known profile";return ui->save_rc;}
 uint32_t n=evidence.backup_bytes;
 ui->total=n;ui->done=0;
 cb_backup_name(ui->path,sizeof(ui->path),&current,"sav",now());
 ui->phase="Create new SAV";paint();
 rc=of_output_open(ui->path,save169_file_size(n));if(rc)return rc;
 if(OF_SVC->file_size("slot:5")!=(long)save169_file_size(n))return OF_ERR_BAD_RANGE;
 ui->phase="SDRAM -> SAV";phase_began=now();
 for(uint32_t off=0;off<n;off+=32768){
  if(sram137_cancel(0)){ui->notice="Incomplete SAV; do not restore";return OF_ERR_DENIED;}
  rc=of_output_write(off,save138_snapshots[0]+off,32768);if(rc)return rc;
  ui->done=off+32768;ui->phase_ms=now()-phase_began;ui->elapsed_ms=now()-began;paint();
 }
 ui->phase="Verify complete SAV";phase_began=now();
 for(uint32_t off=0;off<n;off+=32768){
  rc=backup_read(off,buffer,32768);if(rc)return rc;
  if(memcmp(buffer,save138_snapshots[0]+off,32768))return OF_ERR_IO;
  ui->done=off+32768;ui->phase_ms=now()-phase_began;ui->elapsed_ms=now()-began;paint();
 }
 struct md5_53 md;uint8_t digest[16];md5_init53(&md);
 md5_add53(&md,save138_snapshots[0],n);md5_end53(&md,digest);md5_hex53(digest,ui->hash);
 ui->notice=profile?"Full profile backup + file compare PASS":"PASS; size estimated from repetition";
 return 0;
}
