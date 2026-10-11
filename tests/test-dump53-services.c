#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
static uint32_t regs53[17],features53,wridx[16],wrval[16];static unsigned nw53,rx53;static int busy53;static long size53;
static uint32_t readreg53(unsigned i){assert(i<17);if(i==8){++rx53;return 0x03020100u+0x04040404u*(rx53-1);}return regs53[i];}
static unsigned response53=4;
static void writereg53(unsigned i,uint32_t v){assert(i<17 && nw53<16);wridx[nw53]=i;wrval[nw53++]=v;if(i==3 && v==2)regs53[2]=response53;}
#define DUMP53_TEST
#define RD53(i) readreg53(i)
#define WR53(i,v) writereg53(i,v)
#define FEATURES53 features53
#include "../.integration/openfpgaos/src/firmware/os/targets/pocket/dump53_services.c"
volatile int output_is_open;volatile uint32_t output_open_size,output_open_slot;
int of_file_async_busy(void){return busy53;}
long of_file_size(uint32_t slot){assert(slot==4);return size53;}
static void reset53(void){memset(regs53,0,sizeof(regs53));features53=32;regs53[0]=0x31424746;regs53[1]=0x00100f02;regs53[11]=0x80000303;regs53[9]=32;nw53=rx53=0;response53=4;busy53=0;size53=TOTAL53;output_is_open=17;output_open_slot=18;output_open_size=19;}
int main(void){
    reset53();of_cart_caps_t c;assert(!caps53(&c,sizeof(c)) && c.bus_mask==2 && c.operation_mask==15 && !c.writes_enabled && c.max_read_bytes==32 && c.gba_space_mask==1 && !c.gb_gbc_space_mask);
    assert(caps53(NULL,44)==-3 && caps53(&c,43)==-3 && nw53==0);
    features53=0;assert(caps53(&c,44)==-2 && !c.bus_mask && !c.operation_mask);reset53();
    uint8_t dst[34];memset(dst,0xcc,34);assert(!read53(2,TOTAL53-32,dst+1,32,50000));
    assert(dst[0]==0xcc && dst[33]==0xcc);for(unsigned i=0;i<32;++i)assert(dst[i+1]==i);
    assert(nw53==6 && rx53==8);unsigned idx[]={16,4,5,6,7,3};uint32_t val[]={0,2,TOTAL53-32,32,4500000,2};
    assert(!memcmp(idx,wridx,sizeof(idx)) && !memcmp(val,wrval,sizeof(val)));
    for(unsigned off=0;off<33;++off){reset53();int r=read53(2,off,dst,32,50000);assert((r==0)==(off%32==0));if(r)assert(!nw53);}
    reset53();assert(read53(2,TOTAL53,dst,32,50000)==-11 && !nw53);
    assert(read53(2,0xffffffe0,dst,32,50000)==-11 && !nw53);
    assert(read53(1,0,dst,32,50000)==-11 && !nw53);
    assert(read53(2,0,dst,16,50000)==-11 && !nw53);
    assert(read53(2,0,dst,32,1)==-11 && !nw53);
    assert(read53(2,0,NULL,32,50000)==-3 && !nw53);
    regs53[2]=2;assert(read53(2,0,dst,32,50000)==-100 && !nw53);
    reset53();regs53[11]=0;assert(read53(2,0,dst,32,50000)==-10 && !nw53);
    reset53();regs53[1]=0;assert(read53(2,0,dst,32,50000)==-2 && !nw53);
    for(unsigned i=0;i<5;++i){reset53();response53=(uint32_t[]){2,32,16,8,1}[i];memset(dst,0xcc,sizeof(dst));assert(read53(2,0,dst,32,50000)<0 && !rx53 && dst[0]==0xcc);if(i==0)assert(nw53==7 && wrval[6]==3);}
    reset53();regs53[9]=31;assert(read53(2,0,dst,32,50000)==-13 && !rx53);
    reset53();assert(!space53(2,0,0,dst,32,50000) && rx53==8);
    for(unsigned space=1;space<=4;++space){reset53();assert(space53(2,space,0,dst,32,50000)==-2 && !nw53 && !rx53);}
    for(int busy=-1;busy<=1;++busy)for(unsigned sz=TOTAL53-1;sz<=TOTAL53+1;++sz){reset53();busy53=busy;size53=sz;int r=bind53("dump53.gba",TOTAL53);assert((r==0)==(!busy && sz==TOTAL53));assert(!nw53);if(r)assert(output_is_open==17 && output_open_size==19 && output_open_slot==18);else assert(output_is_open==1 && output_open_size==TOTAL53 && output_open_slot==4);}
    for(unsigned i=0;i<sizeof("dump53.gba");++i){reset53();char s[]="dump53.gba";s[i]^=1;assert(bind53(s,TOTAL53)==-3 && output_is_open==17);}
    reset53();assert(bind53(NULL,0)==MAGIC53 && bind53(NULL,1)==-3 && bind53("dump53.gba",1)==-3 && !nw53);
    puts("dump53 native OS caps/read/bind + no-write-on-reject + bounded polling/abort tests PASS");
}
