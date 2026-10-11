/* Kernel-only read-block transport. Included by nor108.c; output must be an
 * eight-byte kernel buffer, never an unchecked application pointer.
 * Feature builds must add core/save to includes and link core/save/eeprom139.c. */
#include <save/eeprom139.h>
#include <save/detect139.h>
#include "of_save139.h"
struct eeprom139_session { bool stuck; };
static int eeprom139_wait(bool require_done){
 uint32_t start=now_ms(NULL);
 for(unsigned n=0;n<1000000;n++){
  if(!alive(NULL))return CB_NOR_CANCELLED;
  fence();uint32_t status=CART_EEPROM_STATUS;
  if(!(status&1u))return require_done&&((status&7u)!=2u)?CB_NOR_IO:0;
  if((uint32_t)(now_ms(NULL)-start)>=20u)return CB_NOR_TIMEOUT;
 }
 return CB_NOR_TIMEOUT;
}
static int eeprom139_read_owned(void *ctx,unsigned address_bits,unsigned block,uint8_t *out){
 struct eeprom139_session *session=ctx;
 if(!out||(address_bits!=6&&address_bits!=14)||
    block>=(address_bits==6?64u:1024u))return CB_NOR_ARGUMENT;
 memset(out,0,8);
 int rc=alive(NULL)?0:CB_NOR_CANCELLED;bool started=false;
 uint32_t high=0,low=0;
 if(!rc&&((CART_CAPS>>16)!=1u||!(CART_CAPS&0x40u)))rc=CB_NOR_UNSUPPORTED;
 if(!rc){
  if(CART_EEPROM_STATUS&1u)rc=CB_NOR_IO;
  else {
   CART_EEPROM_DESC=(address_bits==14?0x4000u:0u)|block;fence();
   if(!alive(NULL))rc=CB_NOR_CANCELLED;
   else {
    CART_EEPROM_CONTROL=1;fence();started=true;
    rc=eeprom139_wait(true);
    if(!rc){high=CART_EEPROM_HIGH;low=CART_EEPROM_LOW;
     if(!alive(NULL))rc=CB_NOR_CANCELLED;
    }
   }
  }
 }
 // Never send cancellation/cleanup commands to a different cartridge epoch.
 if(private_owned&&!alive(NULL)){private_owned=false;busy=false;return CB_NOR_CANCELLED;}
 if(started&&rc){
  CART_EEPROM_CONTROL=2;fence();
  int drain=eeprom139_wait(false);
  if(drain){
   if(drain==CB_NOR_CANCELLED){private_owned=false;busy=false;}
   // A stuck engine deliberately retains the software lock and ownership.
   session->stuck=drain!=CB_NOR_CANCELLED;
   return drain;
  }
 }
 if(!rc)for(unsigned i=0;i<4;i++){
  out[i]=(uint8_t)(high>>(24-8*i));out[i+4]=(uint8_t)(low>>(24-8*i));
 }
 return rc;
}
/* One 8-byte serial write. Caller keeps the private cartridge epoch and
 * performs the original-file and identity checks before invoking this.
 * A completed bus transaction is NOT proof of EEPROM programming: wait for
 * the worst-case cycle, then independently read back the full save. */
