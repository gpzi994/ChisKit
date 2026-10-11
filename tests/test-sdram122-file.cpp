#include "../core/sdram/file122.h"
#include <cassert>
#include <cstdio>
#include <vector>
struct Model {
 bool owned=false,snapshot=false,hang=false;
 int fail=0,transfers=0; unsigned signature=CB_SD116_SIGNATURE,fault_after=0;
 unsigned address=0,bytes=0; std::vector<int> commands;
 static unsigned read(void *v,unsigned off) {
  auto &m=*(Model*)v;
  if(off==0) return m.signature;
  if(m.fault_after&&m.commands.size()==m.fault_after) {
   if(off==24)return 4;
   if(off==4)return CB_SD116_LOCKED;
  }
  if(off==24) return m.owned?2:1;
  if(off==4) return CB_SD116_TRANSPORT_READY|CB_SD116_AXI_IDLE|
   (m.owned?(CB_SD116_HOST_QUIET|CB_SD116_INGRESS_DRAINED):0)|
   (m.snapshot?CB_SD116_SNAPSHOT:CB_SD116_IDLE);
  assert(false); return 0;
 }
 static void write(void *v,unsigned off,unsigned value) {
  auto &m=*(Model*)v;
  if(off==20) {m.commands.push_back(value);if(!m.hang)m.owned=value==1;}
  else if(off==8)m.address=value;
  else if(off==12)m.bytes=value;
  else if(off==16) {
   assert(m.owned); m.commands.push_back(value==1?3:4);
   m.snapshot=value==1;
  } else assert(false);
 }
 static int transfer(void *v,int wr,unsigned,unsigned,unsigned a,unsigned n) {
  auto &m=*(Model*)v;assert(!m.owned);m.transfers++;
  if(wr){assert(m.snapshot);assert(a==0x21000000u);assert(n==m.bytes);}
  else {assert(!m.snapshot);assert(a==0x10380000u);}
  m.commands.push_back(5);return m.fail;
 }
 cb_sd122_io io(){return {{this,read,write},transfer};}
};
int main() {
 unsigned cases=0;
 // Inject every write-side driver failure; the phase must name the exact
 // failing operation and no recovery/release may run after failure.
 for(unsigned step: {1u,2u,3u,5u,6u,7u}) {
  Model m;auto io=m.io();m.fault_after=step;unsigned phase=99;
  assert(cb_sd122_transfer_trace(&io,1,7,0,0x10380000u,32768,10,&phase)==CB_SD117_FAULT);
  assert(phase==step&&m.commands.size()==step);
 }
 {Model m;auto io=m.io();m.fail=-99;unsigned phase=99;
  assert(cb_sd122_transfer_trace(&io,1,7,0,0x10380000u,32768,10,&phase)==-99);
  assert(phase==CB_SD122_APF&&m.commands.size()==4);}
 {Model m;auto io=m.io();unsigned phase=99;
  assert(cb_sd122_transfer_trace(&io,1,7,0,0x10380000u,32768,10,&phase)==0);
  assert(phase==CB_SD122_DONE);}
 for(unsigned n: {1u,3u,4u,63u,64u,65u,264u,32767u,32768u}) {
  Model m;auto io=m.io();
  for(int round=0;round<20;round++)for(int wr=0;wr<2;wr++) {
   m.commands.clear();
   assert(cb_sd122_transfer(&io,wr,7,round*32768u,0x10380000u,n,10)==0);
   assert(!m.owned&&!m.snapshot);
   assert(m.commands==(wr?std::vector<int>{1,3,2,5,1,4,2}:
                          std::vector<int>{1,2,5,1,2}));cases++;
  }
 }
 {Model m;auto io=m.io();m.signature=0;
  assert(cb_sd122_transfer(&io,1,7,0,0x10380000u,4,10)==CB_SD117_UNAVAILABLE);
  assert(m.commands.empty());}
 {Model m;auto io=m.io();m.hang=true;
  assert(cb_sd122_transfer(&io,1,7,0,0x10380000u,4,10)==CB_SD117_TIMEOUT);
  assert(m.transfers==0&&m.commands==std::vector<int>{1});}
 {Model m;auto io=m.io();m.fail=-99;
  assert(cb_sd122_transfer(&io,1,7,0,0x10380000u,4,10)==-99);
  assert(m.snapshot&&!m.owned&&m.commands==std::vector<int>({1,3,2,5}));}
 {Model m;auto io=m.io();
  for(unsigned n:{0u,32769u,0xffffffffu})
   assert(cb_sd122_transfer(&io,0,7,0,0x10380000u,n,10)==CB_SD117_RANGE);
  assert(cb_sd122_transfer(&io,0,7,0,0x10380004u,4,10)==CB_SD117_RANGE);
  assert(cb_sd122_transfer(&io,0,7,UINT32_MAX,0x10380000u,4,10)==CB_SD117_RANGE);
  assert(m.commands.empty());}
 printf("SDRAM122 PASS: %u ordered transfers; range, signature, timeout and host-failure guards\n",cases);
}
