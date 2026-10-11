/* MBC7 only: FlashGBX 16-bit MSB-first words, EWEN/WRITE/EWDS.
 * No ERASE, ERAL or WRAL. The existing read-only capture is unchanged. */
#include "mbc7_restore185.h"
#include <string.h>
static int stop185(const struct gb175_bus *b){return !b->alive(b->ctx)||b->cancelled(b->ctx);}
static int pin185(const struct gb175_bus *b,uint8_t v){return stop185(b)?GB175_CANCELLED:b->control(b->ctx,0xa080,v);}
static int bit185(const struct gb175_bus *b,unsigned bit){
 uint8_t v=0x80|(bit?2:0);int rc=pin185(b,v);return rc?rc:pin185(b,v|0x40);
}
static int cmd185(const struct gb175_bus *b,unsigned cmd){
 int rc=pin185(b,0);if(!rc)rc=pin185(b,0x80);
 /* OS adapter preserves FlashGBX's 15 ms CS-rise settling interval. */
 for(int bit=10;bit>=0&&!rc;bit--)rc=bit185(b,(cmd>>bit)&1);
 return rc;
}
static int read185(const struct gb175_bus *b,unsigned a,uint16_t *word){
 int rc=cmd185(b,0x600|a);uint8_t v=0;*word=0;
 if(!rc)rc=b->read_ram(b->ctx,0xa080,&v,1);
 if(!rc&&(v&1))rc=GB175_IO;
 for(unsigned i=0;i<16&&!rc;i++){
  rc=bit185(b,0);if(!rc)rc=b->read_ram(b->ctx,0xa080,&v,1);
  *word=(uint16_t)((*word<<1)|(v&1));
 }
 if(!rc)rc=pin185(b,0);
 return rc;
}
static int access185(const struct gb175_bus *b,int enable){
 int rc=cmd185(b,enable?0x4c0:0x400);return rc?rc:pin185(b,0);
}
static int write185(const struct gb185_bus *ops,unsigned a,uint16_t word){
 const struct gb175_bus *b=&ops->io;int rc=cmd185(b,0x500|a);
 for(int i=15;i>=0&&!rc;i--)rc=bit185(b,(word>>i)&1);
 if(!rc)rc=pin185(b,0); /* Commit complete word. */
 if(!rc)rc=pin185(b,0x80); /* CS rising: adapter waits 15 ms before polling. */
 uint32_t start=ops->now_ms(b->ctx);unsigned polls=0;uint8_t v=0;
 while(!rc){
  if(stop185(b)){rc=GB175_CANCELLED;break;}
  rc=b->read_ram(b->ctx,0xa080,&v,1);if(rc||(v&1))break;
  if((uint32_t)(ops->now_ms(b->ctx)-start)>=50||++polls>=10000){rc=GB175_IO;break;}
 }
 if(!rc)rc=pin185(b,0);
 return rc;
}
static int no_cancel185(void *ctx){(void)ctx;return 0;}
int gb185_restore(const struct gb185_bus *ops,const uint8_t h[80],const void *original,
 const void *source,uint32_t bytes,struct gb175_report *r){
 if(!r)return GB175_ARGUMENT;
 memset(r,0,sizeof *r);struct gb175_plan p;int rc=gb179_plan(1,h,bytes,&p);
 if(rc)return r->rc=rc;
 if(p.mapper!=GB179_MBC7||bytes!=256)return r->rc=GB175_UNSUPPORTED;
 if(!ops||!ops->now_ms||!original||!source)return r->rc=GB175_ARGUMENT;
 const struct gb175_bus *b=&ops->io;
 if(!b->acquire||!b->release||!b->read_header||!b->control||!b->read_ram||!b->alive||!b->cancelled)return r->rc=GB175_ARGUMENT;
 uintptr_t a=(uintptr_t)original,s=(uintptr_t)source;
 if(a<=s?s-a<256:a-s<256)return r->rc=GB175_ARGUMENT;
 uint8_t expected[80],current[80];memcpy(expected,h,80);
 rc=b->acquire(b->ctx);if(rc){r->detail=rc;return r->rc=GB175_IO;}
 int touched=0,enabled=0,serial_ready=0;
 for(unsigned pass=0;pass<3&&!rc;pass++){
  if(stop185(b)){rc=GB175_CANCELLED;break;}
  rc=b->read_header(b->ctx,current);if(rc)break;
  if(memcmp(current,expected,80)){rc=GB175_IDENTITY;break;}
  touched=1;rc=b->control(b->ctx,0,10);if(!rc)rc=b->control(b->ctx,0x4000,64);if(rc)break;
  serial_ready=1;
  for(unsigned w=0;w<128&&!rc;w++){
   if(stop185(b)){rc=GB175_CANCELLED;break;}
   const uint8_t *want=(const uint8_t*)(pass?source:original)+2*w;
   uint16_t value=(uint16_t)((want[0]<<8)|want[1]);
   if(pass==1){
    if(!memcmp((const uint8_t*)original+2*w,want,2))continue;
    if(!enabled){rc=access185(b,1);if(rc)break;enabled=1;}
    r->written+=2;rc=write185(ops,w,value);
   }else{
    uint16_t got=0;rc=read185(b,w,&got);
    if(!rc&&got!=value)rc=GB175_CHANGED;
    /* Before writing, also check the qualified 256-byte mirror. */
    if(!rc&&!pass){rc=read185(b,w+128,&got);if(!rc&&got!=value)rc=GB175_UNSUPPORTED;}
    if(!rc){if(pass)r->verified+=2;else r->compared+=2;}
   }
  }
  if(pass==1&&!rc&&enabled){rc=access185(b,0);if(!rc)enabled=0;}
 }
 r->rc=rc;r->detail=rc;
 if(touched&&b->alive(b->ctx)){
  /* Cancel does not suppress EWDS; removal always suppresses bus accesses. */
  struct gb175_bus cleanup=*b;cleanup.cancelled=no_cancel185;
  int cr=serial_ready?access185(&cleanup,0):0;if(cr)r->cleanup_rc=cr;
  const uint16_t addr[]={0xa080,0x4000,0};
  for(unsigned i=0;i<3&&b->alive(b->ctx);i++){cr=b->control(b->ctx,addr[i],0);if(cr&&!r->cleanup_rc)r->cleanup_rc=cr;}
 }
 if(!r->rc){
  if(stop185(b))r->rc=GB175_CANCELLED;
  else if((rc=b->read_header(b->ctx,current)))r->rc=rc;
  else if(memcmp(current,expected,80))r->rc=GB175_IDENTITY;
 }
 rc=b->release(b->ctx);if(rc&&!r->cleanup_rc)r->cleanup_rc=rc;
 if(!r->rc&&r->cleanup_rc)r->rc=GB175_CLEANUP;
 return r->rc;
}
