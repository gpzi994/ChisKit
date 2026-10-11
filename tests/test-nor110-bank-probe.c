// SPDX-License-Identifier: AGPL-3.0-only
#include "bank_probe110.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
struct model {
 struct cb_nor_probe seed;
 unsigned bank,step,writes,probes,releases,fail_write,fail_probe,flip_probe,epoch_write;
 uint32_t epoch,ticks;
 unsigned sequence_length;
 bool frozen,flip_cfi;
};
static int control(void*v,uint32_t a,uint8_t data){
 struct model*m=v;m->writes++;
 if(m->writes==m->fail_write)return CB_NOR_IO;
 assert(a==2+m->step);
 if(!m->step){assert(!(data&15));m->bank=data>>4;}
 if(m->step==1)assert(data==0x40);
 if(m->step==2)assert(!data);
 m->step=(m->step+1)%m->sequence_length;
 if(m->writes==m->epoch_write)m->epoch++;
 return 0;
}
static int probe(void*v,struct cb_nor_probe*p){
 struct model*m=v;assert(!m->step);m->probes++;
 if(m->probes==m->fail_probe)return CB_NOR_IO;
 *p=m->seed;p->baseline[0]=(uint16_t)m->bank;
 if(m->flip_cfi&&m->probes==2)p->cfi_valid=!p->cfi_valid;
 if(m->probes==m->flip_probe)p->baseline[1]++;
 return 0;
}
static bool alive(void*v){(void)v;return true;}
static uint32_t epoch(void*v){return ((struct model*)v)->epoch;}
static uint32_t now(void*v){struct model*m=v;return m->frozen?m->ticks:m->ticks++;}
static void idle(void*v){(void)v;}
static void release(void*v){((struct model*)v)->releases++;}
static void setup(struct model*m,unsigned profile,bool swapped,bool absent){
 memset(m,0,sizeof(*m));m->epoch=17;
 bool intel=profile==1;m->sequence_length=profile<2?3:2;
 uint8_t id[8]={2,1,0x7d,0x7e};
 if(intel)memcpy(id,(uint8_t[]){0x89,0,0xb0,0x88,4,0,0x89,0},8);
 if(profile>=2)memcpy(id,(uint8_t[]){1,0,0x7e,0x22,profile==2?0x23:profile==3?0x28:0x48,0x22,1,0x22},8);
 struct cb_nor_probe*p=&m->seed;memcpy(p->id,id,8);
 p->entry=chisburner_nor_db_lookup_gba(id);assert(p->entry);
 p->method=intel?CB_NOR_INTEL:CB_NOR_MSP55_AAA;p->swapped=swapped;
 if(profile>=2)p->method=profile==2?CB_NOR_AMD_AAA:CB_NOR_AMD_555;
 p->repeated=p->restored=p->cfi_valid=true;
 struct cb_nor_cfi c={0};c.bytes=intel?0x8000000:0x4000000;
 if(profile>=2)c.bytes=UINT32_C(0x4000000)<<(profile-2);
 c.buffer_bytes=intel?1024:32;c.cmdset=intel?CHISBURNER_NOR_CMDSET_INTEL:CHISBURNER_NOR_CMDSET_AMD;
 c.primary_id=intel?1:2;c.region_count=1;
 c.region[0]=(struct cb_nor_region){0,c.bytes/(intel?0x40000:0x20000),intel?0x40000:0x20000};
 assert(!cb_nor_plan_make(p->entry,p->method,swapped,&c,&p->plan));
 if(absent){
  p->cfi_valid=false;
  assert(!cb_nor_plan_make(p->entry,p->method,swapped,NULL,&p->plan));
 }
}
static int run(struct model*m,struct cb_bank110_capture*c){
 struct cb_bank110_io io={m,control,probe,alive,epoch,now,idle,release};
 struct cb_nor_probe scratch;
 return cb_bank110_capture(&io,&m->seed,&scratch,c);
}
int main(void){
 struct model m;struct cb_bank110_capture c;unsigned faults=0;
 for(unsigned absent=0;absent<2;absent++)for(unsigned intel=0;intel<5;intel++)for(unsigned swap=0;swap<2;swap++){
  if(absent&&intel==1)continue; /* Intel requires valid CFI. */
  setup(&m,intel,swap,absent);assert(!run(&m,&c));
  unsigned banks=m.seed.plan.geometry.bytes/CB_BANK110_WINDOW,commands=(banks+1)*m.sequence_length,calls=banks*2+1;
  assert(c.qualified&&c.restored&&c.banks==banks&&m.bank==0&&m.releases==1);
  assert(m.writes==commands&&m.probes==calls);
  for(unsigned b=0;b<banks;b++)assert(c.normal[b][0]==b);
  for(unsigned n=1;n<=commands;n++){
   setup(&m,intel,swap,absent);m.fail_write=n;
   assert(run(&m,&c)==CB_NOR_IO&&!c.qualified&&!c.restored&&m.writes==n&&m.releases==1);faults++;
   setup(&m,intel,swap,absent);m.epoch_write=n;
   assert(run(&m,&c)==CB_NOR_CANCELLED&&!c.qualified&&!c.restored&&m.writes==n);faults++;
  }
  for(unsigned n=1;n<=calls;n++){
   setup(&m,intel,swap,absent);m.fail_probe=n;
   assert(run(&m,&c)==CB_NOR_IO&&!c.qualified&&!c.restored&&m.probes==n);faults++;
  }
  setup(&m,intel,swap,absent);m.flip_probe=2;
  assert(run(&m,&c)==CB_NOR_VERIFY&&!c.qualified&&c.restored&&m.bank==0);
  setup(&m,intel,swap,absent);m.frozen=true;
  assert(run(&m,&c)==CB_NOR_TIMEOUT&&!c.qualified&&m.writes==m.sequence_length&&!m.probes);
  setup(&m,intel,swap,absent);m.seed.plan_rc=CB_NOR_VERIFY;
  assert(run(&m,&c)==CB_NOR_UNSUPPORTED&&!m.writes&&!m.probes);
  setup(&m,intel,swap,absent);m.seed.id[0]^=0x40;
  assert(run(&m,&c)==CB_NOR_UNSUPPORTED&&!m.writes&&!m.probes);
  setup(&m,intel,swap,absent);m.flip_cfi=true;
  assert(run(&m,&c)==CB_NOR_VERIFY&&!c.qualified&&c.restored);
  if(absent){
   setup(&m,intel,swap,true);m.seed.plan.geometry.region[0].count--;
   assert(run(&m,&c)==CB_NOR_UNSUPPORTED&&!m.writes&&!m.probes);
  }
 }
 setup(&m,1,false,false);m.seed.cfi_valid=false;
 assert(run(&m,&c)==CB_NOR_UNSUPPORTED&&!m.writes&&!m.probes);
 printf("PASS bank110 capture: five CFI/four explicit no-CFI profiles/both wirings; %u injected write/epoch/probe failures; stable restore and qualification gates\n",faults);
}
