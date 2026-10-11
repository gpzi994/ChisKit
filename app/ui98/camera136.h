#ifndef CB_CAMERA136_H
#define CB_CAMERA136_H
#include <stdint.h>
/* Caller provides cb_probe_ops from its model header. */
#include <string.h>
/* Application-only diagnostic. No mapper/NOR/save commands or GBA fallback. */
struct cb_camera136_report {
 uint8_t bytes[3][96];
 unsigned mask[3], completed;
 int rc[3], safe_before, safe_after;
};
extern struct cb_camera136_report cb_camera136_report;
static inline void cb_camera136_read(struct cb_camera136_report *r,const struct cb_probe_ops *o){
 uint8_t discard[32];
 memset(r,0,sizeof(*r));
 r->safe_before=o->safe(o->ctx);
 if(r->safe_before)return;
 for(unsigned pass=0;pass<3;pass++){
  r->rc[pass]=o->read(o->ctx,1,0x100,discard,32);
  for(unsigned block=0;block<3&&!r->rc[pass];block++){
   r->rc[pass]=o->read(o->ctx,1,0x100+32*block,r->bytes[pass]+32*block,32);
   if(!r->rc[pass])r->mask[pass]|=1u<<block;
  }
  r->completed=pass+1;
  if(r->rc[pass])break;
 }
 r->safe_after=o->safe(o->ctx);
}
#endif
