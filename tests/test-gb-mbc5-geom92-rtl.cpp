#include "Vchisburner_cart_gb_mbc5_identify.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <vector>

struct Model {
    enum Mode { NORMAL, ID, CFI } mode=NORMAL;
    bool prev_wr=false;
    unsigned reads=0;
    std::vector<uint32_t> writes;
};

static uint8_t normal_byte(uint16_t a) { return uint8_t((a*37u+0x5au)^0xa5u); }
static uint8_t cfi_byte(uint16_t a) {
    switch(a) {
    case 0x20:return 0x51; case 0x22:return 0x52; case 0x24:return 0x59;
    case 0x4e:return 0x17; case 0x54:return 0x05; case 0x56:return 0x00;
    case 0x58:return 0x02; case 0x5a:return 0x07; case 0x5c:return 0x00;
    case 0x5e:return 0x20; case 0x60:return 0x00; case 0x62:return 0x7e;
    case 0x64:return 0x00; case 0x66:return 0x00; case 0x68:return 0x01;
    case 0x80:return 0x50; case 0x82:return 0x52; case 0x84:return 0x49;
    case 0x86:return 0x31; case 0x88:return 0x31; case 0x8a:return 0x00;
    case 0x8c:return 0x02; case 0x9e:return 0x02;
    default:return 0xff;
    }
}

static uint8_t bus_byte(Model &m,uint16_t a) {
    ++m.reads;
    if(m.reads==1) return 0xee;
    if(m.mode==Model::ID) {
        if(a==0)return 0xc2; if(a==2)return 0x7e;
        if(a==0x1c)return 0x10; if(a==0x1e)return 0x00;
        return 0xff;
    }
    return m.mode==Model::CFI ? cfi_byte(a) : normal_byte(a);
}

static void settle(Vchisburner_cart_gb_mbc5_identify &d,Model &m) {
    d.eval();
    uint16_t a=uint16_t(d.bank3_out)|(uint16_t(d.bank2_out)<<8);
    bool wr=d.bank0_dir && ((d.bank0_out&4u)==0);
    bool rd=d.bank0_dir && ((d.bank0_out&2u)==0);
    if(rd && d.bank3_dir && d.bank2_dir && !d.bank1_dir)
        d.bank3_in=bus_byte(m,a);
    if(wr && !m.prev_wr) {
        uint8_t v=d.bank1_out; m.writes.push_back((uint32_t(a)<<8)|v);
        static unsigned unlock=0;
        if(a==0 && (v==0xf0 || v==0xff)) { m.mode=Model::NORMAL; unlock=0; }
        else if(unlock==0 && a==0x0aaa && v==0xaa) unlock=1;
        else if(unlock==1 && a==0x0555 && v==0x55) unlock=2;
        else if(unlock==2 && a==0x0aaa && v==0x90) { m.mode=Model::ID; unlock=0; }
        else if(a==0x00aa && v==0x98) { m.mode=Model::CFI; unlock=0; }
        else unlock=0;
    }
    m.prev_wr=wr; d.eval();
}
static void tick(Vchisburner_cart_gb_mbc5_identify &d,Model &m) {
    d.clk=0; settle(d,m); d.clk=1; settle(d,m);
}
static void wr(Vchisburner_cart_gb_mbc5_identify &d,Model &m,unsigned a,uint32_t v) {
    d.reg_addr=a; d.reg_wr_addr=a; d.reg_wdata=v; d.reg_wr=1; tick(d,m); d.reg_wr=0; tick(d,m);
}
static uint32_t rd(Vchisburner_cart_gb_mbc5_identify &d,unsigned a) {
    d.reg_addr=a; d.eval(); return d.reg_rdata;
}

int main() {
    Vchisburner_cart_gb_mbc5_identify d; Model m;
    d.clk=0; d.reset_n=0; d.lifecycle_ok=1; d.lifecycle_status=0x80000303;
    d.host_b1_raw=0; d.host_diag=0; d.reg_wr=0; d.reg_rd=0;
    d.reg_addr=0; d.reg_wr_addr=0; d.reg_wdata=0; d.bank3_in=0; d.bank2_in=0;
    tick(d,m); d.reset_n=1; tick(d,m);
    wr(d,m,4,1); wr(d,m,16,4); wr(d,m,5,0); wr(d,m,6,32);
    wr(d,m,7,100000); wr(d,m,3,2);
    for(unsigned i=0;i<100000 && (rd(d,2)&2u);++i) tick(d,m);
    assert(rd(d,2)==5u && rd(d,9)==32u);
    uint8_t got[32];
    for(unsigned off=0;off<32;off+=4) {
        d.reg_addr=8; d.reg_rd=1; d.eval(); uint32_t w=d.reg_rdata;
        for(unsigned j=0;j<4;++j) got[off+j]=uint8_t(w>>(8*j));
        tick(d,m); d.reg_rd=0; tick(d,m);
    }
    const uint8_t expected[32]={
        0xc2,0x7e,0x10,0x00,
        0x07,0x00,0x20,0x00,0x7e,0x00,0x00,0x01,
        0x07,0x00,0x20,0x00,0x7e,0x00,0x00,0x01,
        0x50,0x52,0x49,0x31,0x31,0x00,0x02,0x02,
        0x17,0x05,0x00,0x02};
    for(unsigned i=0;i<32;++i) assert(got[i]==expected[i]);
    assert((rd(d,20)&0x17u)==0x17u);
    for(uint32_t w:m.writes) {
        uint16_t a=uint16_t(w>>8); uint8_t v=uint8_t(w);
        assert((a==0 && (v==0xf0 || v==0xff)) ||
               (a==0x0aaa && (v==0xaa || v==0x90)) ||
               (a==0x0555 && v==0x55) || (a==0x00aa && v==0x98));
        assert(v!=0x80 && v!=0xa0 && v!=0x25 && v!=0x29 && v!=0x30);
    }
    std::puts("geometry92 RTL: repeated two-region CFI + PRI bottom boot + fixed safe commands PASS");
    return 0;
}
