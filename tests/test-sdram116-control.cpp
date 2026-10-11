// SPDX-License-Identifier: Apache-2.0
#include "Vchisburner_sdram_control116.h"
#include <cassert>
#include <cstdio>
struct Sim {
 Vchisburner_sdram_control116 d;
 void tick(){d.clk=0;d.eval();d.clk=1;d.eval();}
 void reset(){
  d.reset_n=0;d.selected=d.reg_write=0;d.transport_ready=1;
  d.transport_complete=d.transport_fault=d.transport_rejected=0;
  d.ingress_drained=d.host_quiet=d.axi_idle=1;d.ingress_fault=0;
  tick();d.reset_n=1;tick();assert(d.idle&&!d.locked);
 }
 Sim(){reset();}
 void wr(unsigned offset,unsigned data,unsigned strobe=15){
  d.selected=d.reg_write=1;d.reg_offset=offset;d.reg_wdata=data;
  d.reg_wstrb=strobe;tick();d.reg_write=0;
 }
 unsigned rd(unsigned offset){d.selected=1;d.reg_offset=offset;d.eval();return d.reg_rdata;}
 void config(unsigned addr=0x10004000,unsigned bytes=32768){wr(8,addr);wr(12,bytes);}
 void start(){config();wr(16,1);assert(d.prepare&&!d.idle);d.transport_ready=0;tick();assert(!d.prepare);}
 void snapshot(){d.transport_complete=1;tick();assert(rd(4)&4);}
};
int main(){
 Sim s;
 assert(s.rd(0)==0x53443116);
 s.d.selected=0;s.d.eval();assert(!s.d.reg_rdata);
 // Exclusive select and full offset prevent accidental register aliases.
 s.d.reg_write=1;s.d.reg_offset=8;s.d.reg_wdata=0xdeadbeef;
 s.d.reg_wstrb=15;s.tick();s.d.reg_write=0;assert(s.d.source_address==0);
 for(unsigned a: {0x18u,0x108u,0x10008u,0xffffff08u}){
  s.wr(a,0xdeadbeef);assert(s.d.source_address==0&&!s.d.prepare);
 }
 for(unsigned st=0;st<15;st++){s.wr(8,0xdeadbeef,st);assert(!s.d.source_address);}
 s.wr(16,4);assert(!(s.rd(4)&16));
 // Check each release prerequisite independently; no premature reuse.
 for(unsigned gate=0;gate<3;gate++){
  s.reset();s.start();s.snapshot();
  if(gate==0)s.d.host_quiet=0;
  if(gate==1)s.d.axi_idle=0;
  if(gate==2)s.d.ingress_drained=0;
  s.wr(16,2);assert(!s.d.release_snapshot&&(s.rd(4)&4));
  s.d.host_quiet=s.d.axi_idle=s.d.ingress_drained=1;
  s.wr(16,2);assert(s.d.release_snapshot&&!s.d.idle);
  s.d.transport_complete=0;s.tick();assert(!s.d.idle);
  s.d.transport_ready=1;s.tick();assert(s.d.idle);
 }
 for(unsigned gate=0;gate<4;gate++){
  s.reset();s.config();
  if(gate==0)s.d.host_quiet=0;if(gate==1)s.d.axi_idle=0;
  if(gate==2)s.d.ingress_drained=0;if(gate==3)s.d.transport_ready=0;
  s.wr(16,1);assert(!s.d.prepare);
 }
 for(unsigned n: {0u,32769u,0xffffffffu}){
  s.reset();s.config(0x10000000,n);s.wr(16,1);assert(!s.d.prepare);
 }
 for(unsigned a: {0x10000001u,0x14000000u,0x30000000u,0xfffffffcu}){
  s.reset();s.config(a,4);s.wr(16,1);assert(!s.d.prepare);
 }
 s.reset();s.config(0x13fffffc,4);s.wr(16,1);assert(s.d.prepare);
 s.reset();s.config(0x13fffffc,5);s.wr(16,1);assert(!s.d.prepare);
 // Timeout quarantines new jobs; late completion cannot unlock.
 s.reset();s.start();s.wr(8,0x10008000);assert(s.d.source_address==0x10004000);
 s.wr(16,8);assert(!s.d.idle&&!s.d.prepare);
 for(int i=0;i<20;i++)s.tick();assert(s.d.locked&&(s.rd(4)&32));
 s.d.transport_complete=s.d.transport_ready=1;
 for(unsigned cmd: {1u,2u,4u,8u}){s.wr(16,cmd);assert(s.d.locked&&!s.d.prepare&&!s.d.release_snapshot);}
 for(unsigned f=0;f<3;f++){
  s.reset();s.config();
  if(f==0)s.d.transport_fault=1;if(f==1)s.d.ingress_fault=1;
  if(f==2)s.d.transport_rejected=1;
  s.wr(16,1);assert(s.d.locked&&!s.d.prepare);
  s.d.transport_fault=s.d.ingress_fault=s.d.transport_rejected=0;s.tick();assert(s.d.locked);
 }
 s.reset();s.start();s.snapshot();s.wr(16,2);s.d.transport_complete=0;
 for(int i=0;i<20;i++)s.tick();assert(s.d.locked);
 puts("PASS SDRAM116 control: exact offsets, write strobes, lifecycle gates, bounds, faults, timeout quarantine, rejected local reset. RTL only.");
}
