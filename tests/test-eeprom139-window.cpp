#include "Vchisburner_eeprom139.h"
#include <cstdint>
#include <cstdio>
#include <stdexcept>
static Vchisburner_eeprom139 d;
#ifndef CAPTURE_CYCLES
#define CAPTURE_CYCLES 18
#endif
static void tick(){d.clk=0;d.eval();d.clk=1;d.eval();}
static void require(bool v,const char *s){if(!v)throw std::runtime_error(s);}
// Characterization model, NOT a real EEPROM timing specification:
// output becomes valid after delay clocks of RD-low and is inverted after
// hold clocks of RD-high, to expose dependence on the old ideal model.
// rising_delay >= 0: data is invalid throughout RD-low and becomes valid
// at high_age >= rising_delay; hold is the exclusive end of that window.
static bool run(unsigned mode,unsigned delay,unsigned hold,int rising_delay=-1){
 const uint64_t pattern=0xa57321fedc098647ULL;
 d.start=0;d.cancel=0;d.owner_ok=1;d.reset_n=0;tick();d.reset_n=1;tick();
 d.mode_large=mode;d.block_address=17;d.start=1;tick();d.start=0;
 if(CAPTURE_CYCLES<1||CAPTURE_CYCLES>18){
  require(d.error&&!d.busy&&!d.done&&d.cs_n&&d.rd_n&&d.wr_n&&!d.data_oe,"invalid capture parameter not rejected safely");
  return true;
 }
 bool previous_rd=true;unsigned low_age=0,high_age=0,reads=0,bit=0;
 for(unsigned n=0;d.busy&&n<20000;n++){
  if(!d.rd_n){
   if(previous_rd){if(reads)require(high_age==18,"RD high pulse changed");low_age=0;bit=reads<4?1:unsigned((pattern>>(63-(reads-4)))&1);reads++;}
   d.data_in=rising_delay<0&&low_age>=delay?bit:!bit;low_age++;
  }else if(reads){
   if(!previous_rd){require(low_age==18,"RD low pulse changed");high_age=0;}
   bool valid=high_age<hold&&(rising_delay<0||high_age>=unsigned(rising_delay));
   d.data_in=valid?bit:!bit;high_age++;
  }
  previous_rd=d.rd_n;tick();
 }
 require(d.done&&!d.error&&!d.busy&&reads==68,"transaction did not complete");
 return d.data==pattern;
}
int main(){try{
 if(CAPTURE_CYCLES<1||CAPTURE_CYCLES>18){
  run(0,0,0);run(1,0,0);std::printf("PASS invalid capture=%d rejected in both modes\n",CAPTURE_CYCLES);return 0;
 }
 unsigned good=0,bad=0;
 for(unsigned mode=0;mode<2;mode++)for(unsigned delay: {0u,8u,17u}){
  unsigned minimum=99;
  for(unsigned hold=0;hold<=18;hold++){
   bool same=run(mode,delay,hold);
   require(same==(hold>=CAPTURE_CYCLES),"capture window mismatch");
   if(same){good++;if(minimum==99)minimum=hold;}else bad++;
  }
  std::printf("mode=%u low_delay=%u minimum_high_hold=%u CPU cycles\n",mode,delay,minimum);
  require(minimum==CAPTURE_CYCLES,"sampling-window mismatch");
 }
 std::printf("CHARACTERIZATION COMPLETE: %u matching, %u mismatching payloads across 114 windows; not hardware signoff\n",good,bad);
 unsigned late_good=0,late_bad=0;
 for(unsigned mode=0;mode<2;mode++)for(unsigned ready=0;ready<=18;ready++)
  for(unsigned end=0;end<=18;end++){
   bool same=run(mode,0,end,int(ready));
   // high_age=0 precedes the first RX_HIGH CPU edge; sampling edge C
   // sees age C-1. Empty/inverted windows are deliberately included.
   bool expected=ready<unsigned(CAPTURE_CYCLES)&&end>=unsigned(CAPTURE_CYCLES);
   if(same!=expected){
    std::fprintf(stderr,"capture=%d mode=%u ready=%u end=%u actual=%d expected=%d\n",CAPTURE_CYCLES,mode,ready,end,same,expected);
    require(false,"rising-delay capture window mismatch");
   }
   if(same)late_good++;else late_bad++;
  }
 std::printf("RISING-DELAY PASS: capture=%d, 722/722 expected outcomes, %u matching and %u intentionally mismatching; not hardware signoff\n",CAPTURE_CYCLES,late_good,late_bad);
 return 0;
}catch(const std::exception &e){std::fprintf(stderr,"ERROR: %s\n",e.what());return 1;}}
