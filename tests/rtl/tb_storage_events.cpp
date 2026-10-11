#include "Vpct_bridge_trace.h"
#include <verilated.h>
#include <cstdio>
#include <memory>
#include <vector>
#include <array>
static int pass, fail;
static void eq(const char *name, uint32_t got, uint32_t expected) {
    if (got == expected) ++pass;
    else { ++fail; std::printf("FAIL %s: %08x != %08x\n",name,got,expected); }
}
struct Rig {
    std::unique_ptr<Vpct_bridge_trace> t{new Vpct_bridge_trace};
    unsigned time = 0;
    Rig() {
        t->clk_bridge=0; t->clk_cpu=0; t->arm=0; t->bridge_rd=0;
        t->bridge_wr=0; t->bridge_addr=0; t->bridge_wr_data_wire=0;
        t->tx_valid=0; t->tx_paired=1; t->tx_address=0;
        t->tx_current_address=0; t->tx_data_wire=0;
        t->freeze_read=0; t->data_pop=0; t->reset_cpu=1;
        steps(200); t->reset_cpu=0; steps(200);
    }
    void step() {
        ++time;
        t->clk_bridge=(time/7)%2; t->clk_cpu=(time/5)%2; t->eval();
    }
    void steps(unsigned n) { while(n--) step(); }
    void pulse(unsigned kind, uint32_t addr, uint32_t data=0, uint32_t current=0) {
        while(time%14) step();
        t->bridge_addr=addr; t->bridge_wr_data_wire=data;
        t->tx_address=addr; t->tx_current_address=current; t->tx_data_wire=data;
        t->arm=kind==15; t->bridge_rd=kind==1; t->tx_valid=kind==2 || kind==4;
        t->tx_paired=kind!=4; t->bridge_wr=kind==3;
        steps(14);
        t->arm=0; t->bridge_rd=0; t->tx_valid=0; t->bridge_wr=0;
        steps(56);
    }
    void freeze() {
        t->freeze_read=1; steps(20); t->freeze_read=0; steps(200);
        eq("freeze acknowledged",t->status_cpu,1);
    }
    uint32_t pop() {
        while(time%10) step();
        steps(20);
        uint32_t value=t->data_cpu;
        t->data_pop=1; steps(10); t->data_pop=0; steps(20);
        return value;
    }
};
int main(int argc,char **argv) {
    Verilated::commandArgs(argc,argv);
    {
        Rig r;
        r.pulse(1,0xf8001000); // pre-arm ignored
        r.pulse(15,0);
        std::vector<std::array<uint32_t,4>> want;
        auto event=[&](unsigned kind,uint32_t addr,uint32_t data,uint32_t cur=0){
            r.pulse(kind,addr,data,cur); want.push_back({kind,addr,data,cur});
        };
        event(1,0xf8001000,0);
        r.pulse(1,0xf8001000); // suppress identical request
        event(2,0xf8001000,0x636d0188,0xf8001020);
        r.pulse(2,0xf8001000,0x636d0188,0xf8001020);
        event(1,0xf8001020,0);
        event(2,0xf8001020,4,0xf8002020);
        event(1,0xf8002020,0);
        event(2,0xf8002020,4,0xf8002024);
        event(1,0xf8002024,0);
        event(2,0xf8002024,1048613,0xf8001000);
        event(3,0xf8001000,0x62750000);
        event(3,0xf8001000,0x6f6b0000);
        r.freeze();
        eq("count and armed flag",r.t->count_cpu,0x40000u+want.size()+1);
        r.pulse(3,0xf8001000,0xdeadbeef); // frozen: must not alter record
        eq("arm kind",r.pop()>>28,15); r.pop(); r.pop(); r.pop();
        uint32_t last_time=0;
        for (const auto &e:want) {
            uint32_t meta=r.pop();
            eq("event kind",meta>>28,e[0]);
            eq("monotonic timestamp",(meta&0xfffffff)>=last_time,1);
            last_time=meta&0xfffffff;
            eq("paired address",r.pop(),e[1]);
            eq("exact wire word",r.pop(),e[2]);
            eq("current transfer address",r.pop(),e[3]);
        }
        eq("end of stream",r.pop(),0);
    }
    {
        Rig r; r.pulse(15,0);
        for(unsigned i=0;i<300;++i) r.pulse(2,0xf8002000+4*i,i,0xf8001000);
        r.freeze(); eq("bounded overflow",r.t->count_cpu,0x50100);
        for(unsigned i=0;i<256;++i) {
            eq("overflow retained first events",r.pop()>>28,i?2:15);
            eq("overflow address",r.pop(),i?0xf8002000+4*(i-1):0);
            eq("overflow data",r.pop(),i?i-1:0); r.pop();
        }
        eq("overflow end",r.pop(),0);
    }
    {
        Rig r; r.pulse(15,0);
        r.t->bridge_rd=1; r.t->bridge_addr=0xf8001000;
        r.t->tx_valid=1; r.t->tx_address=0xf8001020; r.t->tx_data_wire=4;
        r.steps(28); r.t->bridge_rd=0; r.t->tx_valid=0;
        r.freeze(); eq("collision explicit",(r.t->count_cpu>>17)&1,1);
    }
    std::printf("Event recorder: %d PASS, %d FAIL\n",pass,fail);
    return fail?1:0;
}
