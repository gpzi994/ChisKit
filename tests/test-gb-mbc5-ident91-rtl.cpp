#include "Vchisburner_cart_gb_mbc5_identify.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <vector>

struct Model {
    enum Mode { NORMAL, ID, CFI } mode=NORMAL;
    bool accept_00aa=true;
    bool prev_wr=false;
    unsigned reads=0;
    std::vector<uint32_t> writes;
};

static uint8_t normal_byte(uint16_t a) { return uint8_t((a*37u+0x5au)^0xa5u); }
static uint8_t cfi_byte(uint16_t a) {
    switch (a) {
    case 0x20:return 0x51; case 0x22:return 0x52; case 0x24:return 0x59;
    case 0x26:return 0x02; case 0x28:return 0x00;
    case 0x2a:return 0x40; case 0x2c:return 0x00;
    case 0x4e:return 0x19; case 0x54:return 0x09; case 0x56:return 0x00;
    case 0x58:return 0x01; case 0x5a:return 0xff; case 0x5c:return 0x00;
    case 0x5e:return 0x00; case 0x60:return 0x02;
    default:return uint8_t(a^0x6d);
    }
}
static uint8_t bus_byte(Model &m,uint16_t a) {
    ++m.reads;
    if (m.reads==1) return 0xee; // must be discarded internally
    if (m.mode==Model::ID) {
        if(a==0)return 0xc2; if(a==2)return 0x7e;
        if(a==0x1c)return 0x22; if(a==0x1e)return 0x00;
        return 0xff;
    }
    if (m.mode==Model::CFI) return cfi_byte(a);
    return normal_byte(a);
}
static void settle(Vchisburner_cart_gb_mbc5_identify &d,Model &m) {
    d.eval();
    uint16_t a=uint16_t(d.bank3_out)|(uint16_t(d.bank2_out)<<8);
    bool wr=d.bank0_dir && ((d.bank0_out & 4u)==0);
    bool rd=d.bank0_dir && ((d.bank0_out & 2u)==0);
    if (rd && d.bank3_dir && d.bank2_dir && !d.bank1_dir)
        d.bank3_in=bus_byte(m,a);
    if (wr && !m.prev_wr) {
        assert(d.bank3_dir && d.bank2_dir && d.bank1_dir);
        uint8_t v=d.bank1_out; m.writes.push_back((uint32_t(a)<<8)|v);
        static unsigned unlock=0;
        if (a==0 && (v==0xf0 || v==0xff)) { m.mode=Model::NORMAL; unlock=0; }
        else if (unlock==0 && a==0x0aaa && v==0xaa) unlock=1;
        else if (unlock==1 && a==0x0555 && v==0x55) unlock=2;
        else if (unlock==2 && a==0x0aaa && v==0x90) { m.mode=Model::ID; unlock=0; }
        else if (v==0x98 && ((a==0x00aa && m.accept_00aa) || a==0)) {
            m.mode=Model::CFI; unlock=0;
        } else unlock=0;
    }
    m.prev_wr=wr;
    d.eval();
}
static void tick(Vchisburner_cart_gb_mbc5_identify &d,Model &m) {
    d.clk=0; settle(d,m); d.clk=1; settle(d,m);
}
static void reg_write(Vchisburner_cart_gb_mbc5_identify &d,Model &m,
                      unsigned a,uint32_t v) {
    d.reg_addr=a; d.reg_wr_addr=a; d.reg_wdata=v; d.reg_wr=1; tick(d,m); d.reg_wr=0; tick(d,m);
}
static uint32_t reg_read(Vchisburner_cart_gb_mbc5_identify &d,unsigned a) {
    d.reg_addr=a; d.eval(); return d.reg_rdata;
}
static void reset(Vchisburner_cart_gb_mbc5_identify &d,Model &m) {
    d.clk=0; d.reset_n=0; d.lifecycle_ok=1; d.lifecycle_status=0x80000303;
    d.host_b1_raw=0; d.host_diag=0; d.reg_wr=0; d.reg_rd=0;
    d.reg_addr=0; d.reg_wr_addr=0; d.reg_wdata=0; d.bank3_in=0; d.bank2_in=0;
    tick(d,m); d.reset_n=1; tick(d,m);
}
static void run(bool accept_first) {
    Vchisburner_cart_gb_mbc5_identify d; Model m; m.accept_00aa=accept_first;
    reset(d,m);
    assert(reg_read(d,0)==0x31424746u);
    assert(reg_read(d,1)==0x00100f01u);
    reg_write(d,m,4,1); reg_write(d,m,16,4); reg_write(d,m,5,0);
    reg_write(d,m,6,32); reg_write(d,m,7,100000); reg_write(d,m,3,2);
    for(unsigned i=0;i<100000 && (reg_read(d,2)&2u);++i) tick(d,m);
    assert(reg_read(d,2)==5u && reg_read(d,9)==32u);
    uint8_t got[32];
    for(unsigned off=0;off<32;off+=4) {
        d.reg_addr=8; d.reg_rd=1; d.eval(); uint32_t w=d.reg_rdata;
        for(unsigned j=0;j<4;++j) got[off+j]=uint8_t(w>>(8*j));
        tick(d,m); d.reg_rd=0; tick(d,m);
    }
    const uint16_t ids[4]={0,2,0x1c,0x1e};
    for(unsigned i=0;i<4;++i) assert(got[i]==normal_byte(ids[i]));
    assert(got[4]==0xc2 && got[5]==0x7e && got[6]==0x22 && got[7]==0);
    assert(got[8]==0x51 && got[9]==0x52 && got[10]==0x59);
    assert(got[24]==0x19 && got[25]==9 && got[27]==1);
    uint32_t profile=reg_read(d,20);
    assert((profile&0x13u)==0x13u);
    assert(((profile>>2)&3u)==(accept_first?1u:2u));
    // No erase/program byte appears, and only Mori's fixed addresses occur.
    for(uint32_t w:m.writes) {
        uint16_t a=uint16_t(w>>8); uint8_t v=uint8_t(w);
        assert((a==0 && (v==0xf0 || v==0xff)) ||
               (a==0x0aaa && (v==0xaa || v==0x90)) ||
               (a==0x0555 && v==0x55) ||
               (a==0x00aa && v==0x98) || (a==0 && v==0x98));
        assert(v!=0x80 && v!=0xa0 && v!=0x25 && v!=0x29 && v!=0x30);
    }
    size_t expected=accept_first?7u:9u;
    assert(m.writes.size()==expected);
}
int main() {
    run(true); run(false);
    std::puts("identify91 RTL: first-read discard + fixed JEDEC + dual CFI entry + matched reset + no erase/program PASS");
    return 0;
}
