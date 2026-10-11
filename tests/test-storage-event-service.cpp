// Execute the assembled RV32I hook/cave, checking register routing and the
// exact MMIO read allowlist. This is not a complete CPU/trap emulator.
#include <array>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <vector>
#include <stdexcept>
static int checks;
static void require(bool ok) { ++checks; if(!ok) throw std::runtime_error("service patch assertion"); }
static int32_t sx(uint32_t x,unsigned n) { return int32_t(x<<(32-n))>>(32-n); }
int main(int argc,char **argv) {
    if(argc!=2) return 2;
    std::ifstream in(argv[1],std::ios::binary);
    std::vector<unsigned char> code{std::istreambuf_iterator<char>(in),{}};
    require(code.size()==0x16b56);
    for(unsigned eid: {0u,1u,13u,14u,15u,16u,255u})
    for(unsigned fid: {0u,1u,0xffffffffu})
    for(unsigned index: {0u,1u,2u,3u,4u,5u,0xffffffffu}) {
        std::array<uint32_t,32> x{};
        x[10]=index; x[14]=14; x[17]=eid; x[16]=fid;
        uint32_t pc=0x16b3c, load_addr=0; unsigned loads=0;
        for(unsigned n=0;n<40;++n) {
            if(pc==0x16b40 || pc==0x16b52 || pc==0x16b2a) break;
            require(pc==0x16b3c || (pc>=0xf084 && pc<0xf0bc));
            uint32_t w=code[pc]|code[pc+1]<<8|code[pc+2]<<16|uint32_t(code[pc+3])<<24;
            unsigned op=w&127, rd=(w>>7)&31, f=(w>>12)&7;
            unsigned a=(w>>15)&31,b=(w>>20)&31; uint32_t next=pc+4;
            if(op==0x6f) {
                int32_t imm=sx(((w>>31)<<20)|(((w>>12)&255)<<12)|(((w>>20)&1)<<11)|(((w>>21)&1023)<<1),21);
                if(rd) x[rd]=pc+4;
                next=pc+imm;
            } else if(op==0x63) {
                int32_t imm=sx(((w>>31)<<12)|(((w>>7)&1)<<11)|(((w>>25)&63)<<5)|(((w>>8)&15)<<1),13);
                bool take=f==1 ? x[a]!=x[b] : f==6 ? x[a]<x[b] : f==7 ? x[a]>=x[b] : false;
                require(f==1||f==6||f==7); if(take) next=pc+imm;
            } else if(op==0x37) x[rd]=w&0xfffff000;
            else if(op==0x13) { require(f==0||f==1); x[rd]=f==0?x[a]+sx(w>>20,12):x[a]<<((w>>20)&31); }
            else if(op==0x33) { require(f==0); x[rd]=x[a]+x[b]; }
            else if(op==0x03) {
                require(f==2); load_addr=x[a]+sx(w>>20,12); ++loads;
                require(load_addr>=0x400001e0 && load_addr<=0x400001f0 && !(load_addr&3));
                x[rd]=0xdf3f1c60; // high-bit data must stay a value, not an error
            } else require(false);
            pc=next; x[0]=0;
        }
        if(eid<=14) { require(pc==0x16b40 && loads==0 && x[10]==index && x[14]==14); }
        else if(eid==15 && fid==0 && index<=4) {
            require(pc==0x16b2a && x[10]==0 && x[11]==0xdf3f1c60);
            require(loads==1 && load_addr==0x400001e0+4*index);
        } else require(pc==0x16b52 && loads==0);
    }
    std::printf("OS diagnostic service instruction checks: %d PASS\n",checks);
}