#if CART_EEPROM162
static int eeprom139_write_owned(struct eeprom139_session *session,
 unsigned address_bits,unsigned block,const uint8_t in[8]){
 if(!in||(address_bits!=6&&address_bits!=14)||
    block>=(address_bits==6?64u:1024u))return CB_NOR_ARGUMENT;
 if(!alive(NULL))return CB_NOR_CANCELLED;
 if((CART_CAPS>>16)!=1u||!(CART_CAPS&0x40u))return CB_NOR_UNSUPPORTED;
 if(CART_EEPROM_STATUS&1u)return CB_NOR_IO;
 uint32_t high=0,low=0;
 for(unsigned i=0;i<4;i++){
  high=(high<<8)|in[i];low=(low<<8)|in[i+4];
 }
 CART_EEPROM_DESC=(address_bits==14?0x4000u:0u)|block;
 CART_EEPROM_HIGH=high;CART_EEPROM_LOW=low;fence();
 if(!alive(NULL))return CB_NOR_CANCELLED;
 CART_EEPROM_CONTROL=CART_EEPROM_WRITE_COMMAND;fence();
 int rc=eeprom139_wait(true);
 if(rc){
  if(private_owned&&!alive(NULL)){private_owned=false;busy=false;return CB_NOR_CANCELLED;}
  CART_EEPROM_CONTROL=2;fence();
  int drain=eeprom139_wait(false);
  if(drain){session->stuck=drain!=CB_NOR_CANCELLED;return drain;}
  return rc;
 }
 uint32_t start=now_ms(NULL);
 while((uint32_t)(now_ms(NULL)-start)<9u)
  if(!alive(NULL))return CB_NOR_CANCELLED;
 return 0;
}
#endif
static int eeprom139_release_owned(void *ctx){
 struct eeprom139_session *session=ctx;
 if(session->stuck)return CB_NOR_TIMEOUT;
 if(private_owned&&!alive(NULL)){private_owned=false;busy=false;return CB_NOR_CANCELLED;}
 int rc=private_owned?leave():0;busy=false;return rc;
}
static int eeprom139_begin(void){
 busy=true;job.authorized=false;job_token=0;mapper_kind=0;
 int rc=enter(2);
 if(!rc&&((CART_CAPS>>16)!=1u||!(CART_CAPS&0x40u)))rc=CB_NOR_UNSUPPORTED;
 return rc;
}
static int eeprom139_same_cart(void *ctx){(void)ctx;return alive(NULL);}
int eeprom139_read_block_kernel(unsigned bits,unsigned block,uint8_t *out){
 if(!out||(bits!=6&&bits!=14)||block>=(bits==6?64u:1024u))return CB_NOR_ARGUMENT;
 memset(out,0,8);if(busy)return CB_NOR_IO;
 struct eeprom139_session session={0};int rc=eeprom139_begin();
 if(!rc)rc=eeprom139_read_owned(&session,bits,block,out);
 int cleanup=eeprom139_release_owned(&session);if(!rc)rc=cleanup;
 if(rc)memset(out,0,8);
 return rc;
}
/* Kernel-owned SDRAM scratch only; retain one epoch for both repeated scans. */
static struct cb_eeprom139_report eeprom139_run_kernel(uint8_t *small,size_t small_n,
 uint8_t *large,size_t large_n,unsigned diagnostic){
 struct cb_eeprom139_report r={.first_difference=UINT32_MAX};
 uintptr_t s=(uintptr_t)small,l=(uintptr_t)large;
 if(!small||!large||small_n<512||large_n<8192||s>UINTPTR_MAX-512||
    l>UINTPTR_MAX-8192||(s<l+8192&&l<s+512)){r.rc=CB_NOR_ARGUMENT;return r;}
 if(busy){r.rc=CB_NOR_IO;return r;}
 struct eeprom139_session session={0};r.rc=eeprom139_begin();
 if(r.rc){r.cleanup_rc=eeprom139_release_owned(&session);return r;}
 const struct cb_eeprom139_ops ops={&session,eeprom139_read_owned,
  eeprom139_same_cart,eeprom139_release_owned};
 if(diagnostic==2)return cb_eeprom139_detect(&ops,small,small_n,large,large_n);
 return diagnostic?cb_eeprom139_capture(&ops,small,small_n,large,large_n):
  cb_eeprom139_probe(&ops,small,small_n,large,large_n);
}
struct cb_eeprom139_report eeprom139_probe_kernel(uint8_t *s,size_t sn,uint8_t *l,size_t ln){
 return eeprom139_run_kernel(s,sn,l,ln,false);
}
/* .bss is mapped to OSDATA SDRAM by os.ld. No DMA or PSRAM alias. */
static uint8_t eeprom139_small[512] __attribute__((aligned(32)));
static uint8_t eeprom139_large[8192] __attribute__((aligned(32)));
static struct cb_save139_result eeprom139_result(struct cb_eeprom139_report r){
 enum cb_eeprom139_state estimate=r.state;
 struct cb_save139_input evidence={0};
 evidence.io_rc=r.rc;evidence.restore_rc=r.cleanup_rc;
 evidence.cleanup_verified=!r.cleanup_rc;
 evidence.stable=estimate==CB_EEPROM139_512||estimate==CB_EEPROM139_8K;
 evidence.eeprom_tested=evidence.stable;
 evidence.eeprom_uniform=r.state==CB_EEPROM139_BLANK;
 evidence.eeprom_prefix_equal=estimate==CB_EEPROM139_8K;
 struct cb_save139_result result=cb_save139_classify(&evidence);
 if(!r.rc&&!r.cleanup_rc&&r.state==CB_EEPROM139_BLANK){
  result.type=CB_SAVE139_NONE;result.basis=CB_SAVE139_SERIAL_PROBE;
 }
 return result;
}
struct cb_save139_result eeprom139_detect_kernel(void){
 return eeprom139_result(eeprom139_run_kernel(eeprom139_small,
  sizeof eeprom139_small,eeprom139_large,sizeof eeprom139_large,2));
}
#ifdef CART_EEPROM139
#include "save_auto139.h"
#include "save_read143.h"
#endif
int eeprom139_detect_service(void *out,uint32_t size){
#ifdef CART_EEPROM139
 if(out&&size==sizeof(of_save143_readonly))return save_read143_kernel(out);
 if(out&&size==sizeof(of_save144_readonly))return save_read144_kernel(out);
 if(out&&size==sizeof(of_save145_banks))return save_read145_kernel(out);
#endif
 if(!out||(size!=sizeof(of_save139_result)&&size!=sizeof(of_save141_diagnostic)))return CB_NOR_ARGUMENT;
 #ifdef CART_EEPROM139
 struct cb_save139_result r=save_auto139_kernel();
 #else
 struct cb_save139_result r=eeprom139_detect_kernel();
 #endif
 of_save139_result wire={r.type,r.rc,r.bytes,r.basis};
 memcpy(out,&wire,sizeof wire);
 if(size==sizeof(of_save141_diagnostic)){
  memset((uint8_t*)out+sizeof wire,0,size-sizeof wire);
#ifdef CART_EEPROM139
  _Static_assert(sizeof(of_save141_diagnostic)-sizeof wire==sizeof save_auto139_diag,"diagnostic ABI");
  memcpy((uint8_t*)out+sizeof wire,&save_auto139_diag,sizeof save_auto139_diag);
#endif
 }
 return r.rc;
}
/* Diagnostic only: new clients get independently stable raw modes. Fatal
 * transport/epoch/cleanup errors suppress ALL payloads. Legacy clients retain
 * strict abort semantics. No identity, file IO, write command or PSRAM use. */
