#include "Vchisburner_cart_gba_readonly.h"
#include <cstdint>
#include <cstdio>
#include <stdexcept>
static void ck(bool b){if(!b)throw std::runtime_error("dump53 RTL assertion");}
static void tick(Vchisburner_cart_gba_readonly &d){
    d.clk=0;d.eval();ck((d.bank0_out&4) && (!d.pin30_dir || d.pin30_out) && !d.writes_enabled && !d.pin31_dir);
    d.clk=1;d.eval();
}
static void wr(Vchisburner_cart_gba_readonly &d,unsigned a,uint32_t v){d.reg_addr=a;d.reg_wdata=v;d.reg_wr=1;tick(d);d.reg_wr=0;}
static uint32_t rd(Vchisburner_cart_gba_readonly &d,unsigned a){d.reg_addr=a;d.reg_rd=0;d.eval();return d.reg_rdata;}
static bool safe(Vchisburner_cart_gba_readonly &d){return !d.bank3_dir && !d.bank2_dir && !d.bank1_dir && d.bank0_dir && d.bank0_out==15 && !d.pin30_dir;}
static void reset(Vchisburner_cart_gba_readonly &d){d.reset_n=0;d.reg_wr=d.reg_rd=0;d.lifecycle_ok=1;tick(d);d.reset_n=1;tick(d);ck(safe(d));}
static void prepare(Vchisburner_cart_gba_readonly &d,uint32_t off,unsigned n=32,unsigned space=0,unsigned bus=2,unsigned timeout=4000){
    wr(d,3,1);wr(d,4,bus);wr(d,16,space);wr(d,5,off);wr(d,6,n);wr(d,7,timeout);wr(d,3,2);
}
static void block(Vchisburner_cart_gba_readonly &d,uint32_t off){
    prepare(d,off);ck(d.bank3_dir && d.bank2_dir && d.bank1_dir);
    ck((uint32_t(d.bank3_out)|(uint32_t(d.bank2_out)<<8)|(uint32_t(d.bank1_out)<<16))==off/2);
    unsigned windows=0;bool was=false;unsigned cycles=0;
    while(!(rd(d,2)&4) && cycles++<3000){
        bool active=(d.bank0_out&3)==0;if(active && !was)++windows;was=active;
        if(active)ck(!d.bank2_dir && !d.bank3_dir);
        d.bank3_in=uint8_t(off+windows);d.bank2_in=uint8_t((off>>8)+windows);tick(d);
    }
    ck(cycles<3000 && windows==16 && rd(d,9)==32 && safe(d));
    for(unsigned i=0;i<8;++i){
        unsigned j=2*i+1;uint32_t w=uint8_t(off+j)|(uint32_t(uint8_t((off>>8)+j))<<8)|
            (uint32_t(uint8_t(off+j+1))<<16)|(uint32_t(uint8_t((off>>8)+j+1))<<24);
        ck(rd(d,8)==w);d.reg_rd=1;tick(d);d.reg_rd=0;
    }
    ck(rd(d,9)==0);
}
int main(){
    Vchisburner_cart_gba_readonly d;reset(d);ck(rd(d,1)==0x00100f02);
    for(uint32_t off:{2u,16u,8705280u,0xffffffe0u}){prepare(d,off);ck(rd(d,10)==1 && safe(d));}
    for(unsigned n:{0u,16u,64u}){prepare(d,0,n);ck(rd(d,10)==1 && safe(d));}
    for(unsigned space:{1u,2u,3u}){prepare(d,0,32,space);ck(rd(d,10)==1 && safe(d));}
    prepare(d,0,32,0,1);ck(rd(d,10)==1 && safe(d));
    d.lifecycle_ok=0;prepare(d,0);ck(rd(d,10)==3 && safe(d));d.lifecycle_ok=1;
    prepare(d,0);tick(d);d.lifecycle_ok=0;tick(d);ck(safe(d) && rd(d,10)==3);reset(d);
    prepare(d,0,32,0,2,2);for(unsigned i=0;i<4;++i)tick(d);ck(safe(d) && (rd(d,2)&32));reset(d);
    prepare(d,0);wr(d,3,3);ck(safe(d) && (rd(d,2)&16));reset(d);
    for(uint32_t off:{0u,160u,4194304u,8388608u,8705248u})block(d,off);
    for(uint32_t off=0;off<8705280;off+=32)block(d,off);
    prepare(d,0);ck(rd(d,10)==1 && safe(d));
    wr(d,3,1);prepare(d,0);ck(rd(d,10)==1 && safe(d));
    puts("dump53 RTL: 272045 reads + FIFO/address/data + exact limit + save rejection + timeout/lifecycle/abort + write pins PASS");
}
