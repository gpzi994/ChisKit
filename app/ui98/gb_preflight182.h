#ifndef CB_GB_PREFLIGHT182_H
#define CB_GB_PREFLIGHT182_H
#include "model.h"
#include "gb_fallback181.h"
#include "../../core/gb_save175/gb_save175.h"
#include <string.h>
struct gb182_trace {
 int stage,rc,read_rc,cleanup_rc,db_rc;
 unsigned blocks,mismatch;
 uint8_t header[96];
};
/* Same reads and guards as dev.181; make every early exit observable. */
static inline int gb182_preflight(const struct cb_cart *expected,const struct gb181_ops *o,
 struct gb175_plan *plan,struct gb182_trace *t){
 memset(t,0,sizeof *t);memset(plan,0,sizeof *plan);t->stage=1;
 int rc=o->safe(o->ctx);t->read_rc=rc;
 if(!rc){t->stage=2;
  for(unsigned off=0;off<96&&!rc;off+=32){
   rc=o->read(o->ctx,0x100+off,t->header+off,32);t->read_rc=rc;
   if(!rc)t->blocks++;
  }
 }
 t->cleanup_rc=o->safe(o->ctx);
 if(!rc&&t->cleanup_rc){t->stage=3;rc=t->cleanup_rc;}
 if(rc){t->rc=rc;return rc;}
 struct cb_cart current={0};t->stage=4;
 if(!cb_parse_gb(&current,t->header)||current.result!=CB_READY){t->rc=GB175_IDENTITY;return t->rc;}
 t->stage=5;
 t->mismatch=(strcmp(current.title,expected->title)?1u:0u)|
  (current.type!=expected->type?2u:0u)|(current.checksum!=expected->checksum?4u:0u)|
  (current.ram_bytes!=expected->ram_bytes?8u:0u);
 if(t->mismatch){t->rc=GB175_IDENTITY;return t->rc;}
 t->stage=6;t->rc=gb179_plan(1,t->header,0,plan);return t->rc;
}
#endif
