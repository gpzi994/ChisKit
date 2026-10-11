#include "rtc186.h"
#include <string.h>
int gb186_header(const uint8_t h[80]){
 if(!h)return GB175_ARGUMENT;
 /* Reuse only the GB header validator, never the GBA save/RTC code. */
 uint8_t copy[80];memcpy(copy,h,80);
 if(h[0x47]!=0x0f&&h[0x47]!=0x10)return GB175_UNSUPPORTED;
 uint8_t sum=0;for(unsigned i=0x34;i<=0x4c;i++)sum-=h[i]+1;
 if(sum!=h[0x4d])return GB175_IDENTITY;
 copy[0x47]=0x10;copy[0x49]=2;copy[0x4d]=0;
 for(unsigned i=0x34;i<=0x4c;i++)copy[0x4d]-=copy[i]+1;
 struct gb175_plan p;return gb175_plan_header(1,copy,&p);
}
static int stop(const struct gb175_bus *b){return !b->alive(b->ctx)||b->cancelled(b->ctx);}
static int put(const struct gb175_bus *b,uint16_t a,uint8_t v){return stop(b)?GB175_CANCELLED:b->control(b->ctx,a,v);}
int gb186_read(const struct gb175_bus *b,const uint8_t h[80],struct gb186_report *r){
 if(!r)return GB175_ARGUMENT;
 memset(r,0,sizeof *r);int rc=gb186_header(h);if(rc)return r->rc=rc;
 if(!b||!b->acquire||!b->release||!b->alive||!b->cancelled||!b->read_header||!b->control||!b->read_ram)return r->rc=GB175_ARGUMENT;
 uint8_t expected[80],check[80],buf[256];memcpy(expected,h,80);
 rc=b->acquire(b->ctx);if(rc){r->detail=rc;return r->rc=GB175_IO;}
 if(stop(b))rc=GB175_CANCELLED;
 if(!rc)rc=b->read_header(b->ctx,check);
 if(!rc&&memcmp(check,expected,80))rc=GB175_IDENTITY;
 int touched=0;
 if(!rc){touched=1;rc=put(b,0,0);}
 if(!rc)rc=put(b,0,10);
 if(!rc)rc=put(b,0x6000,0);
 if(!rc)rc=put(b,0x6000,1);
 unsigned evidence=0;
 for(unsigned reg=0;reg<5&&!rc;reg++){
  rc=put(b,0x4000,(uint8_t)(8+reg));
  for(unsigned pass=0;pass<2&&!rc;pass++){
   if(stop(b)){rc=GB175_CANCELLED;break;}
   rc=b->read_ram(b->ctx,0xa880,buf,sizeof buf);if(rc)break;
   for(unsigned j=1;j<sizeof buf;j++)if(buf[j]!=buf[0]){rc=GB175_CHANGED;break;}
   if(!rc&&pass&&buf[0]!=r->raw[reg])rc=GB175_CHANGED;
   if(!rc)r->raw[reg]=buf[0];
  }
  if(!rc&&r->raw[reg]!=0&&r->raw[reg]!=255)evidence=1;
 }
 if(!rc&&!evidence)rc=GB175_UNSUPPORTED;
 if(!rc&&(r->raw[0]>59||r->raw[1]>59||r->raw[2]>23||(r->raw[4]&0x3e)))rc=GB175_CHANGED;
 r->detail=rc;
 if(touched&&b->alive(b->ctx)){
  int clean=b->control(b->ctx,0,0);if(clean)r->cleanup_rc=clean;
  if(b->alive(b->ctx)){clean=b->control(b->ctx,0x4000,0);if(clean&&!r->cleanup_rc)r->cleanup_rc=clean;}
 }
 if(!rc){
  if(stop(b))rc=GB175_CANCELLED;
  else if((rc=b->read_header(b->ctx,check))==0&&memcmp(check,expected,80))rc=GB175_IDENTITY;
 }
 int clean=b->release(b->ctx);if(clean&&!r->cleanup_rc)r->cleanup_rc=clean;
 if(!rc&&r->cleanup_rc)rc=GB175_CLEANUP;
 if(!rc){r->supported=1;r->seconds=r->raw[0];r->minutes=r->raw[1];r->hours=r->raw[2];r->days=r->raw[3]|((r->raw[4]&1)<<8);r->halt=(r->raw[4]>>6)&1;r->carry=r->raw[4]>>7;}
 return r->rc=rc;
}
