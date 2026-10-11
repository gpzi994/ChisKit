#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sequence.h"
#include "read512.h"
#include "../../.integration/openfpgaos/src/firmware/os/targets/pocket/static_output_smoke50.c"
static unsigned checks;
#define CHECK(x) do {++checks;if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
volatile int output_is_open;
volatile uint32_t output_open_size,output_open_slot;
static int busy_value,busy_calls,size_calls;
static long size_value;
int of_file_async_busy(void){++busy_calls;return busy_value;}
long of_file_size(uint32_t slot){CHECK(slot==4);++size_calls;return size_value;}
static void service_reset(void) {
    busy_value=busy_calls=size_calls=0;size_value=512;
    output_is_open=17;output_open_slot=18;output_open_size=19;
}
static void unchanged(void){CHECK(output_is_open==17 && output_open_slot==18 && output_open_size==19);}
static unsigned calls,phase,mode_now;
static int fail_at,corrupt;
static uint8_t disk_bytes[512];
static int bind_mock(void){CHECK(phase==S_BIND);++calls;return fail_at==1?-100:0;}
static int write_mock(uint32_t off,const void *p,uint32_t len) {
    CHECK(phase==S_WRITE && off==0 && len==512);++calls;
    CHECK(pattern_identify(p)==mode_now);
    if(fail_at==2)return -2;
    memcpy(disk_bytes,p,512);return 0;
}
static int read_mock(uint8_t *p) {
    CHECK(phase==S_READ);++calls;
    if(fail_at==3)return -12;
    memcpy(p,disk_bytes,512);if(corrupt>=0)p[corrupt]^=1;return 0;
}
static int flush_mock(void){CHECK(phase==S_FLUSH && mode_now==P_B);++calls;return fail_at==4?-12:0;}
static void progress_mock(enum step50 p){phase=p;}
static const struct ops50 ops={bind_mock,write_mock,read_mock,flush_mock,progress_mock};
static void *dma_dest;
static void (*dma_cb)(int,int);
static unsigned starts,polls,clock_ms;
static int dma_case;
static int dma_start(int slot,uint32_t off,void *p,uint32_t len,void (*cb)(int,int)) {
    CHECK(slot==4 && off==0 && len==512 && ((uintptr_t)p&63)==0);
    ++starts;dma_dest=p;dma_cb=cb;
    return dma_case==1?-3:7;
}
static int dma_poll(void) {
    ++polls;
    if(dma_case==2)return -13;
    if(dma_case==3 || dma_case==4)return 0;
    pattern_fill(dma_dest,P_A);
    if(dma_case==7)((uint8_t *)dma_dest)[-1]=0;
    if(dma_case==8)((uint8_t *)dma_dest)[512]=0;
    dma_cb(dma_case==5?9:7,dma_case==6?-2:0);
    if(dma_case==9)dma_cb(7,0);
    return 1;
}
static int dma_busy(void){return dma_case==10?1:0;}
static uint32_t dma_now(void){return clock_ms;}
static void dma_idle(void){if(dma_case!=4)++clock_ms;}
static const struct read50_ops dma_ops={dma_start,dma_poll,dma_busy,dma_now,dma_idle};
int main(void) {
    uint8_t p[512];unsigned first;
    for(unsigned tag=0;tag<3;++tag) {
        unsigned kind=tag==0?P_SEED:tag==1?P_A:P_B;
        pattern_fill(p,kind);CHECK(pattern_identify(p)==kind);
        for(unsigned i=0;i<512;++i) {
            p[i]^=1;CHECK(pattern_identify(p)==P_UNKNOWN);
            CHECK(pattern_diff(p,kind,&first)==1 && first==i);p[i]^=1;
        }
    }
    service_reset();CHECK(static50_service(NULL,0)==STATIC50_MAGIC);unchanged();CHECK(!busy_calls&&!size_calls);
    CHECK(static50_service(NULL,512)==-3);unchanged();
    for(unsigned len=0;len<1025;++len) {
        service_reset();int rc=static50_service("static50.bin",len);
        if(len==512)CHECK(rc==0 && output_is_open==1 && output_open_slot==4 && output_open_size==512);
        else {CHECK(rc==-3);unchanged();CHECK(!busy_calls&&!size_calls);}
    }
    char name[14]="static50.bin";
    for(unsigned i=0;i<13;++i) {
        service_reset();name[i]^=1;
        CHECK(static50_service(name,512)==-3);unchanged();name[i]^=1;
    }
    for(int s=-1;s<=1024;++s) {
        service_reset();size_value=s;int rc=static50_service("static50.bin",512);
        if(s==512)CHECK(rc==0);else {CHECK(rc==-11);unchanged();}
    }
    for(int b=-12;b<3;++b) if(b) {
        service_reset();busy_value=b;CHECK(static50_service("static50.bin",512)==(b<0?b:-100));unchanged();CHECK(size_calls==0);
    }
    for(unsigned mode=0;mode<2;++mode) for(fail_at=0;fail_at<=4;++fail_at) {
        calls=0;corrupt=-1;mode_now=mode?P_B:P_A;
        struct result50 r=sequence50(&ops,mode_now);
        CHECK(calls==(unsigned)(fail_at && fail_at<=3?fail_at:mode?4:3));
        CHECK((r.attempted&8)==(mode && (fail_at==0||fail_at==4)?8u:0u));
        CHECK((r.rc!=0)==(fail_at>0 && (mode||fail_at<4)));
    }
    fail_at=0;mode_now=P_B;
    for(corrupt=0;corrupt<512;++corrupt) {
        calls=0;struct result50 r=sequence50(&ops,P_B);
        CHECK(calls==3 && !(r.attempted&8) && r.rc==-1);
        CHECK(r.mismatches==1 && r.first_bad==(unsigned)corrupt);
    }
    calls=0;CHECK(sequence50(&ops,0).rc==-3 && calls==0);
    for(dma_case=0;dma_case<=10;++dma_case) {
        read512_poisoned=0;starts=polls=clock_ms=0;memset(p,0x5b,sizeof(p));
        int rc=read512_run(&dma_ops,p);
        if(dma_case==0) CHECK(rc==0 && pattern_identify(p)==P_A);
        else {CHECK(rc<0);for(unsigned i=0;i<512;++i)CHECK(p[i]==0x5b);}
        CHECK(starts<=1);
        if(dma_case==3) CHECK(clock_ms==5000 && rc==-12);
        if(dma_case==4) CHECK(polls==10000 && rc==-12);
        if(read512_poisoned) {unsigned before=starts;CHECK(read512_run(&dma_ops,p)==-10 && starts==before);}
        if(dma_case==3) {dma_cb(7,0);CHECK(read512_poisoned);}
    }
    printf("Static50 policy/pattern/sequence/async checks: %u PASS\n",checks);
    return 0;
}
