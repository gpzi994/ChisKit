#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "dump.h"
#include "read_block.h"
static uint8_t *rom,*disk;
static unsigned nw,nr,nc,nb,ns,na,maxend;
static int failwrite=-1,failread=-1,failcart=-1,corrupt=-1,capbad,bindbad,safebad,wrongcart;
static uint32_t ticks,step;
static int clocked,verifybad;static enum stage54 phase;
static int cap(of_cart_caps_t *c){*c=(of_cart_caps_t){44,4,2,15,32,0,0x80000303,0,0,0,1};if(capbad)c->writes_enabled=1;return 0;}
static int cart(uint32_t off,uint8_t *p){assert(off%32==0 && off<=TOTAL54-32);++nc;if(clocked)ticks+=1;if((int)off==failcart)return -2;memcpy(p,rom+off,32);if(wrongcart)p[0]^=1;return 0;}
static int safe(void){++ns;if(clocked)ticks+=3;return safebad?-2:0;}
static int abortcart(void){++na;return 0;}
static int bind(void){++nb;return bindbad?-2:0;}
static int writeblock(uint32_t off,const void *p,uint32_t n){
    assert(off%CHUNK54==0 && off<TOTAL54 && n==length54(off));++nw;if(clocked)ticks+=10;
    if((int)(off/CHUNK54)==failwrite)return -12;
    memcpy(disk+off,p,n);if(off+n>maxend)maxend=off+n;
    if((int)(off/CHUNK54)==corrupt)disk[off+n-1]^=1;
    return 0;
}
static int readblock(uint32_t off,uint8_t *p,uint32_t n){
    assert(off%CHUNK54==0 && off<TOTAL54 && n==length54(off));++nr;if(clocked)ticks+=20;
    if((int)(off/CHUNK54)==failread)return -12;
    memcpy(p,disk+off,n);if(verifybad && phase==VERIFY54 && off+n==TOTAL54){if(verifybad==1)p[n-1]^=1;else return -12;}return 0;
}
static uint32_t now(void){ticks+=step;return ticks;}
static void progress(enum stage54 s,unsigned n){assert(s<=DONE54 && n<=BLOCKS54);phase=s;if(clocked)ticks+=2;}
static const struct ops54 ops={cap,cart,safe,abortcart,bind,writeblock,readblock,now,progress};
static struct guard51 armed(void){return (struct guard51){.state=G_DONE,.mode=A51,.confirmations=1};}
static struct scan54 seed(void){return (struct scan54){.kind=ZERO54,.bytes=TOTAL54,.blocks=BLOCKS54};}
static void reset(void){nw=nr=nc=nb=ns=na=maxend=ticks=step=0;failwrite=failread=failcart=corrupt=-1;capbad=bindbad=safebad=wrongcart=clocked=verifybad=0;phase=BOOT54;memset(disk,0,TOTAL54);}
static void vector(const char *s,const char *want){
    for(unsigned chunk=1;chunk<=65;++chunk){struct md5_53 m;uint8_t d[16];char h[33];md5_init53(&m);
        unsigned len=strlen(s);for(unsigned off=0;off<len;off+=chunk){unsigned n=len-off;if(n>chunk)n=chunk;md5_add53(&m,s+off,n);}
        md5_end53(&m,d);md5_hex53(d,h);assert(!strcmp(h,want));}
}
/* Async callback fault harness: exact tail length and permanent poisoning. */
static void (*cb)(int,int);static void *dmadst;static unsigned dman,starts;static int dmamode;
static int start(int slot,uint32_t off,void *p,uint32_t n,void (*f)(int,int)){
    assert(slot==4 && off<TOTAL54);++starts;dmadst=p;dman=n;cb=f;return dmamode==1?-2:7;
}
static int poll(void){
    if(dmamode==2)return -2;
    if(dmamode==3)return 0;
    memset(dmadst,0x57,dman);
    if(dmamode==4)((uint8_t *)dmadst)[dman]=0;
    cb(dmamode==5?8:7,dmamode==6?-2:0);if(dmamode==7)cb(7,0);return 0;
}
static int busy(void){return 0;}
static void idle(void){++ticks;}
static void test_dma(void){
    const struct read_ops54 o={start,poll,busy,now,idle};uint8_t out[CHUNK54];
    for(int mode=0;mode<=7;++mode){poisoned54=0;starts=0;ticks=step=0;dmamode=mode;memset(out,0xaa,sizeof(out));
        int rc=read_block54(&o,265*CHUNK54,out,21760);assert(starts==1);assert((rc==0)==(mode==0));
        if(!rc){for(unsigned i=0;i<21760;++i)assert(out[i]==0x57);assert(out[21760]==0xaa);}
        else{assert(out[0]==0xaa);assert(read_block54(&o,0,out,CHUNK54)==OF_ERR_INVALID_STATE);assert(starts==1);}
    }
    poisoned54=0;starts=0;dmamode=0;
    assert(read_block54(&o,TOTAL54,out,4096)==OF_ERR_BAD_RANGE);
    assert(read_block54(&o,0,out,1280)==OF_ERR_BAD_RANGE);
    assert(read_block54(&o,265*CHUNK54,out,CHUNK54)==OF_ERR_BAD_RANGE);
    assert(read_block54(&o,32,out,4096)==OF_ERR_BAD_RANGE);assert(starts==0);
    assert(!read_block54(&o,0,out,CHUNK54));for(unsigned i=0;i<CHUNK54;++i)assert(out[i]==0x57);
}
int main(int argc,char **argv){
    (void)fill51;(void)identify51;(void)crc51;(void)gate_tick52;
    assert(argc==2);rom=malloc(TOTAL54);disk=malloc(TOTAL54);assert(rom && disk);
    FILE *f=fopen(argv[1],"rb");assert(f);assert(fread(rom,1,TOTAL54,f)==TOTAL54);assert(fgetc(f)==EOF);fclose(f);
    vector("","D41D8CD98F00B204E9800998ECF8427E");vector("a","0CC175B9C0F1B6A831C399E269772661");
    vector("abc","900150983CD24FB0D6963F7D28E17F72");vector("message digest","F96B697D7CB7938D525A2F31AAF161D0");
    vector("abcdefghijklmnopqrstuvwxyz","C3FCD3D76192E4007DFB496CCA67E13B");
    vector("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789","D174AB98D277D9F5A5611C2C9F419D9F");
    vector("12345678901234567890123456789012345678901234567890123456789012345678901234567890","57EDF4A22BE3C955AC49DA2E2107B67A");
    struct md5_53 md;uint8_t digest[16];char hex[33];md5_init53(&md);
    for(unsigned i=0;i<1000000;++i)md5_add53(&md,"a",1);
    md5_end53(&md,digest);md5_hex53(digest,hex);assert(!strcmp(hex,"7707D6AE4E027C70EEA2A935C2296F21"));
    md5_init53(&md);md5_add53(&md,rom,TOTAL54);md5_end53(&md,digest);assert(!memcmp(digest,expected54,16));
    for(unsigned i=0;i<5;++i)assert(crc54(rom+sample_off54[i],32)==sample_crc54[i]);
    test_dma();reset();struct scan54 boot=scan54(&ops,BOOT54);assert(!boot.rc && boot.kind==ZERO54 && nr==BLOCKS54 && !nw && !nc);
    struct guard51 g=armed();struct job54 r=run54(&g,&boot,&ops);
    assert(!r.rc && r.stage==DONE54 && r.bytes==TOTAL54 && r.blocks==BLOCKS54 && r.samples==5);
    assert(nc==272045 && nw==266 && nr==798 && nb==1 && maxend==TOTAL54 && !memcmp(rom,disk,TOTAL54));
    assert(!memcmp(r.cart_md5,expected54,16) && !memcmp(r.verify.md5,expected54,16));
    unsigned old=nw;assert(run54(&g,&boot,&ops).rc==OF_ERR_DENIED && nw==old);
    nw=nc=nb=0;boot=scan54(&ops,BOOT54);assert(!boot.rc && boot.kind==ROM54);g=armed();assert(run54(&g,&boot,&ops).rc==OF_ERR_DENIED && !nw && !nc && !nb);
    disk[TOTAL54-1]^=1;boot=scan54(&ops,BOOT54);assert(boot.rc && boot.kind==UNKNOWN54);
    const int blocks[]={0,16,256,265};
    for(unsigned i=0;i<4;++i){
        reset();failwrite=blocks[i];boot=seed();g=armed();r=run54(&g,&boot,&ops);assert(r.rc==-12 && r.stage==WRITE54 && nw==(unsigned)blocks[i]+1 && na==1);
        reset();corrupt=blocks[i];g=armed();r=run54(&g,&boot,&ops);assert(r.rc && r.stage==LOCAL54 && nw==(unsigned)blocks[i]+1 && r.stop==blocks[i]*CHUNK54+length54(blocks[i]*CHUNK54)-1);
        reset();failread=blocks[i];g=armed();r=run54(&g,&boot,&ops);assert(r.rc==-12 && r.stage==LOCAL54 && nw==(unsigned)blocks[i]+1);
    }
    reset();boot=seed();g=armed();capbad=1;r=run54(&g,&boot,&ops);assert(r.rc && !nc && !nw && !nb);
    reset();g=armed();wrongcart=1;r=run54(&g,&boot,&ops);assert(r.rc && nc==1 && !nw && !nb);
    reset();g=armed();bindbad=1;r=run54(&g,&boot,&ops);assert(r.rc && nc==5 && !nw);
    reset();g=armed();safebad=1;r=run54(&g,&boot,&ops);assert(r.rc && !nc && !nw);
    reset();g=armed();failcart=TOTAL54-32;r=run54(&g,&boot,&ops);assert(r.rc && !nw); /* sample catches it */
    reset();g=armed();failcart=2*CHUNK54;r=run54(&g,&boot,&ops);assert(r.rc && nw==2 && r.stop==2*CHUNK54);
    reset();g=armed();step=900001;r=run54(&g,&boot,&ops);assert(r.rc==OF_ERR_TIMEOUT && !nw);
    reset();g=(struct guard51){0};r=run54(&g,&boot,&ops);assert(r.rc==OF_ERR_DENIED && !nc && !nw);
    for(int fault=1;fault<=2;++fault){reset();g=armed();verifybad=fault;r=run54(&g,&boot,&ops);assert(r.rc && r.stage==VERIFY54 && nw==266 && nr==532 && r.bytes==TOTAL54 && na==1);}
    /* Disjoint stage accounting, timer rollover, and failure-time attribution. */
    for(unsigned wrap=0;wrap<2;++wrap){
        reset();clocked=1;if(wrap)ticks=0xffff0000u;g=armed();r=run54(&g,&boot,&ops);
        assert(!r.rc && r.time.cart==272045 && r.time.write==2660 && r.time.readback==5320);
        assert(r.time.hash==0 && r.time.safe==3*269 && r.time.verify==5320+2*35);
        assert(r.ms==accounted54(&r.time)+2*37);
    }
    reset();clocked=1;failcart=32768;g=armed();r=run54(&g,&boot,&ops);
    assert(r.rc && r.time.cart==5+1024+1 && nw==1 && na==1 && r.ms>=accounted54(&r.time));
    free(rom);free(disk);puts("dump54 MD5 vectors + exact ROM + 266-block flow + cold-read + DMA/fault/gate + disjoint timing/wrap tests PASS");return 0;
}
