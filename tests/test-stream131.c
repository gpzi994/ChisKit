#include "stream131.h"
#include "mori/chisburner_nor_db_data.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned accepted,excluded;
static void table(const chisburner_nor_entry_t *entries,size_t count){
 for(size_t n=0;n<count;n++)for(int m=0;m<CB_NOR_METHOD_COUNT;m++){
  const chisburner_nor_entry_t *e=entries+n;
  if(!cb_nor_method_matches(e,m))continue;
  struct cb_nor_cfi c={0};struct cb_nor_plan p;
  c.cmdset=chisburner_nor_entry_cmdset(e);c.bytes=chisburner_nor_entry_device_size(e);
  if(!c.bytes)c.bytes=8*1024*1024;
  unsigned sector=chisburner_nor_entry_sector_size(e);if(!sector)sector=65536;
  c.buffer_bytes=chisburner_nor_entry_buffer_write_bytes(e);if(!c.buffer_bytes)c.buffer_bytes=32;
  c.region_count=1;c.region[0]=(struct cb_nor_region){0,c.bytes/sector,sector};
  if(e->bus==CHISBURNER_NOR_IDBUS_MBC5&&e->id[0]==0xc2){
   if(e->id[1]==0xcb||e->id[1]==0xc9){assert(!cb_nor_plan_make(e,m,false,NULL,&p));c=p.geometry;}
   else if(e->id[1]==0x7e&&e->id[2]==0x10){
    c.region_count=2;c.region[0]=(struct cb_nor_region){0,8,8192};
    c.region[1]=(struct cb_nor_region){65536,127,65536};
   }
  }
  for(unsigned swap=0;swap<2;swap++){
   assert(!cb_nor_plan_make(e,m,swap,&c,&p));
   unsigned length=(unsigned)cb_nor_chunk(&p,0,1024);
   struct cb_stream131_descriptor d,zero={0};
   int rc=cb_stream131_build(&p,0,0,length,90000000,&d);
   if(p.geometry.bytes>0x2000000u){assert(rc==-2);assert(!memcmp(&d,&zero,sizeof(d)));excluded++;continue;}
   assert(rc==0);accepted++;
   assert(d.words[CB_S131_LENGTH]==length);
   assert(d.words[CB_S131_AA_BB]==((unsigned)p.recipe.aa|((unsigned)p.recipe.bb<<16)));
   assert(d.words[CB_S131_BUFFER_CONFIRM]==((unsigned)p.recipe.buffer|((unsigned)p.recipe.confirm<<16)));
   assert(d.words[CB_S131_RESET_WORD]==((unsigned)p.recipe.reset|((unsigned)p.recipe.word<<16)));
   for(unsigned k=11;k<16;k++)assert(d.words[k]==0);
   assert(cb_stream131_build(&p,0x2000000u,0,length,1,&d)==-1);
   assert(cb_stream131_build(&p,0,0,length,0,&d)==-1);
   assert(cb_stream131_build(&p,0,0,0,1,&d)==-1);
   assert(cb_stream131_build(&p,0,p.accessible_bytes,length,1,&d)==-1);
   if(p.recipe.unit==2)assert(cb_stream131_build(&p,1,0,length,1,&d)==-1);
   if(p.recipe.unit==1){
    assert(!cb_stream131_build(&p,0,0x8000,length,1,&d));
    assert(d.words[CB_S131_PHYSICAL]==0x4000);
   }
  }
 }
}
int main(void){
 const uint8_t id[4]={0xc2,0x7e,0x10,0};
 const chisburner_nor_entry_t *gl=chisburner_nor_db_lookup_mbc5(id);
 assert(gl);
 struct cb_nor_plan p;struct cb_stream131_descriptor d;
 struct cb_nor_cfi c={.bytes=0x800000,.cmdset=CHISBURNER_NOR_CMDSET_AMD,
  .buffer_bytes=32,.region_count=2,.region={{0,8,8192},{65536,127,65536}}};
 assert(cb_nor_plan_make(gl,CB_NOR_AMD_AAA,false,NULL,&p)==-2);
 assert(!cb_nor_plan_make(gl,CB_NOR_AMD_AAA,false,&c,&p));
 c.region[0].count=7;assert(cb_nor_plan_make(gl,CB_NOR_AMD_AAA,false,&c,&p)==-2);
 c.region[0].count=8;c.buffer_bytes=64;assert(cb_nor_plan_make(gl,CB_NOR_AMD_AAA,false,&c,&p)==-2);
 c.buffer_bytes=32;assert(!cb_nor_plan_make(gl,CB_NOR_AMD_AAA,false,&c,&p));
 p.geometry.buffer_bytes=0;
 assert(cb_nor_chunk(&p,0,256)==1); /* Physical byte algorithm unchanged. */
 assert(cb_stream131_chunk(&p,0,512)==256);
 assert(cb_stream131_chunk(&p,0x3fff,256)==1);
 assert(cb_stream131_chunk(&p,8191,256)==1);
 assert(!cb_stream131_build(&p,0,0,256,90000000,&d));
 assert(cb_stream131_build(&p,0,0,257,90000000,&d)==-1);
 assert(cb_stream131_build(&p,0,0x3fff,2,90000000,&d)==-1);
 table(g_chisburner_nor_db_gba,g_chisburner_nor_db_gba_count);
 table(g_chisburner_nor_db_mbc5,g_chisburner_nor_db_mbc5_count);
 printf("PASS stream131 descriptor: %u eligible recipe/wiring cases, %u over-32MiB exclusions\n",accepted,excluded);
}
