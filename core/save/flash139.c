#include "probe139.h"
static int live_read(const struct cb_save139_probe_ops *o,uint32_t a,uint8_t *v){
 return o->same_cart(o->ctx)?o->read(o->ctx,a,v):-2;
}
static int live_write(const struct cb_save139_probe_ops *o,uint32_t a,uint8_t v){
 return o->same_cart(o->ctx)?o->write(o->ctx,a,v):-2;
}
static unsigned known(uint16_t id){
 switch(id){case 0xbfd4:case 0x1f3d:case 0xc21c:case 0x321b:
 case 0xc209:case 0x6213:case 0xbf4b:case 0xbf5b:case 0xbf6d:case 0xffff:return 1;}
 return 0;
}
struct cb_save139_flash_report cb_save139_probe_flash(const struct cb_save139_flash_ops *f){
 struct cb_save139_flash_report r={0};
 if(!f||!f->delay_ms){r.rc=-1;return r;}
 const struct cb_save139_probe_ops *o=&f->bus;
 r.direct=cb_save139_probe_byte_timed(o,f->delay_ms);
 r.rc=r.direct.rc?r.direct.rc:r.direct.restore_rc;
 if(r.rc||r.direct.state!=CB_SAVE139_PROBE_UNCHANGED)return r;
 const uint32_t addresses[]={0x5555,0x2aaa,0};uint8_t v=0,lo=0;
 for(unsigned i=0;i<3;i++){
  r.rc=live_read(o,addresses[i],&r.original[i]);if(r.rc)return r;
  r.rc=live_read(o,addresses[i],&v);if(r.rc)return r;
  if(v!=r.original[i]){r.rc=-3;return r;}
 }
 r.rc=live_write(o,0x5555,0xaa);
 if(!r.rc)r.rc=live_write(o,0x2aaa,0x55);
 if(!r.rc)r.rc=live_write(o,0x5555,0x90);
 /* FlashGBX LK waits 10 ms after an AGB save command sequence: AT29LV512
  * needs settling before its identification bytes can be read. Do not
  * delay between unlock bytes. Even a failed wait must run reset cleanup. */
 if(!r.rc)r.rc=o->same_cart(o->ctx)?f->delay_ms(o->ctx,10):-2;
 if(!r.rc)r.rc=live_read(o,0,&v);
 if(!r.rc)r.rc=live_read(o,1,&lo);
 if(!r.rc){r.id=(uint16_t)((uint16_t)v<<8|lo);r.id_read=1;r.known_id=known(r.id);}
 /* Run the entire reset sequence after any partial ID entry/read failure.
  * Keep the first cleanup failure even if subsequent operations succeed. */
 const uint32_t reset_a[]={0x5555,0x2aaa,0x5555,0};
 const uint8_t reset_v[]={0xaa,0x55,0xf0,0xf0};
 for(unsigned i=0;i<4;i++){
  int rc=live_write(o,reset_a[i],reset_v[i]);
  if(rc&&!r.cleanup_rc)r.cleanup_rc=rc;
  if(i>=2&&o->same_cart(o->ctx)){
   rc=f->delay_ms(o->ctx,10);if(rc&&!r.cleanup_rc)r.cleanup_rc=rc;
  }
 }
 /* If reset failed, don't issue arbitrary saved values into an uncertain
  * command state. Report cleanup failure and leave all operations locked. */
 if(r.cleanup_rc)return r;
 if(!r.known_id){
  for(unsigned i=0;i<3;i++){
   int rc=live_write(o,addresses[i],r.original[i]);
   if(rc&&!r.cleanup_rc)r.cleanup_rc=rc;
  }
 }
 for(unsigned i=0;i<3;i++){
  int rc=live_read(o,addresses[i],&v);
  if(!rc&&v!=r.original[i]){
   if(!r.cleanup_rc){r.mismatch_address=addresses[i];r.mismatch_expected=r.original[i];r.mismatch_actual=v;}
   rc=-4;
  }
  if(rc&&!r.cleanup_rc)r.cleanup_rc=rc;
 }
 r.exit_verified=!r.cleanup_rc;
 r.values_restored=!r.cleanup_rc;
 return r;
}
