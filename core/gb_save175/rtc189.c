#include "rtc189.h"
#include <string.h>
static int valid(const uint8_t *r){return r[0]<60&&r[1]<60&&r[2]<24&&!(r[4]&0x3e);}
int gb189_parse(const uint8_t t[48],struct gb189_value *v){
 if(!t||!v)return -1;
 memset(v,0,sizeof *v);
 for(unsigned j=0;j<2;j++){
  uint8_t r[5];for(unsigned i=0;i<5;i++){
   unsigned off=j*20+i*4;if(t[off+1]||t[off+2]||t[off+3])return -1;r[i]=t[off];
  }
  if(!valid(r))return -1;
 }
 for(unsigned i=0;i<5;i++)v->raw[i]=t[i*4];
 for(unsigned i=0;i<8;i++)v->timestamp|=(uint64_t)t[40+i]<<(8*i);
 return v->timestamp>=946684800ULL&&v->timestamp<=4102444799ULL?0:-1;
}
int gb189_advance(struct gb189_value *v,uint64_t now){
 if(!v||!valid(v->raw)||now<v->timestamp||now>4102444799ULL)return -1;
 if(v->raw[4]&0x40)return 0; /* A stopped clock must stay stopped. */
 uint64_t s=((((uint64_t)(v->raw[3]|((v->raw[4]&1)<<8))*24+v->raw[2])*60+v->raw[1])*60+v->raw[0])+now-v->timestamp;
 unsigned carry=(v->raw[4]&0x80)||(s>=512ULL*86400);
 s%=512ULL*86400;v->raw[0]=s%60;s/=60;v->raw[1]=s%60;s/=60;v->raw[2]=s%24;s/=24;
 v->raw[3]=(uint8_t)s;v->raw[4]=(uint8_t)((s>>8)|(carry?0x80:0));return 0;
}
static int live(const struct gb175_bus *b){return b->alive(b->ctx)&&!b->cancelled(b->ctx);}
static int put(const struct gb175_bus *b,uint16_t a,uint8_t v){return live(b)?b->control(b->ctx,a,v):GB175_CANCELLED;}
static int latch(const struct gb175_bus *b){int rc=put(b,0x6000,0);return rc?rc:put(b,0x6000,1);}
static int read_regs(const struct gb175_bus *b,uint8_t r[5]){
 int rc=latch(b);uint8_t buf[256];
 for(unsigned i=0;i<5&&!rc;i++){
  rc=put(b,0x4000,8+i);if(!rc)rc=b->read_ram(b->ctx,0xa880,buf,256);
  if(!rc&&!live(b))rc=GB175_CANCELLED;
  if(!rc){r[i]=buf[0];for(unsigned j=1;j<256;j++)if(buf[j]!=buf[0]){rc=GB175_CHANGED;break;}}
 }return rc;
}
static int wr(const struct gb189_bus *b,unsigned reg,uint8_t v){
 int rc=put(&b->read,0x4000,reg);if(!rc)rc=live(&b->read)?b->write_rtc(b->read.ctx,v):GB175_CANCELLED;return rc;
}
int gb189_write(const struct gb189_bus *ops,const uint8_t h[80],const uint8_t target[5],struct gb189_report *r){
 if(!r)return GB175_ARGUMENT;
 memset(r,0,sizeof *r);
 if(!ops||!ops->write_rtc||!target||!valid(target))return r->rc=GB175_ARGUMENT;
 const struct gb175_bus *b=&ops->read;
 r->reserved[0]=1;
 struct gb186_report before;int rc=gb186_read(b,h,&before);
 memcpy(r->raw,before.raw,5);
 if(rc||!before.supported){r->detail=rc;return r->rc=rc?rc:GB175_UNSUPPORTED;}
 r->reserved[0]=2;
 rc=b->acquire(b->ctx);if(rc){r->detail=rc;return r->rc=GB175_IO;}
 uint8_t check[80],held[5];int touched=0,halt_attempted=0;
 if(!live(b))rc=GB175_CANCELLED;
 if(!rc)rc=b->read_header(b->ctx,check);
 if(!rc&&memcmp(check,h,80))rc=GB175_IDENTITY;
 if(!rc){touched=1;rc=put(b,0,10);}
 if(!rc)rc=latch(b); /* Unlock/latch before halt, matching FlashGBX. */
 if(!rc){r->reserved[0]=3;halt_attempted=1;rc=wr(ops,12,before.raw[4]|0x40);}
 /* Prove halt before changing the time registers; don't discover an ignored
  * halt only after attempting all five writes. */
 if(!rc){memset(held,0,sizeof held);rc=read_regs(b,held);memcpy(r->raw,held,5);}
 if(!rc&&!(held[4]&0x40))rc=GB175_CHANGED;
 for(unsigned i=0;i<5&&!rc;i++){
  r->reserved[0]=4;r->reserved[1]=(uint8_t)(8+i);
  rc=wr(ops,8+i,target[i]|(i==4?0x40:0));if(!rc)r->written++;
 }
 if(!rc){r->reserved[0]=5;memset(held,0,sizeof held);rc=read_regs(b,held);memcpy(r->raw,held,5);}
 if(!rc)for(unsigned i=0;i<5;i++)if(held[i]!=(uint8_t)(target[i]|(i==4?0x40:0)))rc=GB175_CHANGED;
 if(!rc){r->reserved[0]=6;rc=wr(ops,12,target[4]);}
 if(!rc){r->reserved[0]=7;rc=read_regs(b,r->raw);}
 if(!rc&&memcmp(r->raw,target,5)){
  struct gb189_value tick={{0},1000000000};memcpy(tick.raw,target,5);
  if(gb189_advance(&tick,tick.timestamp+1)||memcmp(r->raw,tick.raw,5))rc=GB175_CHANGED;
 }
 /* On failure restore at least the pre-write halt state; never silently leave a running clock halted. */
 if(rc&&halt_attempted&&b->alive(b->ctx)){
  int c=b->control(b->ctx,0x4000,12);if(!c)c=ops->write_rtc(b->ctx,before.raw[4]);if(c)r->cleanup_rc=c;
 }
 if(touched&&b->alive(b->ctx)){
  int c=b->control(b->ctx,0x4000,0);if(c&&!r->cleanup_rc)r->cleanup_rc=c;
  c=b->control(b->ctx,0,0);if(c&&!r->cleanup_rc)r->cleanup_rc=c;
 }
 int c=b->release(b->ctx);if(c&&!r->cleanup_rc)r->cleanup_rc=c;
 r->detail=rc;if(!rc&&r->cleanup_rc)rc=GB175_CLEANUP;
 if(!rc)r->verified=1;
 return r->rc=rc;
}
