#include "Vtb_storage_tx_tap.h"
#include <verilated.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdint>
static int pass,fail;
static void eq(const char *name,uint32_t got,uint32_t want){
    if(got==want)++pass;else{++fail;printf("FAIL %s: %08x != %08x\n",name,got,want);}
}
static uint32_t swap(uint32_t v){return(v>>24)|((v>>8)&0xff00)|((v<<8)&0xff0000)|(v<<24);}
struct Event{uint64_t time;unsigned signal,value;};
static constexpr uint64_t P=13468,H=P/2;
static void run(unsigned phase,bool little,bool abort){
    auto context=std::make_unique<VerilatedContext>();
    auto t=std::make_unique<Vtb_storage_tx_tap>(context.get());
    t->clk=0;t->endian_little=little;t->host_oe=1;t->host_clk=0;
    t->host_mosi=0;t->host_miso=0;t->host_ss=1;
    t->rd_data=little?swap(0x636d0188):0x636d0188;t->eval();
    const uint32_t addr=0xf8001000;
    std::vector<Event> events{{40*P,0,0}};
    for(unsigned k=0;k<16;++k){
        uint64_t edge=(42+k)*P;
        events.push_back({edge-H,1,0});
        events.push_back({edge-H,2,(addr>>(31-2*k))&1});
        events.push_back({edge-H,3,(addr>>(30-2*k))&1});
        events.push_back({edge,1,1});
    }
    events.push_back({57*P+H,1,0});events.push_back({58*P,4,0});
    std::stable_sort(events.begin(),events.end(),[](const Event&a,const Event&b){return a.time<b.time;});
    uint64_t time=0,next_clk=phase?P*phase/32:P;
    unsigned i=0,q=0,d=0,a=0,reads=0;bool clk=false,abort_pending=false;
    while(time<200*P){
        time=next_clk;if(i<events.size())time=std::min(time,events[i].time);
        while(i<events.size()&&events[i].time==time){
            auto e=events[i++];
            if(e.signal==0)t->host_ss=e.value;
            if(e.signal==1)t->host_clk=e.value;
            if(e.signal==2)t->host_mosi=e.value;
            if(e.signal==3)t->host_miso=e.value;
            if(e.signal==4)t->host_oe=e.value;
        }
        bool rising=false;
        if(time==next_clk){
            if(abort_pending)t->host_ss=1;
            clk=!clk;t->clk=clk;next_clk+=H;rising=clk;
        }
        t->eval();
        if(rising){
            if(t->queued){
                ++q;eq("queued current address",t->current_address,addr);
                eq("exact queued wire data",t->data,0x636d0188);
                eq("first reply marked unpaired",t->paired,0);
                if(abort)abort_pending=true;
            }
            d+=t->done;a+=t->aborted;reads+=t->pmp_rd;
        }
    }
    eq("single queue",q,1);eq("normal done vs abort",d,abort?0:1);
    eq("abort vs normal",a,abort?1:0);eq("single core read",reads,1);
}
int main(int argc,char **argv){
    Verilated::commandArgs(argc,argv);
    for(unsigned ph=0;ph<32;++ph)for(bool little:{false,true})for(bool abort:{false,true})run(ph,little,abort);
    printf("Serial state taps: %d PASS, %d FAIL\n",pass,fail);
    puts("Checks queued word and local completion/abort only; NOT electrical TX waveform or Pocket receipt.");
    return fail?1:0;
}
