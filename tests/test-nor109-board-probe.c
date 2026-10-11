#include "board_probe109.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
struct model {unsigned kind,bank,hi,lo,ops,writes,releases,epoch,clock,interrupt,failwrite,badbank,badcfibank;bool id[2],query[2],dead,freeze,wrongdie;};
static struct model m;
static uint16_t cfiwords[8][512];
static bool full;
static struct cb_board109_scratch scratch;
static void pair(unsigned bank,unsigned a,unsigned value){cfiwords[bank][a]=value&255;cfiwords[bank][a+1]=value>>8;}
static void cfi_fixture(unsigned k){
 memset(cfiwords,0,sizeof(cfiwords));
 for(unsigned b=0;b<8;b++){
  cfiwords[b][0x10]='Q';cfiwords[b][0x11]='R';cfiwords[b][0x12]='Y';cfiwords[b][0x13]=1;
  cfiwords[b][0x27]=k==1?23:k==2?24:25;cfiwords[b][0x2a]=5;
  cfiwords[b][0x2c]=k<3?1:2;
  if(k<3){pair(b,0x2d,k==1?63:127);pair(b,0x2f,512);}
  else if(b%4<2){pair(b,0x2d,3);pair(b,0x2f,128);pair(b,0x31,254);pair(b,0x33,512);}
  else {pair(b,0x2d,254);pair(b,0x2f,512);pair(b,0x31,3);pair(b,0x33,128);}
 }
}
static bool alive(void*c){return !((struct model*)c)->dead;}
static uint32_t epoch(void*c){return ((struct model*)c)->epoch;}
static uint32_t now(void*c){struct model*p=c;return p->freeze?0:p->clock++;}
static void yield(void*c){(void)c;}
static void release(void*c){((struct model*)c)->releases++;}
static void tick(struct model*p){assert(!p->dead);p->ops++;if(p->ops==p->interrupt){p->epoch++;p->dead=true;}}
static int write(void*c,unsigned bus,uint32_t a,uint16_t v){
 struct model*p=c;tick(p);p->writes++;
 if(p->writes==p->failwrite)return -1;
 if(bus==3){assert(a==6&&(v==0||v==0x10));p->hi=v?4:0;return 0;}
 assert(bus==2&&!(a&1)&&a<0x2000000);
 if(a==0||a==2){assert(v==0xff||v==0x90||v==0x98);p->id[a/2]=v==0x90;p->query[a/2]=v==0x98;return 0;}
 if(a==0x16b592e){assert(v<=0xc00&&!(v&0x3ff));p->lo=v/0x400;}
 else if(a==0x130eca8)assert(v==0x5354);
 else if(a==0x1c2468a)assert(v==0xa55a);
 else {assert(a==0x1e2468a&&v==0x9413);p->bank=p->hi+p->lo;}
 return 0;
}
static int read(void*c,uint32_t a,uint16_t*v){
 struct model*p=c;tick(p);assert(a<2048&&!(a&1));
 if(p->query[(a/2)%2]){
  *v=cfiwords[p->bank][a/4];
  if(p->wrongdie&&a%4==2&&a/4==0x27)*v^=1;
  if(p->badcfibank==p->bank+1&&a/4==0x2a)*v=4;
  return 0;
 }
 assert(a<32);
 if(p->id[(a/2)%2]&&a<8){
  *v=a<4?0x89:p->kind==1?0x17:p->kind==2?0x18:p->bank%4<2?0x8810:0x880d;
  if(p->badbank==p->bank+1)*v^=0x40;
 }else *v=(uint16_t)(0x2300+p->bank*32+a);
 return 0;
}
static struct cb_board109_io io={&m,write,read,alive,epoch,now,yield,release};
static int run(unsigned kind,struct cb_board109_capture*out){
 uint8_t seed[8]={0x89,0,0x89,0,0,0,0,0};
 seed[4]=seed[6]=kind==1?0x17:kind==2?0x18:0x10;
 seed[5]=seed[7]=kind==3?0x88:0;
 m.kind=kind;return full?cb_board109_capture_full(&io,kind,seed,&scratch,out):cb_board109_capture_ids(&io,kind,seed,out);
}
int main(void){
 struct cb_board109_capture out;
 for(unsigned k=1;k<=3;k++){
  memset(&m,0,sizeof(m));assert(!run(k,&out));
  assert(out.ids_valid&&out.restored&&out.profile.banks==(k==3?8u:1u));
  assert(!m.bank&&!m.id[0]&&!m.id[1]&&m.releases==1);
  unsigned count=m.ops;
  /* Every individual read/write can lose lifecycle immediately after it.
   * The next I/O callback would assert: no cleanup writes to a changed cart. */
  for(unsigned n=1;n<=count;n++){
   memset(&m,0,sizeof(m));m.interrupt=n;
   assert(run(k,&out)==CB_NOR_CANCELLED&&m.ops==n&&m.releases==1&&!out.ids_valid);
  }
  for(unsigned n=1;n<=4;n++){
   memset(&m,0,sizeof(m));m.failwrite=n;
   assert(run(k,&out)==CB_NOR_IO&&m.writes==n&&m.releases==1&&!out.ids_valid);
  }
 }
 for(unsigned b=1;b<=8;b++){
  memset(&m,0,sizeof(m));m.badbank=b;
  assert(run(3,&out)==CB_NOR_VERIFY&&!out.ids_valid&&!m.bank&&m.releases==1);
 }
 memset(&m,0,sizeof(m));uint8_t badseed[8]={0};
 assert(cb_board109_capture_ids(&io,3,badseed,&out)==CB_NOR_UNSUPPORTED&&!m.ops&&m.releases==1);
 memset(&m,0,sizeof(m));m.freeze=true;
 assert(run(3,&out)==CB_NOR_TIMEOUT&&!out.ids_valid&&m.releases==1);
 full=true;
 for(unsigned k=1;k<=3;k++){
  cfi_fixture(k);memset(&m,0,sizeof(m));assert(!run(k,&out)&&out.geometry_valid&&!out.geometry_rc&&out.restored);
  unsigned count=m.ops;
  for(unsigned n=1;n<count;n+=257){
   memset(&m,0,sizeof(m));m.interrupt=n;
   assert(run(k,&out)==CB_NOR_CANCELLED&&m.ops==n&&!out.geometry_valid&&m.releases==1);
  }
  memset(&m,0,sizeof(m));m.wrongdie=true;
  assert(run(k,&out)==CB_NOR_VERIFY&&!out.geometry_valid&&!m.bank);
  memset(&m,0,sizeof(m));m.badcfibank=k==3?8:1;
  assert(!run(k,&out)&&out.ids_valid&&out.restored&&!out.geometry_valid&&out.geometry_rc==CB_NOR_UNSUPPORTED);
 }
 puts("PASS board109 capture: repeated/restored window IDs; dual-die repeated CFI; geometry gate; lifecycle interruption; partial unlock failure; wrong bank; invalid seed; clock timeout");
}
