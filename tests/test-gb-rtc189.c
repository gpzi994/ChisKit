#define GB186_TEST_MAIN old_rtc186_tests
#include "test-gb-rtc186.c"
#include "../core/gb_save175/rtc189.h"
static unsigned writes,fail_write,ignore_halt;
static int data(void *ctx,uint8_t v){
 (void)ctx;assert(m.bank>=8&&m.bank<=12&&m.owned&&m.enabled&&!m.removed);
 writes++;if(writes==fail_write)return -88;
 if(ignore_halt&&m.bank==12)return 0;
 regs[m.bank-8]=v;return 0;
}
static struct gb189_report out189;
static uint8_t target[]={58,59,23,255,1};
static int run189(void){
 const struct gb189_bus b={{&m,acquire,alive,cancelled,header,ctl,rd,release},data};
 return gb189_write(&b,m.h,target,&out189);
}
int main(void){
 init();assert(!run189()&&out189.verified&&writes==7&&!memcmp(target,regs,5)&&!m.enabled&&!m.bank&&!m.owned);
 for(unsigned i=1;i<=7;i++){init();writes=0;fail_write=i;assert(run189()!=0&&!out189.verified&&!m.owned&&!(regs[4]&0x40));}fail_write=0;
 for(unsigned i=1;i<=25;i++){init();writes=0;m.fail_read=i;assert(run189()!=0&&!out189.verified&&!m.owned);}
 for(unsigned i=1;i<=25;i++){init();writes=0;m.remove_read=i;assert(run189()!=0&&!out189.verified&&!m.owned);}
 init();memset(regs,0,5);writes=0;assert(run189()==GB175_UNSUPPORTED&&!writes);
 init();ignore_halt=1;writes=0;uint8_t unchanged[5];memcpy(unchanged,regs,5);
 assert(run189()==GB175_CHANGED&&out189.reserved[0]==3&&!out189.written&&writes==2&&!memcmp(unchanged,regs,5));ignore_halt=0;
 struct gb189_value v={{59,59,23,255,1},1700000000};assert(!gb189_advance(&v,1700000001));
 assert(!v.raw[0]&&!v.raw[1]&&!v.raw[2]&&!v.raw[3]&&v.raw[4]==0x80);
 v.raw[4]=0x40;assert(!gb189_advance(&v,1700000999)&&v.raw[4]==0x40);assert(gb189_advance(&v,1600000000));
 uint8_t t[48]={0};for(unsigned i=0;i<5;i++)t[i*4]=t[20+i*4]=target[i];
 for(unsigned i=0;i<8;i++)t[40+i]=(uint8_t)(1700000000ULL>>(8*i));
 assert(!gb189_parse(t,&v));
 t[1]=1;assert(gb189_parse(t,&v));t[1]=0;t[0]=60;assert(gb189_parse(t,&v));
 puts("GB189 PASS: halt/write/readback/cleanup, every data-write/read/removal fault, unsupported, elapsed rollover/halt, malformed footer");
}
