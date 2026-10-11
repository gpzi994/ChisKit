// SPDX-License-Identifier: Apache-2.0
#include "Vchisburner_sdram_egress114.h"
#include "verilated.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <utility>
struct Sim {
 Vchisburner_sdram_egress114 d;
 uint64_t time=0;unsigned ch,bh,reads=0,delay=0;
 uint32_t rng=0x114,seed=1,address=0,last_addr=0;
 bool pending=false,held=false,stall=false,bad_resp=false,bad_last=false,extra=false;
 uint32_t random(){rng=rng*1664525u+1013904223u;return rng;}
 static uint8_t byte(uint32_t a,uint32_t s){return (uint8_t)((a*37u)^(a>>8)^(a>>17)^(s*71u));}
 static uint32_t word(uint32_t a,uint32_t s){uint32_t v=0;for(unsigned i=0;i<4;i++)v|=(uint32_t)byte(a+i,s)<<(8*i);return v;}
 Sim(unsigned c,unsigned b):ch(c),bh(b){reset();}
 void reset(){
  d.cpu_reset_n=d.bridge_reset_n=0;d.cmd_prepare=d.cmd_release=d.bridge_read=0;
  pending=held=stall=bad_resp=bad_last=extra=false;reads=delay=0;
  for(unsigned n=0;n<10*(ch+bh);n++)tick();
  d.cpu_reset_n=d.bridge_reset_n=1;d.eval();assert(d.cmd_ready&&!d.cmd_fault&&!d.bridge_ready);
 }
 std::pair<bool,bool> tick(){
  d.m_arready=!stall&&((random()>>24)&3)!=0;
  d.m_rvalid=pending&&delay==0&&!stall;
  d.m_rdata=word(address,seed);d.m_rresp=bad_resp?2:0;d.m_rlast=!bad_last||extra;
  d.eval();
  bool nc=((time+1)/ch)&1,nb=((time+1)/bh)&1;
  bool ce=nc&&!d.cpu_clk,be=nb&&!d.bridge_clk;
  if(ce&&d.cpu_reset_n){
   if(held)assert(d.m_arvalid&&d.m_araddr==last_addr&&d.m_arlen==0);
   held=d.m_arvalid&&!d.m_arready;last_addr=d.m_araddr;
   if(d.m_arvalid&&d.m_arready){assert(!pending&&d.m_arlen==0);pending=true;address=d.m_araddr;delay=1+((random()>>24)&7);reads++;}
   else if(d.m_rvalid&&d.m_rready){
    if(!d.m_rlast){extra=true;delay=3;}else pending=false;
   }else if(pending&&delay)delay--;
  }
  d.cpu_clk=nc;d.bridge_clk=nb;time++;d.eval();return {ce,be};
 }
 void prepare(uint32_t a,uint32_t n){
  assert(d.cmd_ready);d.cmd_address=a;d.cmd_bytes=n;d.cmd_prepare=1;
  while(!tick().first){}d.cmd_prepare=0;d.eval();
 }
 void complete(){for(unsigned i=0;!d.cmd_complete;i++){assert(i<5000000&&!d.cmd_fault);tick();}assert(d.bridge_ready);}
 uint32_t read(uint32_t off,bool valid=true){
  d.bridge_offset=off;d.bridge_read=1;while(!tick().second){}
  assert(!!d.bridge_valid==valid);uint32_t v=d.bridge_data;d.bridge_read=0;d.eval();return v;
 }
 void release(){
  assert(d.cmd_complete);d.cmd_release=1;while(!tick().first){}d.cmd_release=0;d.eval();
  for(unsigned i=0;!d.cmd_ready;i++){assert(i<1000);tick();}assert(!d.bridge_ready);
 }
 void check(uint32_t a,uint32_t n,uint32_t snapshot_seed){
  for(uint32_t off=0;off<n;off+=4){
   uint32_t expected=word(a+off,snapshot_seed);
   if(n-off<4)expected&=(1u<<(8*(n-off)))-1;
   assert(read(off)==expected);
  }
 }
};
int main(int argc,char**argv){
 Verilated::commandArgs(argc,argv);
 unsigned snapshots=0;
 for(auto clocks:{std::pair<unsigned,unsigned>{5,7},{11,3},{2,13}}){
  Sim s(clocks.first,clocks.second);
  // Two full chunks are a 64 KiB file write, with repeated snapshot reuse.
  for(unsigned n:{1u,2u,3u,4u,31u,264u,32767u,32768u,32768u}){
   uint32_t a=0x10320000u+(snapshots&1)*32768u,seed=++s.seed;
   unsigned before=s.reads;s.prepare(a,n);assert(!s.d.cmd_complete);
   s.complete();assert(s.reads-before==(n+3)/4);
   s.seed++; // SDRAM changes after snapshot: APF still reads the same block.
   s.check(a,n,seed);s.check(a,n,seed); // host may reread, not consume a FIFO
   s.release();snapshots++;
  }
  s.prepare(0x13fffffc,4);s.complete();s.check(0x13fffffc,4,s.seed);s.release();
  // Stalled SDRAM does not publish an incomplete block.
  s.stall=true;s.prepare(0x10400000,32);
  for(unsigned n=0;n<1000;n++)s.tick();assert(!s.d.cmd_complete&&!s.d.bridge_ready);
  // A second command must not replace the in-flight source/range.
  s.d.cmd_prepare=1;s.d.cmd_address=0x10500000;
  while(!s.tick().first){}assert(s.d.cmd_rejected);s.d.cmd_prepare=0;
  s.stall=false;s.complete();s.check(0x10400000,32,s.seed);s.release();
  for(auto p:{std::pair<uint32_t,uint32_t>{0x10400000,0},{0x10400000,32769},
              {0x13fffffc,5},{0x10400001,4},{0x30000000,4},{0xfffffffcu,8}}){
   s.reset();s.prepare(p.first,p.second);assert(s.d.cmd_fault&&!s.d.cmd_complete&&s.reads==0);
  }
  for(unsigned kind=0;kind<2;kind++){
   s.reset();s.bad_resp=kind==0;s.bad_last=kind==1;s.prepare(0x10400000,32);
   for(unsigned n=0;n<2000;n++)s.tick();
   assert(s.d.cmd_fault&&!s.d.cmd_complete&&!s.d.bridge_ready&&s.reads==1&&!s.pending);
  }
  for(unsigned off:{1u,32u,32768u,0xfffffffcu}){
   s.reset();s.prepare(0x10400000,32);s.complete();assert(s.read(off,false)==0&&s.d.bridge_fault);
   for(unsigned n=0;n<100;n++)s.tick();assert(s.d.cmd_fault&&!s.d.cmd_complete);
  }
  s.reset();assert(s.read(0,false)==0&&s.d.bridge_fault);
  // Reset in-flight invalidates any old snapshot; a subsequent job is clean.
  s.reset();s.stall=true;s.prepare(0x10400000,32);
  for(unsigned n=0;n<100;n++)s.tick();s.reset();
  s.prepare(0x10400000,264);s.complete();s.check(0x10400000,264,s.seed);s.release();
 }
 printf("PASS SDRAM114 egress: %u varied snapshots, 3 clock ratios, 32 KiB chunks, repeated host reads, partial tails, stalls, command rejection, AXI faults, bounds, reset. RTL only.\n",snapshots);
}
