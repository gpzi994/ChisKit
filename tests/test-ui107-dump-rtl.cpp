#include "Vchisburner_cart_auto98.h"

#include <cstdint>
#include <cstdio>
#include <stdexcept>

static uint16_t selected_bank = 1; static unsigned kind=0, high_bank=0, mbc_mode=0;
static bool previous_wr = false;
static bool previous_rd = false;
static unsigned mapper_write_windows = 0;
static uint64_t physical_read_windows = 0;
static bool expect_discard = true;

static void check(bool ok, const char *what) {
    if (!ok) throw std::runtime_error(what);
}

static uint8_t rom_byte(uint32_t logical) {
    return static_cast<uint8_t>((logical * 13u) ^ (logical >> 8) ^
                                (logical >> 14) ^ ((logical >> 22) * ((logical & 1) ? 0xa5u : 0x37u)) ^ 0x5au);
}

static void observe(Vchisburner_cart_auto98 &d){
 bool wr=(d.bank0_out&4)==0,rd=(d.bank0_out&2)==0;
 check(!(wr&&rd),"read/write overlap");check(!d.writes_enabled,"NOR capability escaped");
 uint16_t a=d.bank3_out|(uint16_t(d.bank2_out)<<8);
 if(wr){
  check(kind!=0&&d.bank1_dir&&d.bank2_dir&&d.bank3_dir,"write directions/type");
  uint8_t v=d.bank1_out;
  if(!previous_wr){
   ++mapper_write_windows;
   if(kind==1){
    check((a==0x6000&&v<=1)||(a==0x4000&&v<=3)||(a==0x2000&&v>=1&&v<=31),"MBC1 write escaped");
    if(a==0x6000)mbc_mode=v;else if(a==0x4000)high_bank=v;else selected_bank=v;
   }else if(kind==5){
    check((a==0x3000&&v<=1)||a==0x2000,"MBC5 write escaped");
    if(a==0x3000)high_bank=v;else selected_bank=v;
   }else{
    check(a==(kind==2?0x2100:0x2000),"write escaped mapper register");
    check(kind!=2||v<16,"MBC2 bank range");check(kind!=3||v<128,"MBC3 bank range");
    selected_bank=(kind==2||kind==3||kind==4)&&v==0?1:v;
   }
  }
 }
 if(rd){
  check(a<=0x7fff&&!d.bank1_dir&&d.bank2_dir&&d.bank3_dir,"ROM read window");
  unsigned bank=selected_bank;
  if(kind==1)bank=(high_bank<<5)|selected_bank;
  if(kind==5)bank=(high_bank<<8)|selected_bank;
  uint32_t logical=a;
  if(a<0x4000&&kind==1&&mbc_mode)logical=(high_bank<<19)+a;
  else if(a>=0x4000&&kind!=0)logical=bank*0x4000+(a-0x4000);
  if(!previous_rd)++physical_read_windows;
  uint8_t v=rom_byte(logical);if(physical_read_windows<=32)v^=255;d.gb_data_in=v;
 }
 previous_wr=wr;previous_rd=rd;
}
static void tick(Vchisburner_cart_auto98 &dut) {
    dut.clk=0; dut.eval(); observe(dut);
    dut.clk=1; dut.eval(); observe(dut);
}

static uint32_t reg_read(Vchisburner_cart_auto98 &dut, unsigned a) {
    dut.reg_rd=0; dut.reg_addr=a; tick(dut); dut.reg_rd=1; dut.eval();
    if(a==2)tick(dut); return dut.reg_rdata;
}

static void reg_write(Vchisburner_cart_auto98 &dut,
                      unsigned a, uint32_t v) {
    dut.reg_rd=0; dut.reg_addr=a; dut.reg_wdata=v; dut.reg_wr=1; tick(dut); dut.reg_wr=0; tick(dut);
}

static bool safe_idle(Vchisburner_cart_auto98 &dut) {
    return !dut.bank3_dir && !dut.bank2_dir && !dut.bank1_dir &&
           dut.bank0_dir && dut.bank0_out == 0xfu &&
           !dut.pin30_dir && !dut.pin31_dir && !dut.writes_enabled;
}

