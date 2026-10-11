// SPDX-License-Identifier: Apache-2.0
#include "Vchisburner_sdram_pocket120.h"
#include "verilated.h"
#include "../core/sdram/driver117.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>
struct Sim {
 Vchisburner_sdram_pocket120 d;
 unsigned t=0,ch,bh,addr=0,delay=0,writes=0,reads=0;
 bool pending=false,stall=false,error=false,wrong_signature=false;
 cb_sd117_io io{this,read,write};
 static unsigned word(unsigned a){return (a*37u)^0xa56c9173u;}
 Sim(unsigned c,unsigned b):ch(c),bh(b){reset();}
 void tick(){
  unsigned nc=((t+1)/ch)&1,nb=((t+1)/bh)&1;
  bool cr=!d.cpu_clk&&nc;
  d.arready=!pending&&!stall;d.rvalid=pending&&!delay&&!stall;
  d.rdata=word(addr);d.rresp=error?2:0;d.rlast=1;d.eval();
  bool ar=cr&&d.arvalid&&d.arready,r=cr&&d.rvalid&&d.rready;
  unsigned a=d.araddr;
  d.cpu_clk=nc;d.bridge_clk=nb;d.eval();++t;
  if(cr&&delay)--delay;
  if(r)pending=false;
  if(ar){pending=true;addr=a;delay=2+(reads++%5);}
 }
 void cycle(){do{tick();}while(!d.cpu_clk);do{tick();}while(d.cpu_clk);}
 void reset(){
  d.power_reset_n=0;d.reg_write=d.selected=d.bridge_rd=0;
  d.host_busy=d.legacy_busy=0;d.bridge_wr=0;d.awready=d.wready=1;d.bvalid=0;d.bresp=0;
  pending=stall=error=wrong_signature=false;delay=0;
  for(unsigned i=0;i<10*(ch+bh);i++)tick();
  d.power_reset_n=1;for(int i=0;i<5;i++)cycle();
 }
 static uint32_t read(void *v,uint32_t offset){
  auto &s=*static_cast<Sim*>(v);s.d.selected=1;s.d.reg_offset=offset;
  s.cycle();s.d.eval();return offset==0&&s.wrong_signature?0:s.d.reg_rdata;
 }
 static void write(void *v,uint32_t offset,uint32_t data){
  auto &s=*static_cast<Sim*>(v);s.d.selected=1;s.d.reg_write=1;
  s.d.reg_offset=offset;s.d.reg_wdata=data;s.d.reg_wstrb=15;
  s.cycle();s.d.reg_write=0;s.d.eval();++s.writes;
 }
 unsigned hostread(unsigned offset){
  d.bridge_rd=0;for(unsigned i=0;i<3*bh;i++)tick();d.bridge_addr=0x21000000+offset;d.bridge_rd=1;
  // Establish low bridge phase first; repeated reads are permitted.
  while(d.bridge_clk)tick();
  while(!d.bridge_clk)tick();
  assert(d.bridge_rvalid);unsigned result=d.bridge_rdata;
  d.bridge_rd=0;return result;
 }
};
int main(){
 unsigned cases=0;
 for(unsigned ratio=0;ratio<3;ratio++){
  Sim s(ratio==0?5:ratio==1?11:2,ratio==0?7:ratio==1?3:13);
  auto acquire=[&](){unsigned budget=10000;while(!(Sim::read(&s,24)&1)&&--budget){}assert(budget);Sim::write(&s,20,1);budget=10000;while(!(Sim::read(&s,24)&2)&&--budget){}if(!budget)fprintf(stderr,"acquire failed ratio=%u case=%u lock=%x ctl=%x\n",ratio,cases,Sim::read(&s,24),Sim::read(&s,4));assert(budget);};
  auto release=[&](){Sim::write(&s,20,2);unsigned budget=10000;while(!(Sim::read(&s,24)&1)&&--budget){}assert(budget);};
  for(unsigned n: {1u,4u,264u,32768u}){
   acquire();assert(s.d.admission_blocked);
   assert(cb_sd117_prepare(&s.io,0x10004000,n,200000)==0);
   release();
   s.d.host_busy=1;
   for(unsigned offset=0;offset<n;offset+=4){
    unsigned expected=Sim::word(0x10004000+offset);
    if(n-offset<4)expected&=(1u<<(8*(n-offset)))-1;
    assert(s.hostread(offset)==expected);
   }
   // APF returns the previous word while requesting this final flush.
   assert(s.hostread((n+3)&~3u)==0);
   s.d.host_busy=0;
   acquire();assert(cb_sd117_release(&s.io,1000)==0);release();++cases;
  }
  auto diag=[&](unsigned page){Sim::write(&s,28,page);return Sim::read(&s,28);};
  assert(diag(3)==0x53443124u&&diag(0)==0);
  // Tail access cannot be used to bypass readiness, final-word ordering,
  // alignment, or the one-word bound.
  for(unsigned kind=0;kind<4;kind++) {
   s.reset();
   if(kind!=0){acquire();assert(cb_sd117_prepare(&s.io,0x10004000,4,200000)==0);release();}
   s.d.host_busy=1;
   if(kind>=2)assert(s.hostread(0)==Sim::word(0x10004000));
   s.d.bridge_rd=0;for(unsigned i=0;i<4*s.bh;i++)s.tick();
   s.d.bridge_addr=0x21000000+(kind==2?8:kind==3?5:4);s.d.bridge_rd=1;
   for(unsigned i=0;i<4*s.bh;i++)s.tick();s.d.bridge_rd=0;
   for(unsigned i=0;i<100*(s.bh+s.ch);i++)s.tick();
   assert(diag(0)==0x80000002u);
   assert(Sim::read(&s,24)&4);
  }
  // A second tail request remains a real fault; the first is a zero flush.
  for(unsigned length: {4u,32768u}) {
  s.reset();acquire();assert(cb_sd117_prepare(&s.io,0x10004000,length,200000)==0);release();
  s.d.host_busy=1;
  for(unsigned off=0;off<length;off+=4)assert(s.hostread(off)==Sim::word(0x10004000+off));
  assert(s.hostread(length)==0);
  s.d.bridge_rd=0;for(unsigned i=0;i<4*s.bh;i++)s.tick();
  s.d.bridge_addr=0x21000000+length;s.d.bridge_rd=1;
  for(unsigned i=0;i<4*s.bh;i++)s.tick();s.d.bridge_rd=0;
  for(unsigned i=0;i<100*(s.bh+s.ch);i++)s.tick();
  assert(diag(0)==0x80000002u&&diag(1)==0x21000000u+length&&diag(2)==length/4+2);
  assert(Sim::read(&s,24)&4);
  s.d.bridge_addr=0x21009999;s.d.legacy_busy=1;
  for(int i=0;i<50;i++)s.cycle();
  assert(diag(0)==0x80000002u&&diag(1)==0x21000000u+length&&diag(2)==length/4+2);
  }
  // Independently distinguish host, legacy and late-read ownership violations.
  for(unsigned kind=0;kind<3;kind++) {
   s.reset();acquire();
   s.d.bridge_addr=0x21000000;
   if(kind==0)s.d.host_busy=1;
   if(kind==1)s.d.legacy_busy=1;
   if(kind==2)s.d.bridge_rd=1;
   for(unsigned i=0;i<100*(s.bh+s.ch);i++)s.tick();
   assert(diag(0)==(0x80000000u|(4u<<kind)));
   assert(Sim::read(&s,24)&4);
  }
  // The two ingress-related causes remain distinct as well.
  s.reset();s.d.bridge_addr=0x10000001;s.d.bridge_wr=1;
  for(unsigned i=0;i<4*s.bh;i++)s.tick();s.d.bridge_wr=0;
  for(unsigned i=0;i<100*(s.bh+s.ch);i++)s.tick();
  assert(diag(0)==0x80000001u);
  s.reset();acquire();s.d.bridge_addr=0x10000000;s.d.bridge_wr=1;
  for(unsigned i=0;i<2*s.bh;i++)s.tick();s.d.bridge_wr=0;
  for(unsigned i=0;i<100*(s.bh+s.ch);i++)s.tick();
  assert(diag(0)==0x80000020u);
 }
 printf("PASS SDRAM125 candidate: zero tail flush, repeated/unarmed/unaligned/beyond-tail rejection; all six diagnostic causes across three clock ratios.\n");
 printf("PASS SDRAM120 integrated wiring: %u snapshots, 3 clock ratios, acquire/prepare/unlock/host-read/reacquire/release. AXI read model; not physical SDRAM/APF system.\n",cases);
}
