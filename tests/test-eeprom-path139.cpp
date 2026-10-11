#include "Vchisburner_eeprom_path139.h"
#include <cstdio>
#include <cstdint>
#include <stdexcept>
static Vchisburner_eeprom_path139 d;
static void ck(bool b,const char*s){if(!b)throw std::runtime_error(s);}
static void tick(){d.clk=0;d.eval();d.clk=1;d.eval();}
static void reset(){d.reset_n=0;d.wr=0;d.other_busy=0;d.other_request=0;tick();d.reset_n=1;d.selected=1;d.lifecycle_ok=1;tick();}
static void wr(unsigned a,unsigned v){d.address=a;d.data=v;d.wr=1;tick();d.wr=0;d.eval();}
static unsigned rd(unsigned a){d.read_address=a;d.eval();return d.read_data;}
static void safe(){ck(d.bank0_out==15&&!d.bank1_dir&&!d.bank2_dir&&!d.bank3_dir&&!d.pin30_dir&&!d.pin31_dir,"safe pins");}
int main(){try{
 reset();unsigned cases=0;
 for(unsigned m=0;m<2;m++)for(unsigned a=0;a<(m?1024u:64u);a++){
  // No reset between reads: exercise real completion retention and restart.
  wr(1,(m<<14)|a);wr(2,1);ck(d.hold_owner,"accepted");
  unsigned reads=0,writes=0,command=0;bool pr=1,pw=1;
  uint64_t pattern=0x81a543b27e06dc19ULL^(uint64_t(a)<<32)^a;
  for(unsigned n=0;d.hold_owner&&n<20000;n++){
   d.other_request=1;d.eval();ck(!d.other_accept,"competing request leaked");
   bool r=d.bank0_out&2,w=d.bank0_out&4;
   if(!w&&pw){ck(d.bank3_dir,"TX direction");command=(command<<1)|(d.bank3_out&1);writes++;}
   if(!r&&pr){ck(!d.bank3_dir,"RX direction");ck(reads<68,"excess reads");d.bank3_in=reads<4?1:((pattern>>(63-(reads-4)))&1);reads++;}
   pr=r;pw=w;tick();
  }
  d.other_request=0;d.eval();
  ck(!d.hold_owner&&(rd(3)&7)==2,"completion");safe();
  ck(reads==68&&writes==(m?17u:9u),"strobes");
  ck(command==(((3u<<(m?14:6))|a)<<1),"command");
  ck(((uint64_t(rd(4))<<32)|rd(5))==pattern,"register payload");cases++;
 }
 unsigned aborts=0;
 for(unsigned n=0;n<3600;n+=13){
  reset();wr(1,0x4000);wr(2,1);for(unsigned i=0;i<n;i++)tick();
  wr(2,2);safe();for(unsigned i=0;i<4;i++)tick();
  ck(!d.hold_owner&&(rd(3)&7)==4&&rd(4)==0&&rd(5)==0,"cancel completion");aborts++;
 }
 for(unsigned k=0;k<2;k++){
  reset();if(k)d.other_busy=1;else d.other_request=1;
  wr(2,1);ck(!d.hold_owner&&(rd(3)&8),"collision accepted");safe();
 }
 for(unsigned k=0;k<2;k++){
  reset();wr(2,1);for(unsigned i=0;i<80;i++)tick();
  if(k)d.selected=0;else d.lifecycle_ok=0;d.eval();safe();tick();
  ck(!d.hold_owner&&rd(4)==0,"lifecycle");
 }
 std::printf("EEPROM path PASS: %u consecutive blocks, %u cancellation offsets, competition, lifecycle, real pin-to-register payload\n",cases,aborts);return 0;
}catch(const std::exception&e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
