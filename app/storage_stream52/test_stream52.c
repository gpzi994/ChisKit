#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stream.h"
#include "read_block.h"
#include "../../.integration/openfpgaos/src/firmware/os/targets/pocket/static_output_smoke52.c"
static unsigned checks;
#define CHECK(x) do {++checks;if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
volatile int output_is_open;
volatile uint32_t output_open_size,output_open_slot;
static int service_busy,service_sizes,service_calls;
static long service_size=TOTAL52;
int of_file_async_busy(void){++service_calls;return service_busy;}
long of_file_size(uint32_t slot){CHECK(slot==4);++service_sizes;return service_size;}
static void reset_service(void) {
    output_is_open=17;output_open_size=19;output_open_slot=18;
    service_busy=service_sizes=service_calls=0;service_size=TOTAL52;
}
static void unchanged(void){CHECK(output_is_open==17 && output_open_size==19 && output_open_slot==18);}
static uint32_t ticks;
static unsigned confirms;
static void sample(struct guard51 *g,uint32_t keys) {
    ticks+=20;confirms+=(unsigned)gate_tick52(g,keys,ticks);
}
static void hold(struct guard51 *g,uint32_t keys,unsigned ms) {
    for(unsigned i=0;i<=ms;i+=20)sample(g,keys);
}
static struct guard51 permitted(void) {
    struct guard51 g={0};confirms=0;
    hold(&g,0,600);hold(&g,LR51,2100);hold(&g,0,600);
    CHECK(g.state==G_CHOOSE);sample(&g,OF_BTN_A);hold(&g,0,600);
    hold(&g,OF_BTN_START,1100);CHECK(confirms==1 && g.state==G_DONE);return g;
}
static uint8_t disk52[TOTAL52];
static unsigned reads,writes,binds,bound,step_ms;
static int read_fail,write_fail,bind_fail,corrupt_at,alias_writes;
static unsigned active_phase;
static void disk_seed(unsigned kind) {fill52(disk52,0,TOTAL52,kind);}
static void io_reset(void) {
    reads=writes=binds=bound=0;step_ms=20;
    read_fail=write_fail=corrupt_at=-1;bind_fail=alias_writes=0;ticks=0;
}
static int mbind(void) {
    CHECK(active_phase==ST52_BIND && !bound);++binds;
    if(bind_fail)return -11;
    bound=1;return 0;
}
static int mwrite(uint32_t off,const void *src,uint32_t n) {
    CHECK(bound && active_phase==ST52_WRITE && n==CHUNK52 && off==writes*CHUNK52);
    CHECK(off<=TOTAL52-CHUNK52);unsigned b=writes++;ticks+=step_ms;
    if((int)b==write_fail)return -2;
    const uint8_t *p=src;for(unsigned i=0;i<n;++i)CHECK(p[i]==byte52(off+i,DATA52));
    memcpy(disk52+(alias_writes?off%65536u:off),src,n);return 0;
}
static int mread(uint32_t off,uint8_t *dst) {
    CHECK((active_phase==ST52_BOOT || active_phase==ST52_VERIFY) && off==reads*CHUNK52);
    CHECK(off<=TOTAL52-CHUNK52);unsigned b=reads++;ticks+=step_ms;
    if((int)b==read_fail)return -12;
    memcpy(dst,disk52+off,CHUNK52);
    if(corrupt_at>=0 && (unsigned)corrupt_at>=off && (unsigned)corrupt_at<off+CHUNK52)
        dst[(unsigned)corrupt_at-off]^=1;
    return 0;
}
static uint32_t mnow(void){return ticks;}
static void mprogress(enum stage52 stage,unsigned blocks) {CHECK(blocks<=256);active_phase=stage;}
static const struct ops52 ops={mbind,mwrite,mread,mnow,mprogress};
static struct scan52 initial_boot(void) {
    io_reset();disk_seed(INITIAL52);struct scan52 s=scan_file52(&ops,0,ST52_BOOT);
    CHECK(valid_boot52(&s) && s.kind==INITIAL52 && s.crc==CRC_INITIAL52 && reads==256);
    reads=0;return s;
}
static void *dest;
static void (*cb)(int,int);
static unsigned starts,polls,dma_offset;
static int dma_case;
static int dstart(int slot,uint32_t off,void *p,uint32_t n,void (*callback)(int,int)) {
    CHECK(slot==4 && off==dma_offset && n==CHUNK52 && ((uintptr_t)p&63u)==0);
    ++starts;dest=p;cb=callback;return dma_case==1?-3:11;
}
static int dpoll(void) {
    ++polls;if(dma_case==2)return -13;
    if(dma_case==3 || dma_case==4)return 0;
    fill52(dest,dma_offset,CHUNK52,DATA52);
    if(dma_case==7)((uint8_t *)dest)[-1]=0;
    if(dma_case==8)((uint8_t *)dest)[CHUNK52]=0;
    cb(dma_case==5?12:11,dma_case==6?-2:0);
    if(dma_case==9)cb(11,0);
    return 1;
}
static int dbusy(void) {
    if(dma_case==10)return 1;
    if(dma_case==11)return -12;
    return dma_case==12 && polls?1:0;
}
static void didle(void){if(dma_case!=4)++ticks;}
static const struct read_ops52 dops={dstart,dpoll,dbusy,mnow,didle};
int main(int argc,char **argv) {
    (void)fill51;(void)identify51;(void)crc51;
    if(argc==3 && (!strcmp(argv[1],"--make-fixture") || !strcmp(argv[1],"--check-fixture"))) {
        unsigned make=!strcmp(argv[1],"--make-fixture");
        FILE *f=fopen(argv[2],make?"wb":"rb");CHECK(f!=NULL);
        if(make){disk_seed(INITIAL52);CHECK(fwrite(disk52,1,TOTAL52,f)==TOTAL52);}
        else {CHECK(fread(disk52,1,TOTAL52,f)==TOTAL52);CHECK(fgetc(f)==EOF);}
        CHECK(fclose(f)==0);
        for(unsigned i=0;i<TOTAL52;++i)CHECK(disk52[i]==byte52(i,INITIAL52));
        CHECK(~crc52(0xffffffffu,disk52,TOTAL52)==CRC_INITIAL52);
        printf("Fixture52 1048576 bytes INITIAL CRC %08X: PASS\n",CRC_INITIAL52);return 0;
    }
    CHECK(~crc52(0xffffffffu,(const uint8_t *)"123456789",9)==0xcbf43926u);
    for(unsigned kind=0;kind<2;++kind) {
        unsigned tag=kind?DATA52:INITIAL52;disk_seed(tag);
        uint32_t c=0xffffffffu;
        for(unsigned off=0;off<TOTAL52;off+=CHUNK52)c=crc52(c,disk52+off,CHUNK52);
        CHECK(~c==(kind?CRC_DATA52:CRC_INITIAL52));
        CHECK(~crc52(0xffffffffu,disk52,TOTAL52)==~c);
        CHECK(disk52[6]==tag && disk52[4104]==1 && disk52[65544]==16 && disk52[TOTAL52-CHUNK52+8]==255);
        CHECK(memcmp(disk52,disk52+65536,CHUNK52)!=0);
    }
    reset_service();CHECK(static52_service(NULL,0)==STATIC52_MAGIC);unchanged();CHECK(!service_calls && !service_sizes);
    CHECK(static52_service(NULL,TOTAL52)==-3);unchanged();
    const uint32_t sizes[]={0,512,TOTAL52-1,TOTAL52,TOTAL52+1,2*TOTAL52,0xffffffffu};
    for(unsigned i=0;i<sizeof(sizes)/sizeof(sizes[0]);++i) {
        reset_service();int rc=static52_service("static52.bin",sizes[i]);
        if(sizes[i]==TOTAL52)CHECK(rc==0 && output_is_open==1 && output_open_size==TOTAL52 && output_open_slot==4);
        else {CHECK(rc==-3);unchanged();}
    }
    char name[14]="static52.bin";
    for(unsigned i=0;i<13;++i){reset_service();name[i]^=1;CHECK(static52_service(name,TOTAL52)==-3);unchanged();name[i]^=1;}
    for(unsigned i=0;i<sizeof(sizes)/sizeof(sizes[0]);++i) {
        reset_service();service_size=sizes[i];int rc=static52_service("static52.bin",TOTAL52);
        if(service_size==TOTAL52)CHECK(rc==0);else {CHECK(rc==-11);unchanged();}
    }
    reset_service();service_busy=-12;CHECK(static52_service("static52.bin",TOTAL52)==-12);unchanged();
    reset_service();service_busy=1;CHECK(static52_service("static52.bin",TOTAL52)==-100);unchanged();
    for(unsigned code=0;code<32768;++code) {
        struct guard51 g={0};confirms=0;unsigned v=code;
        for(unsigned j=0;j<5;++j) {
            unsigned key=v&7u;v>>=3;
            hold(&g,(key&1?OF_BTN_A:0)|(key&2?OF_BTN_B:0)|(key&4?OF_BTN_START:0),2200);
        }
        CHECK(!confirms && !g.unlocks && !take51(&g));
    }
    struct guard51 g={0};confirms=0;
    hold(&g,0,600);hold(&g,LR51,2100);hold(&g,0,600);sample(&g,OF_BTN_B);
    CHECK(g.state==G_QUIET && !g.mode);hold(&g,0,600);hold(&g,OF_BTN_START,2000);CHECK(!confirms);
    struct scan52 seed=initial_boot();ticks=UINT32_MAX-2000;g=permitted();
    struct job52 result=run_stream52(&g,&seed,&ops);
    CHECK(!result.rc && writes==256 && reads==256 && binds==1 && result.stage==ST52_DONE);
    CHECK(result.verify.crc==CRC_DATA52 && result.verify.bytes==TOTAL52 && !result.verify.bad_data);
    unsigned saved_calls=reads+writes+binds;
    CHECK(run_stream52(&g,&seed,&ops).rc==OF_ERR_DENIED && reads+writes+binds==saved_calls);
    /* A new read-only boot sees DATA and cannot rewrite it. */
    io_reset();struct scan52 persisted=scan_file52(&ops,0,ST52_BOOT);
    CHECK(valid_boot52(&persisted) && persisted.kind==DATA52);
    g=permitted();CHECK(run_stream52(&g,&persisted,&ops).rc==OF_ERR_DENIED && !writes && !binds);
    for(unsigned state=G_QUIET;state<G_DONE;++state) {
        io_reset();g=(struct guard51){.state=state,.mode=A51};
        CHECK(run_stream52(&g,&seed,&ops).rc==OF_ERR_DENIED && !writes && !binds && !reads);
    }
    struct scan52 bad=seed;bad.bytes-=CHUNK52;io_reset();g=permitted();
    CHECK(run_stream52(&g,&bad,&ops).rc==OF_ERR_DENIED && !binds);
    bad=seed;bad.crc^=1;CHECK(run_stream52(&g,&bad,&ops).rc==OF_ERR_DENIED && !binds);
    bad=seed;bad.rc=-12;CHECK(run_stream52(&g,&bad,&ops).rc==OF_ERR_DENIED && !binds);
    io_reset();g=permitted();bind_fail=1;result=run_stream52(&g,&seed,&ops);
    CHECK(result.rc==-11 && binds==1 && !writes && !reads);
    const int boundaries[]={0,1,15,16,127,128,255};
    for(unsigned i=0;i<sizeof(boundaries)/sizeof(boundaries[0]);++i) {
        io_reset();g=permitted();write_fail=boundaries[i];result=run_stream52(&g,&seed,&ops);
        CHECK(result.rc==-2 && writes==(unsigned)write_fail+1 && !reads);
        CHECK(result.write_done==(unsigned)write_fail && result.failed_offset==(unsigned)write_fail*CHUNK52);
        io_reset();g=permitted();read_fail=boundaries[i];result=run_stream52(&g,&seed,&ops);
        CHECK(result.rc==-12 && writes==256 && reads==(unsigned)read_fail+1 && result.verify.blocks==(unsigned)read_fail);
    }
    const int spots[]={0,6,4095,4096,65535,65536,524287,524288,1048575};
    for(unsigned i=0;i<sizeof(spots)/sizeof(spots[0]);++i) {
        io_reset();disk_seed(INITIAL52);corrupt_at=spots[i];struct scan52 corrupt=scan_file52(&ops,0,ST52_BOOT);
        CHECK(corrupt.rc && !valid_boot52(&corrupt) && corrupt.first_initial==(unsigned)spots[i]);
        io_reset();g=permitted();corrupt_at=spots[i];result=run_stream52(&g,&seed,&ops);
        CHECK(result.rc==OF_ERR_FAILED && result.failed_offset==(unsigned)spots[i] && result.verify.bad_data==1);
    }
    io_reset();g=permitted();alias_writes=1;result=run_stream52(&g,&seed,&ops);CHECK(result.rc && result.verify.bad_data);
    io_reset();g=permitted();step_ms=1000;result=run_stream52(&g,&seed,&ops);
    CHECK(result.rc==OF_ERR_TIMEOUT && writes==120 && !reads);
    io_reset();disk_seed(INITIAL52);step_ms=1000;bad=scan_file52(&ops,0,ST52_BOOT);
    CHECK(bad.rc==OF_ERR_TIMEOUT && reads==120 && !valid_boot52(&bad));
    static uint8_t out[CHUNK52];
    for(dma_case=0;dma_case<=12;++dma_case) {
        poisoned52=0;starts=polls=0;ticks=0;dma_offset=TOTAL52-CHUNK52;memset(out,0x5b,sizeof(out));
        int rc=read_block52(&dops,dma_offset,out);
        if(!dma_case){CHECK(rc==0);for(unsigned i=0;i<CHUNK52;++i)CHECK(out[i]==byte52(dma_offset+i,DATA52));}
        else {CHECK(rc<0);for(unsigned i=0;i<CHUNK52;++i)CHECK(out[i]==0x5b);}
        CHECK(starts<=1);if(dma_case==3)CHECK(ticks==5000);if(dma_case==4)CHECK(polls==10000);
        if(poisoned52){unsigned before=starts;CHECK(read_block52(&dops,0,out)==OF_ERR_INVALID_STATE && starts==before);}
        if(dma_case==3){cb(11,0);CHECK(poisoned52);}
    }
    dma_case=0;poisoned52=0;starts=polls=0;
    CHECK(read_block52(&dops,TOTAL52,out)==OF_ERR_BAD_RANGE && !starts);
    CHECK(read_block52(&dops,1,out)==OF_ERR_BAD_RANGE && !starts);
    CHECK(read_block52(&dops,0xffffffffu,out)==OF_ERR_BAD_RANGE && !starts);
    CHECK(read_block52(&dops,0,NULL)==OF_ERR_BAD_RANGE && !starts);
    printf("Stream52 fixture/gate/sequence/boundary/timeout/DMA checks: %u PASS\n",checks);return 0;
}
