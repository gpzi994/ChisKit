// Execute the actual linked RV32I patch. Mock only pinned OS callees and the
// cartridge register boundary. Not an APF/electrical/full-machine emulator.
#include <array>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <map>
#include <stdexcept>
#include <vector>
#include <string>
static unsigned checks;
static void check(bool b,unsigned line){++checks;if(!b){std::fprintf(stderr,"instruction test line %u\n",line);throw std::runtime_error("dump54 linked instruction assertion");}}
#define ck(b) check((b),__LINE__)
static int32_t sx(uint32_t x,unsigned n){return int32_t(x<<(32-n))>>(32-n);}
static constexpr uint32_t base=0x1032e200,sp0=0x12001000,ptr=0x12002000,out=0x12003000,stop=0xdead0000;
struct VM {
    std::map<uint32_t,uint8_t> mem;std::array<uint32_t,32>x{};
    std::vector<std::pair<uint32_t,uint32_t>> mmio;
    uint32_t reply=4,lc=0x80000303,features=32,id=0x31424746,caps=0x00100f02,rxlevel=32;
    int busy=0,size=8705280;unsigned bc=0,sc=0,polls=0,rx=0,bsswrites=0;bool active=false;
    VM(const std::vector<uint8_t>&code){
        for(unsigned i=0;i<code.size();++i)mem[base+i]=code[i];
        for(unsigned i=0;i<32;++i)x[i]=0xabc00000+i;
        for(unsigned i=0;i<64;++i)mem[out+i]=0xcc;
        const char name[]="dump54.gba";for(unsigned i=0;i<sizeof(name);++i)mem[ptr+i]=name[i];
        put(0x1038e530,17,4);put(0x1038e534,19,4);put(0x1038e538,18,4);
        x[0]=0;x[1]=stop;x[2]=sp0;
    }
    void put(uint32_t a,uint32_t v,unsigned n){for(unsigned i=0;i<n;++i)mem[a+i]=uint8_t(v>>(8*i));}
    uint32_t get(uint32_t a,unsigned n){uint32_t v=0;for(unsigned i=0;i<n;++i){ck(mem.count(a+i));v|=uint32_t(mem[a+i])<<(8*i);}return v;}
    uint32_t load(uint32_t a,unsigned n){
        if(a==0x40000098){ck(n==4);return features;}
        if(a>=0x400001a0 && a<=0x400001e0){ck(n==4 && !(a&3));switch((a-0x400001a0)/4){
            case 0:return id;case 1:return caps;case 2:return active?(++polls<=2?2:reply):0;
            case 8:ck(active && reply==4 && rx<8);return 0x03020100+0x04040404*(rx++);
            case 9:return rxlevel;case 11:return lc;case 12:return 0x01018000;case 13:return 0x0100013b;default:ck(false);}}
        ck((a>=base && a<base+0x1200)||(a>=sp0-32 && a<sp0)||(a>=ptr && a<ptr+11));return get(a,n);
    }
    void store(uint32_t a,uint32_t v,unsigned n){
        if(a>=0x40000000){ck(n==4);ck(a==0x400001e0||a==0x400001b0||a==0x400001b4||a==0x400001b8||a==0x400001bc||a==0x400001ac);
            mmio.emplace_back(a,v);if(a==0x400001ac){ck(v==2||v==3);active=v==2;}return;}
        ck((a>=sp0-32 && a+n<=sp0)||(a>=out && a+n<=out+44)||
           (n==4 && (a==0x1038e530||a==0x1038e534||a==0x1038e538)));
        if(a>=0x1038e530 && a<=0x1038e538)++bsswrites;
        put(a,v,n);
    }
    int run(uint32_t pc){
        auto initial=x;unsigned steps=0;
        while(pc!=stop && ++steps<5000){
            if(pc==0x10328100 || pc==0x103297c0){if(pc==0x10328100){++bc;x[10]=busy;}else{ck(x[10]==4);++sc;x[10]=size;}pc=x[1];continue;}
            ck((pc>=0x1032e200 && pc<0x1032e300)||(pc>=0x1032e340 && pc<0x1032e344)||
               (pc>=0x1032e3c0 && pc<0x1032e400)||(pc>=0x1032f180 && pc<0x1032f400));ck(!(pc&3));
            uint32_t w=get(pc,4),next=pc+4;unsigned op=w&127,rd=(w>>7)&31,fn=(w>>12)&7,a=(w>>15)&31,b=(w>>20)&31;
            if(op==0x37)x[rd]=w&0xfffff000;
            else if(op==0x17)x[rd]=pc+(w&0xfffff000);
            else if(op==0x13){uint32_t imm=sx(w>>20,12);switch(fn){case 0:x[rd]=x[a]+imm;break;case 7:x[rd]=x[a]&imm;break;case 5:ck(!(w>>25));x[rd]=x[a]>>b;break;default:ck(false);}}
            else if(op==0x33){ck(!(w>>25));if(fn==0)x[rd]=x[a]+x[b];else if(fn==7)x[rd]=x[a]&x[b];else ck(false);}
            else if(op==3){ck(fn==2||fn==4);x[rd]=load(x[a]+sx(w>>20,12),fn==2?4:1);}
            else if(op==0x23){ck(fn==2||fn==0);store(x[a]+sx(((w>>25)<<5)|((w>>7)&31),12),x[b],fn==2?4:1);}
            else if(op==0x63){int imm=sx(((w>>31)<<12)|(((w>>7)&1)<<11)|(((w>>25)&63)<<5)|(((w>>8)&15)<<1),13);bool take=false;
                switch(fn){case 0:take=x[a]==x[b];break;case 1:take=x[a]!=x[b];break;case 5:take=int32_t(x[a])>=int32_t(x[b]);break;case 6:take=x[a]<x[b];break;case 7:take=x[a]>=x[b];break;default:ck(false);}if(take)next=pc+imm;}
            else if(op==0x6f){int imm=sx(((w>>31)<<20)|(((w>>12)&255)<<12)|(((w>>20)&1)<<11)|(((w>>21)&1023)<<1),21);x[rd]=pc+4;next=pc+imm;}
            else if(op==0x67){ck(fn==0);next=(x[a]+sx(w>>20,12))&~1u;x[rd]=pc+4;}
            else ck(false);
            x[0]=0;pc=next;
        }
        ck(pc==stop && x[2]==sp0);for(unsigned r:{3u,4u,8u,9u,18u,19u,20u,21u,22u,23u,24u,25u,26u,27u})ck(x[r]==initial[r]);
        return int32_t(x[10]);
    }
};
int main(int argc,char **argv){
    ck(argc==2);std::ifstream f(argv[1],std::ios::binary);std::vector<uint8_t> code{std::istreambuf_iterator<char>(f),{}};ck(code.size()==0x11fc);
    for(int busy:{-12,0,1})for(int size:{-1,512,8705279,8705280,8705281})for(unsigned name=0;name<13;++name){
        VM v(code);v.busy=busy;v.size=size;v.x[10]=name?ptr:0;v.x[11]=8705280;
        if(name>=2)v.mem[ptr+name-2]^=1;
        int expected=name==1?(busy?(busy<0?busy:-100):size==8705280?0:-11):-3;
        ck(v.run(0x1032f180)==expected);ck(v.mmio.empty() && v.bsswrites==(expected==0?3u:0u));
    }
    {VM v(code);v.x[10]=v.x[11]=0;ck(v.run(0x1032f180)==0x53543534 && v.mmio.empty() && !v.bsswrites);}
    for(unsigned n:{0u,43u,44u,64u})for(unsigned present:{0u,32u}){
        VM v(code);v.features=present;v.x[10]=out;v.x[11]=n;ck(v.run(0x1032e200)==(n<44?-3:present?0:-2));ck(v.mmio.empty());
        if(n>=44){ck(v.get(out,4)==44 && v.get(out+4,4)==4 && !v.get(out+20,4));ck(v.get(out+8,4)==(present?2u:0u));ck(v.get(out+40,4)==(present?1u:0u));}
        ck(v.get(out+44,4)==0xcccccccc);
    }
    for(bool spaceentry:{false,true})for(unsigned test=0;test<13;++test){
        VM v(code);uint32_t bus=2,off=8705248,n=32,us=50000,dst=out+1,space=0;int expect=0;
        switch(test){case 1:bus=1;expect=-11;break;case 2:off=8705280;expect=-11;break;case 3:off=2;expect=-11;break;
        case 4:n=16;expect=-11;break;case 5:us=0;expect=-3;break;case 6:dst=0;expect=-3;break;
        case 7:v.features=0;expect=-2;break;case 8:v.lc=0;expect=-10;break;case 9:v.reply=32;expect=-12;break;
        case 10:v.reply=8;expect=-13;break;case 11:v.rxlevel=31;expect=-13;break;case 12:if(spaceentry){space=1;expect=-2;}break;}
        if(spaceentry){v.x[10]=bus;v.x[11]=space;v.x[12]=off;v.x[13]=dst;v.x[14]=n;v.x[15]=us;}
        else {v.x[10]=bus;v.x[11]=off;v.x[12]=dst;v.x[13]=n;v.x[14]=us;}
        ck(v.run(spaceentry?0x1032e3c0:0x1032e340)==expect);ck(!v.bsswrites);
        if(!expect){ck(v.rx==8 && v.mmio.size()==6);for(unsigned i=0;i<32;++i)ck(v.get(out+1+i,1)==i);ck(v.get(out,1)==0xcc && v.get(out+33,1)==0xcc);}
        else{ck(v.rx==0 && v.get(out+1,1)==0xcc);if(test<9 || test==12)ck(v.mmio.empty());}
    }
    printf("dump54 actual linked caps/read/read_space/bind ABI + memory/MMIO allowlist: %u checks PASS\n",checks);
}
