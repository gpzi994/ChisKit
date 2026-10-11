#include "gb_save175.h"
#include <string.h>
static int valid_header(const uint8_t *h){
 static const uint8_t logo[48]={0xce,0xed,0x66,0x66,0xcc,0x0d,0,0x0b,3,0x73,0,0x83,0,0x0c,0,0x0d,
 0,8,0x11,0x1f,0x88,0x89,0,0x0e,0xdc,0xcc,0x6e,0xe6,0xdd,0xdd,0xd9,0x99,
 0xbb,0xbb,0x67,0x63,0x6e,0x0e,0xec,0xcc,0xdd,0xdc,0x99,0x9f,0xbb,0xb9,0x33,0x3e};
 uint8_t sum=0;for(unsigned i=0x34;i<=0x4c;i++)sum=(uint8_t)(sum-h[i]-1);
 return !memcmp(h+4,logo,48)&&sum==h[0x4d];
}
int gb175_plan_header(unsigned bus,const uint8_t h[80],struct gb175_plan *p){
 if(!p)return GB175_ARGUMENT;
 memset(p,0,sizeof *p);
 if(bus!=1||!h)return GB175_UNSUPPORTED;
 if(!valid_header(h))return GB175_IDENTITY;
 unsigned t=h[0x47],r=h[0x49],mapper,limit;
 if(t==0x10||t==0x12||t==0x13){
  mapper=t==0x10&&r==5?GB175_MBC30:GB175_MBC3;
  limit=mapper==GB175_MBC30?65536:32768;
 }else if(t==0x1a||t==0x1b||t==0x1d||t==0x1e){
  mapper=GB175_MBC5;limit=t>=0x1d?65536:131072;
 }else return GB175_UNSUPPORTED;
 /* Conservative first release: normal 8 KiB banked RAM only. Legacy RAM
  * code 1 (2 KiB) and special mappers require separate qualification. */
 static const uint32_t sizes[]={0,2048,8192,32768,131072,65536};
 if(r<2||r>5||sizes[r]>limit)return GB175_UNSUPPORTED;
 p->mapper=mapper;p->bytes=sizes[r];p->banks=p->bytes/8192;
 p->rumble=t==0x1d||t==0x1e;p->rtc_declared=t==0x10;
 return 0;
}
static int valid_plan(const struct gb175_plan *p){
 if(p&&p->mapper==GB178_MBC2)return p->bytes==512&&p->banks==1&&!p->rumble&&!p->rtc_declared;
 if(!p||!p->bytes||(p->bytes!=2048&&p->bytes%8192)||p->banks!=(p->bytes+8191)/8192)return 0;
 if(p->mapper==GB177_PLAIN)return p->banks==1;
 if(p->mapper==GB177_MBC1)return p->banks<=4;
 if(p->mapper==GB175_MBC3)return p->banks<=4;
 if(p->mapper==GB175_MBC30)return p->banks<=8;
 return p->mapper==GB175_MBC5&&p->banks<=(p->rumble?8u:16u);
}
int gb176_plan(unsigned bus,const uint8_t h[80],uint32_t selected,struct gb175_plan *p){
 int rc=gb175_plan_header(bus,h,p);if(rc)return rc;
 uint32_t limit=p->mapper==GB175_MBC3?32768u:
  p->mapper==GB175_MBC30||p->rumble?65536u:131072u;
 if(selected&&selected!=8192&&selected!=32768&&selected!=65536&&selected!=131072){
  memset(p,0,sizeof *p);return GB175_UNSUPPORTED;
 }
 if(selected>limit){memset(p,0,sizeof *p);return GB175_UNSUPPORTED;}
 p->bytes=selected?selected:limit;p->banks=p->bytes/8192;return 0;
}
uint32_t gb176_content_bytes(const void *data,uint32_t bytes){
 if(!data||(bytes!=8192&&bytes!=32768&&bytes!=65536&&bytes!=131072))return 0;
 const uint8_t *d=data;
 while(bytes>32&&!memcmp(d,d+bytes/2,bytes/2))bytes/=2;
 /* Short/uniform patterns are inconclusive, not physical capacity evidence.
  * MBC2 4-bit RAM and small ordinary RAM remain separately qualified work. */
 return bytes==8192||bytes==32768||bytes==65536||bytes==131072?bytes:0;
}
int gb177_plan(unsigned bus,const uint8_t h[80],uint32_t selected,struct gb175_plan *p){
 if(!p)return GB175_ARGUMENT;
 memset(p,0,sizeof *p);
 if(bus!=1||!h)return GB175_UNSUPPORTED;
 if(!valid_header(h))return GB175_IDENTITY;
 unsigned t=h[0x47],r=h[0x49];uint32_t limit=0;
 if(t==8||t==9){
  if(r!=1&&r!=2)return GB175_UNSUPPORTED;
  p->mapper=GB177_PLAIN;limit=8192;
 }else if(t==2||t==3){
  if(r<1||r>3)return GB175_UNSUPPORTED;
  p->mapper=GB177_MBC1;limit=32768;
 }else if(r==1&&(t==0x10||t==0x12||t==0x13||t==0x1a||t==0x1b||t==0x1d||t==0x1e)){
  p->mapper=t<0x1a?GB175_MBC3:GB175_MBC5;
  p->rumble=t==0x1d||t==0x1e;p->rtc_declared=t==0x10;
  limit=p->mapper==GB175_MBC3?32768:p->rumble?65536:131072;
 }else{
  int rc=gb176_plan(bus,h,0,p);if(rc)return rc;limit=p->bytes;
 }
 if(selected&&selected!=2048&&selected!=8192&&selected!=32768&&selected!=65536&&selected!=131072){memset(p,0,sizeof *p);return GB175_UNSUPPORTED;}
 if(selected>limit){memset(p,0,sizeof *p);return GB175_UNSUPPORTED;}
 p->bytes=selected?selected:limit;p->banks=(p->bytes+8191)/8192;return 0;
}
uint32_t gb177_content_bytes(const void *data,uint32_t bytes){
 if(!data||(bytes!=2048&&bytes!=8192&&bytes!=32768&&bytes!=65536&&bytes!=131072))return 0;
 const uint8_t *d=data;
 while(bytes>32&&!memcmp(d,d+bytes/2,bytes/2))bytes/=2;
 return bytes==2048||bytes==8192||bytes==32768||bytes==65536||bytes==131072?bytes:0;
}
int gb178_plan(unsigned bus,const uint8_t h[80],uint32_t selected,struct gb175_plan *p){
 if(!p)return GB175_ARGUMENT;
 memset(p,0,sizeof *p);
 if(bus!=1||!h)return GB175_UNSUPPORTED;
 if(!valid_header(h))return GB175_IDENTITY;
 if(h[0x47]!=5&&h[0x47]!=6)return gb177_plan(bus,h,selected,p);
 /* MBC2 has internal 512 x 4-bit RAM; RAM header code is not its size. */
 if(selected&&selected!=512)return GB175_UNSUPPORTED;
 p->mapper=GB178_MBC2;p->bytes=512;p->banks=1;return 0;
}
int gb175_chunk(const struct gb175_plan *p,uint32_t off,uint32_t max,
 unsigned *bank,uint16_t *addr,uint32_t *len){
 if(!valid_plan(p)||!bank||!addr||!len||!max||off>=p->bytes)return GB175_ARGUMENT;
 uint32_t n=8192-(off&8191);if(n>p->bytes-off)n=p->bytes-off;if(n>max)n=max;
 *bank=off/8192;*addr=(uint16_t)(0xa000+(off&8191));*len=n;return 0;
}
static int live(const struct gb175_bus *b){return b->alive(b->ctx);}
static int stop(const struct gb175_bus *b){return !live(b)||b->cancelled(b->ctx);}
static int capture_plan(const struct gb175_bus *b,
 unsigned bus,const uint8_t expected[80],void *snap,size_t cap,void *scratch,
 uint32_t chunk,struct gb175_report *r,int extended,uint32_t selected){
 if(!r)return GB175_ARGUMENT;
 memset(r,0,sizeof *r);r->rc=GB175_ARGUMENT;
 struct gb175_plan p;
 int rc=extended==3?gb178_plan(bus,expected,selected,&p):extended==2?gb177_plan(bus,expected,selected,&p):extended?gb176_plan(bus,expected,selected,&p):gb175_plan_header(bus,expected,&p);if(rc)return r->rc=rc;
 if(!b||!b->acquire||!b->alive||!b->cancelled||!b->read_header||
 !b->control||!b->read_ram||!b->release||!snap||!scratch||cap<p.bytes||!chunk||chunk>8192)return r->rc;
 uintptr_t x=(uintptr_t)snap,y=(uintptr_t)scratch;
 if(x<=y?y-x<p.bytes:x-y<chunk)return r->rc;
 uint8_t identity[80];memcpy(identity,expected,80);
 if((rc=b->acquire(b->ctx))) {r->detail=rc;return r->rc=GB175_IO;}
 r->rc=0;int touched=0;
 /* MBC1 mode 1 changes the low ROM window when upper bank bits are set.
  * Normalize before reading identity, and before the second identity read. */
 if(p.mapper==GB177_MBC1){
  touched=1;
  if(stop(b))r->rc=GB175_CANCELLED;
  else if((rc=b->control(b->ctx,0x4000,0))){r->rc=GB175_IO;r->detail=rc;}
  if(!r->rc&&(rc=b->control(b->ctx,0x6000,0))){r->rc=GB175_IO;r->detail=rc;}
 }
 for(unsigned pass=0;pass<2&&!r->rc;pass++){
  uint8_t current[80];
  if(stop(b)){r->rc=GB175_CANCELLED;break;}
  if((rc=b->read_header(b->ctx,current))){r->rc=GB175_IO;r->detail=rc;break;}
  if(memcmp(current,identity,80)){r->rc=GB175_IDENTITY;break;}
  if(p.mapper!=GB177_PLAIN){
   touched=1;
   if(p.mapper==GB177_MBC1&&(rc=b->control(b->ctx,0x6000,1))){r->rc=GB175_IO;r->detail=rc;break;}
   if((rc=b->control(b->ctx,0,0x0a))){r->rc=GB175_IO;r->detail=rc;break;}
  }
  unsigned selected=~0u;
  for(uint32_t off=0;off<p.bytes;){
   unsigned bank;uint16_t addr;uint32_t n;
   if(stop(b)){r->rc=GB175_CANCELLED;break;}
   rc=gb175_chunk(&p,off,chunk,&bank,&addr,&n);
   if(rc){r->rc=rc;break;}
   if(p.mapper!=GB177_PLAIN&&p.mapper!=GB178_MBC2&&bank!=selected){
    rc=b->control(b->ctx,0x4000,(uint8_t)bank);
    if(rc){r->rc=GB175_IO;r->detail=rc;break;}selected=bank;
   }
   if(stop(b)){r->rc=GB175_CANCELLED;break;}
   rc=b->read_ram(b->ctx,addr,pass?scratch:(uint8_t *)snap+off,n);
   if(rc){r->rc=GB175_IO;r->detail=rc;break;}
   /* FlashGBX SAV format: one byte per cell, upper nibble zero. Ignore
    * electrically undefined upper bits before comparing the two reads. */
   if(p.mapper==GB178_MBC2){
    uint8_t *d=pass?scratch:(uint8_t *)snap+off;
    for(uint32_t i=0;i<n;i++)d[i]&=0x0f;
   }
   if(pass){if(memcmp((uint8_t *)snap+off,scratch,n)){r->rc=GB175_CHANGED;break;}r->compared+=n;}
   off+=n;
  }
  if(p.mapper==GB177_MBC1&&!r->rc){
   if(stop(b))r->rc=GB175_CANCELLED;
   else if((rc=b->control(b->ctx,0x4000,0))){r->rc=GB175_IO;r->detail=rc;}
   if(!r->rc&&(rc=b->control(b->ctx,0x6000,0))){r->rc=GB175_IO;r->detail=rc;}
  }
 }
 /* Restore controls best-effort only while the same physical session lives.
  * Cleanup bypasses user cancellation, never removal or epoch change. */
 if(touched&&live(b)){
  if(p.mapper!=GB178_MBC2){rc=b->control(b->ctx,0x4000,0);if(rc)r->cleanup_rc=rc;}
  if(live(b)){rc=b->control(b->ctx,0,0);if(rc&&!r->cleanup_rc)r->cleanup_rc=rc;}
  if(p.mapper==GB177_MBC1&&live(b)){rc=b->control(b->ctx,0x6000,0);if(rc&&!r->cleanup_rc)r->cleanup_rc=rc;}
 }
 if(!r->rc){
  uint8_t current[80];
  if(stop(b))r->rc=GB175_CANCELLED;
  else if((rc=b->read_header(b->ctx,current))){r->rc=GB175_IO;r->detail=rc;}
  else if(memcmp(current,identity,80))r->rc=GB175_IDENTITY;
 }
 rc=b->release(b->ctx);if(rc&&!r->cleanup_rc)r->cleanup_rc=rc;
 if(!r->rc&&r->cleanup_rc)r->rc=GB175_CLEANUP;
 return r->rc;
}
int gb175_capture(const struct gb175_bus *b,unsigned bus,const uint8_t h[80],
 void *snap,size_t cap,void *scratch,uint32_t chunk,struct gb175_report *r){
 return capture_plan(b,bus,h,snap,cap,scratch,chunk,r,0,0);
}
int gb176_capture(const struct gb175_bus *b,unsigned bus,const uint8_t h[80],uint32_t selected,
 void *snap,size_t cap,void *scratch,uint32_t chunk,struct gb175_report *r){
 return capture_plan(b,bus,h,snap,cap,scratch,chunk,r,1,selected);
}
int gb177_capture(const struct gb175_bus *b,unsigned bus,const uint8_t h[80],uint32_t selected,
 void *snap,size_t cap,void *scratch,uint32_t chunk,struct gb175_report *r){
 return capture_plan(b,bus,h,snap,cap,scratch,chunk,r,2,selected);
}
int gb178_capture(const struct gb175_bus *b,unsigned bus,const uint8_t h[80],uint32_t selected,
 void *snap,size_t cap,void *scratch,uint32_t chunk,struct gb175_report *r){
 return capture_plan(b,bus,h,snap,cap,scratch,chunk,r,3,selected);
}
int gb175_backup(const struct gb175_bus *b,const struct gb175_output *o,
 unsigned bus,const uint8_t expected[80],void *snap,size_t cap,void *scratch,
 uint32_t chunk,struct gb175_report *r){
 if(!r)return GB175_ARGUMENT;
 memset(r,0,sizeof *r);
 if(!o||!o->create_new||!o->write||!o->finish||!o->read||!o->abort)return r->rc=GB175_ARGUMENT;
 int rc=gb175_capture(b,bus,expected,snap,cap,scratch,chunk,r);
 if(rc)return rc;
 uint32_t bytes=r->compared;
 if(b->cancelled(b->ctx))return r->rc=GB175_CANCELLED;
 rc=o->create_new(o->ctx,bytes);if(rc){r->detail=rc;return r->rc=GB175_FILE;}
 for(uint32_t off=0;off<bytes;){
  uint32_t n=bytes-off;if(n>chunk)n=chunk;
  if(b->cancelled(b->ctx)){r->rc=GB175_CANCELLED;goto abort;}
  rc=o->write(o->ctx,off,(uint8_t *)snap+off,n);if(rc)goto file_error;
  off+=n;r->written=off;
 }
 rc=o->finish(o->ctx);if(rc)goto file_error;
 for(uint32_t off=0;off<bytes;){
  uint32_t n=bytes-off;if(n>chunk)n=chunk;
  if(b->cancelled(b->ctx)){r->rc=GB175_CANCELLED;goto abort;}
  rc=o->read(o->ctx,off,scratch,n);if(rc)goto file_error;
  if(memcmp((uint8_t *)snap+off,scratch,n)){rc=GB175_CHANGED;goto file_error;}
  off+=n;r->verified=off;
 }
 return 0;
file_error:r->rc=GB175_FILE;r->detail=rc;
abort:o->abort(o->ctx);return r->rc;
}
