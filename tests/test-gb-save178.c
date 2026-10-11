#define main old_main175
#include "test-gb-save175.c"
#undef main
static int control178(void *v,uint16_t a,uint8_t d){
 assert(a==0);return control(v,a,d);
}
static int ram178(void *v,uint16_t a,void *p,uint32_t n){
 assert(a>=0xa000&&a<0xa200&&n<=0xa200u-a);
 int rc=ram(v,a,p,n);
 /* Upper bits deliberately differ between reads, lower bits remain real. */
 for(unsigned i=0;i<n;i++)((uint8_t*)p)[i]=(((uint8_t*)p)[i]&15)|((m.reads&1)?0xf0:0x50);
 return rc;
}
static void init178(unsigned type){setup(type,0);m.bytes=512;}
static int capture178(unsigned selected,unsigned chunk){
 const struct gb175_bus b={&m,acquire,alive,cancelled,header,control178,ram178,release};
 return gb178_capture(&b,1,m.h,selected,snapshot,sizeof snapshot,scratch,chunk,&report);
}
int main(void){
 struct gb175_plan p;unsigned bank;uint16_t addr;uint32_t n;
 for(unsigned type=5;type<=6;type++){
  init178(type);assert(!gb178_plan(1,m.h,0,&p)&&p.bytes==512&&p.mapper==2&&p.banks==1);
  assert(gb177_plan(1,m.h,0,&p)==GB175_UNSUPPORTED);
  assert(gb178_plan(2,m.h,0,&p)==GB175_UNSUPPORTED);
  assert(gb178_plan(1,m.h,8192,&p)==GB175_UNSUPPORTED);
  assert(!gb178_plan(1,m.h,512,&p));
  assert(!gb175_chunk(&p,511,999,&bank,&addr,&n)&&addr==0xa1ff&&n==1&&bank==0);
  assert(gb175_chunk(&p,512,1,&bank,&addr,&n)==GB175_ARGUMENT);
  for(unsigned chunk=1;chunk<=512;chunk*=2){
   init178(type);assert(!capture178(0,chunk)&&report.compared==512&&!m.enabled&&!m.owned&&m.controls==3);
   for(unsigned i=0;i<512;i++)assert(snapshot[i]==(m.ram[i]&15));
  }
  init178(type);memset(m.ram,255,512);assert(!capture178(512,512));
  for(unsigned i=0;i<512;i++)assert(snapshot[i]==15);
 }
 for(unsigned i=1;i<=4;i++){
  init178(6);m.fail_read=i;assert(capture178(0,256)==GB175_IO&&!m.enabled&&!m.owned);
  init178(6);m.remove_read=i;assert(capture178(0,256)==GB175_CANCELLED&&!m.owned);
  init178(6);m.cancel_read=i;assert(capture178(0,256)==GB175_CANCELLED&&!m.enabled&&!m.owned);
 }
 init178(6);m.change_read=3;assert(capture178(0,256)==GB175_CHANGED&&!m.enabled&&!m.owned);
 for(unsigned i=1;i<=3;i++){
  init178(6);m.fail_control=i;assert(capture178(0,512)==(i==3?GB175_CLEANUP:GB175_IO)&&!m.owned);
  init178(6);m.change_header=i;assert(capture178(0,512)==GB175_IDENTITY&&!m.enabled&&!m.owned);
 }
 puts("PASS GB178 MBC2: 512 low-nibble bytes, bounds, no bank writes, double-read, failure cleanup, old API isolation");
 return 0;
}
