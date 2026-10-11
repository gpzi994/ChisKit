// Bounded RV32I runner for the actual linked OS bind service, not the host/APF.
// Only the two pinned read-only OS callees are mocked. Any MMIO load/store,
// other external jump, ABI clobber, or unbounded execution fails closed.
#include <array>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
static unsigned checks;
static void ck(bool v){++checks;if(!v)throw std::runtime_error("RV32 static50 assertion");}
static int32_t sx(uint32_t v,unsigned n){return int32_t(v<<(32-n))>>(32-n);}
int main(int argc,char **argv) {
    if(argc!=2)return 2;
    std::ifstream f(argv[1],std::ios::binary);
    std::vector<uint8_t> code{std::istreambuf_iterator<char>(f),{}};
    ck(code.size()>100 && code.size()<=640);
    constexpr uint32_t base=0x1032f180,sp0=0x12001000,ptr=0x12002000,stop=0xdead0000;
    for(unsigned namecase=0;namecase<15;++namecase)
    for(int size:{0,511,512,513}) for(int busy:{-12,0,1}) for(int actual:{-1,512,513}) {
        std::map<uint32_t,uint8_t> mem;
        for(unsigned i=0;i<code.size();++i)mem[base+i]=code[i];
        std::string name="static50.bin";name.push_back(0);
        if(namecase>=2)name[namecase-2]^=1;
        for(unsigned i=0;i<name.size();++i)mem[ptr+i]=name[i];
        auto put=[&](uint32_t a,uint32_t v){for(unsigned i=0;i<4;++i)mem[a+i]=uint8_t(v>>(8*i));};
        auto get=[&](uint32_t a,unsigned n){uint32_t v=0;for(unsigned i=0;i<n;++i){ck(mem.count(a+i));v|=uint32_t(mem[a+i])<<(8*i);}return v;};
        put(0x1038e530,17);put(0x1038e534,19);put(0x1038e538,18);
        std::array<uint32_t,32> x{};
        for(unsigned i=0;i<32;++i)x[i]=0xabc00000+i;
        x[0]=0;x[1]=stop;x[2]=sp0;x[10]=namecase?ptr:0;x[11]=size;
        auto initial=x;uint32_t pc=base;unsigned steps=0,bc=0,sc=0,stores=0;
        for(;steps<500 && pc!=stop;++steps) {
            if(pc==0x10328100 || pc==0x103297c0) {
                if(pc==0x10328100){++bc;x[10]=busy;}
                else {++sc;ck(x[10]==4);x[10]=actual;}
                pc=x[1];continue;
            }
            ck(pc>=base && pc+4<=base+code.size() && !(pc&3));
            uint32_t w=get(pc,4),next=pc+4;
            unsigned op=w&127,rd=(w>>7)&31,fn=(w>>12)&7,a=(w>>15)&31,b=(w>>20)&31;
            if(op==0x37)x[rd]=w&0xfffff000;
            else if(op==0x17)x[rd]=pc+(w&0xfffff000);
            else if(op==0x13){ck(fn==0);x[rd]=x[a]+sx(w>>20,12);}
            else if(op==0x33){ck(fn==0 && (w>>25)==0);x[rd]=x[a]+x[b];}
            else if(op==0x03){
                ck(fn==2||fn==4);uint32_t addr=x[a]+sx(w>>20,12);
                ck((addr>=base && addr<base+code.size()) || (addr>=ptr && addr<ptr+13) || (addr>=sp0-16 && addr<sp0));
                x[rd]=get(addr,fn==2?4:1);
            } else if(op==0x23){
                ck(fn==2);uint32_t addr=x[a]+sx(((w>>25)<<5)|((w>>7)&31),12);
                ck((addr>=sp0-16 && addr<sp0) || addr==0x1038e530 || addr==0x1038e534 || addr==0x1038e538);
                if(addr<sp0-16 || addr>=sp0)++stores;
                put(addr,x[b]);
            } else if(op==0x63){
                int imm=sx(((w>>31)<<12)|(((w>>7)&1)<<11)|(((w>>25)&63)<<5)|(((w>>8)&15)<<1),13);
                ck(fn==0||fn==1||fn==5);
                bool take=fn==0?x[a]==x[b]:fn==1?x[a]!=x[b]:int32_t(x[a])>=int32_t(x[b]);
                if(take)next=pc+imm;
            } else if(op==0x6f){
                int imm=sx(((w>>31)<<20)|(((w>>12)&255)<<12)|(((w>>20)&1)<<11)|(((w>>21)&1023)<<1),21);
                x[rd]=pc+4;next=pc+imm;
            } else if(op==0x67){ck(fn==0);next=(x[a]+sx(w>>20,12))&~1u;x[rd]=pc+4;}
            else ck(false);
            x[0]=0;pc=next;
        }
        ck(pc==stop && steps<500 && x[2]==sp0);
        for(unsigned reg:{3u,4u,8u,9u,18u,19u,20u,21u,22u,23u,24u,25u,26u,27u})ck(x[reg]==initial[reg]);
        int expected=namecase==0 && size==0?0x53543530:-3;
        if(namecase==1 && size==512)expected=busy?(busy<0?busy:-100):actual==512?0:-11;
        ck(int32_t(x[10])==expected);
        bool success=expected==0;
        ck(stores==(success?3u:0u));
        ck(get(0x1038e530,4)==(success?1u:17u));
        ck(get(0x1038e534,4)==(success?512u:19u));
        ck(get(0x1038e538,4)==(success?4u:18u));
        ck(bc==unsigned(namecase==1 && size==512));
        ck(sc==unsigned(namecase==1 && size==512 && busy==0));
    }
    printf("Linked OS static50 instruction/ABI/allowlist checks: %u PASS\n",checks);
}
