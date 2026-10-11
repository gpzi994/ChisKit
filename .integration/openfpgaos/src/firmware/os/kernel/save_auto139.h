/* Automatic parallel -> Flash -> EEPROM classification.
 * All scratch is OSDATA SDRAM. No PSRAM alias or write authorization.
 * Parallel acquisition and temporary-write/ID probing share one epoch.
 */
static uint8_t save_auto139_parallel[131072] __attribute__((aligned(32)));
static struct cb_save139_diag save_auto139_diag;
/* EEPROM cartridges leave the parallel save data bus undriven. A bus keeper
 * can echo the probe write (and its restoration), falsely proving SRAM.
 * GBA ROM reads drive A16..A23, the same physical pins used by save D0..D7.
 * Read one save address after alternating ROM address-high values. A real
 * parallel memory drives the same byte independently of this prior traffic.
 * Only reads are issued here; transport errors remain fatal. */
static int save_auto161_parallel_driver(int *driven){
 const uint32_t primes[4]={0x00aa0000u,0x01540000u,0x00aa0000u,0x01540000u};
 uint8_t sample[4];
 *driven=0;
 for(unsigned i=0;i<4;i++){
  if(!alive(NULL))return CB_NOR_CANCELLED;
  CART_BUS=2;fence();
  uint16_t ignored=0;int rc=read_bus(NULL,primes[i],&ignored);
  if(rc)return rc;
  rc=save139_read(NULL,4,&sample[i]);if(rc)return rc;
 }
 /* Repeat each precharge separately: random read instability must not be
  * mistaken for a reproducible undriven-bus response. */
 if(sample[0]!=sample[2]||sample[1]!=sample[3])return CB_NOR_IO;
 *driven=sample[0]==sample[1];
 return 0;
}
/* An optional OS-only continuation runs while the classified SRAM and the
 * captured bytes are still owned by the same lifecycle epoch. Applications
 * cannot supply this callback. The ordinary detection entry remains read-only. */
typedef int (*save_auto139_sram_owned_fn)(const struct cb_save139_result *,
 const uint8_t *,const uint8_t *,void *);
