#include "Vpct_bridge_audit.h"
#include <verilated.h>
#include <cstdio>
#include <memory>
#include <vector>
#include <array>
static int pass,fail;
static void eq(const char *s,uint32_t got,uint32_t want) {
    if(got==want) ++pass; else {++fail; printf("FAIL %s %08x != %08x\n",s,got,want);}
}
struct Rig {
    std::unique_ptr<Vpct_bridge_audit> t{new Vpct_bridge_audit}; unsigned time=0;
    Rig() {
        t->clk_bridge=0;t->clk_cpu=0;t->open_request=0;t->flush_request=0;
        t->bridge_rd=0;t->bridge_wr=0;t->bridge_addr=0;t->bridge_wr_data_wire=0;
        t->tx_valid=0;t->tx_paired=1;t->tx_done=0;t->tx_abort=0;
        t->tx_address=0;t->tx_current_address=0;t->tx_data_wire=0;
        t->arm_read=0;t->freeze_read=0;t->data_pop=0;t->reset_cpu=1;
        steps(200);t->reset_cpu=0;steps(200);
    }
    void step(){++time;t->clk_bridge=(time/7)%2;t->clk_cpu=(time/5)%2;t->eval();}
    void steps(unsigned n){while(n--)step();}
    void pulse(unsigned kind,uint32_t addr=0,uint32_t data=0,uint32_t cur=0) {
        while(time%14)step();
        t->bridge_addr=addr;t->bridge_wr_data_wire=data;t->tx_address=addr;
        t->tx_current_address=cur;t->tx_data_wire=data;
        t->bridge_rd=kind==1;t->tx_valid=kind==2||kind==4;t->tx_paired=kind!=4;
        t->bridge_wr=kind==3;t->tx_abort=kind==5;t->tx_done=kind==6;
        t->open_request=kind==13;t->flush_request=kind==14;
        steps(14);
        t->bridge_rd=0;t->tx_valid=0;t->bridge_wr=0;t->tx_abort=0;t->tx_done=0;
        t->open_request=0;t->flush_request=0;steps(56);
    }
    void arm(){t->arm_read=1;steps(20);t->arm_read=0;steps(200);eq("arm ready",t->count_cpu>>31,1);}
    void freeze(){t->freeze_read=1;steps(20);t->freeze_read=0;steps(200);eq("frozen",t->status_cpu,1);}
    uint32_t pop(){while(time%10)step();steps(20);auto v=t->data_cpu;t->data_pop=1;steps(10);t->data_pop=0;steps(20);return v;}
    std::vector<uint32_t> read(){unsigned n=t->count_cpu&65535;std::vector<uint32_t> s;for(unsigned i=0;i<72+4*n;++i)s.push_back(pop());eq("EOF",pop(),0);return s;}
};
int main(int argc,char**argv){
    Verilated::commandArgs(argc,argv);
    {
        Rig r;r.pulse(13);r.pulse(1,0xf8001000);eq("prearm ignored",r.t->count_cpu,0);
        r.arm();r.pulse(13);
        const std::array<uint32_t,7> addr={0xf8001000,0xf8001004,0xf8001008,0xf8001020,0xf8001024,0xf8001028,0xf800102c};
        const std::array<uint32_t,7> open={0x636d0192,0x20,0x40,4,0x20000000,0,0};
        for(unsigned k=0;k<7;++k){r.pulse(1,addr[k]);r.pulse(2,addr[k],open[k],addr[k]);r.pulse(6);}
        r.pulse(3,0xf8001000,0x62750000);r.pulse(3,0xf8001000,0x6f6b0000);
        r.pulse(14);
        constexpr unsigned rounds=4096;
        for(unsigned j=0;j<rounds;++j) for(unsigned k=0;k<7;++k) {
            r.pulse(1,addr[k]);r.pulse(2,addr[k],k==0?0x636d0188:k==4?0:open[k],addr[k]);r.pulse(6);
        }
        // Revisit same field after other addresses: only a changed value logs.
        r.pulse(2,0xf8001000,0x62750000,0xf8001004);r.pulse(6);
        r.pulse(2,0xf8001000,0x636d0188,0xf8001004);r.pulse(6);
        // Canonical alias must be deduplicated, but counted.
        r.pulse(1,0xf8011000);r.pulse(2,0xf8011000,0x636d0188,0xf8011004);r.pulse(6);
        r.pulse(1,0xf8002020);r.pulse(2,0xf8002020,4,0xf8002024);r.pulse(6);
        r.pulse(1,0xf8002024);r.pulse(2,0xf8002024,1048613,0xf8001000);r.pulse(6);
        r.freeze();eq("no polling overflow or collision",r.t->count_cpu&0xffff0000u,0x40000);
        eq("compact two-phase record count",r.t->count_cpu&65535,39);
        auto s=r.read();eq("header magic",s[0],0x41554437);eq("header length",s[2],72);eq("phase",s[6],2);
        unsigned o=8,f=40;
        eq("OPEN read count",s[o],7);eq("OPEN TX count",s[o+1],7);eq("OPEN write count",s[o+2],2);
        eq("OPEN busy",s[o+7],1);eq("OPEN ok",s[o+8],1);eq("OPEN status value",s[o+9],0x6f6b0000);
        eq("OPEN command",s[o+12],0x636d0192);eq("OPEN TX done",s[o+16],7);
        eq("FLUSH full-span reads",s[f],rounds*7+3);eq("FLUSH full-span TX",s[f+1],rounds*7+5);
        eq("FLUSH no status write",s[f+6],0);eq("FLUSH command polls",s[f+4],rounds+1);
        eq("FLUSH table reads",s[f+3],2);eq("FLUSH command TX",s[f+12],0x636d0188);
        eq("FLUSH slot",s[f+13],4);eq("FLUSH size",s[f+14],1048613);eq("observed fields",s[f+15],7);
        eq("FLUSH TX done",s[f+16],rounds*7+5);eq("FLUSH TX all queue",s[f+26],rounds*7+5);
        eq("no events lost",s[f+30],0);eq("window ordering",s[f+18]>=s[o+19],1);
        // Ensure capture remains immutable after freezing.
        auto c=r.t->count_cpu;r.pulse(3,0xf8001000,0xdeadbeef);eq("frozen count stable",r.t->count_cpu,c);
        unsigned phases[3]={0,0,0};for(unsigned i=72;i<s.size();i+=4){++phases[(s[i]>>26)&3];}
        eq("both phases retained",phases[1]>0&&phases[2]>0,1);
    }
    {
        Rig r;r.arm();r.pulse(13);
        for(unsigned i=0;i<300;++i)r.pulse(3,0xf8001000,0x62750000+i);
        r.pulse(14);r.pulse(1,0xf8002020);r.pulse(2,0xf8002020,4,0xf8001000);r.pulse(5);
        r.pulse(3,0xf8001000,0x6f6b0000);r.freeze();eq("overflow flagged",(r.t->count_cpu>>16)&1,1);
        auto s=r.read();eq("writes counted after full",s[10],300);eq("OPEN dropped events",s[38],46);
        eq("FLUSH counted after full",s[40],1);eq("FLUSH OK after full",s[48],1);
        eq("abort counted",s[57],1);eq("all FLUSH candidate drops",s[70],4);
    }
    {
        Rig r;r.arm();r.pulse(13);
        while(r.time%14)r.step();r.t->bridge_rd=1;r.t->bridge_addr=0xf8001000;
        r.t->tx_valid=1;r.t->tx_address=0xf8001020;r.t->tx_data_wire=4;
        r.steps(14);r.t->bridge_rd=0;r.t->tx_valid=0;r.freeze();auto s=r.read();
        eq("collision flagged",(s[5]>>17)&1,1);eq("collided read counted",s[8],1);
        eq("collided TX counted",s[9],1);eq("lost event counted",s[38],1);
    }
    {
        Rig r;r.arm();r.pulse(13);
        // A sample on every bridge clock exercises synchronous history RAM
        // read/write collisions. Same-field consecutive equal values dedup;
        // changes, aliases and other fields retain their own histories.
        const uint32_t addresses[]={0xf8001000,0xf8001000,0xf8001000,0xf8011000,
                                    0xf8001020,0xf8001000,0xf8001000,0xf8001020};
        const uint32_t values[]={0x636d0192,0x636d0192,0x62750000,0x62750000,
                                 4,0x62750000,0x636d0192,4};
        while(r.time%14)r.step();
        r.t->tx_valid=1;r.t->tx_paired=1;
        for(unsigned i=0;i<8;++i){
            r.t->tx_address=addresses[i];r.t->tx_current_address=addresses[i];
            r.t->tx_data_wire=values[i];r.steps(14);
        }
        r.t->tx_valid=0;r.freeze();auto s=r.read();
        eq("adjacent samples all counted",s[9],8);
        eq("adjacent samples matching commands",s[19],3);
        eq("adjacent dedup record count",s[3],6);
        eq("adjacent no lost records",s[38],0);
        eq("adjacent command first",s[32],0x636d0192);
        eq("adjacent command last",s[20],0x636d0192);
        const uint32_t kept[]={0x636d0192,0x62750000,4,0x636d0192};
        for(unsigned i=0;i<4;++i){
            eq("adjacent event kind",s[72+(i+2)*4]>>28,2);
            eq("adjacent event wire value",s[74+(i+2)*4],kept[i]);
        }
    }
    {
        Rig r;r.arm();
        // OPEN edge and reply can coincide; it is counted in OPEN, while the
        // marker wins the log slot and reports the collision explicitly.
        while(r.time%14)r.step();r.t->open_request=1;r.t->tx_valid=1;
        r.t->tx_address=0xf8001000;r.t->tx_current_address=0xf8001000;
        r.t->tx_data_wire=0x636d0192;r.steps(14);
        r.t->open_request=0;r.steps(14);r.t->tx_valid=0;
        r.pulse(14);r.pulse(2,0xf8001000,0x636d0188,0xf8001000);
        r.freeze();auto s=r.read();
        eq("phase boundary count",s[9],2);
        eq("phase boundary command matches",s[19],2);
        eq("phase boundary lost",s[38],1);
        eq("phase boundary collision",(s[5]>>17)&1,1);
        eq("FLUSH history rearmed",s[41],1);
        eq("FLUSH first command",s[64],0x636d0188);
        eq("phase boundary record count",s[3],5);
    }
    {
        Rig r;r.arm();r.pulse(13);
        // OPENFILE's 264-byte structure lives in CRAM0.  Reads have no payload;
        // paired TX events carry the word actually queued for the host.
        r.pulse(1,0x20400000);r.pulse(2,0x20400000,0x2f536176,0x20400004);
        r.pulse(1,0x20400100);r.pulse(2,0x20400100,0x03000000,0x20400104);
        r.pulse(1,0x20400104);r.pulse(2,0x20400104,0x00200000,0xf8001000);
        r.freeze();auto s=r.read();
        eq("parameter F8 reads unchanged",s[8],0);
        eq("parameter tail queues next F8 TX",s[9],1);
        eq("parameter record count",s[3],8);
        for(unsigned i=0;i<3;++i){
            eq("parameter read kind",s[72+(2+i*2)*4]>>28,1);
            eq("parameter TX kind",s[72+(3+i*2)*4]>>28,2);
        }
        eq("parameter path wire word",s[72+3*4+2],0x2f536176);
        eq("parameter flags wire word",s[72+5*4+2],0x03000000);
        eq("parameter size wire word",s[72+7*4+2],0x00200000);
    }
    printf("Bridge audit: %d PASS, %d FAIL\n",pass,fail);return fail?1:0;
}
