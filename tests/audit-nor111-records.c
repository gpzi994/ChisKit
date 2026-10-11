// SPDX-License-Identifier: AGPL-3.0-only
/* Database/recipe audit only. No invented CFI and no hardware authorization. */
#include "recipe.h"
#include "mori/chisburner_nor_db_data.h"
#include <assert.h>
#include <stdio.h>
static const char *methods[]={"AMD_AAA","AMD_555","AMD_A9","MSP54",
 "MSP55_AAA","MSP55_5555","INTEL","INTEL_DUAL","SHARP"};
static unsigned records, unknown_geometry, gb_extended;
static void table(const chisburner_nor_entry_t *entries,size_t count){
 for(size_t i=0;i<count;i++){
  const chisburner_nor_entry_t *e=entries+i;
  const chisburner_nor_entry_t *found=e->bus==CHISBURNER_NOR_IDBUS_GBA?
   chisburner_nor_db_lookup_gba(e->id):chisburner_nor_db_lookup_mbc5(e->id);
  assert(found==e); /* Detect unreachable/shadowed imported records. */
  uint32_t size=chisburner_nor_entry_device_size(e);
  unsigned matched=0,fallback=0;
  printf("%s\t%zu\t%s\t%u\t%u\t%u\t",e->bus==CHISBURNER_NOR_IDBUS_GBA?"GBA":"GB",i,
   chisburner_nor_entry_name(e),size,chisburner_nor_entry_sector_size(e),
   chisburner_nor_entry_buffer_write_bytes(e));
  for(unsigned m=0;m<CB_NOR_METHOD_COUNT;m++){
   if(!cb_nor_method_matches(e,m))continue;
   printf("%s%s",matched?",":"",methods[m]);matched++;
   for(unsigned swap=0;swap<2;swap++){
    struct cb_nor_recipe r;struct cb_nor_plan p;
    assert(!cb_nor_recipe_make(m,e->bus==CHISBURNER_NOR_IDBUS_GBA?2:1,swap,&r));
    if(!cb_nor_plan_make(e,m,swap,NULL,&p)){
     fallback++;assert(p.entry==e&&p.geometry.bytes==size);
    }
   }
  }
  assert(matched);assert(!fallback||fallback==matched*2);
  const char *range="GBA_32MiB_window";
  if(e->bus==CHISBURNER_NOR_IDBUS_MBC5){
   range=size>0x800000?"GB_above_standard_MBC5_range":"GB_mapper_dependent";
   if(size>0x800000)gb_extended++;
  }else if(size>0x2000000)range="GBA_requires_qualified_board_route";
  if(!size)unknown_geometry++;
  printf("\t%s\t%s\n",fallback?"explicit_metadata":"CFI_required",range);records++;
 }
}
int main(void){
 _Static_assert(sizeof(methods)/sizeof(methods[0])==CB_NOR_METHOD_COUNT,"method labels");
 puts("bus\tindex\trecord\tdeclared_bytes\tsector_bytes\tbuffer_max\tmethods\tno_CFI\taddress_requirement");
 table(g_chisburner_nor_db_gba,g_chisburner_nor_db_gba_count);
 table(g_chisburner_nor_db_mbc5,g_chisburner_nor_db_mbc5_count);
 assert(records==60); /* A database change requires re-auditing scope. */
 fprintf(stderr,"AUDIT: %u reachable records; %u unknown-size records require measured CFI; %u GB records exceed standard MBC5 full-chip range. No silicon claims.\n",records,unknown_geometry,gb_extended);
}