#ifdef CB_SAVE171
#include <save/blank171.h>
#endif
static struct cb_save139_result save_auto139_kernel_owned_impl(
 save_auto139_sram_owned_fn sram_owned,void *owned_ctx,uint32_t selected_bytes){
 struct cb_save139_result r={.type=CB_SAVE139_UNKNOWN};
 struct cb_save139_input p={0};
 memset(&save_auto139_diag,0,sizeof save_auto139_diag);
 save_auto139_diag.stage=1;
 if(busy){r.rc=CB_NOR_IO;return r;}
 busy=true;job.authorized=false;job_token=0;mapper_kind=0;
 int rc=enter(2),cleanup=0;bool bank_touched=false;
 if(!rc&&((CART_CAPS>>16)!=1u||(CART_CAPS&0x7fu)!=0x7fu))rc=CB_NOR_UNSUPPORTED;
 uint8_t header[192];
 if(!rc)save_auto139_diag.stage=2;
 for(unsigned i=0;i<sizeof header&&!rc;i+=2){
  uint16_t word=0;rc=read_bus(NULL,i,&word);
  header[i]=(uint8_t)word;header[i+1]=(uint8_t)(word>>8);
 }
 bool marker=false;
 if(!rc){r=cb_save139_header_marker(header,sizeof header);marker=r.type!=CB_SAVE139_UNKNOWN;}
 if(!rc&&!marker&&!memcmp(header+0xa0,"NGC-HIKARU3",10)&&
    !memcmp(header+0xac,"GHTJ",4)&&header[0xbd]==0xb3){
  r=(struct cb_save139_result){.type=CB_SAVE139_DACS,.bytes=1048576,.basis=CB_SAVE139_SPECIAL_SIGNATURE};marker=true;
 }
 if(!rc&&!marker){
  save_auto139_diag.stage=3;
  uint8_t signature[12];
  for(unsigned i=0;i<sizeof signature&&!rc;i+=2){
   uint16_t word=0;rc=read_bus(NULL,0x1ffe000u+i,&word);
   signature[i]=(uint8_t)word;signature[i+1]=(uint8_t)(word>>8);
  }
  if(!rc&&!memcmp(signature,"AGBFLASHDACS",12)){
   r=(struct cb_save139_result){.type=CB_SAVE139_DACS,.bytes=1048576,.basis=CB_SAVE139_SPECIAL_SIGNATURE};marker=true;
  }
 }
 int parallel_driven=1;
 if(!rc&&!marker){
  save_auto139_diag.stage=11;
  rc=save_auto161_parallel_driver(&parallel_driven);
 }
 /* A valid extension marker bypasses active probing, including size=0. */
 for(unsigned bank=0;bank<2&&!rc&&!marker&&parallel_driven;bank++){
  save_auto139_diag.stage=4+bank;
  bank_touched=true;rc=sram138_bank(bank);
  for(unsigned i=0;i<65536&&!rc;i++)rc=save139_read(NULL,i,&save_auto139_parallel[bank*65536+i]);
 }
 if(bank_touched){cleanup=alive(NULL)?sram138_bank(0):CB_NOR_CANCELLED;if(cleanup)save_auto139_diag.stage=6;}
 if(!rc&&!cleanup&&!marker&&parallel_driven){
  const struct cb_save139_flash_ops ops={{NULL,save139_read,save139_write,save139_alive},save139_delay};
  struct cb_save139_flash_report f=cb_save139_probe_flash(&ops);
  save_auto139_diag.stage=(f.direct.rc||f.direct.restore_rc)?7:8;
  save_auto139_diag.id=f.id;save_auto139_diag.id_read=f.id_read;
  save_auto139_diag.probe_rc=f.rc;save_auto139_diag.cleanup_rc=f.cleanup_rc;
  save_auto139_diag.direct_rc=f.direct.rc;save_auto139_diag.restore_rc=f.direct.restore_rc;
  save_auto139_diag.original=f.direct.original;save_auto139_diag.target=f.direct.target;
  save_auto139_diag.observed=f.direct.observed;save_auto139_diag.restored=f.direct.restored;
  save_auto139_diag.mismatch_address=f.mismatch_address;
  save_auto139_diag.expected=f.mismatch_expected;save_auto139_diag.actual=f.mismatch_actual;
  rc=f.rc;cleanup=f.cleanup_rc?f.cleanup_rc:f.direct.restore_rc;
  p.stable=1;p.cleanup_verified=f.direct.restore_verified&&
   (f.direct.state==CB_SAVE139_PROBE_SRAM_LIKE||f.exit_verified);
  p.parallel_writable=f.direct.state==CB_SAVE139_PROBE_SRAM_LIKE;
  p.bank0_read=p.bank1_read=1;
  p.repeating_bytes=cb_save139_repeat(save_auto139_parallel,sizeof save_auto139_parallel);
  /* Only a semantic mismatch AFTER verified restoration may try serial.
   * Never swallow transport -3, initial instability, ID or cleanup errors. */
  if(f.direct.state==CB_SAVE139_PROBE_INCONCLUSIVE&&f.rc==-3&&
     f.direct.rc==-3&&!cleanup&&f.direct.restore_verified&&
     !f.id_read&&alive(NULL)&&p.repeating_bytes>=32&&p.repeating_bytes<=256){
   rc=0;p.cleanup_verified=1;
  }
  /* ID zero means no Flash ID; an unknown nonzero ID blocks serial fallback. */
  p.flash_id_valid=f.id_read&&f.id!=0;p.flash_id=f.id;
  p.banks_equal=!memcmp(save_auto139_parallel,save_auto139_parallel+65536,65536);
  p.parallel_all_ff=1;
  for(unsigned i=0;i<sizeof save_auto139_parallel;i++)if(save_auto139_parallel[i]!=255){p.parallel_all_ff=0;break;}
 }
 p.io_rc=rc;p.restore_rc=cleanup;
 int selected_blank=0;
#ifdef CB_SAVE171
 selected_blank=sram_owned&&selected_bytes&&!marker&&
  cb_save171_blank_sram(&p,save_auto139_parallel,sizeof save_auto139_parallel,selected_bytes);
#endif
 /* Prior ROM address dependence excludes a stable parallel device without
  * writing any save byte. This is separate from the old full-bank evidence. */
 if(!marker&&!selected_blank&&((!rc&&!cleanup&&!parallel_driven)||cb_save139_needs_eeprom(&p))){
  save_auto139_diag.stage=9;
  if(sram_owned){rc=CB_NOR_UNSUPPORTED;goto finish_owned;}
  struct eeprom139_session session={0};
  const struct cb_eeprom139_ops ops={&session,eeprom139_read_owned,
   eeprom139_same_cart,eeprom139_release_owned};
  /* Same owner and epoch through both serial reads and pin release. */
  return eeprom139_result(cb_eeprom139_detect(&ops,eeprom139_small,
   sizeof eeprom139_small,eeprom139_large,sizeof eeprom139_large));
 }
 if(!rc&&!cleanup&&!marker){
  p.io_rc=0;p.restore_rc=0;
  r=cb_save139_classify(&p);
 }
 if(sram_owned&&!rc&&!cleanup){
#ifdef CB_SAVE171
  if(selected_bytes&&!marker&&r.type==CB_SAVE139_UNKNOWN&&
     cb_save171_blank_sram(&p,save_auto139_parallel,sizeof save_auto139_parallel,selected_bytes)){
   /* Re-read the selected range under the same owner after temporary probe
    * restoration. A changing/open bus must not become a writable profile. */
   for(unsigned bank=0;bank<(selected_bytes==131072u?2u:1u)&&!rc;bank++){
    bank_touched=true;rc=sram138_bank(bank);
    uint32_t limit=selected_bytes==32768u?32768u:65536u;
    for(uint32_t a=0;a<limit&&!rc;a++){
     uint8_t value=0;rc=save139_read(NULL,a,&value);
     if(!rc&&value!=save_auto139_parallel[bank*65536u+a])rc=CB_NOR_IO;
    }
   }
   cleanup=alive(NULL)?sram138_bank(0):CB_NOR_CANCELLED;
   if(rc||cleanup)goto finish_owned;
   /* This temporary result is private to this explicit continuation. The
    * ordinary detector keeps returning UNKNOWN for a uniform SRAM. */
   r.type=selected_bytes==32768u?CB_SAVE139_SRAM32:CB_SAVE139_SRAM128;
   r.bytes=selected_bytes;r.backup_allowed=1;r.basis=CB_SAVE139_PARALLEL_PROBE;
  }
#else
  (void)selected_bytes;
#endif
  if(marker||!r.backup_allowed||
     (r.type!=CB_SAVE139_SRAM32&&r.type!=CB_SAVE139_SRAM64&&
      r.type!=CB_SAVE139_SRAM128))rc=CB_NOR_UNSUPPORTED;
  else rc=sram_owned(&r,header,save_auto139_parallel,owned_ctx);
 }
finish_owned:
 int end_rc=private_owned?leave():0;busy=false;
 save_auto139_diag.leave_rc=end_rc;
 if(end_rc&&!rc&&!cleanup)save_auto139_diag.stage=10;
 if(!cleanup)cleanup=end_rc;
 if(rc||cleanup){r=(struct cb_save139_result){.type=CB_SAVE139_UNKNOWN,.rc=cleanup?cleanup:rc};return r;}
 return r;
}
static struct cb_save139_result save_auto139_kernel_owned(
 save_auto139_sram_owned_fn sram_owned,void *owned_ctx){
 return save_auto139_kernel_owned_impl(sram_owned,owned_ctx,0);
}
static struct cb_save139_result save_auto139_kernel(void){
 return save_auto139_kernel_owned(NULL,NULL);
}
