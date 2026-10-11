#include "Vchisburner_eeprom_pins139.h"
#include <cstdint>
#include <cstdio>
#include <stdexcept>
static Vchisburner_eeprom_pins139 d;
static void check(bool v,const char*s){if(!v)throw std::runtime_error(s);}
static void tick(){d.clk=0;d.eval();d.clk=1;d.eval();}
static void reset(){d.start=0;d.cancel=0;d.granted=1;d.lifecycle_ok=1;d.reset_n=0;tick();d.reset_n=1;tick();}
static void begin(unsigned m,unsigned a){d.mode_large=m;d.block_address=a;d.start=1;tick();d.start=0;}
static void released(){check(d.bank0_out==15&&d.bank0_dir&&!d.bank1_dir&&!d.bank2_dir&&!d.bank3_dir&&!d.pin30_dir&&!d.pin31_dir,"pin release");}
int main(){try{
 unsigned cases=0;
 const uint64_t pattern=0x893a17cf6502b4deULL;
 for(unsigned m=0;m<2;m++)for(unsigned a=0;a<(m?1024u:64u);a++){
  reset();begin(m,a);unsigned rd=0,wr=0,command=0,setup=0;bool pcs=1,prd=1,pwr=1;
  for(unsigned n=0;d.busy&&n<20000;n++){
   unsigned c=d.bank0_out;bool cs=c&1,r=c&2,w=c&4;
   check(d.bank0_dir&&d.bank1_dir&&d.bank1_out==255,"upper address");
   check(d.pin30_dir&&d.pin30_out&&!d.pin30_pwroff_reset&&!d.pin31_dir,"sideband safety");
   check(r||w,"overlap");
   if(cs&&wr==0){check(d.bank3_dir&&d.bank2_dir&&d.bank3_out==128&&d.bank2_out==255,"preload");setup++;}
   if(!cs&&pcs&&wr==0)check(setup==18&&d.bank3_out==128&&d.bank3_dir,"CS address latch");
   if(!w&&pwr){check(!cs&&d.bank3_dir&&d.bank2_dir,"TX directions");command=(command<<1)|(d.bank3_out&1);wr++;}
   if(!r&&prd){check(!cs&&!d.bank3_dir&&!d.bank2_dir,"RX directions");d.bank3_in=rd<4?255:((pattern>>(63-(rd-4)))&1);rd++;}
   pcs=cs;prd=r;pwr=w;tick();
  }
  check(d.done&&!d.error&&!d.busy,"completion");released();
  check(wr==(m?17u:9u)&&rd==68,"pulse counts");
  check(command==(((3u<<(m?14:6))|a)<<1)&&d.data==pattern,"command/payload");cases++;
 }
 for(unsigned kind=0;kind<4;kind++)for(unsigned n=0;n<4000;n+=11){
  reset();begin(1,100);for(unsigned i=0;i<n&&d.busy;i++)tick();
  if(kind==0)d.granted=0;else if(kind==1)d.lifecycle_ok=0;else if(kind==2)d.cancel=1;else d.reset_n=0;
  d.eval();released();tick();check(!d.busy&&!d.done,"interrupted result");
 }
 reset();d.granted=0;begin(0,0);check(!d.busy,"ungranted start");released();
 std::printf("EEPROM pins PASS: %u addresses, preload, pin directions, commands, payload, ownership/reset/cancel release\n",cases);return 0;
}catch(const std::exception&e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
