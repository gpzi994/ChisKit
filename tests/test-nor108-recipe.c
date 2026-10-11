#include "recipe.h"
#include "mori/chisburner_nor_db_data.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void table(const chisburner_nor_entry_t *entries,size_t count){
 for(size_t i=0;i<count;i++){
   const chisburner_nor_entry_t *e=entries+i;
   bool lv=e->bus==CHISBURNER_NOR_IDBUS_MBC5&&e->id[0]==0xc2&&(e->id[1]==0xcb||e->id[1]==0xc9);
   bool gl=e->bus==CHISBURNER_NOR_IDBUS_MBC5&&e->id[0]==0xc2&&e->id[1]==0x7e&&e->id[2]==0x10;
  unsigned matched=0;
  for(int method=0;method<CB_NOR_METHOD_COUNT;method++){
   struct cb_nor_plan p;
   if(!cb_nor_method_matches(e,method))continue;
   matched++;
   /* Independent expectation from Mori's explicit no-CFI metadata fallback.
    * Unknown geometry and Intel must remain locked; mixed MX is below. */
   uint32_t known_size=chisburner_nor_entry_device_size(e);
   uint32_t known_sector=chisburner_nor_entry_sector_size(e);
   bool fallback=!gl&&chisburner_nor_entry_cmdset(e)==CHISBURNER_NOR_CMDSET_AMD&&known_size&&known_sector;
   assert((cb_nor_plan_make(e,method,false,NULL,&p)==0)==fallback);
   if(fallback){
    assert(p.geometry.bytes==known_size);
    if(!lv)
     assert(p.geometry.region_count==1&&p.geometry.region[0].count==known_size/known_sector&&
            p.geometry.region[0].sector_bytes==known_sector);
   }
   struct cb_nor_cfi c={0};
   c.cmdset=chisburner_nor_entry_cmdset(e);
   c.bytes=chisburner_nor_entry_device_size(e);
   if(!c.bytes)c.bytes=8*1024*1024;
   uint32_t sector=chisburner_nor_entry_sector_size(e);
   if(!sector)sector=65536;
   c.buffer_bytes=chisburner_nor_entry_buffer_write_bytes(e);
   if(!c.buffer_bytes)c.buffer_bytes=32;
   c.region_count=1;c.region[0]=(struct cb_nor_region){0,c.bytes/sector,sector};
   if(lv){
    assert(!cb_nor_plan_make(e,method,false,NULL,&p));c=p.geometry;
   }
   if(gl){c.region_count=2;c.region[0]=(struct cb_nor_region){0,8,8192};c.region[1]=(struct cb_nor_region){65536,127,65536};}
   for(unsigned swap=0;swap<2;swap++){
    assert(cb_nor_plan_make(e,method,swap,&c,&p)==0);
    uint32_t unit=p.recipe.unit;
    for(uint32_t a=0;a<c.bytes;){
     uint32_t start,len;
     assert(!cb_nor_sector_at(&p,a,&start,&len)&&start==a);
     uint32_t step=len;
     assert(!cb_nor_sector_at(&p,a+step-1,&start,&len)&&start==a&&len==step);
     for(unsigned n=1;n<=2048;n*=2){
      size_t chunk=cb_nor_chunk(&p,a,n);
      assert(chunk<=n&&chunk%unit==0&&chunk<=sector&&chunk<=1024);
      assert(cb_nor_chunk(&p,a+step-unit,n)<=unit);
     }
     a+=step;
    }
    assert(cb_nor_chunk(&p,c.bytes,1024)==0);
    assert(cb_nor_chunk(&p,0,0)==0);
    if(unit==2)assert(cb_nor_chunk(&p,1,32)==0);
    if(p.flags&CHISBURNER_NOR_FLAG_DISABLE_BUFFER_PROGRAM)
     assert(cb_nor_chunk(&p,0,1024)==unit);
    if(p.flags&CHISBURNER_NOR_FLAG_INTEL_88B0)
     assert(p.recipe.buffer==(swap?0xea:0xe9));
   }
   if(chisburner_nor_entry_flags(e)&CHISBURNER_NOR_FLAG_DUAL_DIE){
    c.bytes/=2;
    if(method==CB_NOR_INTEL_DUAL)c.region[0].sector_bytes/=2;
    else c.region[0].count/=2;
    assert(!cb_nor_plan_make(e,method,false,&c,&p));
    assert(p.geometry.bytes==2*c.bytes);
    assert(p.geometry.region_count==(method==CB_NOR_INTEL_DUAL?1:2));
   }
   c.region[0].start=1;
   memset(&p,0xa5,sizeof(p));
   assert(cb_nor_plan_make(e,method,false,&c,&p)==-2&&p.entry==NULL);
  }
  assert(matched);
  printf("recipe covered: %s (%u methods)\n",chisburner_nor_entry_name(e),matched);
 }
}
int main(void){
 struct cb_nor_recipe r;
 assert(!cb_nor_recipe_make(CB_NOR_AMD_AAA,2,false,&r));
 assert(r.unlock0==0xaaa&&r.unlock1==0x554);
 assert(!cb_nor_recipe_make(CB_NOR_AMD_AAA,1,false,&r));
 assert(r.unlock0==0xaaa&&r.unlock1==0x555);
 assert(!cb_nor_recipe_make(CB_NOR_MSP54,2,true,&r));
 assert(r.aa==0x69&&r.bb==0x96&&r.id==0x50&&r.word==0x60&&r.erase==0x40);
 assert(cb_nor_count(&r,3)==2);
 assert(!cb_nor_recipe_make(CB_NOR_AMD_A9,2,true,&r));
 assert(r.aa==0xa9&&r.buffer==0x26&&r.confirm==0x2a&&cb_nor_count(&r,3)==1);
 assert(!cb_nor_recipe_make(CB_NOR_MSP55_5555,2,false,&r));
 assert(r.unlock0==0xaaaa&&r.unlock1==0x5554&&r.reset==0xf0f0);
 assert(cb_nor_recipe_make(CB_NOR_INTEL,1,false,&r));
 table(g_chisburner_nor_db_gba,g_chisburner_nor_db_gba_count);
 table(g_chisburner_nor_db_mbc5,g_chisburner_nor_db_mbc5_count);
 for(unsigned i=0;i<2;i++){
  struct cb_nor_plan p;
  uint32_t start,len;
  const uint8_t id[4]={0xc2,i?0xc9:0xcb,0,0};
  assert(!cb_nor_plan_make(chisburner_nor_db_lookup_mbc5(id),CB_NOR_AMD_AAA,false,NULL,&p));
  assert(!cb_nor_sector_at(&p,i?0x7fffff:0,&start,&len)&&len==8192);
  assert(!cb_nor_sector_at(&p,i?0:0x7fffff,&start,&len)&&len==65536);
 }
 struct cb_nor_plan zero={0};
 uint32_t start,len;
 assert(cb_nor_sector_at(&zero,0,&start,&len));
 assert(!cb_nor_chunk(&zero,0,1024));
 puts("NOR108 recipes PASS: every record, wire modes, sector/buffer/window boundaries");
}
