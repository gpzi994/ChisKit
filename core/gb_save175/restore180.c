#include "restore180.h"
#include <string.h>
static int stop180(const struct gb175_bus *b){return !b->alive(b->ctx)||b->cancelled(b->ctx);}
static int control180(const struct gb175_bus *b,uint16_t a,uint8_t d){
 return stop180(b)?GB175_CANCELLED:b->control(b->ctx,a,d);
}
static int differs184(const void *a,const void *b,uint32_t n,int mbc2){
 if(!mbc2)return memcmp(a,b,n)!=0;
 for(uint32_t i=0;i<n;i++)if((((const uint8_t*)a)[i]^((const uint8_t*)b)[i])&15)return 1;
 return 0;
}
int gb180_restore(const struct gb180_bus *ops,const uint8_t expected[80],const void *original,
 const void *source,uint32_t bytes,void *scratch,uint32_t chunk,struct gb175_report *r){
 if(!r)return GB175_ARGUMENT;
 memset(r,0,sizeof *r);struct gb175_plan p;
 int rc=gb180_plan(expected,bytes,&p);if(rc)return r->rc=rc;
 if(!ops||!original||!source||!scratch||!chunk||chunk>8192)return r->rc=GB175_ARGUMENT;
 const struct gb175_bus *b=&ops->read;
 if(!b->acquire||!b->alive||!b->cancelled||!b->read_header||!b->control||!b->read_ram||!b->release||!ops->write_ram)return r->rc=GB175_ARGUMENT;
 uintptr_t s=(uintptr_t)scratch,a=(uintptr_t)original,t=(uintptr_t)source;
 if((s<=a?a-s<chunk:s-a<bytes)||(s<=t?t-s<chunk:s-t<bytes)||
    (a<=t?t-a<bytes:a-t<bytes))return r->rc=GB175_ARGUMENT;
 uint8_t identity[80],current[80];memcpy(identity,expected,80);
 rc=b->acquire(b->ctx);if(rc){r->detail=rc;return r->rc=GB175_IO;}
 int touched=0;
 /* Three separate sweeps: compare backed-up original before ANY data write,
  * program changed chunks, then compare the entire resulting RAM. */
 for(unsigned pass=0;pass<3&&!rc;pass++){
  if(stop180(b)){rc=GB175_CANCELLED;break;}
  if(p.mapper==GB177_MBC1){
   touched=1;rc=control180(b,0x4000,0);if(!rc)rc=control180(b,0x6000,0);if(rc)break;
  }
  rc=b->read_header(b->ctx,current);if(rc)break;
  if(memcmp(current,identity,80)){rc=GB175_IDENTITY;break;}
  if(p.mapper!=GB177_PLAIN){
   touched=1;
   if(p.mapper==GB177_MBC1)rc=control180(b,0x6000,1);
   if(!rc)rc=control180(b,0,10);
   if(rc)break;
  }
  unsigned selected=~0u;
  for(uint32_t off=0;off<bytes&&!rc;){
   unsigned bank;uint16_t addr;uint32_t n;
   if(stop180(b)){rc=GB175_CANCELLED;break;}
   rc=gb175_chunk(&p,off,chunk,&bank,&addr,&n);if(rc)break;
   if(p.mapper!=GB177_PLAIN&&p.mapper!=GB178_MBC2&&bank!=selected){rc=control180(b,0x4000,(uint8_t)bank);if(rc)break;selected=bank;}
   if(stop180(b)){rc=GB175_CANCELLED;break;}
   if(pass==1){
    if(differs184((const uint8_t*)original+off,(const uint8_t*)source+off,n,p.mapper==GB178_MBC2)){
     const void *data=(const uint8_t*)source+off;
     if(p.mapper==GB178_MBC2){
      for(uint32_t i=0;i<n;i++)((uint8_t*)scratch)[i]=((const uint8_t*)data)[i]&15;
      data=scratch;
     }
     /* Count attempted bytes even if the lower layer fails mid-chunk. */
     r->written+=n;rc=ops->write_ram(b->ctx,addr,data,n);
    }
   }else{
    rc=b->read_ram(b->ctx,addr,scratch,n);
    if(!rc&&differs184(scratch,(const uint8_t*)(pass?source:original)+off,n,p.mapper==GB178_MBC2))rc=GB175_CHANGED;
    if(!rc){if(pass)r->verified+=n;else r->compared+=n;}
   }
   off+=n;
  }
 }
 r->rc=rc;r->detail=rc;
 if(touched&&b->alive(b->ctx)){
  int cr=0;
  if(p.mapper!=GB178_MBC2){cr=b->control(b->ctx,0x4000,0);if(cr)r->cleanup_rc=cr;}
  if(b->alive(b->ctx)){cr=b->control(b->ctx,0,0);if(cr&&!r->cleanup_rc)r->cleanup_rc=cr;}
  if(p.mapper==GB177_MBC1&&b->alive(b->ctx)){cr=b->control(b->ctx,0x6000,0);if(cr&&!r->cleanup_rc)r->cleanup_rc=cr;}
 }
 if(!r->rc){
  if(stop180(b))r->rc=GB175_CANCELLED;
  else if((rc=b->read_header(b->ctx,current)))r->rc=rc;
  else if(memcmp(current,identity,80))r->rc=GB175_IDENTITY;
 }
 rc=b->release(b->ctx);if(rc&&!r->cleanup_rc)r->cleanup_rc=rc;
 if(!r->rc&&r->cleanup_rc)r->rc=GB175_CLEANUP;
 return r->rc;
}
