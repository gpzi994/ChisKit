// SPDX-License-Identifier: AGPL-3.0-only
// Adapted geometry rules: MoriBunrnner-for-ESP32-S3 burner_gba_lowlevel.c
// burner_bacon_gba_get_cfi(). See mori/PROVENANCE.md for attribution/license.
#include "cfi.h"
#include <string.h>
static uint8_t byte_at(const uint16_t *w, unsigned p, unsigned lane) {
 return (uint8_t)(w[p] >> (lane * 8));
}
static uint16_t pair_at(const uint16_t *w, unsigned p, unsigned lane) {
 return (uint16_t)(byte_at(w,p,lane) | ((uint16_t)byte_at(w,p+1,lane)<<8));
}
int cb_nor_decode_cfi(const uint16_t *w, size_t n, struct cb_nor_cfi *out) {
 struct cb_nor_cfi c={0};
 uint32_t counts[CB_NOR_REGIONS], sizes[CB_NOR_REGIONS];
 if(!out)return -1;
 memset(out,0,sizeof(*out));
 if(!w || n<0x200)return -1;
 bool lo=byte_at(w,0x10,0)=='Q' && byte_at(w,0x11,0)=='R' && byte_at(w,0x12,0)=='Y';
 bool hi=byte_at(w,0x10,1)=='Q' && byte_at(w,0x11,1)=='R' && byte_at(w,0x12,1)=='Y';
 if(!lo && !hi)return -2;
 c.lane=lo?0:1;
 /* If both lanes claim CFI, conflicting captures must not be accepted. */
 if(lo && hi)for(unsigned i=0x10;i<0x200;i++)
  if(byte_at(w,i,0)!=byte_at(w,i,1))return -2;
 c.primary_id=pair_at(w,0x13,c.lane);
 c.cmdset=chisburner_nor_cmdset_from_cfi_primary_id(c.primary_id);
 unsigned size_exp=byte_at(w,0x27,c.lane), buf_exp=pair_at(w,0x2a,c.lane);
 if(size_exp>=31 || size_exp<8 || buf_exp>=16)return -3;
 c.bytes=1u<<size_exp;
 c.buffer_bytes=buf_exp>1?1u<<buf_exp:0;
 if(c.buffer_bytes>c.bytes)return -3;
 c.region_count=byte_at(w,0x2c,c.lane);
 if(!c.region_count || c.region_count>CB_NOR_REGIONS)return -3;
 for(unsigned i=0;i<c.region_count;i++) {
  counts[i]=(uint32_t)pair_at(w,0x2d+i*4,c.lane)+1u;
  sizes[i]=(uint32_t)pair_at(w,0x2f+i*4,c.lane)*256u;
  /* Match Mori's rejection of a zero-size erase record. */
  if(!sizes[i])return -3;
 }
 c.pri_address=pair_at(w,0x15,c.lane);
 if((uint32_t)c.pri_address+0x1e>=0x200)c.pri_address=0x40;
 unsigned p=c.pri_address;
 c.pri_valid=byte_at(w,p,c.lane)=='P' && byte_at(w,p+1,c.lane)=='R' && byte_at(w,p+2,c.lane)=='I';
 if(c.pri_valid) {
  c.pri_major=byte_at(w,p+3,c.lane);c.pri_minor=byte_at(w,p+4,c.lane);
  c.boot=byte_at(w,p+0x1e,c.lane);
 }
 /* Mori uses PRI+0x1e to reverse its sector records. In this adapter only
  * AMD interprets that field as boot orientation; Intel extensions differ. */
 bool reverse=c.cmdset==CHISBURNER_NOR_CMDSET_AMD && c.pri_valid && c.boot==3;
 uint64_t total=0;
 for(unsigned i=0;i<c.region_count;i++) {
  unsigned j=reverse?c.region_count-1-i:i;
  c.region[i]=(struct cb_nor_region){(uint32_t)total,counts[j],sizes[j]};
  total+=(uint64_t)counts[j]*sizes[j];
  if(total>c.bytes)return -3;
 }
 if(total!=c.bytes)return -3;
 *out=c;return 0;
}
int cb_nor_match_cfi(const chisburner_nor_entry_t *e,const struct cb_nor_cfi *c) {
 if(!e || !c || !c->region_count || c->region_count>CB_NOR_REGIONS)return -1;
 if(c->cmdset==CHISBURNER_NOR_CMDSET_UNKNOWN || c->cmdset!=chisburner_nor_entry_cmdset(e))return -2;
 uint32_t bytes=chisburner_nor_entry_device_size(e), sector=chisburner_nor_entry_sector_size(e);
 if(!bytes || c->bytes!=bytes)return -3;
 uint64_t total=0;
 for(unsigned i=0;i<c->region_count;i++) {
  const struct cb_nor_region *r=&c->region[i];
  if(r->start!=total || !r->count || !r->sector_bytes)return -3;
  if(sector && sector!=r->sector_bytes)return -3;
  total+=(uint64_t)r->count*r->sector_bytes;
  if(total>bytes)return -3;
 }
 if(total!=bytes)return -3;
 return 0;
}
