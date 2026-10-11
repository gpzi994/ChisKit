// SPDX-License-Identifier: Apache-2.0
#include "Vchisburner_sdram_read117.h"
#include "verilated.h"
#include "../core/sdram/driver117.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>
struct Sim {
 Vchisburner_sdram_read117 d;
 unsigned t=0,ch,bh,addr=0,delay=0,writes=0,reads=0;
 bool pending=false,stall=false,error=false,wrong_signature=false;
 cb_sd117_io io{this,read,write};
 static unsigned word(unsigned a){return (a*37u)^0xa56c9173u;}
 Sim(unsigned c,unsigned b):ch(c),bh(b){reset();}
 void tick(){
  unsigned nc=((t+1)/ch)&1,nb=((t+1)/bh)&1;
  bool cr=!d.cpu_clk&&nc;
  d.m_arready=!pending&&!stall;d.m_rvalid=pending&&!delay&&!stall;
  d.m_rdata=word(addr);d.m_rresp=error?2:0;d.m_rlast=1;d.eval();
  bool ar=cr&&d.m_arvalid&&d.m_arready,r=cr&&d.m_rvalid&&d.m_rready;
  unsigned a=d.m_araddr;
  d.cpu_clk=nc;d.bridge_clk=nb;d.eval();++t;
  if(cr&&delay)--delay;
  if(r)pending=false;
  if(ar){pending=true;addr=a;delay=2+(reads++%5);}
 }
 void cycle(){do{tick();}while(!d.cpu_clk);do{tick();}while(d.cpu_clk);}
 void reset(){
  d.cpu_reset_n=d.bridge_reset_n=0;d.reg_write=d.selected=d.bridge_read=0;
  d.host_quiet=d.axi_idle=d.ingress_drained=1;d.ingress_fault=0;
  pending=stall=error=wrong_signature=false;delay=0;
  for(unsigned i=0;i<10*(ch+bh);i++)tick();
  d.cpu_reset_n=d.bridge_reset_n=1;for(int i=0;i<5;i++)cycle();
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
  d.bridge_offset=offset;d.bridge_read=1;
  // Establish low bridge phase first; repeated reads are permitted.
  while(d.bridge_clk)tick();
  while(!d.bridge_clk)tick();
  assert(d.bridge_valid);unsigned result=d.bridge_data;
  d.bridge_read=0;return result;
 }
};
int main(){
 unsigned cases=0;
 for(unsigned ratio=0;ratio<3;ratio++){
  Sim s(ratio==0?5:ratio==1?11:2,ratio==0?7:ratio==1?3:13);
  for(unsigned n: {1u,2u,3u,4u,264u,32767u,32768u}){
   unsigned base=0x10004000;
   assert(cb_sd117_prepare(&s.io,base,n,200000)==0);
   assert(s.d.bridge_ready&&!s.d.locked);
   s.d.host_quiet=0;
   unsigned w=s.writes;
   assert(cb_sd117_release(&s.io,100)==CB_SD117_BUSY&&w==s.writes);
   for(unsigned pass=0;pass<2;pass++)for(unsigned i=0;i<n;i+=4){
    unsigned expect=Sim::word(base+i);
    if(n-i<4)expect&=(1u<<(8*(n-i)))-1;
    assert(s.hostread(i)==expect);
   }
   s.d.host_quiet=1;
   assert(cb_sd117_release(&s.io,1000)==0);
   assert(!s.d.bridge_ready&&s.d.idle);++cases;
  }
  unsigned w=s.writes;
  assert(cb_sd117_prepare(&s.io,0x13fffffc,5,100)==CB_SD117_RANGE);
  assert(cb_sd117_prepare(&s.io,0x30000000,4,100)==CB_SD117_RANGE);
  assert(w==s.writes);
  s.wrong_signature=true;
  assert(cb_sd117_prepare(&s.io,0x10000000,4,100)==CB_SD117_UNAVAILABLE);
  assert(w==s.writes);s.wrong_signature=false;
  s.stall=true;
  assert(cb_sd117_prepare(&s.io,0x10000000,4,30)==CB_SD117_TIMEOUT);
  w=s.writes;
  assert(cb_sd117_release(&s.io,30)==CB_SD117_BUSY&&w==s.writes);
  s.reset();s.error=true;
  assert(cb_sd117_prepare(&s.io,0x10000000,4,1000)==CB_SD117_FAULT);
  w=s.writes;
  assert(cb_sd117_release(&s.io,30)==CB_SD117_FAULT&&w==s.writes);
 }
 printf("PASS SDRAM117: C driver + control + dual-clock read RTL, %u snapshots, 3 clock ratios, exact tails, busy host, timeout, AXI fault. No live OS/APF or silicon claim.\n",cases);
}
