#include "../core/save/eeprom139.h"
#include <assert.h>
#include <stdio.h>
struct mock {unsigned calls,fail_at,release_count,mode,corrupt,dead_at,cleanup_fail;};
static uint8_t small[512],large[8192];
static int live(void*x){struct mock*m=x;return !m->dead_at||m->calls<m->dead_at;}
static int release(void*x){struct mock*m=x;m->release_count++;return m->cleanup_fail?-8:0;}
static int read_block(void*x,unsigned bits,unsigned b,uint8_t*out){
 struct mock*m=x;assert((bits==6&&b<64)||(bits==14&&b<1024));m->calls++;
 if(m->calls==m->fail_at)return -7;
 for(unsigned i=0;i<8;i++)out[i]=m->mode==2?255:m->mode==3?0:m->mode==4?0x5a:(uint8_t)(b*8+i+1+(m->mode==1&&bits==6?3:0));
 if((m->corrupt&1)&&m->calls==65)out[0]^=1;
 if((m->corrupt&2)&&bits==14&&b==0&&m->calls>1024)out[0]^=1;
 return 0;
}
static struct cb_eeprom139_report run(struct mock*m){struct cb_eeprom139_ops o={m,read_block,live,release};return cb_eeprom139_probe(&o,small,sizeof(small),large,sizeof(large));}
int main(void){
 struct mock m={0};struct cb_eeprom139_report r=run(&m);
 assert(r.state==CB_EEPROM139_8K&&r.prefix_equal&&m.calls==2176&&m.release_count==1);
 m=(struct mock){.mode=1};r=run(&m);assert(r.state==CB_EEPROM139_512&&!r.prefix_equal);
 for(unsigned mode=2;mode<=3;mode++){m=(struct mock){.mode=mode};r=run(&m);assert(r.state==CB_EEPROM139_BLANK);}
 m=(struct mock){.corrupt=1};r=run(&m);assert(r.state==CB_EEPROM139_UNSTABLE&&r.first_difference==0&&m.release_count==1);
 assert(r.address_bits==6&&r.pass==2&&r.block==0&&r.stage==4);
 for(unsigned i=1;i<=2176;i++){
  m=(struct mock){.fail_at=i};r=run(&m);assert(r.rc==-7&&m.calls==i&&m.release_count==1);
  unsigned k=i-1,blocks=k<128?64:1024;
  assert(r.address_bits==(k<128?6u:14u));if(k>=128)k-=128;
  assert(r.pass==k/blocks+1&&r.block==k%blocks&&r.stage==2);
 }
 m=(struct mock){.dead_at=1};r=run(&m);assert(r.rc==-2&&m.calls==1&&m.release_count==1);
 m=(struct mock){.cleanup_fail=1};r=run(&m);assert(r.cleanup_rc==-8&&r.state==CB_EEPROM139_UNKNOWN);
 struct cb_eeprom139_ops o={&m,read_block,live,release};
 assert(cb_eeprom139_probe(&o,large,512,large,8192).rc==-1);
 assert(cb_eeprom139_probe(&o,small,511,large,8192).rc==-1);
 for(unsigned mask=0;mask<4;mask++){
  m=(struct mock){.corrupt=mask};
  r=cb_eeprom139_capture(&o,small,512,large,8192);
  assert(!r.rc&&!r.cleanup_rc&&r.mismatch_mask==mask&&r.stable_mask==(3^mask));
  assert(m.release_count==1);
  if(mask)assert(r.state==CB_EEPROM139_UNSTABLE);
 }
 /* After the small mismatch: first large read follows call 65. */
 for(unsigned i=66;i<=2113;i++){
  m=(struct mock){.corrupt=1,.fail_at=i};
  r=cb_eeprom139_capture(&o,small,512,large,8192);
  assert(r.rc==-7&&r.stage==2&&m.calls==i&&m.release_count==1);
 }
 m=(struct mock){.corrupt=1,.dead_at=65};
 r=cb_eeprom139_capture(&o,small,512,large,8192);
 assert(r.rc==-2&&m.calls==65&&m.release_count==1);
 m=(struct mock){.corrupt=1,.cleanup_fail=1};
 r=cb_eeprom139_capture(&o,small,512,large,8192);
 assert(r.cleanup_rc==-8&&m.release_count==1);
 r=(struct cb_eeprom139_report){.stable_mask=2,.mismatch_mask=1};
 for(unsigned i=0;i<8192;i++)large[i]=(uint8_t)(i/8+i/256);
 for(unsigned i=0;i<512;i++)small[i]=large[i];
 assert(cb_eeprom139_estimate(&r,small,large)==CB_EEPROM139_8K);
 r.stable_mask=1;r.mismatch_mask=2;
 assert(cb_eeprom139_estimate(&r,small,large)==CB_EEPROM139_512);
 r.stable_mask=3;r.mismatch_mask=0;
 assert(cb_eeprom139_estimate(&r,small,large)==CB_EEPROM139_8K);
 small[0]^=1;assert(cb_eeprom139_estimate(&r,small,large)==CB_EEPROM139_512);
 r.rc=-3;assert(cb_eeprom139_estimate(&r,small,large)==CB_EEPROM139_UNKNOWN);r.rc=0;
 r.cleanup_rc=-8;assert(cb_eeprom139_estimate(&r,small,large)==CB_EEPROM139_UNKNOWN);r.cleanup_rc=0;
 r.stable_mask=2;r.mismatch_mask=1;
 for(unsigned i=0;i<8192;i++)large[i]=small[i%512];
 assert(cb_eeprom139_estimate(&r,small,large)==CB_EEPROM139_UNKNOWN);
 for(unsigned i=0;i<8192;i++)large[i]=255;
 assert(cb_eeprom139_estimate(&r,small,large)==CB_EEPROM139_UNKNOWN);
 r.stable_mask=2;r.mismatch_mask=0;
 assert(cb_eeprom139_estimate(&r,small,large)==CB_EEPROM139_UNKNOWN);
 for(unsigned mode=0;mode<5;mode++){
  m=(struct mock){.mode=mode};r=cb_eeprom139_detect(&o,small,512,large,8192);
  assert(!r.rc&&!r.cleanup_rc&&m.calls==1088&&m.release_count==1);
  assert(r.state==((mode==2||mode==3)?CB_EEPROM139_BLANK:mode==1?CB_EEPROM139_512:CB_EEPROM139_8K));
 }
 for(unsigned i=1;i<=1088;i++){
  m=(struct mock){.fail_at=i};r=cb_eeprom139_detect(&o,small,512,large,8192);
  assert(r.rc==-7&&m.calls==i&&m.release_count==1);
 }
 puts("EEPROM139 PASS: legacy capture and single-pass detection, all read failures, lifecycle/release; host mock only");
}
