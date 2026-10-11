// SPDX-License-Identifier: Apache-2.0
// Two unrelated clocks and an independent single-beat AXI response model.
// Does not model SDRAM electrical timing or APF firmware behavior.
#include "Vchisburner_sdram_ingress113.h"
#include "verilated.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <utility>
#ifndef FIFO_WORDS
#define FIFO_WORDS 16
#endif
struct Sim {
 Vchisburner_sdram_ingress113 d;
 uint64_t t=0;unsigned sh,dh;uint32_t rng=0x113;
 std::deque<std::pair<uint32_t,uint32_t>> expected;
 bool aw=false,w=false,b=false,stall=false,error_response=false,hold_response=false;
 bool held_aw=false,held_w=false;
 uint32_t aa=0,wd=0,old_aa=0,old_wd=0;
 unsigned delay=0,accepted=0,completed=0;
 uint32_t random(){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
 explicit Sim(unsigned s,unsigned c):sh(s),dh(c){reset();}
 void reset(){
  d.src_reset_n=d.dst_reset_n=0;d.src_valid=0;
  expected.clear();aw=w=b=held_aw=held_w=false;delay=0;
  accepted=completed=0;stall=error_response=hold_response=false;
  for(unsigned i=0;i<10*(sh+dh);i++)tick();
  d.src_reset_n=d.dst_reset_n=1;d.eval();
  assert(d.src_drained&&!d.src_fault);
 }
 bool tick(){
  d.m_awready=!stall&&((random()&7)!=0);
  d.m_wready=!stall&&((random()&3)==0);
  d.m_bvalid=b&&!hold_response;d.m_bresp=error_response?2:0;
  d.eval();
  bool ns=((t+1)/sh)&1,nd=((t+1)/dh)&1;
  bool se=ns&&!d.src_clk,de=nd&&!d.dst_clk;
  if(se&&d.src_reset_n&&d.src_valid&&d.src_ready&&
     d.src_address>=0x10000000u&&d.src_address<0x14000000u&&!(d.src_address&3)){
   expected.emplace_back(d.src_address,d.src_data);accepted++;
  }
  if(de&&d.dst_reset_n){
   if(held_aw)assert(d.m_awvalid&&d.m_awaddr==old_aa);
   if(held_w)assert(d.m_wvalid&&d.m_wdata==old_wd&&d.m_wstrb==15&&d.m_wlast);
   held_aw=d.m_awvalid&&!d.m_awready;old_aa=d.m_awaddr;
   held_w=d.m_wvalid&&!d.m_wready;old_wd=d.m_wdata;
   if(d.m_awvalid&&d.m_awready){assert(!aw&&d.m_awlen==0);aw=true;aa=d.m_awaddr;}
   if(d.m_wvalid&&d.m_wready){assert(!w&&d.m_wlast&&d.m_wstrb==15);w=true;wd=d.m_wdata;}
   if(d.m_bvalid&&d.m_bready){
    assert(aw&&w&&!expected.empty());
    assert(expected.front().first==aa&&expected.front().second==wd);
    expected.pop_front();completed++;aw=w=b=false;delay=0;
   }else if(aw&&w&&!b){if(++delay>3+(rng&7))b=true;}
  }
  d.src_clk=ns;d.dst_clk=nd;t++;d.eval();
  if(d.src_reset_n&&!expected.empty())assert(!d.src_drained);
  return se;
 }
 void send(uint32_t a,uint32_t data,bool obey=true){
  if(obey){for(unsigned n=0;!d.src_ready;n++){assert(n<100000&&!d.src_fault);tick();}}
  d.src_address=a;d.src_data=data;d.src_valid=1;
  while(!tick()){}
  d.src_valid=0;d.eval();
 }
 void drain(){
  for(unsigned n=0;!d.src_drained;n++){assert(n<2000000&&!d.src_fault);tick();}
  assert(expected.empty()&&completed==accepted&&!d.src_fault);
 }
};
int main(int argc,char**argv){
 Verilated::commandArgs(argc,argv);
 for(auto clocks:{std::pair<unsigned,unsigned>{7,5},{3,11},{13,2}}){
  Sim s(clocks.first,clocks.second);
  for(unsigned n=0;n<20000;n++){
   uint32_t a=n==0?0x10000000:n==1?0x13fffffc:0x10320000+4*n;
   s.send(a,s.random());if(n%127==0)s.drain();
  }
  s.drain();assert(s.completed==20000);
  // Final completion must wait for B, even after AW/W both accepted.
  s.reset();s.stall=true;s.send(0x10400000,0x01234567);
  for(unsigned n=0;n<500;n++)s.tick();
  assert(!s.d.src_drained&&s.completed==0&&!s.d.src_fault);
  s.stall=false;s.drain();
  s.reset();s.hold_response=true;s.send(0x10400000,0x76543210);
  for(unsigned n=0;n<1000;n++)s.tick();
  assert(s.aw&&s.w&&s.b&&s.completed==0&&!s.d.src_drained);
  s.hold_response=false;s.drain();
  // APF-like producer ignores ready: fault must stick, never silently pass.
  s.reset();s.stall=true;
  for(unsigned n=0;n<FIFO_WORDS+8;n++)s.send(0x10400000+4*n,n,false);
  assert(s.d.src_fault&&!s.d.src_ready&&!s.d.src_drained);
  s.stall=false;for(unsigned n=0;n<500;n++)s.tick();assert(s.d.src_fault);
  // Both domains reset together: all stale queued words are discarded.
  s.reset();s.send(0x13fffffc,0xabcdef01);s.drain();assert(s.completed==1);
  for(uint32_t bad:{0x0ffffffcu,0x14000000u,0x30000000u,0x10400001u}){
   s.reset();s.send(bad,0);assert(s.d.src_fault&&s.accepted==0);
   for(unsigned n=0;n<100;n++)s.tick();assert(s.completed==0);
  }
  s.reset();s.error_response=true;s.send(0x10400000,0xff00aa55);
  for(unsigned n=0;n<2000&&!s.d.src_fault;n++)s.tick();
  assert(s.d.src_fault&&s.completed==1&&!s.d.src_ready);
  s.reset();s.stall=true;s.send(0x10400000,0xdeadbeef);
  for(unsigned n=0;n<100;n++)s.tick();
  s.reset();s.send(0x10400000,0xaabbccdd);s.drain();assert(s.completed==1);
 }
 puts("PASS SDRAM113 ingress: 60000 words, 3 clock ratios, FIFO wrap, AW/W skew, B completion, overflow, bad addresses, AXI error, reset. RTL only; not Pocket/SDRAM silicon validation.");
}
