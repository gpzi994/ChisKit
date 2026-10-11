#include "../core/save/probe139.h"
#include <assert.h>
#include <stdio.h>
struct mock {unsigned reads,writes,fail_read,fail_write,phase,id_mode,sram,removed;unsigned delays,settled,atmel,fail_delay,remove_delay;uint8_t data[65536];};
static int alive(void*x){return !((struct mock*)x)->removed;}
static int rd(void*x,uint32_t a,uint8_t*v){struct mock*m=x;assert(a<65536);m->reads++;
 if(m->reads==m->fail_read)return -20;
 if(m->id_mode&&a<2)assert(m->settled);
 *v=m->id_mode&&a<2?(m->atmel?(a?0x3d:0x1f):(a?0x09:0xc2)):m->data[a];return 0;}
static int wr(void*x,uint32_t a,uint8_t v){struct mock*m=x;m->writes++;
 if(m->sram)m->data[a]=v;
 else if(v==0xf0){m->id_mode=0;m->phase=0;}
 else if(a==0x5555&&v==0xaa)m->phase=1;
 else if(a==0x2aaa&&v==0x55&&m->phase==1)m->phase=2;
 else if(a==0x5555&&v==0x90&&m->phase==2){m->id_mode=1;m->phase=0;m->settled=0;}
 else m->phase=0;
 return m->writes==m->fail_write?-21:0;
}
static int delay(void*x,unsigned ms){struct mock*m=x;assert(ms==10);m->delays++;
 if(m->delays==m->remove_delay){m->removed=1;return -2;}
 if(m->delays==m->fail_delay)return -22;
 if(m->id_mode)m->settled=1;
 return 0;}
static struct cb_save139_flash_report run(struct mock*m){struct cb_save139_flash_ops o={{m,rd,wr,alive},delay};return cb_save139_probe_flash(&o);}
int main(void){
 struct mock m={0};struct cb_save139_flash_report r=run(&m);
 assert(!r.rc&&!r.cleanup_rc&&r.id==0xc209&&r.known_id&&r.exit_verified&&!m.id_mode&&m.delays==5);
 unsigned reads=m.reads,writes=m.writes;
 for(unsigned i=1;i<=reads;i++){
  m=(struct mock){.fail_read=i};r=run(&m);
  assert(r.rc||r.cleanup_rc);assert(!m.id_mode);
 }
 for(unsigned i=1;i<=writes;i++){
  m=(struct mock){.fail_write=i};r=run(&m);
  assert(r.rc||r.cleanup_rc);assert(!m.id_mode);
 }
 m=(struct mock){.sram=1};m.data[4]=0x72;r=run(&m);
 assert(!r.rc&&r.direct.state==CB_SAVE139_PROBE_SRAM_LIKE&&!r.id_read&&m.writes==2&&m.data[4]==0x72);
 m=(struct mock){.removed=1};r=run(&m);assert(r.rc&&!m.writes);
 m=(struct mock){.atmel=1};r=run(&m);
 assert(!r.rc&&!r.cleanup_rc&&r.id==0x1f3d&&r.known_id&&r.exit_verified&&m.delays==5);
 for(unsigned i=1;i<=5;i++){
  m=(struct mock){.atmel=1,.fail_delay=i};r=run(&m);
  assert(r.rc||r.cleanup_rc);assert(!m.id_mode);
  if(i==1)assert(r.rc==-22&&!r.id_read&&r.direct.restore_verified);
  if(i==2)assert(r.direct.restore_rc==-22&&!r.id_read);
  if(i==3)assert(r.rc==-22&&!r.id_read&&r.exit_verified);
 }
 m=(struct mock){.atmel=1,.remove_delay=1};r=run(&m);
 assert(r.rc==-2&&r.direct.restore_rc==-2&&!r.id_read&&m.writes==1);
 puts("PASS: Flash ID byte order, SRAM bypass, reset sequence, every read/write fault (host mock only)");
}
