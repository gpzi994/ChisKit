#include "probe139.h"
struct cb_save139_probe_report cb_save139_probe_byte_timed(const struct cb_save139_probe_ops *o,int (*delay_ms)(void *,unsigned)){
 struct cb_save139_probe_report r={0};uint8_t second=0,restored=0;
 if(!o||!o->read||!o->write||!o->same_cart){r.rc=-1;return r;}
 if(!o->same_cart(o->ctx)){r.rc=-2;return r;}
 r.rc=o->read(o->ctx,4,&r.original);if(r.rc)return r;
 if(!o->same_cart(o->ctx)){r.rc=-2;return r;}
 r.rc=o->read(o->ctx,4,&second);if(r.rc)return r;
 if(second!=r.original){r.state=CB_SAVE139_PROBE_UNSTABLE;r.rc=-3;return r;}
 r.target=r.original^255;
 if(!o->same_cart(o->ctx)){r.rc=-2;return r;}
 /* Mark before issuing: a failed transaction may still have reached SRAM. */
 r.modified_possible=1;r.rc=o->write(o->ctx,4,r.target);
 /* LK firmware waits after each complete AGB save-write command group,
  * including this one-byte test, not only after Flash-ID entry. */
 if(!r.rc&&delay_ms)r.rc=o->same_cart(o->ctx)?delay_ms(o->ctx,10):-2;
 if(!r.rc){
  if(!o->same_cart(o->ctx))r.rc=-2;
  else r.rc=o->read(o->ctx,4,&r.observed);
 }
 /* Independent cleanup even when the test write/read failed. Never write the
  * saved byte into a replacement cartridge after a lifecycle change. */
 if(!o->same_cart(o->ctx)){r.restore_rc=-2;return r;}
 r.restore_rc=o->write(o->ctx,4,r.original);
 if(!r.restore_rc&&delay_ms)r.restore_rc=o->same_cart(o->ctx)?delay_ms(o->ctx,10):-2;
 if(!r.restore_rc){
  if(!o->same_cart(o->ctx))r.restore_rc=-2;
  else r.restore_rc=o->read(o->ctx,4,&restored);
  r.restored=restored;
  if(!r.restore_rc&&restored!=r.original)r.restore_rc=-4;
 }
 r.restore_verified=!r.restore_rc;
 if(r.rc||r.restore_rc)return r;
 if(r.observed==r.target)r.state=CB_SAVE139_PROBE_SRAM_LIKE;
 else if(r.observed==r.original)r.state=CB_SAVE139_PROBE_UNCHANGED;
 /* All transport and restoration checks passed, but the parallel response
  * proves neither SRAM nor unchanged Flash. Distinct from read failure. */
 else{r.state=CB_SAVE139_PROBE_INCONCLUSIVE;r.rc=-3;}
 return r;
}
struct cb_save139_probe_report cb_save139_probe_byte(const struct cb_save139_probe_ops *o){
 return cb_save139_probe_byte_timed(o,0);
}
