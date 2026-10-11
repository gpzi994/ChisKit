/* Preserve the accepted UI97 and GBA parsers. */
#ifdef CB_GB_SAVE175
#include "../../core/gb_save175/gb_save175.c"
#ifdef CB_GB_SAVE179
#include "../../core/gb_save175/mbc7179.c"
#endif
#endif
#define cb_parse_gb cb_parse_gb_base98
#define cb_probe cb_probe_base98
#include "../ui97/model.c"
#undef cb_parse_gb
#undef cb_probe
#include "header_diag.h"
#ifndef CB_ROM164
#include "size107.h"
#endif
struct cb_header_diag cb_header_diag;
struct cb_header150_trace cb_header150_trace;
struct cb_size151_trace cb_size151_trace;
#ifndef CB_ROM164
static int size151_read(void *v,unsigned bus,uint32_t addr,void *p,unsigned n){
 const struct cb_probe_ops *o=v;
 int rc=o->read(o->ctx,bus,addr,p,n);
 cb_size151_trace.last[0]=cb_size151_trace.last[1];
 struct cb_header150_read *r=&cb_size151_trace.last[1];
 memset(r,0,sizeof *r);r->bus=bus;r->address=addr;r->length=n;r->rc=rc;
 if(!rc&&n<=32)memcpy(r->bytes,p,n);
 cb_size151_trace.count++;return rc;
}
#endif
static int header150_active;
#ifdef CB_CAMERA136
#include "camera136.h"
struct cb_camera136_report cb_camera136_report;
#endif
static int diagnostic_read(void *v,unsigned bus,uint32_t addr,void *p,unsigned n){
 const struct cb_probe_ops *o=v;
 int rc=o->read(o->ctx,bus,addr,p,n);
 if(header150_active&&cb_header150_trace.count<64){
  struct cb_header150_read *r=&cb_header150_trace.reads[cb_header150_trace.count++];
  r->bus=bus;r->address=addr;r->length=n;r->rc=rc;
  if(!rc&&n<=32)memcpy(r->bytes,p,n);
 }
 if(bus==1){
  cb_header_diag.rc=rc;
  if(addr==0x100)cb_header_diag.mask=0;
  if(!rc&&n==32&&addr>=0x100&&addr<=0x140&&!(addr&31)){
   memcpy(cb_header_diag.bytes+addr-0x100,p,32);
   cb_header_diag.mask|=1u<<((addr-0x100)/32);
  }
 }
 return rc;
}
static int diagnostic_safe(void *v){const struct cb_probe_ops *o=v;return o->safe(o->ctx);}
static void diagnostic_progress(void *v,const char *s){const struct cb_probe_ops *o=v;if(o->progress)o->progress(o->ctx,s);}
static void classify_gb_header(struct cb_cart *c){
 /* FlashGBX 2d9682c RomFileDMG: raw type 0x10 + RAM code 0x05.
  * This labels the ROM requirement, not verified hardware capability.
  * Keep type as the raw byte, rather than FlashGBX's internal 0x110. */
 if(c->platform==CB_GB && c->result==CB_READY &&
    c->type==0x10 && c->ram_bytes==65536u)strcpy(c->mapper,"MBC30");
 if(c->platform==CB_GB && c->result==CB_READY){
  if(c->type==0x22){strcpy(c->mapper,"MBC7");strcpy(c->save,"EEPROM; not probed");}
  if(c->type==0xfc)strcpy(c->mapper,"Camera (MBC-GBD)");
 }
}
int cb_parse_gb(struct cb_cart *c,const uint8_t *h){
 int ok=cb_parse_gb_base98(c,h);if(ok)classify_gb_header(c);return ok;
}
void cb_probe(struct cb_cart *c,const struct cb_probe_ops *o){
 memset(&cb_header_diag,0,sizeof(cb_header_diag));
 memset(&cb_size151_trace,0,sizeof cb_size151_trace);
 memset(&cb_header150_trace,0,sizeof cb_header150_trace);header150_active=1;
 struct cb_probe_ops wrapped={(void *)o,diagnostic_read,diagnostic_safe,diagnostic_progress};
 cb_probe_base98(c,&wrapped);classify_gb_header(c);
 header150_active=0;
 c->header_valid=c->result==CB_READY;
#ifndef CB_ROM164
 if(c->platform==CB_GBA&&c->result==CB_READY){
  if(o->progress)o->progress(o->ctx,"Estimating GBA ROM capacity");
  struct cb_probe_ops size_ops=*o;size_ops.ctx=(void *)o;size_ops.read=size151_read;
  c->rc=cb_gba_size107(&size_ops,&c->rom_bytes);c->safe_rc=o->safe(o->ctx);
  if(c->rc||c->safe_rc){c->result=CB_IO;c->rom_bytes=0;strcpy(c->rom,"Size probe failed");}
  else snprintf(c->rom,sizeof(c->rom),"%lu KiB (%s)",(unsigned long)(c->rom_bytes>>10),c->rom_bytes==0x2000000?"window":"mirror");
 }
#endif
}
