#include "save.h"
#include <string.h>

void cb_save_scan_init(struct cb_save_scan *s) { memset(s,0,sizeof(*s)); }
void cb_save_scan_feed(struct cb_save_scan *s,const void *data,size_t size) {
 static const struct { const char *text; unsigned bit; } sig[]={
  {"SRAM_V",1},{"SRAM_F_V",1},{"FLASH_V",2},{"FLASH512_V",2},
  {"FLASH1M_V",4},{"EEPROM_V",8}};
 const uint8_t *p=data;
 while(size--) {
  if(s->used==sizeof(s->tail)) {
   memmove(s->tail,s->tail+1,sizeof(s->tail)-1); --s->used;
  }
  s->tail[s->used++]=*p++;
  /* Require three numeric SDK version digits, including across chunks. */
  for(unsigned i=0;i<sizeof(sig)/sizeof(sig[0]);++i) {
   size_t n=strlen(sig[i].text);
   if(s->used<n+3)continue;
   const uint8_t *q=s->tail+s->used-n-3;
   if(!memcmp(q,sig[i].text,n)&&q[n]>='0'&&q[n]<='9'&&
      q[n+1]>='0'&&q[n+1]<='9'&&q[n+2]>='0'&&q[n+2]<='9')
    s->matches|=sig[i].bit;
  }
 }
}
struct cb_save_hint cb_save_scan_result(const struct cb_save_scan *s) {
 switch(s->matches) {
 case 0:return (struct cb_save_hint){CB_SAVE_UNKNOWN,0};
 case 1:return (struct cb_save_hint){CB_SAVE_SRAM,32768};
 case 2:return (struct cb_save_hint){CB_SAVE_FLASH64,65536};
 case 4:return (struct cb_save_hint){CB_SAVE_FLASH128,131072};
 case 8:return (struct cb_save_hint){CB_SAVE_EEPROM,0};
 default:return (struct cb_save_hint){CB_SAVE_CONFLICT,0};
 }
}
static int valid_size(enum cb_save_kind k,uint32_t n) {
 return (k==CB_SAVE_SRAM&&n==32768)||(k==CB_SAVE_FLASH64&&n==65536)||
 (k==CB_SAVE_FLASH128&&n==131072)||
 (k==CB_SAVE_EEPROM&&(n==512||n==8192));
}
int cb_save_backup(const struct cb_save_ops *o,enum cb_save_kind kind,uint32_t n,
 void *snapshot,size_t capacity,void *scratch,uint32_t chunk,struct cb_save_report *r) {
 uint8_t *a=snapshot; int opened=0,rc; uint32_t off,part;
 if(!r)return CB_SAVE_ARGUMENT;
 memset(r,0,sizeof(*r)); r->rc=CB_SAVE_ARGUMENT;
 if(!o||!o->prepare||!o->read||!o->restore||!o->create_new||!o->write||
    !o->finish||!o->read_output||!o->abort_output||!snapshot||!scratch||
    !chunk||chunk>32768||capacity<n||!valid_size(kind,n))return r->rc;
 /* Reject overlap without pointer-end overflow. */
 uintptr_t x=(uintptr_t)snapshot,y=(uintptr_t)scratch;
 if(x<=y ? y-x<n : x-y<chunk)return r->rc;
 r->rc=CB_SAVE_OK;
 rc=o->prepare(o->ctx,kind,n);
 if(rc){r->rc=CB_SAVE_UNCONFIRMED;r->detail=rc;goto restore;}
 for(unsigned pass=0;pass<2;++pass)for(off=0;off<n;off+=part) {
  part=n-off<chunk?n-off:chunk;
  if(o->cancelled&&o->cancelled(o->ctx)){r->rc=CB_SAVE_CANCELLED;goto restore;}
  rc=o->read(o->ctx,off,pass?scratch:a+off,part);
  if(rc){r->rc=CB_SAVE_READ;r->detail=rc;goto restore;}
  if(pass) {
   if(memcmp(a+off,scratch,part)){r->rc=CB_SAVE_CHANGED;goto restore;}
   r->compared+=part;
  }
 }
restore:
 r->restore_rc=o->restore(o->ctx);
 if(!r->rc&&r->restore_rc)r->rc=CB_SAVE_RESTORE;
 if(r->rc)return r->rc;
 if(o->cancelled&&o->cancelled(o->ctx))return r->rc=CB_SAVE_CANCELLED;
 rc=o->create_new(o->ctx,n);
 if(rc){r->detail=rc;return r->rc=CB_SAVE_OUTPUT;}
 opened=1;
 for(off=0;off<n;off+=part) {
  part=n-off<chunk?n-off:chunk;
  if(o->cancelled&&o->cancelled(o->ctx)){r->rc=CB_SAVE_CANCELLED;goto out;}
  rc=o->write(o->ctx,off,a+off,part);
  if(rc){r->rc=CB_SAVE_OUTPUT;r->detail=rc;goto out;}
  r->written+=part;
 }
 rc=o->finish(o->ctx);
 if(rc){r->rc=CB_SAVE_OUTPUT;r->detail=rc;goto out;}
 for(off=0;off<n;off+=part) {
  part=n-off<chunk?n-off:chunk;
  if(o->cancelled&&o->cancelled(o->ctx)){r->rc=CB_SAVE_CANCELLED;goto out;}
  rc=o->read_output(o->ctx,off,scratch,part);
  if(rc||memcmp(a+off,scratch,part)) {
   r->rc=CB_SAVE_VERIFY;r->detail=rc;goto out;
  }
 }
out:
 if(opened&&r->rc)o->abort_output(o->ctx);
 return r->rc;
}
