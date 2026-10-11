#include "Vchisburner_nor_bus108.h"
#include <cstdint>
#include <cstdio>
#include <stdexcept>
static Vchisburner_nor_bus108 d;
static unsigned writes, reads, pulse, expected_bus;
static bool lastwr, lastrd, monitor=true;
static uint32_t latched, expected_addr;
static uint16_t expected_data;
static void check(bool c,const char*s){if(!c)throw std::runtime_error(s);}
static void observe(){
 bool wr=!(d.bank0_out&4),rd=!(d.bank0_out&2);
 check(!(wr&&rd),"RD/WR overlap");
 if(!d.reset_n||!d.lifecycle_ok||!d.selected){
  check(!wr&&!rd&&!d.bank3_dir&&!d.bank2_dir&&!d.bank1_dir&&!d.pin30_dir,"unsafe inactive pins");
 }
 if((expected_bus==2||expected_bus==5)&&d.busy&&d.bank0_out==15&&d.bank3_dir)
  latched=2u*(d.bank3_out|(d.bank2_out<<8)|(d.bank1_out<<16));
 if(expected_bus!=2&&expected_bus!=5&&(wr||rd))latched=d.bank3_out|(d.bank2_out<<8);
 if(expected_bus==3&&wr)check(d.pin30_dir&&!d.pin30_out&&(d.bank0_out&1),"board CS2/CS1 selection");
 if(expected_bus==6&&wr)check(d.pin30_dir&&!d.pin30_out&&(d.bank0_out&1),"save probe CS2 low / ROM CS high");
 if(expected_bus==7&&wr)check(d.pin30_dir&&!d.pin30_out&&(d.bank0_out&1),"save data CS2 low / ROM CS high");
 if(expected_bus==4&&d.busy){
  check(!wr&&!d.write_active,"SRAM write strobe forbidden");
  if(rd)check(d.pin30_dir&&!d.pin30_out&&(d.bank0_out&1),"SRAM CS2 low / ROM CS high");
 }
 if(expected_bus==8&&d.busy){
  check(!d.pin30_dir&&d.pin30_pwroff_reset,"GB save reset release / no GBA CS2 drive");
  if(rd)check((d.bank0_out&1)==(expected_addr>=0xa000?0u:1u),"GB RAM CS vs ROM header CS");
  if(wr)check((d.bank0_out&1)&&expected_addr<0x8000,"GB save control CS / address");
 }
 if((expected_bus==9||expected_bus==11)&&d.busy){
  check(!d.pin30_dir&&d.pin30_pwroff_reset,"MBC7 reset / no GBA CS2");
  if(rd||wr)check((d.bank0_out&1)==(expected_addr==0xa080?0u:1u),"MBC7 serial CS");
 }
 if(expected_bus==10&&d.busy){
  check(!d.pin30_dir&&d.pin30_pwroff_reset,"GB180 no GBA CS2 drive");
  if(rd||wr)check((d.bank0_out&1)==(expected_addr>=0xa000?0u:1u),"GB180 RAM/control CS");
 }
 if(monitor&&(wr||rd))check(latched==expected_addr,"address mismatch");
 if(wr){
  if(!lastwr){writes++;pulse=0;}pulse++;
  check(d.write_active&&d.bank3_dir&&d.bank2_dir&&d.bank1_dir,"write direction");
  if(monitor)check(((expected_bus==2||expected_bus==5)?(d.bank3_out|(d.bank2_out<<8)):d.bank1_out)==expected_data,"write data");
 }else if(lastwr&&monitor)check(pulse==36,"write pulse length");
 if(rd){
  if(!lastrd)reads++;
  if(expected_bus==2)check(!d.bank3_dir&&!d.bank2_dir&&d.bank1_dir,"GBA read direction");
  else check(d.bank3_dir&&d.bank2_dir&&!d.bank1_dir,"GB read direction");
 }
 lastwr=wr;lastrd=rd;
}
static void tick(){d.clk=0;d.eval();observe();d.clk=1;d.eval();}
static void wr(unsigned a,uint32_t v){d.reg_wr_addr=a;d.reg_wdata=v;d.reg_wr=1;tick();d.reg_wr=0;tick();}
static uint32_t rr(unsigned a){d.reg_addr=a;d.eval();return d.reg_rdata;}
static uint32_t legacy_caps(){
#ifdef TEST_GB175
 check((rr(1)&0x100)!=0,"GB175 capability missing");
#ifdef TEST_GB177
 check((rr(1)&0x200)!=0,"GB177 capability missing");
#ifdef TEST_GB179
 check((rr(1)&0x400)!=0,"GB179 capability missing");
#ifdef TEST_GB180
 check((rr(1)&0x800)!=0,"GB180 capability missing");
#ifdef TEST_GB185
 check((rr(1)&0x1000)!=0,"GB185 capability missing");
#ifdef TEST_GB191
 check((rr(1)&0x2000)!=0,"GB191 capability missing");return rr(1)&~0x3f00u;
#else
 return rr(1)&~0x1f00u;
#endif
#else
 check((rr(1)&0x1000)==0,"GB185 leaked");return rr(1)&~0xf00u;
#endif
#else
 check((rr(1)&0x800)==0,"GB180 leaked");return rr(1)&~0x700u;
#endif
#else
 check((rr(1)&0x400)==0,"GB179 leaked");return rr(1)&~0x300u;
#endif
#else
 check((rr(1)&0x200)==0,"GB177 leaked");return rr(1)&~0x100u;
#endif
#else
 check((rr(1)&0x100)==0,"GB175 capability leaked");return rr(1);
#endif
}
static void idle(){check(!d.busy&&d.bank0_out==15&&!d.bank3_dir&&!d.bank2_dir&&!d.bank1_dir,"idle pins");}
static void waitdone(){for(int n=0;n<1000&&d.busy;n++)tick();tick();idle();}
static void setup(unsigned bus,uint32_t addr,uint16_t data){
 expected_bus=bus;expected_addr=addr;expected_data=data;
 wr(4,bus);wr(5,addr);wr(7,500);wr(17,data);
}
static void arm(){wr(19,0x43424e31);wr(19,0x43424e32);}
#ifdef TEST_GB179
static bool serial179(unsigned v){
 expected_addr=0xa080;expected_data=v;wr(5,expected_addr);wr(17,v);
 unsigned before=writes;arm();wr(3,7);waitdone();
 bool ok=rr(10)==0;check(writes==before+(ok?1u:0u),"MBC7 rejected strobe");return ok;
}
#endif
int main(){try{
 d.reset_n=0;d.lifecycle_ok=1;d.selected=1;d.gb_data_in=0x69;d.bank3_in=0x5a;d.bank2_in=0xa5;tick();d.reset_n=1;tick();
#ifdef TEST_GB185
 auto command185=[](unsigned command){
  check(serial179(0)&&serial179(0x80),"185 CS start");
  for(int b=10;b>=0;b--){unsigned v=0x80|(((command>>b)&1)*2);
   if(!serial179(v)||!serial179(v|64))return false;
  }return true;
 };
 for(unsigned en=0;en<2;en++)for(unsigned cmd=0;cmd<2048;cmd++){
  setup(11,0xa080,0);
  if(en){check(command185(0x4c0)&&serial179(0),"185 EWEN");}
  bool ok=command185(cmd);
  bool allowed=(cmd>=0x600&&cmd<=0x6ff)||cmd==0x400||cmd==0x4c0||(en&&cmd>=0x500&&cmd<=0x57f);
  check(ok==allowed,"185 opcode/address whitelist");
  if(ok&&(cmd>>8)==5){
   for(unsigned b=0;b<16;b++){unsigned v=0x80|((b&1)*2);check(serial179(v)&&serial179(v|64),"185 word payload");}
   check(serial179(0x80)&&!serial179(0xc0),"185 excess word bit rejected");
  }
 }
 setup(11,0xa080,0);check(command185(0x4c0)&&serial179(0),"185 enable");
 check(command185(0x400)&&serial179(0),"185 disable");check(!command185(0x500),"185 write after EWDS rejected");
 for(uint32_t a: {0xa000u,0xa081u,0xbfffu,0x1a080u,0x100a080u,0xffffffffu}){
  setup(11,a,0);unsigned before=writes;arm();wr(3,7);waitdone();check(rr(10)==7&&writes==before,"185 address guard");
 }
 setup(11,0xa080,0);wr(3,7);waitdone();check(rr(10)==6,"185 requires arm");
 for(unsigned reason=0;reason<3;reason++){
  setup(11,0xa080,0);check(command185(0x4c0)&&serial179(0),"185 before revoke");
  if(reason==0)d.selected=0;else if(reason==1)d.lifecycle_ok=0;else{wr(3,3);}
  tick();tick();d.selected=1;d.lifecycle_ok=1;tick();check(!command185(0x500),"185 revoked write enable");
 }
 puts("GB185 PASS: 4096 opcode cases, EWEN/EWDS, 128-word bounds, extra clocks, address aliases, authorization and revoke; old ports tested below");
#endif
 for(unsigned bus=1;bus<=2;bus++){
  setup(bus,bus==1?0x7fff:0x1fffffe,bus==1?0x96:0xabcd);
  unsigned before=writes;wr(3,7);idle();check(rr(10)==6&&writes==before,"unarmed accepted");
  arm();wr(3,7);waitdone();check(rr(2)==5&&writes==before+1,"armed write");
  wr(3,7);check(rr(10)==6,"arm reused");
  wr(3,2);waitdone();check(rr(8)==(bus==1?0x69:0xa55a)&&rr(9)==bus,"read result");
  d.reg_addr=8;d.reg_rd=1;tick();d.reg_rd=0;check(rr(9)==0,"RX consume");
  arm();wr(17,0);wr(3,7);check(rr(10)==6,"config did not revoke arm");
 }
 for(unsigned addr=0;addr<16;addr++){
  setup(3,addr,0x40);unsigned before=writes;arm();wr(3,7);waitdone();
  bool allowed=addr==2||addr==3||addr==4||addr==6;
  check(writes==before+(allowed?1:0),"board register whitelist");
  check(rr(10)==(allowed?0:7),"board register result");
  wr(3,2);waitdone();check(rr(10)==7,"board reads must be rejected");
 }
 setup(3,2,0x100);arm();wr(3,7);waitdone();check(rr(10)==7,"board data width");
 for(unsigned n=0;n<4;n++){
  setup(n==3?0:n==2?1:2,n==0?1:n==1?0x2000000:n==2?0xa000:0,0);
  arm();wr(3,7);check(rr(10)==7,"range guard");idle();
 }
 setup(2,0x123456,0x1357);arm();wr(3,7);wr(5,0x9876);waitdone();check(rr(5)==0x123456,"busy config changed");
 for(unsigned mode=0;mode<4;mode++)for(unsigned cycle=0;cycle<100;cycle++){
  setup(2,0x123456,0x1357);arm();wr(3,7);
  for(unsigned n=0;n<cycle&&d.busy;n++)tick();
  monitor=false;
  if(mode==0)d.lifecycle_ok=0;else if(mode==1)d.selected=0;else if(mode==2)d.reset_n=0;else wr(3,3);
  tick();tick();idle();check(rr(19)==0&&rr(9)==0,"abort state retained");
  d.reset_n=1;d.lifecycle_ok=1;d.selected=1;tick();monitor=true;
 }
 setup(2,0,0);wr(7,3);arm();wr(3,7);waitdone();check(rr(2)==41&&rr(10)==4,"timeout");
 arm();wr(3,99);check(rr(10)==1&&rr(19)==0,"bad command");
#ifdef TEST_SRAM137
#ifdef TEST_SRAM138
#ifdef TEST_SAVE139
#ifdef TEST_SAVE158
 check(legacy_caps()==0x100bf,"SAVE158 capability");
#else
 check(legacy_caps()==0x1003f,"SAVE139 capability");
#endif
#else
 check(legacy_caps()==0x1001f,"SRAM138 capability");
#endif
 const unsigned save_limit=65536;
 for(unsigned bank=0;bank<2;bank++){
  setup(5,0x1000000,bank);unsigned before=writes;
  wr(3,7);waitdone();check(rr(10)==6&&writes==before,"bank requires arm");
  arm();wr(3,7);waitdone();check(rr(2)==5&&writes==before+1,"save bank latch");
  wr(3,2);waitdone();check(rr(10)==7,"bank latch read rejected");
 }
 for(uint32_t a: {0u,0xfffffeu,0x1000001u,0x1000002u,0xffffffffu}){
  setup(5,a,0);unsigned before=writes;arm();wr(3,7);waitdone();
  check(rr(10)==7&&writes==before,"bank address whitelist");
 }
 for(unsigned v: {2u,0xffu,0x100u,0xffffu}){
  setup(5,0x1000000,v);unsigned before=writes;arm();wr(3,7);waitdone();
  check(rr(10)==7&&writes==before,"bank value whitelist");
 }
#else
 check(legacy_caps()==0x1000f,"SRAM capability");
 const unsigned save_limit=32768;
#endif
 unsigned oldwrites=writes;
 for(unsigned a=0;a<save_limit;a++){
  setup(4,a,0);d.gb_data_in=(a*31u+a/257u)&255u;
  wr(3,2);waitdone();check(rr(2)==5&&rr(9)==1&&rr(8)==d.gb_data_in,"SRAM full range data");
  if(a==0||a==2||a==3||a==4||a==6||a==32767){arm();wr(3,7);waitdone();check(rr(10)==7,"SRAM armed write rejected");}
 }
 for(uint32_t a: {save_limit,save_limit+1u,0xffffffffu}){
  setup(4,a,0);wr(3,2);waitdone();check(rr(10)==7,"SRAM out of bounds");
 }
 for(unsigned mode=0;mode<4;mode++)for(unsigned cycle=0;cycle<100;cycle++){
  setup(4,123,0);wr(3,2);for(unsigned n=0;n<cycle&&d.busy;n++)tick();
  monitor=false;
  if(mode==0)d.lifecycle_ok=0;else if(mode==1)d.selected=0;else if(mode==2)d.reset_n=0;else wr(3,3);
  tick();tick();idle();check(!d.pin30_dir&&rr(9)==0,"SRAM abort releases CS2/RX");
  d.reset_n=1;d.lifecycle_ok=1;d.selected=1;tick();monitor=true;
 }
 setup(4,0,0);wr(7,3);wr(3,2);waitdone();check(rr(10)==4,"SRAM watchdog");
 check(writes==oldwrites,"SRAM mode generated a write");
 std::printf("SRAM PASS: %u byte addresses, boundaries, armed writes denied, 400 aborts, timeout; zero SRAM writes\n",save_limit);
#endif
#ifdef TEST_SAVE139
 for(unsigned a=0;a<65536;a++){
  setup(6,a,0x5a);unsigned before=writes;arm();wr(3,7);waitdone();
  bool allowed=a==0||a==4||a==0x5555||a==0x2aaa;
  check(writes==before+(allowed?1:0)&&rr(10)==(allowed?0:7),"save probe whitelist");
 }
 for(unsigned a: {0u,4u,0x5555u,0x2aaau}){
  for(unsigned v=0;v<256;v++){
   setup(6,a,v);unsigned before=writes;wr(3,7);waitdone();
   check(rr(10)==6&&writes==before,"probe unarmed");
   arm();wr(3,7);waitdone();check(writes==before+1&&rr(10)==0,"probe saved byte");
   wr(3,7);waitdone();check(rr(10)==6&&writes==before+1,"probe arm reused");
  }
  setup(6,a,256);arm();wr(3,7);waitdone();check(rr(10)==7,"probe byte width");
  setup(6,a,0);wr(3,2);waitdone();check(rr(10)==7,"probe read forbidden");
 }
 for(uint32_t a: {65536u,0x1005555u,0xffffffffu}){
  setup(6,a,0);arm();wr(3,7);waitdone();check(rr(10)==7,"probe high address alias");
 }
 for(unsigned mode=0;mode<4;mode++)for(unsigned cycle=0;cycle<100;cycle++){
  setup(6,4,0x96);arm();wr(3,7);
  for(unsigned n=0;n<cycle&&d.busy;n++)tick();monitor=false;
  if(mode==0)d.lifecycle_ok=0;else if(mode==1)d.selected=0;else if(mode==2)d.reset_n=0;else wr(3,3);
  tick();tick();idle();check(!d.pin30_dir&&rr(19)==0,"probe abort pins/arm");
  d.reset_n=1;d.lifecycle_ok=1;d.selected=1;tick();monitor=true;
 }
 setup(6,4,0);wr(7,3);arm();wr(3,7);waitdone();check(rr(10)==4,"probe watchdog");
 std::puts("SAVE139 PASS: 65536 address whitelist, 1024 byte writes, authorization, CS2, 400 aborts, timeout");
#else
 setup(6,4,0);arm();wr(3,7);waitdone();check(rr(10)==7,"probe disabled in legacy build");
#endif
#ifdef TEST_SAVE158
 for(unsigned a=0;a<65536;a++){
  setup(7,a,((a*17u)^0xa5u)&0xffu);unsigned before=writes;
  wr(3,7);waitdone();check(rr(10)==6&&writes==before,"save data requires one-shot arm");
  arm();wr(3,7);waitdone();check(rr(10)==0&&writes==before+1,"bounded save data write");
  wr(3,7);waitdone();check(rr(10)==6&&writes==before+1,"save data arm reused");
 }
 for(uint32_t a: {65536u,0x1000000u,0xffffffffu}){
  setup(7,a,0);unsigned before=writes;arm();wr(3,7);waitdone();
  check(rr(10)==7&&writes==before,"save data high address rejected");
 }
 for(unsigned v: {0x100u,0xffffu}){
  setup(7,0,v);unsigned before=writes;arm();wr(3,7);waitdone();
  check(rr(10)==7&&writes==before,"save data must be one byte");
 }
 setup(7,0,0);wr(3,2);waitdone();check(rr(10)==7,"save data read rejected");
 for(unsigned mode=0;mode<4;mode++)for(unsigned cycle=0;cycle<100;cycle++){
  setup(7,123,0x96);arm();wr(3,7);
  for(unsigned n=0;n<cycle&&d.busy;n++)tick();monitor=false;
  if(mode==0)d.lifecycle_ok=0;else if(mode==1)d.selected=0;else if(mode==2)d.reset_n=0;else wr(3,3);
  tick();tick();idle();check(!d.pin30_dir&&rr(19)==0,"save data abort pins/arm");
  d.reset_n=1;d.lifecycle_ok=1;d.selected=1;tick();monitor=true;
 }
 std::puts("SAVE158 PASS: 65536 bounded writes, one-shot arm, byte width, CS2, 400 aborts");
#else
 setup(7,0,0);arm();wr(3,7);waitdone();check(rr(10)==7,"save data disabled in legacy build");
#endif
#ifdef TEST_GB175
 check((rr(1)&0x100)!=0,"GB175 capability");
 for(unsigned a=0;a<65536;a++){
  setup(8,a,0);unsigned before=reads;
  wr(3,2);waitdone();bool allowed=(a>=0x100&&a<0x150)||(a>=0xa000&&a<0xc000);
  check(reads==before+(allowed?1:0)&&rr(10)==(allowed?0:7),"GB175 complete read aperture");
  unsigned bw=writes;arm();wr(3,7);waitdone();bool cw=a==0||a==0x4000;
#ifdef TEST_GB177
  cw=cw||a==0x6000;
#endif
  check(writes==bw+(cw?1:0)&&rr(10)==(cw?0:7),"GB175 complete write aperture");
 }
 for(unsigned a: {0u,0x4000u})for(unsigned v=0;v<65536;v++){
  setup(8,a,v);unsigned before=writes;arm();wr(3,7);waitdone();
  bool ok=a==0?(v==0||v==10):v<16;
  check(writes==before+(ok?1:0)&&rr(10)==(ok?0:7),"GB175 control value bounds");
 }
 for(uint32_t a: {0x10000u,0x100a000u,0xffffffffu}){
  setup(8,a,0);wr(3,2);waitdone();check(rr(10)==7,"GB175 high address alias");
 }
 setup(8,0,10);wr(3,7);waitdone();check(rr(10)==6,"GB175 control needs arm");
#ifdef TEST_GB177
 for(unsigned v=0;v<65536;v++){
  setup(8,0x6000,v);unsigned before=writes;arm();wr(3,7);waitdone();
  check(writes==before+(v<=1?1:0)&&rr(10)==(v<=1?0:7),"GB177 exact mode values");
 }
 setup(8,0x6000,1);wr(3,7);waitdone();check(rr(10)==6,"GB177 mode needs arm");
 setup(8,0x1006000,1);arm();wr(3,7);waitdone();check(rr(10)==7,"GB177 high alias");
 std::puts("GB177 PASS: 65536 mode values, one-shot authorization, high alias; other apertures unchanged");
#endif
 for(unsigned mode=0;mode<4;mode++)for(unsigned cycle=0;cycle<100;cycle++){
  setup(8,0xa123,0);wr(3,2);for(unsigned n=0;n<cycle&&d.busy;n++)tick();monitor=false;
  if(mode==0)d.lifecycle_ok=0;else if(mode==1)d.selected=0;else if(mode==2)d.reset_n=0;else wr(3,3);
  tick();tick();idle();check(rr(9)==0&&rr(19)==0,"GB175 abort RX/arm");
  d.reset_n=1;d.lifecycle_ok=1;d.selected=1;tick();monitor=true;
 }
 setup(8,0xa000,0);wr(7,3);wr(3,2);waitdone();check(rr(10)==4,"GB175 timeout");
 std::puts("GB175 PASS: complete 16-bit address/value guards, CS, high aliases, aborts, timeout; no RAM writes");
#else
 setup(8,0xa000,0);wr(3,2);waitdone();check(rr(10)==7,"GB175 absent in legacy build");
#endif
#ifdef TEST_GB179
 for(unsigned a=0;a<65536;a++){
  setup(9,a,0);unsigned before=reads;wr(3,2);waitdone();
  bool allowed=(a>=0x100&&a<0x150)||a==0xa080;
  check(reads==before+(allowed?1:0)&&rr(10)==(allowed?0:7),"MBC7 read aperture");
  before=writes;arm();wr(3,7);waitdone();allowed=a==0||a==0x4000||a==0xa080;
  check(writes==before+(allowed?1:0)&&rr(10)==(allowed?0:7),"MBC7 write aperture");
 }
 for(unsigned a: {0u,0x4000u,0xa080u})for(unsigned v=0;v<65536;v++){
  setup(9,a,v);unsigned before=writes;arm();wr(3,7);waitdone();
  bool ok=a==0?(v==0||v==10):a==0x4000?(v==0||v==64):(v==0||v==0x80||v==0x82||v==0xc2);
  check(writes==before+(ok?1:0)&&rr(10)==(ok?0:7),"MBC7 value/prefix whitelist");
 }
 for(unsigned command=0;command<2048;command++){
  setup(9,0xa080,0);check(serial179(0)&&serial179(0x80),"MBC7 start");
  bool ok=true;
  for(int bit=10;bit>=0&&ok;bit--){unsigned v=0x80|(((command>>bit)&1)*2);check(serial179(v),"MBC7 low clock");ok=serial179(v|0x40);}
  check(ok==((command>>8)==6),"MBC7 only READ opcode allowed");
  if(ok){for(unsigned bit=0;bit<16;bit++)check(serial179(0x80)&&serial179(0xc0),"MBC7 data clocks");
   check(serial179(0x82)&&!serial179(0xc2),"MBC7 trailing DI rejected");}
 }
 for(uint32_t a: {0x1a080u,0x100a080u,0xffffffffu}){
  setup(9,a,0);arm();wr(3,7);waitdone();check(rr(10)==7,"MBC7 high alias write");
  wr(3,2);waitdone();check(rr(10)==7,"MBC7 high alias read");
 }
 setup(9,0xa080,0);wr(3,7);waitdone();check(rr(10)==6,"MBC7 authorization");
 for(unsigned mode=0;mode<4;mode++)for(unsigned cycle=0;cycle<100;cycle++){
  setup(9,0xa080,0x82);arm();wr(3,7);for(unsigned n=0;n<cycle&&d.busy;n++)tick();monitor=false;
  if(mode==0)d.lifecycle_ok=0;else if(mode==1)d.selected=0;else if(mode==2)d.reset_n=0;else wr(3,3);
  tick();tick();idle();check(!rr(19)&&!rr(9),"MBC7 abort");
  d.reset_n=1;d.lifecycle_ok=1;d.selected=1;tick();monitor=true;
 }
 setup(9,0xa080,0);wr(7,3);arm();wr(3,7);waitdone();check(rr(10)==4,"MBC7 timeout");
 std::puts("GB179 PASS: complete address/value guards, all 2048 opcode prefixes, READ only, CS, aborts and timeout");
#else
 setup(9,0xa080,0);arm();wr(3,7);waitdone();check(rr(10)==7,"MBC7 absent in legacy build");
#endif
#ifdef TEST_GB180
 for(unsigned a=0;a<65536;a++){
  setup(10,a,0);unsigned before=reads;wr(3,2);waitdone();
  bool ram=a>=0xa000&&a<0xc000,ok=ram||(a>=0x100&&a<0x150);
  check(reads==before+(ok?1:0)&&rr(10)==(ok?0:7),"GB180 read aperture");
  before=writes;arm();wr(3,7);waitdone();ok=ram||a==0||a==0x4000||a==0x6000;
  check(writes==before+(ok?1:0)&&rr(10)==(ok?0:7),"GB180 write aperture");
 }
 for(unsigned a: {0u,0x4000u,0x6000u,0xa000u,0xbfffu})for(unsigned v=0;v<65536;v++){
  setup(10,a,v);unsigned before=writes;arm();wr(3,7);waitdone();
  bool ok=a==0?(v==0||v==10):a==0x4000?v<16:a==0x6000?v<=1:v<256;
  check(writes==before+(ok?1:0)&&rr(10)==(ok?0:7),"GB180 value guard");
 }
 for(uint32_t a: {0x1a000u,0x100a000u,0xffffffffu}){
  setup(10,a,0);arm();wr(3,7);waitdone();check(rr(10)==7,"GB180 high alias");
 }
 setup(10,0xa000,0);wr(3,7);waitdone();check(rr(10)==6,"GB180 arm required");
 for(unsigned mode=0;mode<4;mode++)for(unsigned cycle=0;cycle<100;cycle++){
  setup(10,0xbfff,0xa5);arm();wr(3,7);for(unsigned n=0;n<cycle&&d.busy;n++)tick();monitor=false;
  if(mode==0)d.lifecycle_ok=0;else if(mode==1)d.selected=0;else if(mode==2)d.reset_n=0;else wr(3,3);
  tick();tick();idle();check(!rr(19)&&!rr(9),"GB180 abort");
  d.reset_n=1;d.lifecycle_ok=1;d.selected=1;tick();monitor=true;
 }
 setup(10,0xa000,0);wr(7,3);arm();wr(3,7);waitdone();check(rr(10)==4,"GB180 timeout");
 std::puts("GB180 PASS: full address/value guards, byte data, CS, arm, 400 aborts, timeout");
#else
 setup(10,0xa000,0);arm();wr(3,7);waitdone();check(rr(10)==7,"GB180 absent in legacy build");
#endif
 std::printf("NOR108 bus PASS: %u writes, %u reads; interruption cases\n",writes,reads);return 0;
 }catch(const std::exception&e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
