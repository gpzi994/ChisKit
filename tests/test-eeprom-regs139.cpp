#include "Vchisburner_eeprom_regs139.h"
#include <cstdio>
#include <stdexcept>
static Vchisburner_eeprom_regs139 d;
static void ck(bool b,const char*s){if(!b)throw std::runtime_error(s);}
static void tick(){d.clk=0;d.eval();d.clk=1;d.eval();}
static void reset(){d.reset_n=0;d.wr=0;d.other_busy=0;d.other_request=0;d.engine_busy=0;d.engine_done=0;d.engine_error=0;tick();d.reset_n=1;d.lifecycle_ok=1;d.selected=1;tick();}
static void wr(unsigned a,unsigned v){d.address=a;d.data=v;d.wr=1;tick();d.wr=0;d.eval();}
static unsigned rd(unsigned a){d.read_address=a;d.eval();return d.read_data;}
int main(){try{
 unsigned cases=0;
 for(unsigned m=0;m<2;m++)for(unsigned a=0;a<16384;a++){
  reset();wr(1,(m<<14)|a);wr(2,1);
  bool ok=a<(m?1024u:64u);ck(bool(d.engine_start)==ok,"range");
  ck(bool(d.hold_owner)==ok,"ownership");cases++;
 }
 for(unsigned bit=15;bit<32;bit++){reset();wr(1,1u<<bit);wr(2,1);ck(!d.engine_start,"high alias");}
 for(unsigned kind=0;kind<2;kind++){
  reset();if(kind)d.other_request=1;else d.other_busy=1;
  wr(2,1);ck(!d.hold_owner&&!d.engine_start&&(rd(3)&8),"collision");
 }
 reset();d.engine_done=1;wr(1,0x4005);wr(2,1);
 ck(d.hold_owner&&!d.allow_other,"launch lock");tick();
 ck(d.hold_owner,"stale done during launch");d.engine_done=0;d.engine_busy=1;
 wr(1,0);ck(d.mode_large&&d.block_address==5,"busy mutation");
 wr(2,1);ck(!d.engine_start&&d.hold_owner,"busy resubmit");
 d.engine_busy=0;d.engine_done=1;d.engine_data=0x123456789abcdef0ULL;tick();
 ck(!d.hold_owner&&(rd(3)&2)&&rd(4)==0x12345678&&rd(5)==0x9abcdef0,"completion data");
 reset();wr(2,1);tick();d.engine_busy=1;wr(2,2);
 ck(d.hold_owner&&!(rd(3)&2),"abort drain lock");
 d.engine_busy=0;d.engine_error=1;tick();ck(!d.hold_owner&&(rd(3)&4)&&rd(4)==0,"abort drain");
 for(unsigned kind=0;kind<2;kind++){
  reset();wr(2,1);if(kind)d.selected=0;else d.lifecycle_ok=0;
  d.eval();ck(!d.engine_owner&&d.engine_cancel,"immediate lifecycle gate");tick();
  ck(!d.hold_owner&&rd(4)==0,"lifecycle cleanup");
 }
 std::printf("EEPROM registers PASS: %u descriptors, high aliases, collisions, busy immutability, stale DONE, abort drain, lifecycle, payload\n",cases);
 return 0;
}catch(const std::exception&e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
