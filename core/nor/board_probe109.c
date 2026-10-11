// SPDX-License-Identifier: AGPL-3.0-only
#include "board_probe109.h"
#include <string.h>
struct session109 {const struct cb_board109_io*io;uint32_t epoch;bool route;};
static bool live(struct session109*s){return s->io->alive(s->io->ctx)&&s->io->epoch(s->io->ctx)==s->epoch;}
static int wr(struct session109*s,unsigned bus,uint32_t a,uint16_t v){
 if(!live(s))return CB_NOR_CANCELLED;
 int rc=s->io->write(s->io->ctx,bus,a,v);
 if(!live(s))return CB_NOR_CANCELLED;
 return rc?CB_NOR_IO:0;
}
static int rd(struct session109*s,uint32_t a,uint16_t*v){
 if(!live(s))return CB_NOR_CANCELLED;
 int rc=s->io->read(s->io->ctx,a,v);
 if(!live(s))return CB_NOR_CANCELLED;
 return rc?CB_NOR_IO:0;
}
static int settle(struct session109*s){
 uint32_t start=s->io->now_ms(s->io->ctx);
 for(unsigned n=0;n<100000;n++){
  if(!live(s))return CB_NOR_CANCELLED;
  if((uint32_t)(s->io->now_ms(s->io->ctx)-start)>=1)return 0;
  s->io->yield(s->io->ctx);
 }
 return CB_NOR_TIMEOUT;
}
static int sequence(struct session109*s,const struct cb_board109_write*w,size_t n){
 if(!n)return CB_NOR_ARGUMENT;
 s->route=false;
 for(size_t k=0;k<n;k++){
  int rc=wr(s,w[k].bus,w[k].address,w[k].value);if(rc)return rc;
 }
 s->route=true;return settle(s);
}
static int select_bank(struct session109*s,enum cb_board109_kind k,unsigned bank){
 struct cb_board109_write w[8];
 if(k!=CB_BOARD109_F2AULTRA)return bank?CB_NOR_ARGUMENT:0;
 return sequence(s,w,cb_board109_select(k,bank,w,8));
}
static int reset(struct session109*s){
 int rc=wr(s,2,0,0xff);if(!rc)rc=wr(s,2,2,0xff);
 return rc?rc:settle(s);
}
static int normal(struct session109*s,uint16_t out[16]){
 for(unsigned n=0;n<16;n++){int rc=rd(s,n*2,out+n);if(rc)return rc;}return 0;
}
static int id(struct session109*s,uint8_t out[8]){
 for(unsigned n=0;n<4;n++){
  uint16_t v;int rc=rd(s,n*2,&v);if(rc)return rc;
  out[2*n]=(uint8_t)v;out[2*n+1]=(uint8_t)(v>>8);
 }return 0;
}
static int query(struct session109*s,unsigned die,uint16_t words[512]){
 int rc=reset(s);if(rc)return rc;
 rc=wr(s,2,0,0x98);if(!rc)rc=wr(s,2,2,0x98);if(rc)return rc;
 rc=settle(s);if(rc)return rc;
 for(unsigned n=0;n<512;n++){rc=rd(s,n*4+die*2,words+n);if(rc)return rc;}
 return 0;
}
static int capture(const struct cb_board109_io*i,enum cb_board109_kind kind,
 const uint8_t seed[8],struct cb_board109_scratch*scratch,struct cb_board109_capture*out){
 struct cb_board109_capture p={0};struct cb_board109_write w[8];
 uint8_t expected[8],again[8];uint16_t after[16];
 bool baseline0=false;int rc=CB_NOR_ARGUMENT;
 if(out)memset(out,0,sizeof(*out));
 if(!i||!i->write||!i->read||!i->alive||!i->epoch||!i->now_ms||!i->yield||!i->release)return rc;
 if(!out||!seed||cb_board109_profile(kind,&p.profile)||cb_board109_bank_id(kind,0,expected))goto release;
 if(memcmp(seed,expected,8)){rc=CB_NOR_UNSUPPORTED;goto release;}
 struct session109 s={i,i->epoch(i->ctx),false};p.epoch=s.epoch;
 p.geometry_valid=scratch!=NULL;p.geometry_rc=scratch?0:CB_NOR_UNSUPPORTED;
 if(!live(&s)){rc=CB_NOR_CANCELLED;goto release;}
 rc=sequence(&s,w,cb_board109_unlock(kind,w,8));if(rc)goto cleanup;
 for(unsigned bank=0;bank<p.profile.banks;bank++){
  rc=select_bank(&s,kind,bank);if(rc)goto cleanup;
  rc=reset(&s);if(rc)goto cleanup;
  rc=normal(&s,p.normal[bank]);if(rc)goto cleanup;
  if(!bank)baseline0=true;
  rc=wr(&s,2,2,0x90);if(!rc)rc=wr(&s,2,0,0x90);if(rc)goto cleanup;
  rc=settle(&s);if(rc)goto cleanup;
  rc=id(&s,p.id[bank]);if(!rc)rc=id(&s,again);if(rc)goto cleanup;
  cb_board109_bank_id(kind,bank,expected);
  if(memcmp(p.id[bank],expected,8)||memcmp(again,expected,8)){rc=CB_NOR_VERIFY;goto cleanup;}
  /* ROM bytes that happen to spell the ID are not a valid mode response. */
  bool changed=false;
  for(unsigned n=0;n<4;n++)if(p.normal[bank][n]!=(uint16_t)(expected[2*n]|expected[2*n+1]<<8))changed=true;
  if(!changed){rc=CB_NOR_VERIFY;goto cleanup;}
  if(scratch){
   rc=query(&s,0,scratch->first);if(rc)goto cleanup;
   /* Both dies, twice each, must agree on all raw query words. */
   for(unsigned sample=1;sample<4;sample++){
    rc=query(&s,sample/2,scratch->repeat);if(rc)goto cleanup;
    if(memcmp(scratch->first,scratch->repeat,sizeof(scratch->first))){rc=CB_NOR_VERIFY;goto cleanup;}
   }
   if(cb_nor_decode_cfi(scratch->first,512,p.cfi+bank)||
      cb_board109_match_die_cfi(kind,bank,p.cfi+bank)){
    p.geometry_valid=false;p.geometry_rc=CB_NOR_UNSUPPORTED;
   }
  }
  rc=reset(&s);if(!rc)rc=normal(&s,after);if(rc)goto cleanup;
  if(memcmp(after,p.normal[bank],sizeof(after))){rc=CB_NOR_VERIFY;goto cleanup;}
 }
 p.ids_valid=true;
cleanup:
 /* Failed partial select => unknown mapping: issue no further bus commands.
  * A lifecycle change also forbids restoring a potentially different cart. */
 if(s.route&&live(&s)){
  int clean=reset(&s);
  if(!clean)clean=select_bank(&s,kind,0);
  if(!clean)clean=reset(&s);
  if(!clean&&baseline0){
   clean=normal(&s,after);
   if(!clean&&memcmp(after,p.normal[0],sizeof(after)))clean=CB_NOR_VERIFY;
  }
  if(!clean&&baseline0)p.restored=true;
  if(!rc)rc=clean;
 }
 if(!rc&&(!p.ids_valid||!p.restored))rc=CB_NOR_VERIFY;
 if(!rc)*out=p;
release:
 i->release(i->ctx);return rc;
}
int cb_board109_capture_ids(const struct cb_board109_io*i,enum cb_board109_kind kind,
 const uint8_t seed[8],struct cb_board109_capture*out){return capture(i,kind,seed,NULL,out);}
int cb_board109_capture_full(const struct cb_board109_io*i,enum cb_board109_kind kind,
 const uint8_t seed[8],struct cb_board109_scratch*scratch,struct cb_board109_capture*out){
 if(!scratch){if(out)memset(out,0,sizeof(*out));return CB_NOR_ARGUMENT;}
 return capture(i,kind,seed,scratch,out);
}
