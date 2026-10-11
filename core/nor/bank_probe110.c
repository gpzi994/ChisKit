// SPDX-License-Identifier: AGPL-3.0-only
#include "bank_probe110.h"
#include <string.h>
static bool candidate(const struct cb_nor_probe*p,struct cb_bank110_capture*c){
 if(!p||!p->entry||!p->repeated||!p->restored||p->plan_rc||
    p->entry->bus!=CHISBURNER_NOR_IDBUS_GBA||p->plan.entry!=p->entry||
    p->plan.recipe.unit!=2||p->plan.window_bytes!=CB_BANK110_WINDOW||
    chisburner_nor_db_lookup_gba(p->id)!=p->entry)return false;
 /* The ordinary probe distinguishes absent from invalid CFI. Known AMD
  * metadata fallback is also usable across windows, but only if rebuilding
  * that exact database plan produces the same geometry. Intel has no such
  * fallback. Never infer a layout merely from the requested capacity. */
 if(!p->cfi_valid){
  struct cb_nor_plan fallback;
  if(cb_nor_plan_make(p->entry,p->method,p->swapped,NULL,&fallback)||
     memcmp(&fallback.geometry,&p->plan.geometry,sizeof(fallback.geometry)))return false;
 }
 /* These IDs are already normalized by the full probe. No raw-wire guess. */
 if((p->method==CB_NOR_MSP55_AAA||p->method==CB_NOR_MSP55_5555)&&
    !memcmp(p->id,(uint8_t[]){2,1,0x7d,0x7e},4)&&
    p->plan.geometry.bytes==0x4000000){
  c->kind=CB_BANK110_TYPE1;c->bytes=0x4000000;c->reset_stride=0x400000;
 }else if(p->method==CB_NOR_INTEL&&
    chisburner_nor_entry_flags(p->entry)&CHISBURNER_NOR_FLAG_INTEL_88B0&&
    p->plan.geometry.bytes==0x8000000){
  c->kind=CB_BANK110_TYPE1;c->bytes=0x8000000;
 }else{
  /* Mori's ordinary AMD route uses two SRAM-style latch writes, not type 1.
   * Bind to the exact imported family IDs, never any large unknown chip. */
  static const uint8_t ids[3][8]={
   {1,0,0x7e,0x22,0x23,0x22,1,0x22},
   {1,0,0x7e,0x22,0x28,0x22,1,0x22},
   {1,0,0x7e,0x22,0x48,0x22,1,0x22}};
  bool found=false;
  for(unsigned k=0;k<3;k++){
   enum cb_nor_method m=k?CB_NOR_AMD_555:CB_NOR_AMD_AAA;
   uint32_t size=UINT32_C(0x4000000)<<k;
   if(p->method==m&&!memcmp(p->id,ids[k],8)&&p->plan.geometry.bytes==size){
    c->kind=CB_BANK110_AMD_BASIC;c->bytes=size;found=true;break;
   }
  }
  if(!found)return false;
 }
 c->banks=c->bytes/CB_BANK110_WINDOW;return true;
}
static bool live(const struct cb_bank110_io*i,uint32_t epoch){
 return i->alive(i->ctx)&&i->epoch(i->ctx)==epoch;
}
static int select_bank(const struct cb_bank110_io*i,struct cb_bank110_capture*c,
 unsigned bank,bool*route){
 struct cb_bank110_write w[3];size_t n=cb_bank110_select(c->kind,c->bytes,bank,w);
 *route=false;if(!n)return CB_NOR_ARGUMENT;
 for(size_t k=0;k<n;k++){
  if(!live(i,c->epoch))return CB_NOR_CANCELLED;
  int rc=i->control(i->ctx,w[k].address,w[k].value);
  if(!live(i,c->epoch))return CB_NOR_CANCELLED;
  if(rc)return rc;
 }
 uint32_t start=i->now_ms(i->ctx);
 for(unsigned k=0;k<100000;k++){
  if(!live(i,c->epoch))return CB_NOR_CANCELLED;
  if((uint32_t)(i->now_ms(i->ctx)-start)>=1){*route=true;return 0;}
  i->yield(i->ctx);
 }
 return CB_NOR_TIMEOUT;
}
static bool matches(const struct cb_nor_probe*a,const struct cb_nor_probe*b){
 return b->repeated&&b->restored&&a->cfi_valid==b->cfi_valid&&!b->plan_rc&&
  a->entry==b->entry&&a->method==b->method&&a->swapped==b->swapped&&
  !memcmp(a->id,b->id,sizeof(a->id))&&
  !memcmp(&a->plan.geometry,&b->plan.geometry,sizeof(a->plan.geometry));
}
int cb_bank110_capture(const struct cb_bank110_io*i,const struct cb_nor_probe*seed,
 struct cb_nor_probe*s,struct cb_bank110_capture*out){
 if(!out)return CB_NOR_ARGUMENT;
 memset(out,0,sizeof(*out));
 if(!i||!s||s==seed||!i->control||!i->probe||!i->alive||!i->epoch||
    !i->now_ms||!i->yield||!i->release)return CB_NOR_ARGUMENT;
 if(!candidate(seed,out))return CB_NOR_UNSUPPORTED;
 out->epoch=i->epoch(i->ctx);
 bool route=false;int rc=0;
 for(unsigned bank=0;bank<out->banks&&!rc;bank++){
  rc=select_bank(i,out,bank,&route);if(rc)break;
  for(unsigned pass=0;pass<2&&!rc;pass++){
   if(!live(i,out->epoch)){rc=CB_NOR_CANCELLED;break;}
   memset(s,0,sizeof(*s));rc=i->probe(i->ctx,s);
   if(!live(i,out->epoch))rc=CB_NOR_CANCELLED;
   if(rc){route=false;break;} /* Probe cleanup state is unknown on failure. */
   if(!matches(seed,s)){rc=CB_NOR_VERIFY;break;}
   if(!pass){memcpy(out->id[bank],s->id,8);memcpy(out->normal[bank],s->baseline,sizeof(s->baseline));}
   else if(memcmp(out->normal[bank],s->baseline,sizeof(s->baseline)))rc=CB_NOR_VERIFY;
   if(!bank&&memcmp(seed->baseline,s->baseline,sizeof(s->baseline)))rc=CB_NOR_VERIFY;
  }
 }
 /* No writes after a partial latch sequence, failed probe or epoch change. */
 if(route&&live(i,out->epoch)){
  int cleanup=select_bank(i,out,0,&route);
  if(!cleanup){
   memset(s,0,sizeof(*s));cleanup=i->probe(i->ctx,s);
   if(!live(i,out->epoch))cleanup=CB_NOR_CANCELLED;
   if(!cleanup&&(!matches(seed,s)||memcmp(seed->baseline,s->baseline,sizeof(s->baseline))))cleanup=CB_NOR_VERIFY;
  }
  out->restored=!cleanup;
  if(!rc)rc=cleanup;
 }
 i->release(i->ctx);
 out->qualified=!rc&&out->restored;
 return rc?rc:out->qualified?0:CB_NOR_VERIFY;
}
