// SPDX-License-Identifier: AGPL-3.0-only
// Adapted from Mori's read_id_with_cmdset, raw_id_methods and CFI capture.
#include "probe.h"
#include <string.h>
#define TRY(x) do {rc=(x);if(rc)goto cleanup;}while(0)
static uint16_t swap01(uint16_t v){return (uint16_t)((v&0xfffcu)|((v&1u)<<1)|((v&2u)>>1));}
static int check(const struct cb_nor_io*i){
 return !i->alive(i->ctx)||i->cancelled(i->ctx)?CB_NOR_CANCELLED:0;
}
static int wr(const struct cb_nor_io*i,uint32_t a,uint16_t v){
 if(!i->alive(i->ctx))return CB_NOR_CANCELLED;
 return i->write(i->ctx,a,v)?CB_NOR_IO:0;
}
static int rd(const struct cb_nor_io*i,uint32_t a,uint16_t*v){
 int rc=check(i);if(rc)return rc;
 return i->read(i->ctx,a,v)?CB_NOR_IO:0;
}
static int reset(const struct cb_nor_io*i,const struct cb_nor_recipe*r){
 int rc;
 if(r->method>=CB_NOR_INTEL){
  rc=wr(i,0,r->swapped?swap01(0x50):0x50);if(rc)return rc;
 }
 rc=wr(i,0,r->reset);
 if(!rc&&r->method==CB_NOR_INTEL_DUAL)rc=wr(i,2,r->reset);
 return rc;
}
static int settle(const struct cb_nor_io*i){
 uint32_t t=i->now_ms(i->ctx);
 for(unsigned n=0;n<100000;n++){
  int rc=check(i);if(rc)return rc;
  if((uint32_t)(i->now_ms(i->ctx)-t)>=1)return 0;
  i->yield(i->ctx);
 }
 return CB_NOR_TIMEOUT;
}
static int normal(const struct cb_nor_io*i,unsigned unit,uint16_t v[16]){
 for(unsigned n=0;n<16;n++){
  int rc=rd(i,n<8?n*2:0x100+(n-8)*unit,&v[n]);if(rc)return rc;
 }
 return 0;
}
static int identifier(const struct cb_nor_io*i,const struct cb_nor_recipe*r,uint8_t id[8]){
 static const uint32_t offsets[]={0,2,0x1c,0x1e};
 memset(id,0,8);
 for(unsigned n=0;n<4;n++){
  uint16_t v;
  int rc=rd(i,r->method==CB_NOR_INTEL_DUAL?n*2:offsets[n],&v);
  if(rc)return rc;
  if(r->swapped)v=swap01(v);
  if(r->unit==1)id[n]=(uint8_t)v;
  else {id[n*2]=(uint8_t)v;id[n*2+1]=(uint8_t)(v>>8);}
 }
 return 0;
}
static int enter(const struct cb_nor_io*i,const struct cb_nor_recipe*r){
 int rc;
 if(r->method>=CB_NOR_INTEL){
  if(r->method==CB_NOR_INTEL_DUAL){rc=wr(i,2,r->id);if(rc)return rc;}
  return wr(i,0,r->id);
 }
 rc=wr(i,r->unlock0,r->aa);if(rc)return rc;
 rc=wr(i,r->unlock1,r->bb);return rc?rc:wr(i,r->unlock0,r->id);
}
static int query(const struct cb_nor_io*i,const struct cb_nor_recipe*r,uint32_t address,uint16_t command,uint16_t words[512]){
 int rc=reset(i,r);if(rc)return rc;
 rc=wr(i,address,r->swapped?swap01(command):command);if(rc)return rc;
 rc=settle(i);if(rc)return rc;
 for(unsigned n=0;n<512;n++){
  rc=rd(i,n*2,&words[n]);if(rc)return rc;
  if(r->swapped)words[n]=swap01(words[n]);
 }
 return 0;
}
static int dual_query(const struct cb_nor_io*i,const struct cb_nor_recipe*r,unsigned die,uint16_t words[512]){
 int rc=reset(i,r);if(rc)return rc;
 rc=wr(i,0,r->swapped?swap01(0x98):0x98);if(rc)return rc;
 rc=wr(i,2,r->swapped?swap01(0x98):0x98);if(rc)return rc;
 rc=settle(i);if(rc)return rc;
 /* Two x16 devices interleave halfwords. A chip-local word advances
  * FOUR cartridge bytes; die 1 occupies the halfword at byte offset 2. */
 for(unsigned n=0;n<512;n++){
  rc=rd(i,n*4+die*2,&words[n]);if(rc)return rc;
  if(r->swapped)words[n]=swap01(words[n]);
 }
 return 0;
}
static int dual_unlock(const struct cb_nor_io*i){
 /* Flash2Advance board latch, raw values (not flash D0/D1 transforms).
  * Source: Mori GBX path / FlashGBX Flash2Advance 128M, 256M, Ultra. */
 static const uint32_t a[]={0x130eca8,0x1c2468a,0x130eca8,0x1e2468a};
 static const uint16_t v[]={0x5354,0xa55a,0x5354,0x9413};
 for(unsigned n=0;n<4;n++){int rc=wr(i,a[n],v[n]);if(rc)return rc;}
 return 0;
}
static bool has_qry(const uint16_t*w){
 for(unsigned shift=0;shift<=8;shift+=8)
  if((uint8_t)(w[0x10]>>shift)=='Q'&&(uint8_t)(w[0x11]>>shift)=='R'&&
     (uint8_t)(w[0x12]>>shift)=='Y')return true;
 return false;
}
static void intel_compat_cfi(const struct cb_nor_probe*p,struct cb_nor_cfi*c){
 /* Mori burner_gba_cfi_primary_is_intel_compat: these unusual primary
  * values are meaningful only with an Intel-style ID. Keep the raw primary
  * value for diagnostics and require our repeated, method-matched DB entry.
  * Do not globally reinterpret 0200 as AMD or accept an arbitrary vendor. */
 if(!p->repeated||!p->entry||p->entry->bus!=CHISBURNER_NOR_IDBUS_GBA||
    (p->method!=CB_NOR_INTEL&&p->method!=CB_NOR_SHARP)||
    chisburner_nor_entry_cmdset(p->entry)!=CHISBURNER_NOR_CMDSET_INTEL||
    c->cmdset!=CHISBURNER_NOR_CMDSET_UNKNOWN||
    (c->primary_id!=0x0100&&c->primary_id!=0x0200&&c->primary_id!=0x0300)||
    p->id[1]||(p->id[3]!=0&&p->id[3]!=0x88&&p->id[3]!=0x89))return;
 switch(p->id[0]){
 case 0x1c:case 0x20:case 0x89:case 0x8a:case 0xb0:
  c->cmdset=CHISBURNER_NOR_CMDSET_INTEL;break;
 default:break;
 }
}
static int attempt(const struct cb_nor_io*i,unsigned unit,enum cb_nor_method method,bool swapped,
 struct cb_nor_probe_scratch*s,struct cb_nor_probe*out){
 struct cb_nor_recipe r;
 struct cb_nor_probe p={0};
 struct cb_nor_cfi cfi;
 uint8_t baseline_id[8],repeat_id[8];
 uint16_t after[16];
 bool cfi_rejected=false;
 int rc,cleanup_rc;
 if(cb_nor_recipe_make(method,unit,swapped,&r))return CB_NOR_UNSUPPORTED;
 p.method=method;p.swapped=swapped;p.plan_rc=CB_NOR_UNSUPPORTED;
 if(method==CB_NOR_INTEL_DUAL)TRY(dual_unlock(i));
 TRY(reset(i,&r));TRY(settle(i));TRY(normal(i,unit,p.baseline));
 TRY(identifier(i,&r,baseline_id));
 TRY(enter(i,&r));TRY(settle(i));TRY(identifier(i,&r,p.id));TRY(identifier(i,&r,repeat_id));
 if(!memcmp(baseline_id,p.id,8)||memcmp(p.id,repeat_id,8)){rc=CB_NOR_UNSUPPORTED;goto cleanup;}
 p.entry=unit==1?chisburner_nor_db_lookup_mbc5(p.id):chisburner_nor_db_lookup_gba(p.id);
 if(!p.entry||!cb_nor_method_matches(p.entry,method)){rc=CB_NOR_UNSUPPORTED;goto cleanup;}
 p.repeated=true;
 if(method==CB_NOR_INTEL_DUAL){
  TRY(dual_query(i,&r,0,s->cfi));
  int decoded=cb_nor_decode_cfi(s->cfi,512,&cfi);
  if(!decoded||has_qry(s->cfi)){
   /* Two reads per die, plus cross-die equality. Never infer the second
    * chip's capacity or sector map from the first chip alone. */
   for(unsigned sample=1;sample<4;sample++){
    TRY(dual_query(i,&r,sample/2,s->repeat));
    if(memcmp(s->cfi,s->repeat,sizeof(s->cfi))){rc=CB_NOR_VERIFY;goto cleanup;}
   }
   p.cfi_valid=!decoded;cfi_rejected=decoded!=0;
  }
 }else{
  static const uint32_t query_addresses[]={0xaa,0};
  /* Mori's generic path uses 0098; the matched FlashGBX MSP55LV100S
   * profile used by its GBX executor specifies raw 9898. Probe both only
   * after the repeated known wide-ID/method gate above. */
  unsigned attempts=(method==CB_NOR_MSP55_AAA||method==CB_NOR_MSP55_5555)?4:2;
  for(unsigned n=0;n<attempts;n++){
   uint16_t command=n<2?0x98:0x9898;
   TRY(query(i,&r,query_addresses[n%2],command,s->cfi));
   int decoded=cb_nor_decode_cfi(s->cfi,512,&cfi);
   if(!decoded||has_qry(s->cfi)){
    TRY(query(i,&r,query_addresses[n%2],command,s->repeat));
    if(memcmp(s->cfi,s->repeat,sizeof(s->cfi))){rc=CB_NOR_VERIFY;goto cleanup;}
    p.cfi_valid=!decoded;cfi_rejected=decoded!=0;break;
   }
  }
 }
 if(p.cfi_valid)intel_compat_cfi(&p,&cfi);
 /* A responding but invalid CFI table is not absence of CFI. In particular,
  * never enable the explicit MX boot-layout fallback after a contradictory
  * or corrupted query. Repeated raw captures above also reject instability. */
 p.plan_rc=cfi_rejected?CB_NOR_UNSUPPORTED:
  cb_nor_plan_make(p.entry,method,swapped,p.cfi_valid?&cfi:NULL,&p.plan);
 if(!p.plan_rc&&p.cfi_valid&&unit==2&&cfi.lane==1){
  /* High-byte QRY carries no D0/D1 evidence. If the opposite ID transform
   * also matches this method, choosing the first record cannot authorize
   * count/status transformations safely. Keep this capture diagnostic-only. */
  uint8_t other_id[8];memcpy(other_id,p.id,8);
  for(unsigned k=0;k<8;k+=2)other_id[k]=(uint8_t)swap01(other_id[k]);
  const chisburner_nor_entry_t*other=chisburner_nor_db_lookup_gba(other_id);
  if(other&&cb_nor_method_matches(other,method))p.plan_rc=CB_NOR_UNSUPPORTED;
 }
 rc=0;
cleanup:
 cleanup_rc=reset(i,&r);
 if(!cleanup_rc&&rc!=CB_NOR_CANCELLED){
  cleanup_rc=settle(i);
  if(!cleanup_rc)cleanup_rc=normal(i,unit,after);
  if(!cleanup_rc&&memcmp(p.baseline,after,sizeof(after)))cleanup_rc=CB_NOR_VERIFY;
 }
 if(cleanup_rc)return cleanup_rc;
 if(!rc){p.restored=true;*out=p;}
 return rc;
}
int cb_nor_probe_bus(const struct cb_nor_io*i,unsigned unit,struct cb_nor_probe_scratch*s,struct cb_nor_probe*out){
 static const enum cb_nor_method methods[]={
  CB_NOR_MSP54,CB_NOR_MSP55_AAA,CB_NOR_MSP55_5555,CB_NOR_AMD_A9,
  CB_NOR_AMD_AAA,CB_NOR_AMD_555,CB_NOR_INTEL_DUAL,CB_NOR_INTEL,CB_NOR_SHARP
 };
 int rc=CB_NOR_UNSUPPORTED;
 struct cb_nor_probe candidate={0},fallback={0};
 if(!out)return CB_NOR_ARGUMENT;
 memset(out,0,sizeof(*out));
 if(!i||!s||!i->select||!i->read||!i->write||!i->now_ms||!i->yield||
    !i->alive||!i->cancelled||!i->release||(unit!=1&&unit!=2))return CB_NOR_ARGUMENT;
 rc=check(i);if(rc)goto done;
 if(i->select(i->ctx,0)){rc=CB_NOR_UNSUPPORTED;goto done;}
 for(unsigned n=0;n<sizeof(methods)/sizeof(methods[0]);n++){
  if(unit==1&&methods[n]!=CB_NOR_AMD_AAA)continue;
  for(unsigned swapped=0;swapped<2;swapped++){
   rc=attempt(i,unit,methods[n],swapped!=0,s,&candidate);
   if(!rc){
    /* Masked IDs can be invariant under D0/D1 swapping. Do not stop at
     * the first ID match when its CFI/geometry cannot be established. */
    /* A valid explicit no-CFI fallback must not hide a later method/wiring
     * that provides actual CFI. Prefer measured geometry over metadata. */
    if(!candidate.plan_rc&&candidate.cfi_valid){*out=candidate;goto done;}
    if(!fallback.entry||(!candidate.plan_rc&&fallback.plan_rc))fallback=candidate;
   }else if(rc!=CB_NOR_UNSUPPORTED)goto done;
  }
 }
 if(fallback.entry){*out=fallback;rc=0;}
done:
 i->release(i->ctx);return rc;
}
