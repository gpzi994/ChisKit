#include "execute.h"
#include "mori/chisburner_nor_db_data.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Command-trace oracle, NOT an electrical flash-chip model. Expected writes
 * are independent of execute.c. Readback is prescribed and checked in order. */
struct event {char kind;uint32_t address;uint16_t value;};
static struct event events[600000];
static size_t count,pos,fail_at;
static bool cancel_now,live,corrupt;
static unsigned releases;
static uint32_t clock_ms;
static uint32_t clock_step;
static void add(char k,uint32_t a,uint16_t v){assert(count<600000);events[count++]=(struct event){k,a,v};}
static int consume(char k,uint32_t a,uint16_t *v){
 assert(pos<count);
 struct event e=events[pos++];
 if(e.kind!=k||e.address!=a){
  fprintf(stderr,"trace mismatch %zu expected %c/%x got %c/%x\n",pos-1,e.kind,e.address,k,a);abort();
 }
 if(k=='W')assert(*v==e.value);else if(k=='R')*v=corrupt?(uint16_t)(e.value^0x80):e.value;
 clock_ms++;
 return pos==fail_at?-1:0;
}
static int select_io(void*c,uint32_t a){(void)c;uint16_t v=0;return consume('S',a,&v);}
static int write_io(void*c,uint32_t a,uint16_t v){(void)c;return consume('W',a,&v);}
static int read_io(void*c,uint32_t a,uint16_t*v){(void)c;return consume('R',a,v);}
static uint32_t now(void*c){(void)c;clock_ms+=clock_step;return clock_ms;}
static void yield(void*c){(void)c;}
static bool alive(void*c){(void)c;return live;}
static bool cancelled(void*c){(void)c;return cancel_now;}
static void release(void*c){(void)c;releases++;}
static struct cb_nor_io io={NULL,select_io,read_io,write_io,now,yield,alive,cancelled,release};
static uint16_t swap01(uint16_t v){return (uint16_t)((v&0xfffcu)|((v&1u)<<1)|((v&2u)>>1));}
static uint16_t cmd(struct cb_nor_plan*p,uint16_t v){return p->recipe.swapped?swap01(v):v;}
static uint32_t pa(struct cb_nor_plan*p,uint32_t a){return p->recipe.unit==1?(a<0x4000?a:0x4000+(a&0x3fff)):a&0x1ffffff;}
static void reset_trace(struct cb_nor_plan*p,uint32_t a){
 for(unsigned d=0;d<(p->recipe.method==CB_NOR_INTEL_DUAL?2u:1u);d++){
  if(p->geometry.cmdset==CHISBURNER_NOR_CMDSET_INTEL)add('W',a+d*2,cmd(p,0x50));
  add('W',a+d*2,p->recipe.reset);
 }
}
static void fresh(void){count=pos=0;fail_at=0;cancel_now=corrupt=false;live=true;releases=0;clock_ms=0xfffffff0u;clock_step=0;}
static struct cb_nor_plan plan(const chisburner_nor_entry_t*e,int m,bool swap){
 struct cb_nor_cfi c={0};
 struct cb_nor_plan p;
 c.cmdset=chisburner_nor_entry_cmdset(e);c.bytes=chisburner_nor_entry_device_size(e);
 if(!c.bytes)c.bytes=8*1024*1024;
 uint32_t sector=chisburner_nor_entry_sector_size(e);
 if(!sector)sector=65536;
 c.region_count=1;c.region[0]=(struct cb_nor_region){0,c.bytes/sector,sector};
 c.buffer_bytes=chisburner_nor_entry_buffer_write_bytes(e);
 if(!c.buffer_bytes)c.buffer_bytes=32;
 if(e->bus==CHISBURNER_NOR_IDBUS_MBC5&&e->id[0]==0xc2&&e->id[1]==0x7e&&e->id[2]==0x10){
  c.region_count=2;c.region[0]=(struct cb_nor_region){0,8,8192};c.region[1]=(struct cb_nor_region){65536,127,65536};
 }
 if(e->bus==CHISBURNER_NOR_IDBUS_MBC5&&e->id[0]==0xc2&&(e->id[1]==0xcb||e->id[1]==0xc9)){
  assert(!cb_nor_plan_make(e,m,swap,NULL,&p));c=p.geometry;
 }
 assert(!cb_nor_plan_make(e,m,swap,&c,&p));return p;
}
static void program_trace_expected(struct cb_nor_plan*p,uint32_t address,const uint8_t*data,size_t len,bool readback){
 unsigned unit=p->recipe.unit,dies=p->recipe.method==CB_NOR_INTEL_DUAL?2:1;
 uint32_t a=pa(p,address);
 add('S',address,0);reset_trace(p,0);
 if(p->geometry.cmdset==CHISBURNER_NOR_CMDSET_INTEL){
  if(p->flags&CHISBURNER_NOR_FLAG_DISABLE_BUFFER_PROGRAM){
   add('W',0,cmd(p,0x70));add('R',0,0x80);add('W',0,cmd(p,0x10));
   add('W',a,(uint16_t)(data[0]|data[1]<<8));
  }else if(!p->geometry.buffer_bytes){
   add('W',a,p->recipe.word);add('W',a,(uint16_t)(data[0]|data[1]<<8));
  }else{
   for(unsigned d=0;d<dies;d++){add('W',a+d*2,p->recipe.buffer);add('R',a+d*2,0x80);}
   for(unsigned d=0;d<dies;d++)add('W',a+d*2,cb_nor_count(&p->recipe,(unsigned)len/unit/dies));
   for(size_t n=0;n<len;n+=unit)add('W',a+(uint32_t)n,(uint16_t)(data[n]|data[n+1]<<8));
   for(unsigned d=0;d<dies;d++)add('W',a+d*2,p->recipe.confirm);
  }
  for(unsigned d=0;d<dies;d++)add('R',a+d*2,0x80);
 }else{
  add('W',p->recipe.unlock0,p->recipe.aa);add('W',p->recipe.unlock1,p->recipe.bb);
  if(p->geometry.buffer_bytes){
   add('W',a,p->recipe.buffer);add('W',a,cb_nor_count(&p->recipe,(unsigned)len/unit));
  }else add('W',p->recipe.unlock0,p->recipe.word);
  for(size_t n=0;n<len;n+=unit)add('W',a+(uint32_t)n,(uint16_t)(data[n]|(unit==2?data[n+1]<<8:0)));
  if(p->geometry.buffer_bytes)add('W',a,p->recipe.confirm);
  uint16_t v=(uint16_t)(data[len-unit]|(unit==2?data[len-1]<<8:0));
  add('R',a+(uint32_t)len-unit,v);add('R',a+(uint32_t)len-unit,v);
 }
 reset_trace(p,a);
 if(readback){
  for(size_t n=0;n<len;n+=unit){
   add('S',address+(uint32_t)n,0);
   add('R',a+(uint32_t)n,(uint16_t)(data[n]|(unit==2?data[n+1]<<8:0)));
  }
 }
 reset_trace(p,0);
}
static void program_trace(struct cb_nor_plan*p,uint32_t address,const uint8_t*data,size_t len){program_trace_expected(p,address,data,len,true);}
static void program_poll_trace(struct cb_nor_plan*p,uint32_t address,const uint8_t*data,size_t len){program_trace_expected(p,address,data,len,false);}
static unsigned tested;
static void erase_trace_expected(struct cb_nor_plan*p,uint32_t address,bool readback){
 uint32_t start,len,a=pa(p,address);
 assert(!cb_nor_sector_at(p,address,&start,&len)&&start==address);
 add('S',address,0);reset_trace(p,0);
 if(p->geometry.cmdset==CHISBURNER_NOR_CMDSET_INTEL){
  unsigned dies=p->recipe.method==CB_NOR_INTEL_DUAL?2:1;
  for(unsigned d=0;d<dies;d++){
   if(!p->skip_block_unlock){add('W',a+d*2,cmd(p,0x60));add('W',a+d*2,cmd(p,0xd0));}
   add('W',a+d*2,p->recipe.erase);add('W',a+d*2,p->recipe.sector);
  }
  for(unsigned d=0;d<dies;d++)add('R',a+d*2,0x80);
 }else{
  add('W',p->recipe.unlock0,p->recipe.aa);add('W',p->recipe.unlock1,p->recipe.bb);
  add('W',p->recipe.unlock0,p->recipe.erase);
  add('W',p->recipe.unlock0,p->recipe.aa);add('W',p->recipe.unlock1,p->recipe.bb);
  add('W',a,p->recipe.sector);add('R',a,0xffff);add('R',a,0xffff);
 }
 reset_trace(p,a);
 if(readback){
  for(uint32_t n=0;n<len;n+=p->recipe.unit){
   add('S',address+n,0);add('R',pa(p,address+n),p->recipe.unit==1?0xff:0xffff);
  }
 }
 reset_trace(p,0);
}
static void erase_trace(struct cb_nor_plan*p,uint32_t address){erase_trace_expected(p,address,true);}
static void erase_poll_trace(struct cb_nor_plan*p,uint32_t address){erase_trace_expected(p,address,false);}
static void test_table(const chisburner_nor_entry_t*t,size_t n){
 uint8_t data[1024];for(unsigned i=0;i<sizeof(data);i++)data[i]=(uint8_t)(i*37+0xf0);
 for(size_t i=0;i<n;i++)for(int m=0;m<CB_NOR_METHOD_COUNT;m++)if(cb_nor_method_matches(t+i,m)){
  for(unsigned swap=0;swap<2;swap++){
   struct cb_nor_plan p=plan(t+i,m,swap);
   uint32_t a=p.geometry.bytes-1024;
   size_t len=cb_nor_chunk(&p,a,sizeof(data));
   struct cb_nor_job j={&p,io,true,0,0};
   fresh();program_trace(&p,a,data,len);
   assert(!cb_nor_program(&j,a,data,len,1000)&&pos==count&&releases==1&&j.authorized);
   uint32_t sector_start,sector_len;
   assert(!cb_nor_sector_at(&p,p.geometry.bytes-1,&sector_start,&sector_len));
   fresh();erase_trace(&p,sector_start);
   assert(!cb_nor_erase(&j,sector_start,1000)&&pos==count&&releases==1&&j.authorized);
   fresh();program_trace(&p,0,data,len);
   assert(!cb_nor_program(&j,0,data,len,1000)&&pos==count&&releases==1);
   fresh();erase_trace(&p,0);
   assert(!cb_nor_erase(&j,0,1000)&&pos==count&&releases==1);
   fresh();program_poll_trace(&p,0,data,len);
   assert(!cb_nor_program_poll(&j,0,data,len,1000)&&pos==count&&releases==1&&j.authorized);
   fresh();erase_poll_trace(&p,0);
   assert(!cb_nor_erase_poll(&j,0,1000)&&pos==count&&releases==1&&j.authorized);
   tested++;
   fresh();j.authorized=false;
   assert(cb_nor_program(&j,a,data,len,1000)==CB_NOR_UNARMED&&pos==0);
   j.authorized=true;live=false;
   assert(cb_nor_program(&j,a,data,len,1000)==CB_NOR_CANCELLED&&!j.authorized&&pos==0);
   live=true;j.authorized=true;
   assert(cb_nor_program(&j,p.geometry.bytes,data,len,1000)==CB_NOR_ARGUMENT&&!j.authorized&&pos==0);
  }
 }
}
int main(void){
 /* Wide MSP asynchronous byte lanes. Inject busy samples ahead of the
  * independently prescribed success trace; early reset fails the oracle. */
 for(size_t k=0;k<g_chisburner_nor_db_gba_count;k++)
  for(int m=CB_NOR_MSP55_AAA;m<=CB_NOR_MSP55_5555;m++){
   if(!cb_nor_method_matches(g_chisburner_nor_db_gba+k,m))continue;
   for(unsigned swapped=0;swapped<2;swapped++){
    struct cb_nor_plan p=plan(g_chisburner_nor_db_gba+k,m,swapped);
    struct cb_nor_job j={&p,io,true,0,0};uint8_t data[2]={0xff,0xff};
    assert(p.recipe.ready_mask==0x8080&&p.recipe.error_mask==0x2020);
    const uint16_t busy[]={0x00ff,0xff00,0x7fff};
    for(unsigned b=0;b<3;b++){
     fresh();program_trace(&p,0,data,2);size_t first=0;
     while(first<count&&events[first].kind!='R')first++;
     assert(first<count&&count+1<600000);
     memmove(events+first+1,events+first,(count-first)*sizeof(*events));count++;
     events[first]=(struct event){'R',0,busy[b]};
     /* 7fff signals high DQ5; next read FFFF is its required final reread.
      * It is not sufficient for completion: the regular two reads follow. */
     if(b==2){
      memmove(events+first+2,events+first+1,(count-first-1)*sizeof(*events));count++;
      events[first+1]=(struct event){'R',0,0xffff};
     }
     j.authorized=true;
     assert(!cb_nor_program(&j,0,data,2,1000)&&pos==count&&j.authorized);
    }
    const uint16_t errors[]={0x20ff,0xff20};
    for(unsigned b=0;b<2;b++){
     fresh();program_trace(&p,0,data,2);
     for(size_t n=0;n<count;n++)if(events[n].kind=='R'){count=n;break;}
     add('R',0,errors[b]);add('R',0,errors[b]);reset_trace(&p,0);j.authorized=true;
     assert(cb_nor_program(&j,0,data,2,1000)==CB_NOR_STATUS&&pos==count&&!j.authorized);
    }
   }
  }
 puts("PASS MSP wide polling: independent byte-lane completion, completed-lane DQ5 data, high/low errors and final reread");
 /* Board overrides do not inherit ordinary Intel block-unlock/reset rules. */
 for(size_t k=0;k<g_chisburner_nor_db_gba_count;k++){
  if(!cb_nor_method_matches(g_chisburner_nor_db_gba+k,CB_NOR_INTEL_DUAL))continue;
  struct cb_nor_plan b=plan(g_chisburner_nor_db_gba+k,CB_NOR_INTEL_DUAL,false);
  struct cb_nor_job q={&b,io,true,0,0};
  b.skip_block_unlock=true;fresh();erase_trace(&b,0);
  assert(!cb_nor_erase(&q,0,1000)&&pos==count);
  b.accessible_bytes=0x10000000;b.reset_stride=0x400000;
  fresh();for(uint32_t a=0;a<b.accessible_bytes;a+=b.reset_stride){add('S',a,0);reset_trace(&b,pa(&b,a));}
  add('S',0,0);reset_trace(&b,0);
  assert(!cb_nor_finish(&q)&&pos==count&&!q.authorized);
  fresh();q.authorized=true;b.reset_stride=0x300000;
  assert(cb_nor_finish(&q)==CB_NOR_ARGUMENT&&!q.authorized&&!pos);
 }
 test_table(g_chisburner_nor_db_gba,g_chisburner_nor_db_gba_count);
 test_table(g_chisburner_nor_db_mbc5,g_chisburner_nor_db_mbc5_count);
 struct cb_nor_plan p=plan(g_chisburner_nor_db_gba+3,CB_NOR_AMD_AAA,false);
 struct cb_nor_job j={&p,io,true,0,0};
 uint8_t data[2]={0x11,0x22};
 /* No reset or command writes through a failed board selection. */
 fresh();add('S',0,0);fail_at=1;
 assert(cb_nor_program(&j,0,data,2,100)==CB_NOR_UNSUPPORTED&&!j.authorized&&pos==count);
 /* Also fail selection during readback, AFTER actual programming. No reset
  * is allowed through the now-unknown route and authorization is revoked. */
 fresh();program_trace(&p,0,data,2);
 for(size_t n=1;n<count;n++)if(events[n].kind=='S'){count=n+1;fail_at=count;break;}
 j.authorized=true;
 assert(cb_nor_program(&j,0,data,2,100)==CB_NOR_UNSUPPORTED&&!j.authorized&&pos==count&&releases==1);
 j.authorized=true;
 /* A failed first bus command must stop, reset and release, not program. */
 fresh();add('S',0,0);add('W',0,0xf0);reset_trace(&p,0);fail_at=2;
 assert(cb_nor_program(&j,0,data,2,100)==CB_NOR_IO&&!j.authorized&&pos==count&&releases==1);
 /* AMD DQ5 must be checked after a final DQ7 read. */
 data[0]=data[1]=0xff;
 fresh();program_trace(&p,0,data,2);
 for(size_t n=0;n<count;n++)if(events[n].kind=='R'){count=n;break;}
 add('R',0,0x20);add('R',0,0x20);reset_trace(&p,0);j.authorized=true;
 assert(cb_nor_program(&j,0,data,2,100)==CB_NOR_STATUS&&!j.authorized&&pos==count);
 /* Explicit timeout with a wrapping clock: no unbounded status loop. */
 fresh();program_trace(&p,0,data,2);
 for(size_t n=0;n<count;n++)if(events[n].kind=='R'){count=n;break;}
 reset_trace(&p,0);clock_step=1000;j.authorized=true;
 assert(cb_nor_program(&j,0,data,2,100)==CB_NOR_TIMEOUT&&!j.authorized&&pos==count);
 /* Status success does not replace readback verification. */
 fresh();program_trace(&p,0,data,2);
 for(size_t n=0,reads_seen=0;n<count;n++)if(events[n].kind=='R'&&++reads_seen==3){events[n].value=0;break;}
 j.authorized=true;
 assert(cb_nor_program(&j,0,data,2,100)==CB_NOR_VERIFY&&!j.authorized&&pos==count);
 /* CFI may report no write buffer; Intel/Sharp still support single words. */
 for(size_t k=0;k<g_chisburner_nor_db_gba_count;k++){
  const chisburner_nor_entry_t*e=g_chisburner_nor_db_gba+k;
  if(!cb_nor_method_matches(e,CB_NOR_INTEL)&&!cb_nor_method_matches(e,CB_NOR_SHARP))continue;
  p=plan(e,cb_nor_method_matches(e,CB_NOR_INTEL)?CB_NOR_INTEL:CB_NOR_SHARP,false);
  p.geometry.buffer_bytes=0;j=(struct cb_nor_job){&p,io,true,0,0};
  fresh();program_trace(&p,0,data,2);
  assert(!cb_nor_program(&j,0,data,2,1000)&&pos==count);
 }
 printf("NOR108 executor PASS: %u per-record program+erase traces; guards, DQ5, timeout, readback and I/O failure tests\n",tested);
}
