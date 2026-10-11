#include "Vchisburner_eeprom139.h"
#include <cstdio>
#include <stdexcept>
#include <cstdint>
static Vchisburner_eeprom139 d;
static void check(bool v,const char*s){if(!v)throw std::runtime_error(s);}
static void tick(){d.clk=0;d.eval();d.clk=1;d.eval();}
static void reset(){d.reset_n=0;tick();d.reset_n=1;d.owner_ok=1;d.cancel=0;d.start=0;d.write_mode=0;tick();}
static void begin(unsigned mode,unsigned address){d.mode_large=mode;d.block_address=address;d.start=1;tick();d.start=0;}
int main(){try{
 const uint64_t pattern=0xa57321fedc098647ULL;
 unsigned cases=0;
 for(unsigned mode=0;mode<2;mode++)for(unsigned address=0;address<(mode?1024u:64u);address++){
  reset();begin(mode,address);unsigned writes=0,reads=0,command=0;
  unsigned setup=0,hold=0;bool wr=true,rd=true;
  for(unsigned cycles=0;d.busy&&cycles<20000;cycles++){
   check(d.rd_n||d.wr_n,"overlapping strobes");
   if(d.address_hold){
    check(d.rd_n&&d.wr_n,"address phase strobe");
    if(d.cs_n)setup++;else hold++;
   }
   if(!d.wr_n)check(!d.address_hold&&setup==18&&hold==36,"address setup/hold before serial command");
   if(!d.wr_n&&wr){check(d.data_oe&&!d.cs_n,"TX direction/CS");command=(command<<1)|d.data_out;writes++;}
   if(!d.rd_n&&rd){check(!d.data_oe&&!d.cs_n,"RX direction/CS");
    d.data_in=reads<4?1:((pattern>>(63-(reads-4)))&1);reads++;
   }
   wr=d.wr_n;rd=d.rd_n;tick();
  }
  check(d.done&&!d.error&&!d.busy,"completion");
  check(!d.address_hold,"address not released");
  check(writes==(mode?17u:9u)&&reads==68,"pulse counts");
  check(command==(((3u<<(mode?14:6))|address)<<1),"read command/address/stop");
  check(d.data==pattern,"payload byte order/dummy bits");cases++;
 }
 for(unsigned mode=0;mode<2;mode++){
  reset();begin(mode,mode?1024:64);check(d.error&&!d.busy,"invalid address accepted");
 }
 for(unsigned mode=0;mode<2;mode++)for(unsigned address: {0u,1u,mode?1023u:63u}){
  reset();d.write_mode=1;d.write_data=pattern;begin(mode,address);
  unsigned count=0,reads=0;bool wr=true,rd=true;uint64_t payload=0;
  unsigned command=0;unsigned prefix=mode?16u:8u;
  for(unsigned cycles=0;d.busy&&cycles<20000;cycles++){
   check(d.rd_n||d.wr_n,"write/read strobes overlap");
   if(!d.wr_n&&wr){
    check(d.data_oe&&!d.cs_n,"write TX direction/CS");
    if(count<prefix)command=(command<<1)|d.data_out;
    else if(count<prefix+64u)payload=(payload<<1)|d.data_out;
    else check(!d.data_out,"write stop bit not zero");
    count++;
   }
   if(!d.rd_n&&rd)reads++;
   wr=d.wr_n;rd=d.rd_n;tick();
  }
  check(d.done&&!d.error&&!d.busy,"write completion");
  check(count==prefix+65u&&reads==0,"write pulse count");
  check(command==((2u<<(mode?14:6))|address),"write opcode/address");
  check(payload==pattern,"write data order");
  check(d.cs_n&&d.rd_n&&d.wr_n&&!d.data_oe,"write idle pins");
 }
 for(unsigned kind=0;kind<3;kind++)for(unsigned cycle=0;cycle<3600;cycle+=7){
  reset();begin(1,17);for(unsigned n=0;n<cycle&&d.busy;n++)tick();
  if(kind==0)d.owner_ok=0;else if(kind==1)d.cancel=1;else d.reset_n=0;
  d.eval();check(d.cs_n&&d.rd_n&&d.wr_n&&!d.data_oe&&!d.cart_clk&&!d.address_hold,"immediate release");
  tick();check(!d.busy&&!d.done,"aborted result valid");
 }
 for(unsigned kind=0;kind<3;kind++)for(unsigned cycle=0;cycle<3600;cycle+=53){
  reset();d.write_mode=1;d.write_data=pattern;begin(1,17);
  for(unsigned n=0;n<cycle&&d.busy;n++)tick();
  if(kind==0)d.owner_ok=0;else if(kind==1)d.cancel=1;else d.reset_n=0;
  d.eval();
  check(d.cs_n&&d.rd_n&&d.wr_n&&!d.data_oe&&!d.cart_clk&&!d.address_hold,
   "write pins did not release immediately");
  tick();check(!d.busy&&!d.done,"aborted write result valid");
 }
 std::printf("EEPROM RTL PASS: %u read blocks; 6 write blocks; command/data/stop timing, range and interruption guards\n",cases);
 return 0;
}catch(const std::exception&e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
