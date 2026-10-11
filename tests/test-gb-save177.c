#define main old_main175
#include "test-gb-save175.c"
#undef main
static unsigned mode177,plain177,bankmax177,modewrites177;
static int control177(void *v,uint16_t a,uint8_t d){
 struct mock *s=v;assert(!plain177&&s->owned&&!s->removed);
 if(a==0x6000){assert(d<=1);mode177=d;modewrites177++;s->controls++;return s->controls==s->fail_control?-77:0;}
 if(a==0x4000){assert(d<=bankmax177);s->controls++;s->bank=d;return s->controls==s->fail_control?-77:0;}
 return control(v,a,d);
}
static int header177(void *v,uint8_t *h){assert(!mode177||!m.bank);return header(v,h);}
static int ram177(void *v,uint16_t a,void *p,uint32_t n){
 struct mock *s=v;
 if(plain177){assert(!s->controls&&s->bank==0);s->enabled=1;}
 else if(s->h[0x47]==2||s->h[0x47]==3)assert(mode177==1);
 return ram(v,a,p,n);
}
static void init177(unsigned t,unsigned r,unsigned bytes){
 setup(t,r);m.bytes=bytes;mode177=plain177=modewrites177=0;bankmax177=(bytes+8191)/8192-1;
 plain177=t==8||t==9;
}
static int capture177(unsigned selected){
 struct gb175_bus b={&m,acquire,alive,cancelled,header177,control177,ram177,release};
 return gb177_capture(&b,1,m.h,selected,snapshot,sizeof snapshot,scratch,512,&report);
}
int main(void){
 struct gb175_plan p;unsigned bank;uint16_t addr;uint32_t n;
 for(unsigned t=2;t<=9;t++)for(unsigned r=0;r<=5;r++){
  setup(t,r);int ok=((t==2||t==3)&&r>=1&&r<=3)||((t==8||t==9)&&(r==1||r==2));
  assert((gb177_plan(1,m.h,0,&p)==0)==ok);
 }
 init177(3,1,2048);assert(!gb177_plan(1,m.h,2048,&p)&&p.banks==1);
 assert(!gb175_chunk(&p,2047,512,&bank,&addr,&n)&&addr==0xa7ff&&n==1);
 assert(gb175_chunk(&p,2048,512,&bank,&addr,&n)==GB175_ARGUMENT);
 assert(!capture177(2048)&&report.compared==2048&&!mode177&&!m.bank&&!m.enabled);
 for(unsigned t=8;t<=9;t++){
  init177(t,1,2048);assert(!capture177(2048)&&!m.controls&&report.compared==2048);
  init177(t,2,8192);assert(!capture177(0)&&!m.controls);
  assert(gb177_plan(1,m.h,32768,&p)==GB175_UNSUPPORTED);
 }
 init177(3,3,32768);assert(!capture177(0)&&!memcmp(snapshot,m.ram,32768));
 assert(!mode177&&!m.bank&&!m.enabled&&m.headers==3&&modewrites177);
 unsigned controls=m.controls;
 for(unsigned i=1;i<=controls;i++){
  init177(3,3,32768);m.fail_control=i;assert(capture177(0)<0&&!m.owned);
 }
 for(unsigned i=1;i<=128;i++){
  init177(3,3,32768);m.fail_read=i;assert(capture177(0)==GB175_IO&&!mode177&&!m.enabled&&!m.owned);
  init177(3,3,32768);m.remove_read=i;assert(capture177(0)==GB175_CANCELLED&&!m.owned);
 }
 init177(3,3,32768);m.change_read=65;assert(capture177(0)==GB175_CHANGED&&!mode177&&!m.bank&&!m.enabled);
 init177(3,1,2048);for(unsigned i=0;i<2048;i++)snapshot[i]=(unsigned char)(i*13+i/256);
 for(unsigned i=2048;i<32768;i++)snapshot[i]=snapshot[i%2048];
 assert(gb177_content_bytes(snapshot,32768)==2048);
 memset(snapshot,0,sizeof snapshot);assert(!gb177_content_bytes(snapshot,32768));
 puts("PASS GB177: MBC1 modes/banks/cleanup and injected failures, plain RAM no control writes, 2KiB bounds");
 return 0;
}
