#include "Vchisburner_nor_bus108.h"
#include <cassert>
#include <cstdio>
static Vchisburner_nor_bus108 d;
static void tick(){d.clk=0;d.eval();d.clk=1;d.eval();}
static void wr(unsigned a,unsigned v){d.reg_wr=1;d.reg_wr_addr=a;d.reg_wdata=v;tick();d.reg_wr=0;d.eval();}
static unsigned rr(unsigned a){d.reg_addr=a;d.eval();return d.reg_rdata;}
static void start(unsigned bus,bool armed=true,unsigned timeout=10000){
 wr(4,bus);wr(7,timeout);if(armed){wr(19,0x43424e31);wr(19,0x43424e32);}wr(3,8);
}
static void safe(){assert((d.bank0_out&7)==7&&!d.bank1_dir&&!d.bank2_dir&&!d.bank3_dir&&!d.pin30_dir&&!d.write_active);}
int main(){
 d.reset_n=0;d.selected=1;d.lifecycle_ok=1;tick();d.reset_n=1;tick();
 assert(rr(1)&0x2000);
 start(12);unsigned low=0,high=0,fall=0,rise=0,last=1;
 for(unsigned n=0;d.busy&&n<7000;n++){
  safe();assert(d.pin30_pwroff_reset);unsigned v=(d.bank0_out>>3)&1;
  if(v!=last){if(v)rise++;else fall++;last=v;}if(v)high++;else low++;tick();
 }
 assert(!d.busy&&low==3200&&high==3200&&fall==50&&rise==50&&d.bank0_out==15&&rr(10)==0);
 start(12,false);assert(!d.busy);safe();
 for(unsigned b=0;b<12;b++){start(b);assert(!d.busy&&(d.bank0_out&8));safe();}
 for(unsigned mode=0;mode<4;mode++)for(unsigned n=0;n<6400;n+=37){
  start(12);for(unsigned i=0;i<n;i++)tick();
  if(mode==0)d.lifecycle_ok=0;else if(mode==1)d.selected=0;else if(mode==2)d.reset_n=0;else wr(3,3);
  tick();safe();assert(!d.busy&&d.bank0_out==15);
  d.lifecycle_ok=1;d.selected=1;d.reset_n=1;tick();
 }
 start(12,true,20);for(unsigned i=0;i<30;i++)tick();assert(!d.busy&&rr(10)==4);safe();
 puts("GB191 clock PASS: 50 pulses, 64-cycle halves, no RD/WR/CS/data drive, other buses denied, 692 reset/removal/abort points, timeout, one-shot arm");
}