int eeprom139_capture_service(void *out,uint32_t size){
 if(!out||(size!=sizeof(of_eeprom139_snapshot)&&
    size!=offsetof(of_eeprom139_snapshot,address_bits)&&
    size!=offsetof(of_eeprom139_snapshot,stable_mask)))return CB_NOR_ARGUMENT;
 of_eeprom139_snapshot *wire=out;
 memset(wire,0,size);
 struct cb_eeprom139_report r=eeprom139_run_kernel(eeprom139_small,
  sizeof eeprom139_small,eeprom139_large,sizeof eeprom139_large,size==sizeof(*wire));
 wire->rc=r.rc;wire->cleanup_rc=r.cleanup_rc;wire->state=r.state;
 wire->bytes_read=r.bytes_read;wire->first_difference=r.first_difference;
 wire->prefix_equal=r.prefix_equal;
 if(size>=offsetof(of_eeprom139_snapshot,stable_mask)){
  wire->address_bits=r.address_bits;wire->pass=r.pass;
  wire->block=r.block;wire->stage=r.stage;
 }
 if(size==sizeof(*wire)){
  wire->stable_mask=r.stable_mask;wire->mismatch_mask=r.mismatch_mask;
  memcpy(wire->mode_difference,r.mode_difference,sizeof wire->mode_difference);
 }
 if(r.rc||r.cleanup_rc)return r.rc?r.rc:r.cleanup_rc;
 if(r.stable_mask&1u)memcpy(wire->small,eeprom139_small,sizeof wire->small);
 if(r.stable_mask&2u)memcpy(wire->large,eeprom139_large,sizeof wire->large);
 return 0;
}
