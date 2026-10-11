#include "Vchisburner_cart_auto108.h"
#include <cstdio>
#include <stdexcept>
static Vchisburner_cart_auto108 d;
static unsigned writes;
static bool last_wr;
static void check(bool c,const char*s){if(!c)throw std::runtime_error(s);}
static void tick(){
#ifdef PROGRAM131_TEST
 d.memory_clk=0;d.memory_reset_n=d.reset_n;
 d.stream_p0_data=0xa55aa55a;d.stream_p1_data=0xa55aa55a;
 d.stream_p0_valid=d.stream_p0_rd;d.stream_p1_valid=d.stream_p1_rd;
#endif
 d.clk=0;d.eval();
 bool wr=!(d.bank0_out&4),rd=!(d.bank0_out&2);
 check(!(wr&&rd),"RD/WR overlap");
 if(!d.lifecycle_ok||!d.reset_n)check(!wr&&!rd&&!d.bank1_dir&&!d.bank2_dir&&!d.bank3_dir,"inactive owner");
 if(wr&&!last_wr)writes++;last_wr=wr;
 d.clk=1;
#ifdef PROGRAM131_TEST
 d.memory_clk=1;
#endif
 d.eval();
}
static void wr(unsigned a,unsigned v){d.reg_rd=0;d.reg_addr=a;d.reg_wdata=v;d.reg_wr=1;tick();d.reg_wr=0;tick();}
static unsigned rr(unsigned a){d.reg_rd=0;d.reg_addr=a;tick();d.reg_rd=1;tick();unsigned v=d.reg_rdata;d.reg_rd=0;return v;}
static void waitdone(){unsigned n=0;while((rr(2)&2)&&n++<20000){}check(n<20000,"command timeout");}
int main(){try{
 d.reset_n=0;d.lifecycle_ok=1;d.lifecycle_status=0x80000303;
 d.gb_data_in=0x69;d.bank3_in=0x5a;d.bank2_in=0xa5;tick();d.reset_n=1;tick();
#ifdef TEST_SRAM137
 check(rr(31)==0x384e4243&&rr(1)==0x02713f03,"extension discovery/SRAM ABI");
#else
 check(rr(31)==0x384e4243&&rr(1)==0x02313f03,"extension discovery/legacy ABI");
#endif
 wr(16,31);check(rr(16)==31&&rr(0)==0x384e4243,"private select");
 check(rr(11)==0x80000303,"private lifecycle visibility");
 wr(4,2);wr(5,0xaaa);wr(7,1000);wr(17,0xf0);
 wr(3,7);waitdone();check(rr(10)==6&&writes==0,"unarmed private write");
 wr(19,0x43424e31);wr(19,0x43424e32);wr(3,7);
 check(rr(2)&2,"queued command not busy");
 wr(16,0);check(rr(16)==31,"owner changed during raw transaction");
 waitdone();check(rr(2)==5&&writes==1&&!d.writes_enabled,"raw write or legacy unlock leaked");
 wr(3,2);waitdone();check(rr(8)==0xa55a,"raw RX");
 wr(16,0);check(rr(16)!=31,"private exit");
 wr(3,1);waitdone();wr(4,2);wr(16,0);
 wr(5,0);wr(6,32);wr(7,10000);wr(3,2);
 wr(16,31);check(rr(16)!=31,"owner changed during legacy read");
 waitdone();check(!rr(10)&&rr(9)==32,"legacy read after private exit");
 wr(16,31);check(rr(16)==31&&rr(19)==0,"private reentry retained authorization");
 wr(4,1);wr(5,0x100);wr(7,1000);wr(3,2);waitdone();check(rr(8)==0x69,"GB private read");
 wr(19,0x43424e31);d.lifecycle_ok=0;tick();tick();d.lifecycle_ok=1;tick();
 wr(16,31);check(rr(16)==31&&rr(19)==0,"lifecycle authorization retained");
 check(rr(14)==1,"lifecycle generation not retained");
#ifdef TEST_SRAM137
 unsigned before_save=writes;
 wr(4,4);wr(5,32767);wr(7,2000);wr(3,2);waitdone();
 check(rr(9)==1&&rr(8)==0x69,"private SRAM byte read");
 wr(19,0x43424e31);wr(19,0x43424e32);wr(3,7);waitdone();
 check(rr(10)==7&&writes==before_save,"private SRAM write blocked");
 wr(3,6);waitdone();wr(16,0);check(rr(1)==0x02713f03,"SRAM returns legacy caps");
 wr(3,1);waitdone();wr(4,2);wr(16,0);wr(5,0);wr(6,32);wr(7,10000);wr(3,2);waitdone();
 check(rr(9)==32&&!rr(10),"ROM read after SRAM");
 wr(16,31);
 puts("SRAM137 router PASS: save byte read, write denied, ROM restored");
#endif
#ifdef PROGRAM131_TEST
 wr(4,2);wr(7,2000);
 wr(25,1);check(d.stream_lease_request && (rr(25)&1),"lease request not routed");
 d.stream_lease_busy=1;wr(16,0);check(rr(16)==31,"lease acquisition allowed exit");
 wr(25,0);check(!d.stream_lease_request && (rr(25)&2),"lease release/status not routed");
 d.stream_lease_busy=0;
 check((rr(25)&0xffff0000u)==0x53130000u,"stream capability missing");
 d.stream_lease_busy=1;d.lifecycle_ok=0;tick();tick();
 check((rr(25)&0xffff0002u)==0x53130002u,"lease drain invisible after lifecycle loss");
 d.stream_lease_busy=0;d.lifecycle_ok=1;tick();wr(16,31);wr(4,2);wr(7,2000);
 unsigned descriptor[16]={1,0,0x1000,2,0,0xaaa,0x554,0x005500aa,0x00290025,0x00a000f0,100000};
 auto submit=[&](){for(unsigned i=0;i<16;i++){wr(20,i);wr(21,descriptor[i]);}
  wr(24,0x43425331);wr(24,0x43425332);wr(22,1);};
 d.stream_authorized=0;d.stream_psram_granted=0;submit();
 check((rr(23)&0xf3)==0x32,"ungranted stream not rejected");
 d.stream_authorized=1;d.stream_psram_granted=1;
 wr(26,0x53413131);wr(26,0x53413132);check(rr(26)==1,"OS authority not armed");
 unsigned before=writes;submit();
 check(rr(23)&1,"stream not busy");wr(16,0);check(rr(16)==31,"stream released pin owner early");
 unsigned limit=0;while((rr(23)&1) && limit++<20000){};
 check(limit<20000 && (rr(23)&0xf3)==2,"stream routed program failed");
 check(writes==before+7 && !d.stream_hold_owner,"stream command count/drain");
 check(rr(26)==0,"completed stream retained OS authority");
 wr(16,0);check(rr(16)!=31,"stream owner did not release");
 std::puts("STREAM131 router PASS: denied grant, descriptor submission, program, guarded exit/drain");
#endif
#ifdef TEST_EEPROM139
 wr(16,31);d.stream_psram_granted=0;d.stream_authorized=0;
 check((rr(1)&0x10040u)==0x10040u,"EEPROM capability missing");
 unsigned ew=writes;
 wr(27,0x4000);wr(28,1);check(rr(28)&1,"EEPROM queued start");
 wr(16,0);check(rr(16)==31,"EEPROM owner switched early");
 wr(4,2);wr(3,7); // Raw request must not disturb serial sequence.
 unsigned el=0;while((rr(28)&1)&&el++<10000){
  check(!d.stream_p0_rd&&!d.stream_p1_rd,"EEPROM used PSRAM");
 }
 check(el<10000&&(rr(28)&7)==2,"EEPROM routed completion");
 check(rr(29)==0&&rr(30)==0&&writes==ew+17,"EEPROM payload/pulses");
 wr(28,1);for(unsigned i=0;i<100;i++)tick();wr(28,2);
 for(unsigned i=0;i<5;i++)tick();check((rr(28)&7)==4,"EEPROM routed cancel");
 wr(16,0);check(rr(16)!=31,"EEPROM owner failed to release");
 std::puts("EEPROM139 router PASS: queued submit, selector lock, raw exclusion, no PSRAM, readback, cancel, return");
#endif
 std::puts("NOR108 router PASS: legacy/private ownership, queued status, return to legacy, arm revocation");return 0;
 }catch(const std::exception&e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