static void reset(Vchisburner_cart_auto98 &dut) {
    dut.reset_n=0; dut.reg_wr=0; dut.reg_rd=0; dut.lifecycle_ok=1;
    dut.lifecycle_status=0x3fu; dut.host_b1_raw=0; dut.host_diag=0;
    dut.gb_data_in=0; dut.bank2_in=0; selected_bank=1; high_bank=mbc_mode=0;
    previous_wr=false; previous_rd=false; mapper_write_windows=0;
    physical_read_windows=0; expect_discard=true;
    tick(dut); dut.reset_n=1; tick(dut);
    check(safe_idle(dut), "reset not safe idle");
}

static void begin_read(Vchisburner_cart_auto98 &dut,
                       uint32_t address, uint32_t length=32,
                       uint32_t bus=1, uint32_t space=6) {
    reg_write(dut,4,bus); reg_write(dut,16,space);
    reg_write(dut,5,address); reg_write(dut,6,length); reg_write(dut,7,100000);
    reg_write(dut,3,2);
    (void)reg_read(dut,2); // Real OS polls status, including rejected commands.
}

static void read_block(Vchisburner_cart_auto98 &dut,
                       uint32_t request, uint32_t logical) {
    uint64_t before=physical_read_windows;
    begin_read(dut,request,32,1,16+kind);
    unsigned cycles=0;
    while ((reg_read(dut,2)&4u)==0 && cycles++<100000) tick(dut);
    check(cycles<100000,"command timeout");
    uint64_t expect_reads=request==0 && expect_discard ? 64u : 32u;
    check(physical_read_windows-before==expect_reads,
          "wrong physical read/discard count");
    if (request==0 && expect_discard) expect_discard=false;
    check(selected_bank==1 && high_bank==0 && mbc_mode==0,"mapper not restored");
    check(safe_idle(dut),"command completion not safe idle");
    check(reg_read(dut,9)==32,"wrong FIFO level");
    for (unsigned word=0;word<8;++word) {
        uint32_t data=reg_read(dut,8);
        for (unsigned lane=0;lane<4;++lane) {
            unsigned i=word*4+lane;
            check(static_cast<uint8_t>(data>>(lane*8))==rom_byte(logical+i),
                  "mapped ROM byte mismatch");
        }
        dut.reg_addr=8; dut.reg_rd=1; tick(dut); dut.reg_rd=0;
    }
}

int main(int argc,char**argv){
 check(argc==2,"supply mapper kind");kind=unsigned(argv[1][0]-'0');check(kind<8,"kind");
 const uint32_t limits[]={32768,2097152,262144,2097152,4194304,8388608,4194304,1048576};
 uint32_t total=limits[kind];Vchisburner_cart_auto98 d;reset(d);
 begin_read(d,32,32,1,16+kind);check(reg_read(d,10)==1&&safe_idle(d),"out of order accepted");
 begin_read(d,0,16,1,16+kind);check(reg_read(d,10)==1&&safe_idle(d),"short accepted");
 for(uint32_t off=0;off<total;off+=32)read_block(d,off,off);
 check(physical_read_windows==uint64_t(total)+32,"physical count");
 begin_read(d,total,32,1,16+kind);check(reg_read(d,10)==1&&safe_idle(d),"capacity overrun");
 begin_read(d,0,32,1,16+kind);check(reg_read(d,10)==1&&safe_idle(d),"repeat accepted");
 reg_write(d,3,1);expect_discard=true;read_block(d,0,0);
 // Cannot change mapper in an active sequence.
 (void)reg_read(d,2);tick(d);tick(d);d.reg_rd=0;
 begin_read(d,32,32,1,16+((kind+1)%8));for(int i=0;i<12;i++)tick(d);
 check(reg_read(d,10)==1&&safe_idle(d),"mapper changed mid dump");
 reset(d);begin_read(d,0,32,1,16+kind);while(d.bank0_out&2)tick(d);
 reg_write(d,3,3);for(int i=0;i<20;i++)tick(d);check(safe_idle(d),"abort unsafe");
 reset(d);begin_read(d,0,32,1,16+kind);while(d.bank0_out&2)tick(d);
 d.lifecycle_ok=0;for(int i=0;i<20;i++)tick(d);check(safe_idle(d),"lifecycle unsafe");
 printf("PASS UI107 mapper %u: full %u bytes, boundaries, restore, no SRAM/RTC/NOR writes\n",kind,total);
}
