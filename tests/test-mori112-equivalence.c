// SPDX-License-Identifier: AGPL-3.0-only
/* Compile the unmodified user-supplied Mori DB beside our production DB.
 * This is a software equivalence test, never physical write qualification. */
#include "burner_nor_db_data.h"
#include "mori/chisburner_nor_db_data.h"
#include "write_policy112.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned cases;
static int same(const char*a,const char*b){return (!a&&!b)||(a&&b&&!strcmp(a,b));}
static void match(const uint8_t id[8]){
 const burner_nor_entry_t*a=burner_nor_db_lookup_gba(id);
 const chisburner_nor_entry_t*b=chisburner_nor_db_lookup_gba(id);
 cases++;assert(!!a==!!b);if(!a)return;
 assert(a-g_burner_nor_db_gba==b-g_chisburner_nor_db_gba);
 assert(same(burner_gba_profile_name(id),chisburner_gba_profile_name(id)));
 assert(same(burner_nor_entry_name(a),chisburner_nor_entry_name(b)));
 assert((int)burner_nor_entry_cmdset(a)==(int)chisburner_nor_entry_cmdset(b));
 assert(burner_nor_entry_flags(a)==chisburner_nor_entry_flags(b));
 assert(burner_nor_entry_device_size(a)==chisburner_nor_entry_device_size(b));
 assert(burner_nor_entry_sector_size(a)==chisburner_nor_entry_sector_size(b));
 assert(burner_nor_entry_buffer_write_bytes(a)==chisburner_nor_entry_buffer_write_bytes(b));
}
int main(void){
 assert(g_burner_nor_db_gba_count==51&&g_chisburner_nor_db_gba_count==51);
 unsigned eligible=0,excluded=0,unknown=0;
 puts("index\tmodel\tbytes\tscope_only_not_write_authorization");
 for(unsigned i=0;i<51;i++){
  const burner_nor_entry_t*a=g_burner_nor_db_gba+i;
  const chisburner_nor_entry_t*b=g_chisburner_nor_db_gba+i;
  assert((int)a->bus==(int)b->bus&&a->id_len==b->id_len);
  assert(!memcmp(a->id,b->id,8)&&!memcmp(a->id_mask,b->id_mask,8));
  assert(same(a->name,b->name)&&same(a->profile,b->profile));
  assert((int)a->cmdset==(int)b->cmdset&&a->flags==b->flags);
  assert(a->device_size==b->device_size&&a->sector_size==b->sector_size&&a->buffer_write_bytes==b->buffer_write_bytes);
  assert(!!a->family==!!b->family);
  if(a->family){
   assert(same(a->family->name,b->family->name)&&same(a->family->gba_profile,b->family->gba_profile));
   assert((int)a->family->cmdset==(int)b->family->cmdset&&a->family->flags==b->family->flags);
   assert(a->family->device_size==b->family->device_size&&a->family->sector_size==b->family->sector_size);
   assert(a->family->buffer_write_bytes_gba==b->family->buffer_write_bytes_gba&&a->family->buffer_write_bytes_mbc5==b->family->buffer_write_bytes_mbc5);
  }
  match(a->id);
  assert(chisburner_nor_db_lookup_gba(a->id)==b);
  for(unsigned byte=0;byte<8;byte++)for(unsigned value=0;value<256;value++){
   uint8_t id[8];memcpy(id,a->id,8);id[byte]=(uint8_t)value;match(id);
  }
  uint32_t size=burner_nor_entry_device_size(a);
  const char*scope=!size?"measured_CFI_required":size>CB_ROM_CACHE112_BYTES?"excluded_over_32MiB":"within_32MiB";
  if(!size)unknown++;else if(size>CB_ROM_CACHE112_BYTES)excluded++;else eligible++;
  assert(cb_nor_write_capacity112(2,size)==(size&&size<=0x2000000u));
  printf("%u\t%s\t%u\t%s\n",i,burner_nor_entry_name(a),size,scope);
 }
 uint32_t seed=0x11251;
 for(unsigned k=0;k<65536;k++){
  uint8_t id[8];for(unsigned n=0;n<8;n++){seed=seed*1664525u+1013904223u;id[n]=(uint8_t)(seed>>24);}match(id);
 }
 match(NULL);
 assert(!cb_nor_write_capacity112(2,0)&&cb_nor_write_capacity112(2,0x2000000));
 assert(!cb_nor_write_capacity112(2,0x2000001)&&!cb_nor_write_capacity112(2,UINT32_MAX));
 assert(!cb_nor_write_capacity112(0,0x2000000)&&!cb_nor_write_capacity112(3,0x2000000));
 assert(cb_nor_write_capacity112(1,0x800000));
 fprintf(stderr,"PASS Mori112: 51 exact records; %u lookup comparisons; within scope %u, excluded %u, unknown %u. No hardware claims.\n",cases,eligible,excluded,unknown);
}
