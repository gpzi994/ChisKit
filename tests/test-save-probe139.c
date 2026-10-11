#include "../core/save/probe139.h"
#include <assert.h>
#include <stdio.h>
struct mock {unsigned reads,writes,fail_read,fail_write,removed_after_write;
 unsigned ignore_writes,bad_observation,unstable;unsigned char value;};
static int alive(void *ctx){struct mock *m=ctx;return !m->removed_after_write||m->writes<m->removed_after_write;}
static int rd(void *ctx,uint32_t addr,uint8_t *v){
 struct mock*m=ctx;assert(addr==4);m->reads++;
 if(m->reads==m->fail_read)return -20;
 *v=m->value;if(m->unstable&&m->reads==2)*v^=1;
 if(m->bad_observation&&m->reads==3)*v=0x42;
 return 0;
}
static int wr(void *ctx,uint32_t addr,uint8_t v){
 struct mock*m=ctx;assert(addr==4);m->writes++;
 if(!m->ignore_writes)m->value=v;
 return m->writes==m->fail_write?-21:0;
}
static struct cb_save139_probe_report run(struct mock*m){
 struct cb_save139_probe_ops ops={m,rd,wr,alive};return cb_save139_probe_byte(&ops);
}
static unsigned waits,fail_wait;
static int delay(void *ctx,unsigned ms){
 struct mock*m=ctx;assert(ms==10&&m->writes==waits+1);
 return ++waits==fail_wait?-22:0;
}
int main(void){
 struct mock m={.value=0x5a};struct cb_save139_probe_report r=run(&m);
 assert(r.state==CB_SAVE139_PROBE_SRAM_LIKE&&r.restore_verified&&m.value==0x5a);
 m=(struct mock){.value=0x5a,.ignore_writes=1};r=run(&m);
 assert(r.state==CB_SAVE139_PROBE_UNCHANGED&&r.restore_verified);
 for(unsigned i=1;i<=4;i++){
  m=(struct mock){.value=0x5a,.fail_read=i};r=run(&m);
  assert(r.rc||r.restore_rc);assert(m.value==0x5a);
  assert(m.writes==(i<3?0:2));
  if(i==3)assert(r.restore_verified);
  if(i==4)assert(!r.restore_verified);
 }
 for(unsigned i=1;i<=2;i++){
  m=(struct mock){.value=0x5a,.fail_write=i};r=run(&m);
  assert(r.rc||r.restore_rc);assert(m.value==0x5a&&m.writes==2);
 }
 m=(struct mock){.value=0x5a,.unstable=1};r=run(&m);
 assert(r.state==CB_SAVE139_PROBE_UNSTABLE&&!m.writes);
 m=(struct mock){.value=0x5a,.bad_observation=1};r=run(&m);
 assert(r.state==CB_SAVE139_PROBE_INCONCLUSIVE&&r.restore_verified&&m.value==0x5a);
 m=(struct mock){.value=0x5a,.removed_after_write=1};r=run(&m);
 assert(r.restore_rc&&!r.restore_verified&&m.writes==1&&r.original==0x5a);
 for(fail_wait=0;fail_wait<=2;fail_wait++){
  m=(struct mock){.value=0x4c};waits=0;
  struct cb_save139_probe_ops ops={&m,rd,wr,alive};
  r=cb_save139_probe_byte_timed(&ops,delay);
  assert(waits==2&&m.writes==2&&m.value==0x4c);
  if(fail_wait==1)assert(r.rc==-22&&r.restore_verified);
  else if(fail_wait==2)assert(r.restore_rc==-22&&!r.restore_verified);
  else assert(!r.rc&&r.restore_verified);
 }
 puts("PASS: byte probe and 10ms test/restore waits, delay failures retain cleanup (host mock)");
}
