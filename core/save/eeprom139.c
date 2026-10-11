#include "eeprom139.h"
#include <string.h>
static struct cb_eeprom139_report probe(const struct cb_eeprom139_ops *o,
 uint8_t *small,size_t small_n,uint8_t *large,size_t large_n,int diagnostic){
 struct cb_eeprom139_report r={.first_difference=UINT32_MAX,
  .mode_difference={UINT32_MAX,UINT32_MAX}};uint8_t check[8];
 if(!o||!o->read_block||!o->same_cart||!o->release||!small||!large||small_n<512||large_n<8192){r.rc=-1;return r;}
 uintptr_t s=(uintptr_t)small,l=(uintptr_t)large;
 if(s>UINTPTR_MAX-512||l>UINTPTR_MAX-8192||(s<l+8192&&l<s+512)){r.rc=-1;return r;}
 for(unsigned mode=0;mode<2&&!r.rc;mode++){
  unsigned bits=mode?14:6,blocks=mode?1024:64;uint8_t *dst=mode?large:small;
  for(unsigned pass=0;pass<(diagnostic==2?1u:2u)&&!r.rc;pass++)for(unsigned b=0;b<blocks;b++){
   r.address_bits=bits;r.pass=pass+1;r.block=b;r.stage=1;
   if(!o->same_cart(o->ctx)){r.rc=-2;break;}
   r.stage=2;r.rc=o->read_block(o->ctx,bits,b,pass?check:dst+b*8);if(r.rc)break;
   r.stage=3;
   if(!o->same_cart(o->ctx)){r.rc=-2;break;}
   r.bytes_read+=8;
   if(pass&&memcmp(check,dst+b*8,8)){
    r.stage=4;r.state=CB_EEPROM139_UNSTABLE;r.rc=-3;
    for(unsigned i=0;i<8;i++)if(check[i]!=dst[b*8+i]){r.first_difference=(mode?512:0)+b*8+i;break;}
    r.mismatch_mask|=1u<<mode;
    r.mode_difference[mode]=r.first_difference-(mode?512:0);
    if(diagnostic)r.rc=0;
    break;
   }
  }
  if(!r.rc&&!(r.mismatch_mask&(1u<<mode)))r.stable_mask|=1u<<mode;
 }
 /* This callback is pin release, not a command to a replacement cartridge. */
 r.cleanup_rc=o->release(o->ctx);
 if(!r.rc)r.stage=r.cleanup_rc?5:0;
 if(r.rc||r.cleanup_rc)return r;
 if(r.mismatch_mask){r.state=CB_EEPROM139_UNSTABLE;return r;}
 unsigned zeros=0,ffs=0;
 for(unsigned i=0;i<8192;i++){zeros+=large[i]==0;ffs+=large[i]==255;}
 if(zeros==8192||ffs==8192){r.state=CB_EEPROM139_BLANK;return r;}
 r.prefix_equal=memcmp(small,large,512)==0;
 r.state=r.prefix_equal?CB_EEPROM139_8K:CB_EEPROM139_512;
 return r;
}
struct cb_eeprom139_report cb_eeprom139_probe(const struct cb_eeprom139_ops *o,
 uint8_t *s,size_t sn,uint8_t *l,size_t ln){return probe(o,s,sn,l,ln,0);}
struct cb_eeprom139_report cb_eeprom139_capture(const struct cb_eeprom139_ops *o,
 uint8_t *s,size_t sn,uint8_t *l,size_t ln){return probe(o,s,sn,l,ln,1);}
struct cb_eeprom139_report cb_eeprom139_detect(const struct cb_eeprom139_ops *o,
 uint8_t *s,size_t sn,uint8_t *l,size_t ln){return probe(o,s,sn,l,ln,2);}

static int informative(const uint8_t *p,unsigned n){
 /* Uniform and short periodic content cannot establish an address span. */
 for(unsigned period=1;period<=256;period*=2){
  unsigned i=period;for(;i<n&&p[i]==p[i%period];i++){}
  if(i==n)return 0;
 }
 return 1;
}
enum cb_eeprom139_state cb_eeprom139_estimate(const struct cb_eeprom139_report *r,
 const uint8_t *s,const uint8_t *l){
 if(!r||!s||!l||r->rc||r->cleanup_rc||!r->stable_mask||
    (r->stable_mask|r->mismatch_mask)!=3u||(r->stable_mask&r->mismatch_mask))
  return CB_EEPROM139_UNKNOWN;
 if(r->stable_mask==3){
  unsigned i=1;for(;i<8192&&l[i]==l[0];i++){}
  if(i==8192)return CB_EEPROM139_UNKNOWN;
  if(memcmp(s,l,512))return informative(s,512)?CB_EEPROM139_512:CB_EEPROM139_UNKNOWN;
  return informative(l,8192)?CB_EEPROM139_8K:CB_EEPROM139_UNKNOWN;
 }
 if(r->stable_mask==1&&informative(s,512))return CB_EEPROM139_512;
 if(r->stable_mask==2&&informative(l,8192)){
  /* A repeated 512-byte image does not demonstrate the larger address range. */
  for(unsigned i=512;i<8192;i++)if(l[i]!=l[i%512])return CB_EEPROM139_8K;
 }
 return CB_EEPROM139_UNKNOWN;
}
