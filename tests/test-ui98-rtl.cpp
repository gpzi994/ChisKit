#include "Vchisburner_cart_auto98.h"
#include <cstdint>
#include <cstdio>
#include <stdexcept>
static unsigned bus=1, latched=0, reads=0;
static bool rd_prev=false,cs_prev=false;
static void check(bool p,const char*s){if(!p){fprintf(stderr,"bus=%u reads=%u: %s\n",bus,reads,s);throw std::runtime_error(s);}}
static uint8_t value(unsigned a){return (a*13u)^(a>>8)^0x5au;}
static void observe(Vchisburner_cart_auto98&d){
 check(d.bank0_out&4,"WR became active");
 bool rd=!(d.bank0_out&2),cs=!(d.bank0_out&1);
 if(bus==1){check(!d.bank1_dir,"GB data driven");check(!cs,"GB ROM /CS must stay high");
  if(rd){unsigned a=d.bank3_out|(d.bank2_out<<8);check(a>=0x100&&a<0x160,"GB window escaped");d.gb_data_in=value(a);}}
 else {
  if(cs&&!cs_prev)latched=(d.bank3_out|(d.bank2_out<<8)|(d.bank1_out<<16))*2;
  if(rd){check(!d.bank3_dir&&!d.bank2_dir,"GBA read data driven");check(latched<192,"GBA window escaped");d.bank3_in=value(latched);d.bank2_in=value(latched+1);}
  if(!rd&&rd_prev)latched+=2;
 }
 if(rd&&!rd_prev)reads++;
 rd_prev=rd;cs_prev=cs;
}
static void tick(Vchisburner_cart_auto98&d){d.clk=0;d.eval();observe(d);d.eval();d.clk=1;d.eval();observe(d);d.eval();}
static uint32_t rr(Vchisburner_cart_auto98&d,unsigned a){d.reg_rd=0;d.reg_addr=a;tick(d);d.reg_rd=1;d.eval();if(a==2)tick(d);return d.reg_rdata;}
static void wr(Vchisburner_cart_auto98&d,unsigned a,uint32_t v){d.reg_rd=0;d.reg_addr=a;d.reg_wdata=v;d.reg_wr=1;tick(d);d.reg_wr=0;tick(d);}
static bool idle(Vchisburner_cart_auto98&d){return !d.bank3_dir&&!d.bank2_dir&&!d.bank1_dir&&d.bank0_out==15;}
static void select_bus(Vchisburner_cart_auto98&d,unsigned b){wr(d,3,1);bus=b;wr(d,4,b);check(rr(d,4)==b,"bus selection failed");}
static void start(Vchisburner_cart_auto98&d,unsigned addr,unsigned n=32){wr(d,5,addr);wr(d,6,n);wr(d,7,10000);wr(d,3,2);}
static void block(Vchisburner_cart_auto98&d,unsigned b,unsigned a){
 select_bus(d,b);unsigned before=reads;start(d,a);
 // A bus change during the command must neither remux pins nor update BUS.
 wr(d,4,b==1?2:1);check(rr(d,4)==b,"busy bus changed");
 unsigned cycles=0;while(!(rr(d,2)&4)&&cycles++<10000)tick(d);
 check(cycles<10000&&!rr(d,10)&&idle(d),"read not completed safely");
 check(reads-before==(b==1?32u:16u),"wrong physical read count");check(rr(d,9)==32,"FIFO count");
 for(unsigned w=0;w<8;w++){uint32_t v=rr(d,8);for(unsigned l=0;l<4;l++)check(uint8_t(v>>(l*8))==value(a+w*4+l),"FIFO data");d.reg_rd=1;tick(d);d.reg_rd=0;}
}
int main(){
 Vchisburner_cart_auto98 d;d.reset_n=0;d.reg_wr=0;d.reg_rd=0;d.lifecycle_ok=1;d.lifecycle_status=0x80000303;tick(d);d.reset_n=1;tick(d);
 check(idle(d)&&rr(d,1)==0x02313f03,"initial caps/safety");
 for(unsigned round=0;round<3;round++){for(unsigned a=0x100;a<0x160;a+=32)block(d,1,a);for(unsigned a=0;a<192;a+=32)block(d,2,a);}
 for(unsigned b=1;b<=2;b++){
  select_bus(d,b);start(d,b==1?0:0x2000000);check(rr(d,10)&&idle(d),"out-of-window accepted");
  select_bus(d,b);wr(d,3,0x80);check(rr(d,10)&&idle(d),"unknown command accepted");
  select_bus(d,b);start(d,b==1?0x100:0);for(unsigned i=0;i<7;i++)tick(d);wr(d,3,3);tick(d);check(idle(d),"abort unsafe");
  select_bus(d,b);start(d,b==1?0x100:0);d.lifecycle_ok=0;
  d.clk=0;d.eval();d.clk=1;d.eval();bus=1;observe(d);tick(d);
  check(idle(d),"lifecycle unsafe");d.lifecycle_ok=1;tick(d);
 }
 puts("PASS: dual bus header windows/data, read-only pins, busy ownership, abort/lifecycle, repeat scans");
}
