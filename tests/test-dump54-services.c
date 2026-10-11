#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
static uint32_t regs54[17],features54,wridx[16],wrval[16];static unsigned nw54,rx54;static int busy54;static long size54;
static uint32_t readreg54(unsigned i){assert(i<17);if(i==8){++rx54;return 0x03020100u+0x04040404u*(rx54-1);}return regs54[i];}
static unsigned response54=4;
static void writereg54(unsigned i,uint32_t v){assert(i<17 && nw54<16);wridx[nw54]=i;wrval[nw54++]=v;if(i==3 && v==2)regs54[2]=response54;}
#define DUMP54_TEST
#define RD54(i) readreg54(i)
#define WR54(i,v) writereg54(i,v)
#define FEATURES54 features54
#include "../.integration/openfpgaos/src/firmware/os/targets/pocket/dump54_services.c"
volatile int output_is_open;volatile uint32_t output_open_size,output_open_slot;
int of_file_async_busy(void){return busy54;}
long of_file_size(uint32_t slot){assert(slot==4);return size54;}
static void reset54(void){memset(regs54,0,sizeof(regs54));features54=32;regs54[0]=0x31424746;regs54[1]=0x00100f02;regs54[11]=0x80000303;regs54[9]=32;nw54=rx54=0;response54=4;busy54=0;size54=TOTAL54;output_is_open=17;output_open_slot=18;output_open_size=19;}
int main(void){
    reset54();of_cart_caps_t c;assert(!caps54(&c,sizeof(c)) && c.bus_mask==2 && c.operation_mask==15 && !c.writes_enabled && c.max_read_bytes==32 && c.gba_space_mask==1 && !c.gb_gbc_space_mask);
    assert(caps54(NULL,44)==-3 && caps54(&c,43)==-3 && nw54==0);
    features54=0;assert(caps54(&c,44)==-2 && !c.bus_mask && !c.operation_mask);reset54();
    uint8_t dst[34];memset(dst,0xcc,34);assert(!read54(2,TOTAL54-32,dst+1,32,50000));
    assert(dst[0]==0xcc && dst[33]==0xcc);for(unsigned i=0;i<32;++i)assert(dst[i+1]==i);
    assert(nw54==6 && rx54==8);unsigned idx[]={16,4,5,6,7,3};uint32_t val[]={0,2,TOTAL54-32,32,4500000,2};
    assert(!memcmp(idx,wridx,sizeof(idx)) && !memcmp(val,wrval,sizeof(val)));
    for(unsigned off=0;off<33;++off){reset54();int r=read54(2,off,dst,32,50000);assert((r==0)==(off%32==0));if(r)assert(!nw54);}
    reset54();assert(read54(2,TOTAL54,dst,32,50000)==-11 && !nw54);
    assert(read54(2,0xffffffe0,dst,32,50000)==-11 && !nw54);
    assert(read54(1,0,dst,32,50000)==-11 && !nw54);
    assert(read54(2,0,dst,16,50000)==-11 && !nw54);
    assert(read54(2,0,dst,32,1)==-11 && !nw54);
    assert(read54(2,0,NULL,32,50000)==-3 && !nw54);
    regs54[2]=2;assert(read54(2,0,dst,32,50000)==-100 && !nw54);
    reset54();regs54[11]=0;assert(read54(2,0,dst,32,50000)==-10 && !nw54);
    reset54();regs54[1]=0;assert(read54(2,0,dst,32,50000)==-2 && !nw54);
    for(unsigned i=0;i<5;++i){reset54();response54=(uint32_t[]){2,32,16,8,1}[i];memset(dst,0xcc,sizeof(dst));assert(read54(2,0,dst,32,50000)<0 && !rx54 && dst[0]==0xcc);if(i==0)assert(nw54==7 && wrval[6]==3);}
    reset54();regs54[9]=31;assert(read54(2,0,dst,32,50000)==-13 && !rx54);
    reset54();assert(!space54(2,0,0,dst,32,50000) && rx54==8);
    for(unsigned space=1;space<=4;++space){reset54();assert(space54(2,space,0,dst,32,50000)==-2 && !nw54 && !rx54);}
    for(int busy=-1;busy<=1;++busy)for(unsigned sz=TOTAL54-1;sz<=TOTAL54+1;++sz){reset54();busy54=busy;size54=sz;int r=bind54("dump54.gba",TOTAL54);assert((r==0)==(!busy && sz==TOTAL54));assert(!nw54);if(r)assert(output_is_open==17 && output_open_size==19 && output_open_slot==18);else assert(output_is_open==1 && output_open_size==TOTAL54 && output_open_slot==4);}
    for(unsigned i=0;i<sizeof("dump54.gba");++i){reset54();char s[]="dump54.gba";s[i]^=1;assert(bind54(s,TOTAL54)==-3 && output_is_open==17);}
    reset54();assert(bind54(NULL,0)==MAGIC54 && bind54(NULL,1)==-3 && bind54("dump54.gba",1)==-3 && !nw54);
    puts("dump54 native OS caps/read/bind + no-write-on-reject + bounded polling/abort tests PASS");
}
