#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sequence.h"
#include "../storage_static_control/pattern.h"
static unsigned checks;
#define CHECK(x) do {++checks;if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static uint32_t ticks;
static unsigned confirmed;
static void sample(struct guard51 *g,uint32_t keys) {
    ticks+=20;confirmed+=(unsigned)update51(g,keys,ticks);
}
static void hold(struct guard51 *g,uint32_t keys,unsigned ms) {
    for(unsigned i=0;i<=ms;i+=20)sample(g,keys);
}
static struct guard51 permitted(unsigned mode) {
    struct guard51 g={0};confirmed=0;
    hold(&g,0,600);CHECK(g.state==G_LOCKED);
    hold(&g,LR51,2100);CHECK(g.state==G_RELEASE_LR && !confirmed);
    hold(&g,0,600);CHECK(g.state==G_CHOOSE);
    sample(&g,mode==A51?OF_BTN_A:OF_BTN_B);CHECK(g.state==G_RELEASE_MODE);
    hold(&g,0,600);CHECK(g.state==G_CONFIRM);
    hold(&g,OF_BTN_START,1100);CHECK(g.state==G_DONE && confirmed==1);
    return g;
}
static unsigned calls,phase,mode_now;
static int fail_at,corrupt;
static uint8_t disk[BYTES51];
static int mbind(void){CHECK(phase==S51_BIND);++calls;return fail_at==1?-11:0;}
static int mwrite(uint32_t off,const void *p,uint32_t len) {
    CHECK(phase==S51_WRITE && off==0 && len==512);++calls;
    CHECK(identify51(p)==mode_now);
    if(fail_at==2)return -2;
    memcpy(disk,p,512);return 0;
}
static int mread(uint8_t *p) {
    CHECK(phase==S51_READ);++calls;if(fail_at==3)return -12;
    memcpy(p,disk,512);if(corrupt>=0)p[corrupt]^=1;return 0;
}
static int mflush(void){CHECK(phase==S51_FLUSH && mode_now==B51);++calls;return fail_at==4?-12:0;}
static void mprogress(enum step51 p){phase=p;}
static const struct ops51 ops={mbind,mwrite,mread,mflush,mprogress};
int main(int argc,char **argv) {
    uint8_t p[512];unsigned first;
    if(argc==3 && !strcmp(argv[1],"--check-fixture")) {
        FILE *f=fopen(argv[2],"rb");CHECK(f!=NULL);
        CHECK(fread(p,1,512,f)==512);CHECK(fgetc(f)==EOF);CHECK(fclose(f)==0);
        CHECK(identify51(p)==INITIAL51 && diff51(p,INITIAL51,&first)==0);
        printf("Fixture512 INITIAL; CRC %08X; A/B rejected: PASS\n",crc51(p,BYTES51));return 0;
    }
    CHECK(crc51((const uint8_t *)"123456789",9)==0xcbf43926u);
    memset(p,0,sizeof(p));CHECK(crc51(p,BYTES51)==0xb2aa7578u);
    for(unsigned n=0;n<3;++n) {
        unsigned kind=n==0?INITIAL51:n==1?A51:B51;
        fill51(p,kind);CHECK(identify51(p)==kind && diff51(p,kind,&first)==0 && first==512);
        CHECK(pattern_identify(p)==P_UNKNOWN);
        for(unsigned i=0;i<512;++i) {
            p[i]^=1;CHECK(identify51(p)==UNKNOWN51);
            CHECK(diff51(p,kind,&first)==1 && first==i);p[i]^=1;
        }
    }
    for(unsigned n=0;n<3;++n) {
        pattern_fill(p,n==0?P_SEED:n==1?P_A:P_B);CHECK(identify51(p)==UNKNOWN51);
    }
    /* Exhaust all A/B/START streams of length five.
     * Each symbol persists for 2200ms, enough to expose accidental holds. */
    for(unsigned code=0;code<32768;++code) {
        struct guard51 g={0};confirmed=0;unsigned x=code;
        for(unsigned n=0;n<5;++n) {
            unsigned c=x&7u;x>>=3;
            uint32_t key=(c&1?OF_BTN_A:0)|(c&2?OF_BTN_B:0)|(c&4?OF_BTN_START:0);
            hold(&g,key,2200);
        }
        CHECK(!confirmed && !g.unlocks && !take51(&g));
    }
    /* Any single held key, including shoulder keys, may never authorize. */
    for(unsigned bit=0;bit<16;++bit) {
        struct guard51 g={0};confirmed=0;hold(&g,0,600);
        hold(&g,1u<<bit,12000);CHECK(!confirmed && !take51(&g));
    }
    struct guard51 g={0};confirmed=0;
    hold(&g,LR51,5000);CHECK(!g.unlocks && !confirmed); /* held during launch */
    hold(&g,0,600);hold(&g,LR51,1900);sample(&g,0);CHECK(!g.unlocks);
    hold(&g,0,600);hold(&g,LR51|OF_BTN_A,2500);CHECK(!g.unlocks);
    g=(struct guard51){0};hold(&g,0,600);hold(&g,LR51,2100);
    hold(&g,OF_BTN_A,1500);CHECK(g.state==G_RELEASE_LR && !g.mode);
    hold(&g,0,600);sample(&g,OF_BTN_A);hold(&g,OF_BTN_START,2000);
    CHECK(!confirmed && g.state==G_RELEASE_MODE); /* start cannot skip release */
    hold(&g,0,600);hold(&g,OF_BTN_START,800);sample(&g,0);CHECK(!confirmed);
    g=(struct guard51){0};hold(&g,0,600);hold(&g,LR51,2100);
    hold(&g,0,11000);CHECK(g.state==G_LOCKED && !g.mode && !confirmed);
    g=(struct guard51){0};hold(&g,0,600);sample(&g,LR51);
    ticks+=3000;CHECK(!update51(&g,LR51,ticks) && g.state==G_QUIET);
    g=(struct guard51){0};hold(&g,0,600);hold(&g,LR51,2100);
    hold(&g,0,600);sample(&g,OF_BTN_A|OF_BTN_B);CHECK(g.state==G_QUIET);
    g=(struct guard51){0};hold(&g,0,600);hold(&g,LR51,2100);
    hold(&g,0,600);sample(&g,OF_BTN_A);hold(&g,0,600);
    sample(&g,OF_BTN_START|OF_BTN_A);CHECK(g.state==G_QUIET && !confirmed);
    g=(struct guard51){0};hold(&g,0,600);hold(&g,LR51,2100);
    hold(&g,0,600);sample(&g,OF_BTN_A);hold(&g,0,600);sample(&g,OF_BTN_START);
    ticks+=1200;CHECK(!update51(&g,OF_BTN_START,ticks) && g.state==G_QUIET);
    /* A delayed selection A event, after the launch release, stays locked. */
    g=(struct guard51){0};hold(&g,0,2000);hold(&g,OF_BTN_A,5000);
    CHECK(!g.unlocks && !confirmed && !take51(&g));
    for(unsigned mode=0;mode<2;++mode) {
        mode_now=mode?B51:A51;ticks=UINT32_MAX-2000;g=permitted(mode_now);
        CHECK(take51(&g)==mode_now && take51(&g)==0);
        hold(&g,0,1000);hold(&g,LR51,3000);hold(&g,OF_BTN_START,2000);
        CHECK(confirmed==1 && !take51(&g));
    }
    /* Calling the operation layer with any pre-confirmation state does nothing. */
    for(unsigned state=G_QUIET;state<G_DONE;++state) {
        g=(struct guard51){0};g.state=state;g.mode=A51;calls=0;
        CHECK(run51(&g,&ops).rc==OF_ERR_DENIED && calls==0);
    }
    for(unsigned mode=0;mode<2;++mode)for(fail_at=0;fail_at<=4;++fail_at) {
        mode_now=mode?B51:A51;g=permitted(mode_now);calls=0;corrupt=-1;
        struct result51 r=run51(&g,&ops);
        CHECK(calls==(unsigned)(fail_at && fail_at<=3?fail_at:mode?4:3));
        CHECK((r.attempted&8)==(mode && (fail_at==0||fail_at==4)?8u:0u));
        CHECK((r.rc!=0)==(fail_at>0 && (mode||fail_at<4)));
        unsigned before=calls;CHECK(run51(&g,&ops).rc==OF_ERR_DENIED && calls==before);
    }
    fail_at=0;mode_now=B51;
    for(corrupt=0;corrupt<512;++corrupt) {
        g=permitted(B51);calls=0;struct result51 r=run51(&g,&ops);
        CHECK(calls==3 && !(r.attempted&8) && r.rc==OF_ERR_FAILED);
        CHECK(r.mismatches==1 && r.first_bad==(unsigned)corrupt);
    }
    printf("Guard51 pattern/input/gate/one-shot/fault checks: %u PASS\n",checks);return 0;
}
