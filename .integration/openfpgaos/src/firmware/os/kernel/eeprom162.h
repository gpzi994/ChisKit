/* EEPROM restore is deliberately separate from the read-only capture ABI.
 * A private cartridge epoch spans identity, original capture, write and full
 * readback. The caller must have verified a persistent original SAV first. */
#include "of_save162.h"
static uint8_t eeprom162_original[8192] __attribute__((aligned(32)));

static int eeprom162_sdram(const void *p,uint32_t n){
 uintptr_t a=(uintptr_t)p;
 return p&&a>=0x10000000u&&a<0x14000000u&&n<=0x14000000u-a;
}
static int eeprom162_overlap(const void *a,uint32_t an,const void *b,uint32_t bn){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return x<y+bn&&y<x+an;
}
static int eeprom162_header(const uint8_t expected_id[16]){
 if(!alive(NULL))return CB_NOR_CANCELLED;
 for(unsigned i=0;i<16;i+=2){
  uint16_t word=0;int rc=read_bus(NULL,0xa0u+i,&word);
  if(rc)return rc;
  if((uint8_t)word!=expected_id[i]||
     (uint8_t)(word>>8)!=expected_id[i+1])return CB_NOR_IO;
 }
 return 0;
}
static int eeprom162_full_read(struct eeprom139_session *session,
 unsigned bits,uint8_t *dst,uint32_t bytes){
 uint8_t second[8];
 for(uint32_t off=0;off<bytes;off+=8){
  int rc=eeprom139_read_owned(session,bits,off/8,dst+off);
  if(rc)return rc;
  rc=eeprom139_read_owned(session,bits,off/8,second);
  if(rc)return rc;
  if(memcmp(dst+off,second,8))return CB_NOR_IO;
 }
 return 0;
}
static int eeprom162_restore_impl(const void *source,void *disk_backup,
 uint32_t bytes,const uint8_t expected_id[16],void *out,uint32_t out_size,
 unsigned selected_type){
 of_save162_report r={0};
 if((selected_type&&selected_type!=(bytes==512u?1u:2u))||
    out_size!=sizeof r||!eeprom162_sdram(out,out_size)||
    (bytes!=512u&&bytes!=8192u)||!eeprom162_sdram(source,bytes)||
    !eeprom162_sdram(disk_backup,bytes)||!eeprom162_sdram(expected_id,16u)||
    eeprom162_overlap(source,bytes,disk_backup,bytes)||
    eeprom162_overlap(source,bytes,out,out_size)||
    eeprom162_overlap(disk_backup,bytes,out,out_size))return CB_NOR_ARGUMENT;
 uint8_t id[16];memcpy(id,expected_id,16);
 unsigned bits=bytes==512u?6u:14u;
 /* Reclassify hardware, not a ROM signature or stale UI hint. This service
  * is only reachable in an explicit EEPROM162 OS build. */
 struct cb_save139_result detected=save_auto139_kernel();
 int blank_selected=0;
#ifdef CB_SAVE171
 /* Explicit selected profile, not the ordinary automatic restore ABI.
  * Stage 9 proves the fresh detector actually reached serial acquisition
  * after parallel/Flash exclusion. An unknown ID/marker is not that proof. */
 /* Empty serial acquisition can classify as NONE, not only UNKNOWN.
  * This exception still requires stage 9 and uniform repeated reads below. */
 blank_selected=selected_type&&
  (detected.type==CB_SAVE139_UNKNOWN||detected.type==CB_SAVE139_NONE)&&
  !detected.rc&&!detected.flash_id&&!detected.bytes&&
  save_auto139_diag.stage==9&&!save_auto139_diag.probe_rc&&
  !save_auto139_diag.cleanup_rc&&!save_auto139_diag.restore_rc&&
  !save_auto139_diag.leave_rc;
#endif
 if(detected.rc||detected.type!=(bytes==512u?CB_SAVE139_EEPROM512:CB_SAVE139_EEPROM8K)||
    detected.bytes!=bytes){
  if(!blank_selected){r.rc=detected.rc?detected.rc:CB_NOR_UNSUPPORTED;goto finish;}
 }
 if(of_file_size(5u)!=(long)bytes){r.rc=CB_NOR_ARGUMENT;goto finish;}
 for(uint32_t off=0;off<bytes;off+=512u){
  r.rc=of_file_read(5u,off,(uint8_t *)disk_backup+off,512u);
  if(r.rc)goto finish;
 }
 if(busy){r.rc=CB_NOR_IO;goto finish;}
 struct eeprom139_session session={0};
 r.rc=eeprom139_begin();
 if(r.rc)goto release;
 r.stage=1;
 r.rc=eeprom162_header(id);if(r.rc)goto release;
 r.stage=2;
 r.rc=eeprom162_full_read(&session,bits,eeprom162_original,bytes);
 if(r.rc)goto release;
 if(blank_selected){
  /* Stable open bus is not a size detector. Only an explicit selected
   * capacity and uniform erased data qualify for this narrow fallback. */
  uint8_t fill=eeprom162_original[0];
  if(fill!=0&&fill!=255){r.rc=CB_NOR_UNSUPPORTED;goto release;}
  for(uint32_t off=1;off<bytes;off++)if(eeprom162_original[off]!=fill){
   r.rc=CB_NOR_UNSUPPORTED;goto release;
  }
 }
 if(memcmp(eeprom162_original,disk_backup,bytes)){
  r.rc=CB_NOR_IO;goto release;
 }
 r.stage=3;
 uint32_t touched=0;
 for(uint32_t off=0;off<bytes;off+=8){
  if(!memcmp((const uint8_t *)source+off,eeprom162_original+off,8))continue;
  touched=off+8; /* Include the block if its write fails mid-command. */
  r.rc=eeprom139_write_owned(&session,bits,off/8,(const uint8_t *)source+off);
  if(r.rc)goto rollback;
  r.attempted+=8;
 }
 r.stage=4;
 for(uint32_t off=0;off<bytes;off+=8){
  uint8_t actual[8];
  r.rc=eeprom139_read_owned(&session,bits,off/8,actual);
  if(r.rc)goto rollback;
  if(memcmp(actual,(const uint8_t *)source+off,8)){
   r.rc=CB_NOR_IO;goto rollback;
  }
  r.verified+=8;
 }
 r.stage=5;goto release;
rollback:
 r.stage=6;
 if(alive(NULL)&&!session.stuck){
  int failure=r.rc;
  for(uint32_t off=0;off<touched;off+=8){
   if(!memcmp((const uint8_t *)source+off,eeprom162_original+off,8))continue;
   int rc=eeprom139_write_owned(&session,bits,off/8,eeprom162_original+off);
   if(rc){r.restore_rc=rc;break;}
   r.restore_attempted+=8;
  }
  if(!r.restore_rc){
   for(uint32_t off=0;off<bytes;off+=8){
    uint8_t actual[8];
    int rc=eeprom139_read_owned(&session,bits,off/8,actual);
    if(rc||memcmp(actual,eeprom162_original+off,8)){
     r.restore_rc=rc?rc:CB_NOR_IO;break;
    }
    r.restore_verified+=8;
   }
  }
  r.rc=failure;
 }
release:
 {
  int cleanup=eeprom139_release_owned(&session);
  if(cleanup&&!r.rc)r.rc=cleanup;
  if(cleanup)r.cleanup_rc=cleanup;
 }
finish:
 memcpy(out,&r,sizeof r);
 return r.rc;
}
int eeprom162_restore_service(const void *source,void *disk_backup,
 uint32_t bytes,const uint8_t expected_id[16],void *out,uint32_t out_size){
 return eeprom162_restore_impl(source,disk_backup,bytes,expected_id,out,out_size,0);
}
#ifdef CB_SAVE171
/* OS-internal entry. Not exported until the versioned request dispatcher and
 * UI preflight agree on the selected type and capacity. */
static int eeprom171_restore_selected(const void *source,void *disk_backup,
 uint32_t bytes,const uint8_t expected_id[16],void *out,uint32_t out_size,
 unsigned selected_type){
 if(selected_type!=1&&selected_type!=2)return CB_NOR_ARGUMENT;
 return eeprom162_restore_impl(source,disk_backup,bytes,expected_id,out,out_size,selected_type);
}
#endif
