// SPDX-License-Identifier: Apache-2.0
#include "Vchisburner_sdram_interlock119.h"
#include "verilated.h"
#include <cassert>
#include <cstdio>
#include <cstdint>
struct Sim {
 Vchisburner_sdram_interlock119 d;
 uint64_t t=0;unsigned ch,bh,rng=119,accepted=0,blocked=0;
 unsigned host=0,readtail=0,writetail=0;
 bool busy_hold=false,read_hold=false,write_hold=false,freeze_bridge=false;
 bool auto_jobs=true,check_owned=true;
 Sim(unsigned c,unsigned b):ch(c),bh(b){reset();}
 unsigned random(){rng=rng*1664525u+1013904223u;return rng;}
 void tick(){
  unsigned nc=((t+1)/ch)&1,nb=freeze_bridge?d.bridge_clk:((t+1)/bh)&1;
  bool br=!d.bridge_clk&&nb;
  d.bridge_host_busy=busy_hold||host;
  d.bridge_read_pending=read_hold||readtail;
  d.bridge_writes_drained=!(write_hold||writetail);
  d.eval();
  bool take=br&&d.bridge_accept,deny=br&&d.bridge_new_job&&d.bridge_blocked;
  if(take)assert(!d.bridge_blocked&&!d.cpu_owned);
  d.cpu_clk=nc;d.bridge_clk=nb;d.eval();++t;
  if(br){
   if(host)--host;if(readtail)--readtail;if(writetail)--writetail;
   if(take){++accepted;if(auto_jobs){host=2+random()%15;readtail=host+2;writetail=host+4;}}
   if(deny)++blocked;
  }
  if(check_owned&&d.cpu_owned){
   assert(d.bridge_blocked);assert(!d.bridge_host_busy);
   assert(!d.bridge_read_pending&&d.bridge_writes_drained);
  }
 }
 void steps(unsigned n){while(n--)tick();}
 void cmd(bool acquire,bool release){
  while(d.cpu_clk)tick();d.cpu_acquire=acquire;d.cpu_release=release;
  do{tick();}while(!d.cpu_clk);d.cpu_acquire=d.cpu_release=0;d.eval();
 }
 void reset(){
  d.reset_n=0;d.cpu_acquire=d.cpu_release=d.bridge_new_job=d.bridge_error=0;
  d.cpu_axi_idle=1;busy_hold=read_hold=write_hold=freeze_bridge=false;
  check_owned=true;host=readtail=writetail=0;steps(12*(ch+bh));
  assert(d.bridge_blocked&&!d.cpu_owned);d.reset_n=1;
  steps(12*(ch+bh));assert(d.cpu_idle&&!d.bridge_blocked);
 }
 void owned(){for(unsigned n=0;n<30000&&!d.cpu_owned&&!d.cpu_fault;n++)tick();assert(d.cpu_owned&&!d.cpu_fault);}
 void idle(){for(unsigned n=0;n<30000&&!d.cpu_idle&&!d.cpu_fault;n++)tick();assert(d.cpu_idle&&!d.cpu_fault);}
 void fault(){for(unsigned n=0;n<40000&&!d.cpu_fault;n++)tick();assert(d.cpu_fault&&!d.cpu_owned);steps(12*(ch+bh));assert(d.bridge_blocked);}
};
int main(){
 unsigned rounds=0,total_admit=0,total_block=0;
 for(unsigned ratio=0;ratio<3;ratio++){
  Sim s(ratio==0?5:ratio==1?11:2,ratio==0?7:ratio==1?3:13);
  // Continuous new-job pressure overlaps asynchronous reserve requests.
  s.d.bridge_new_job=1;
  for(unsigned i=0;i<150;i++){
   s.steps(1+s.random()%150);s.cmd(true,false);s.owned();
   unsigned a=s.accepted;s.steps(50+s.random()%100);assert(s.accepted==a);
   s.d.cpu_axi_idle=0;s.cmd(false,true);assert(s.d.cpu_rejected&&s.d.cpu_owned);
   s.d.cpu_axi_idle=1;s.cmd(false,true);assert(!s.d.cpu_owned);
   s.cmd(true,false);assert(s.d.cpu_rejected); // release handshake not finished
   s.idle();++rounds;
  }
  total_admit+=s.accepted;total_block+=s.blocked;
  // Each external drain condition independently prevents acknowledgement.
  for(unsigned gate=0;gate<4;gate++){
   s.reset();if(gate==0)s.busy_hold=true;if(gate==1)s.read_hold=true;
   if(gate==2)s.write_hold=true;if(gate==3)s.d.cpu_axi_idle=0;
   s.cmd(true,false);s.steps(30*(s.ch+s.bh));assert(!s.d.cpu_owned&&!s.d.cpu_fault);
   s.busy_hold=s.read_hold=s.write_hold=false;s.d.cpu_axi_idle=1;
   s.owned();s.cmd(false,true);s.idle();
  }
  s.reset();s.cmd(true,true);assert(s.d.cpu_rejected&&s.d.cpu_idle);
  // Timeout quarantines; late drain and commands must not unlock.
  s.reset();s.busy_hold=true;s.cmd(true,false);s.fault();assert(s.d.cpu_timeout);
  s.busy_hold=false;s.cmd(false,true);s.steps(100);assert(s.d.cpu_fault&&!s.d.cpu_idle);
  s.reset();s.cmd(true,false);s.owned();s.freeze_bridge=true;
  s.cmd(false,true);s.fault();assert(s.d.cpu_timeout);s.freeze_bridge=false;
  s.steps(20*(s.ch+s.bh));assert(s.d.cpu_fault&&s.d.bridge_blocked);
  // A bridge error blocks admission immediately, then crosses to CPU.
  s.reset();s.d.bridge_error=1;s.d.bridge_new_job=1;s.d.eval();assert(!s.d.bridge_accept);
  s.fault();s.d.bridge_error=0;s.steps(100);assert(s.d.cpu_fault);
  // Activity that bypasses the closed admission gate is a sticky violation.
  s.reset();s.cmd(true,false);s.owned();s.check_owned=false;s.read_hold=true;
  s.fault();s.read_hold=false;s.steps(100);assert(s.d.cpu_fault);
 }
 printf("PASS SDRAM119: %u asynchronous ownership rounds, %u admitted jobs, %u blocked requests; independent drains, busy AXI release, stopped clock, timeout, sticky errors. RTL only.\n",rounds,total_admit,total_block);
}
