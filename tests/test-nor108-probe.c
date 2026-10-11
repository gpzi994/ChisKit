#include "probe.h"
#include "mori/chisburner_nor_db_data.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
struct chip {
 const chisburner_nor_entry_t *entry;
 struct cb_nor_recipe recipe;
 uint16_t cfi[512];
 unsigned phase,mode,queries,release_count,reads;
 bool unstable,pretend_rom,dead,wrong_die,zero_query_only,wide_query_only;
 uint32_t ticks;
};
static uint16_t swap01(uint16_t v){return (uint16_t)((v&0xfffcu)|((v&1u)<<1)|((v&2u)>>1));}
static int sel(void*ctx,uint32_t a){(void)ctx;return a?1:0;}
static int write_chip(void*ctx,uint32_t a,uint16_t v){
 struct chip*c=ctx;const struct cb_nor_recipe*r=&c->recipe;
 if(a>=0x1000000)return 0; /* board unlock latch, not a flash opcode */
 assert(v!=r->word&&v!=r->erase&&v!=r->sector); /* no erase/program opcodes */
 if(v==r->reset){c->phase=c->mode=0;return 0;}
 uint16_t query_command=c->wide_query_only?0x9898:0x98;
 if(v==(r->swapped?swap01(query_command):query_command)&&(a==0xaa||a==0||(r->method==CB_NOR_INTEL_DUAL&&a==2))){
  if(c->zero_query_only&&a!=0)return 0;
  c->mode=2;if(a!=2)c->queries++;return 0;
 }
 if(r->method>=CB_NOR_INTEL){if(v==r->id&&(a==0||a==2))c->mode=1;return 0;}
 if(c->phase==0&&a==r->unlock0&&v==r->aa)c->phase=1;
 else if(c->phase==1&&a==r->unlock1&&v==r->bb)c->phase=2;
 else if(c->phase==2&&a==r->unlock0&&v==r->id){c->mode=1;c->phase=0;}
 else c->phase=0;
 return 0;
}
static int read_chip(void*ctx,uint32_t a,uint16_t*v){
 struct chip*c=ctx;c->reads++;
 *v=(uint16_t)((a*31u)^0xa55au);
 unsigned stride=c->recipe.method==CB_NOR_INTEL_DUAL?4:2;
 if(c->mode==2&&a/stride<512){
  *v=c->cfi[a/stride];
  if(c->wrong_die&&(a&2)&&a/stride==0x27)*v^=1;
  if(c->unstable&&c->queries==2&&a==0x4e)*v^=1;
 }else if(c->mode==1||c->pretend_rom){
  static const unsigned offset[]={0,2,0x1c,0x1e};
  for(unsigned n=0;n<4;n++)if(a==(c->recipe.method==CB_NOR_INTEL_DUAL?n*2:offset[n]))
   *v=c->recipe.unit==1?c->entry->id[n]:
    (uint16_t)(c->entry->id[n*2]|(uint16_t)c->entry->id[n*2+1]<<8);
 }
 if(c->recipe.swapped)*v=swap01(*v);
 if(c->recipe.unit==1)*v&=255;
 return 0;
}
static uint32_t now(void*ctx){return ((struct chip*)ctx)->ticks++;}
static void yield(void*ctx){(void)ctx;}
static bool alive(void*ctx){return !((struct chip*)ctx)->dead;}
static bool cancel(void*ctx){(void)ctx;return false;}
static void release(void*ctx){((struct chip*)ctx)->release_count++;}
static void init(struct chip*c,const chisburner_nor_entry_t*e,int method,bool swapped){
 memset(c,0,sizeof(*c));c->entry=e;c->ticks=0xfffffff0u;
 unsigned unit=e->bus==CHISBURNER_NOR_IDBUS_GBA?2:1;
 assert(!cb_nor_recipe_make(method,unit,swapped,&c->recipe));
 uint32_t bytes=chisburner_nor_entry_device_size(e),sector=chisburner_nor_entry_sector_size(e);
 unsigned buf=chisburner_nor_entry_buffer_write_bytes(e);
 if(!bytes)bytes=8*1024*1024;
 if(!sector)sector=65536;
 if(!buf)buf=32;
 if(method==CB_NOR_INTEL_DUAL){bytes/=2;sector/=2;buf/=2;}
 c->cfi[0x10]='Q';c->cfi[0x11]='R';c->cfi[0x12]='Y';
 c->cfi[0x13]=chisburner_nor_entry_cmdset(e)==CHISBURNER_NOR_CMDSET_AMD?2:1;
 while((1u<<c->cfi[0x27])<bytes)c->cfi[0x27]++;
 while((1u<<c->cfi[0x2a])<buf)c->cfi[0x2a]++;
 c->cfi[0x2c]=1;
 unsigned sectors=bytes/sector-1,units=sector/256;
 c->cfi[0x2d]=sectors&255;c->cfi[0x2e]=sectors>>8;
 c->cfi[0x2f]=units&255;c->cfi[0x30]=units>>8;
 if(unit==1&&e->id[0]==0xc2&&(e->id[1]==0xcb||e->id[1]==0xc9||(e->id[1]==0x7e&&e->id[2]==0x10))){
  unsigned boot=e->id[1]!=0xc9?0x2d:0x31,large=e->id[1]!=0xc9?0x31:0x2d;
  c->cfi[0x2c]=2;
  c->cfi[boot]=7;c->cfi[boot+1]=0;c->cfi[boot+2]=32;c->cfi[boot+3]=0;
  c->cfi[large]=126;c->cfi[large+1]=0;c->cfi[large+2]=0;c->cfi[large+3]=1;
 }
}
static unsigned tested;
static void table(const chisburner_nor_entry_t*t,size_t n){
 for(size_t k=0;k<n;k++)for(int method=0;method<CB_NOR_METHOD_COUNT;method++){
  if(!cb_nor_method_matches(t+k,method))continue;
  for(unsigned swapped=0;swapped<2;swapped++){
   struct chip c;struct cb_nor_probe p;struct cb_nor_probe_scratch scratch;
   init(&c,t+k,method,swapped);
   struct cb_nor_io io={&c,sel,read_chip,write_chip,now,yield,alive,cancel,release};
   int rc=cb_nor_probe_bus(&io,c.recipe.unit,&scratch,&p);
   if(rc||p.entry!=t+k){
    fprintf(stderr,"probe %s method %d swap %u rc %d\n",chisburner_nor_entry_name(t+k),method,swapped,rc);
    assert(!rc&&p.entry==t+k);
   }
   assert(p.repeated&&p.restored&&c.mode==0&&c.release_count==1);
   {
    if(p.plan_rc||!p.cfi_valid)fprintf(stderr,"plan %s method %d swap %u rc %d cfi %d\n",chisburner_nor_entry_name(t+k),method,swapped,p.plan_rc,p.cfi_valid);
    assert(!p.plan_rc&&p.cfi_valid);
   }
   tested++;
  }
 }
}
int main(void){
 table(g_chisburner_nor_db_gba,g_chisburner_nor_db_gba_count);
 table(g_chisburner_nor_db_mbc5,g_chisburner_nor_db_mbc5_count);
 struct chip c;struct cb_nor_probe p;struct cb_nor_probe_scratch s;
 struct cb_nor_io io={&c,sel,read_chip,write_chip,now,yield,alive,cancel,release};
 unsigned wide_cases=0;
 for(size_t k=0;k<g_chisburner_nor_db_gba_count;k++)for(int m=CB_NOR_MSP55_AAA;m<=CB_NOR_MSP55_5555;m++){
  const chisburner_nor_entry_t*e=g_chisburner_nor_db_gba+k;
  if(!cb_nor_method_matches(e,m))continue;
  for(unsigned swapped=0;swapped<2;swapped++){
   init(&c,e,m,swapped);c.wide_query_only=true;
   assert(!cb_nor_probe_bus(&io,2,&s,&p)&&p.entry==e&&p.cfi_valid&&!p.plan_rc&&p.restored);
   assert(c.queries==2);wide_cases++;
   init(&c,e,m,swapped);c.wide_query_only=c.unstable=true;
   assert(cb_nor_probe_bus(&io,2,&s,&p)==CB_NOR_VERIFY&&!p.entry&&c.mode==0);
  }
 }
 assert(wide_cases==8);
 puts("PASS MSP wide CFI: 8 record/method/wiring cases requiring 9898 entry");
 for(size_t k=0;k<g_chisburner_nor_db_gba_count;k++){
  const chisburner_nor_entry_t*e=g_chisburner_nor_db_gba+k;
  for(int m=0;m<CB_NOR_METHOD_COUNT;m++){
   if(m==CB_NOR_INTEL_DUAL||!cb_nor_method_matches(e,m))continue;
   for(unsigned swapped=0;swapped<2;swapped++){
    init(&c,e,m,swapped);c.zero_query_only=true;
    for(unsigned n=0;n<512;n++)c.cfi[n]<<=8;
    uint8_t alternate[8];memcpy(alternate,e->id,8);
    for(unsigned n=0;n<8;n+=2)alternate[n]=(uint8_t)swap01(alternate[n]);
    const chisburner_nor_entry_t*alias=chisburner_nor_db_lookup_gba(alternate);
    bool ambiguous=alias&&cb_nor_method_matches(alias,m);
    int rc=cb_nor_probe_bus(&io,2,&s,&p);
    assert(!rc&&p.entry&&p.cfi_valid&&p.restored);
    if(ambiguous)assert(p.plan_rc);
    else assert(p.entry==e&&!p.plan_rc&&p.plan.geometry.lane==1);
    assert(c.queries>=2);
   }
  }
 }
 for(size_t k=0;k<g_chisburner_nor_db_mbc5_count;k++){
  const chisburner_nor_entry_t*e=g_chisburner_nor_db_mbc5+k;
  if(e->id[0]!=0xc2||(e->id[1]!=0xcb&&e->id[1]!=0xc9))continue;
  for(unsigned swapped=0;swapped<2;swapped++){
   init(&c,e,CB_NOR_AMD_AAA,swapped);c.cfi[0x2d]^=1;
   assert(!cb_nor_probe_bus(&io,1,&s,&p)&&p.entry==e&&!p.cfi_valid&&p.plan_rc&&p.restored);
   init(&c,e,CB_NOR_AMD_AAA,swapped);memset(c.cfi,0,sizeof(c.cfi));
   assert(!cb_nor_probe_bus(&io,1,&s,&p)&&p.entry==e&&!p.cfi_valid&&!p.plan_rc&&p.restored);
  }
 }
 unsigned compat_tested=0;
 unsigned fallback_tested=0;
 for(unsigned bus=0;bus<2;bus++){
  const chisburner_nor_entry_t*table=bus?g_chisburner_nor_db_mbc5:g_chisburner_nor_db_gba;
  size_t total=bus?g_chisburner_nor_db_mbc5_count:g_chisburner_nor_db_gba_count;
  for(size_t k=0;k<total;k++)for(int method=0;method<CB_NOR_METHOD_COUNT;method++){
   const chisburner_nor_entry_t*e=table+k;
   if(!cb_nor_method_matches(e,method))continue;
   for(unsigned swapped=0;swapped<2;swapped++){
    init(&c,e,method,swapped);memset(c.cfi,0,sizeof(c.cfi));
    assert(!cb_nor_probe_bus(&io,bus?1:2,&s,&p)&&p.entry&&p.restored&&!p.cfi_valid);
    bool allowed=chisburner_nor_entry_cmdset(p.entry)==CHISBURNER_NOR_CMDSET_AMD&&
     chisburner_nor_entry_device_size(p.entry)&&chisburner_nor_entry_sector_size(p.entry);
    if(bus&&e->id[0]==0xc2&&e->id[1]==0x7e&&e->id[2]==0x10)allowed=false;
    assert((!p.plan_rc)==allowed);
    /* A visible QRY whose sector map does not sum to capacity may NEVER
     * take the metadata fallback, even on a supported uniform chip. */
    init(&c,e,method,swapped);c.cfi[0x2d]^=1;
    assert(!cb_nor_probe_bus(&io,bus?1:2,&s,&p)&&p.entry&&p.restored&&p.plan_rc);
    fallback_tested++;
   }
  }
 }
 printf("PASS Mori metadata fallback: %u absent-versus-invalid CFI acquisitions\n",fallback_tested);
 for(size_t k=0;k<g_chisburner_nor_db_gba_count;k++){
  const chisburner_nor_entry_t*e=g_chisburner_nor_db_gba+k;
  int m=cb_nor_method_matches(e,CB_NOR_INTEL)?CB_NOR_INTEL:
        cb_nor_method_matches(e,CB_NOR_SHARP)?CB_NOR_SHARP:-1;
  if(m<0)continue;
  bool style=(e->id[0]==0x1c||e->id[0]==0x20||e->id[0]==0x89||e->id[0]==0x8a||e->id[0]==0xb0)&&
   !e->id[1]&&(!e->id[3]||e->id[3]==0x88||e->id[3]==0x89);
  for(unsigned primary=1;primary<=4;primary++)for(unsigned swapped=0;swapped<2;swapped++){
   init(&c,e,m,swapped);c.cfi[0x13]=0;c.cfi[0x14]=primary;
   int rc=cb_nor_probe_bus(&io,2,&s,&p);
   if(rc||!p.entry||!p.restored)
    fprintf(stderr,"compat %s primary %u swap %u rc %d entry %s cfi %d restored %d\n",chisburner_nor_entry_name(e),primary,swapped,rc,p.entry?chisburner_nor_entry_name(p.entry):"none",p.cfi_valid,p.restored);
   assert(!rc&&p.entry&&p.restored);
   if(style&&primary<=3){assert(p.entry==e&&p.cfi_valid&&!p.plan_rc&&p.plan.geometry.primary_id==primary*256);compat_tested++;}
   else assert(p.plan_rc); /* Unsupported primary and vendor remain locked. */
   if(style&&primary==1&&!swapped){
    c.cfi[0x2d]^=1; /* Compatibility must not excuse inconsistent capacity. */
    assert(!cb_nor_probe_bus(&io,2,&s,&p)&&p.plan_rc);
   }
  }
 }
 init(&c,g_chisburner_nor_db_gba+3,CB_NOR_AMD_AAA,false);
 c.cfi[0x13]=0;c.cfi[0x14]=2;
 assert(!cb_nor_probe_bus(&io,2,&s,&p)&&p.plan_rc); /* AMD JS28F256 is not Intel by vendor alone. */
 assert(compat_tested);
 printf("PASS Intel-compatible CFI: %u known-ID cases; unknown primary and AMD rejection\n",compat_tested);
 init(&c,g_chisburner_nor_db_gba+3,CB_NOR_AMD_AAA,false);c.unstable=true;
 assert(cb_nor_probe_bus(&io,2,&s,&p)==CB_NOR_VERIFY&&c.mode==0&&c.release_count==1&&p.entry==NULL);
 init(&c,g_chisburner_nor_db_gba+3,CB_NOR_AMD_AAA,false);c.pretend_rom=true;
 assert(cb_nor_probe_bus(&io,2,&s,&p)==CB_NOR_UNSUPPORTED&&c.mode==0&&p.entry==NULL);
 init(&c,g_chisburner_nor_db_gba+3,CB_NOR_AMD_AAA,false);c.dead=true;
 assert(cb_nor_probe_bus(&io,2,&s,&p)==CB_NOR_CANCELLED&&c.reads==0&&c.release_count==1);
 for(size_t n=0;n<g_chisburner_nor_db_gba_count;n++)if(cb_nor_method_matches(g_chisburner_nor_db_gba+n,CB_NOR_INTEL_DUAL)){
  init(&c,g_chisburner_nor_db_gba+n,CB_NOR_INTEL_DUAL,false);c.wrong_die=true;
  assert(cb_nor_probe_bus(&io,2,&s,&p)==CB_NOR_VERIFY&&p.entry==NULL);
 }
 printf("NOR108 probe PASS: %u model acquisitions, changing CFI, ROM-as-ID and lifecycle rejection\n",tested);
}
