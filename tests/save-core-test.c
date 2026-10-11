#include "../core/save/save.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned char snapshot[CB_SAVE_MAX_BYTES],scratch[32768],output[CB_SAVE_MAX_BYTES];
struct fake { unsigned n,reads,created,restored,aborted,calls,fail,corrupt,change,cancel; };
static int fault(struct fake *f){return ++f->calls==f->fail?-99:0;}
static int prepare(void *v,enum cb_save_kind k,uint32_t n){struct fake *f=v;(void)k;f->n=n;return fault(f);}
static int read_cart(void *v,uint32_t off,void *dst,uint32_t n){
 struct fake *f=v;assert(off<=f->n&&n<=f->n-off);++f->reads;
 for(unsigned i=0;i<n;++i)((unsigned char *)dst)[i]=(unsigned char)((off+i)*31u+(off+i)/257u);
 if(f->change&&f->reads>f->change)((unsigned char *)dst)[0]^=1;
 return fault(f);
}
static int restore(void *v){struct fake *f=v;++f->restored;return fault(f);}
static int create(void *v,uint32_t n){struct fake *f=v;assert(n==f->n);assert(f->restored==1);++f->created;return fault(f);}
static int write_out(void *v,uint32_t off,const void *src,uint32_t n){struct fake *f=v;memcpy(output+off,src,n);return fault(f);}
static int finish(void *v){return fault(v);}
static int read_out(void *v,uint32_t off,void *dst,uint32_t n){struct fake *f=v;memcpy(dst,output+off,n);if(f->corrupt)((unsigned char *)dst)[0]^=1;return fault(f);}
static void abort_out(void *v){++((struct fake *)v)->aborted;}
static int cancel(void *v){return ((struct fake *)v)->cancel;}
static struct cb_save_ops ops(struct fake *f){return (struct cb_save_ops){f,prepare,read_cart,restore,create,write_out,finish,read_out,abort_out,cancel};}
static int run(struct fake *f,enum cb_save_kind k,unsigned n,unsigned chunk,struct cb_save_report *r){struct cb_save_ops o=ops(f);return cb_save_backup(&o,k,n,snapshot,sizeof(snapshot),scratch,chunk,r);}
static void signatures(void){
 const char *tokens[]={"SRAM_V110","SRAM_F_V103","FLASH_V123","FLASH512_V130","FLASH1M_V103","EEPROM_V124"};
 enum cb_save_kind kinds[]={CB_SAVE_SRAM,CB_SAVE_SRAM,CB_SAVE_FLASH64,CB_SAVE_FLASH64,CB_SAVE_FLASH128,CB_SAVE_EEPROM};
 for(unsigned i=0;i<6;++i)for(size_t cut=0;cut<=strlen(tokens[i]);++cut){
  struct cb_save_scan s;cb_save_scan_init(&s);
  cb_save_scan_feed(&s,tokens[i],cut);cb_save_scan_feed(&s,tokens[i]+cut,strlen(tokens[i])-cut);
  struct cb_save_hint h=cb_save_scan_result(&s);assert(h.kind==kinds[i]);if(i==5)assert(h.bytes==0);
 }
 struct cb_save_scan s;cb_save_scan_init(&s);
 const char *mixed="FLASH_V123..FLASH1M_V103";
 for(size_t i=0;i<strlen(mixed);++i)cb_save_scan_feed(&s,mixed+i,1);
 assert(cb_save_scan_result(&s).kind==CB_SAVE_CONFLICT);
 cb_save_scan_init(&s);cb_save_scan_feed(&s,"EEPROM_Vxyz",11);assert(cb_save_scan_result(&s).kind==CB_SAVE_UNKNOWN);
 cb_save_scan_init(&s);cb_save_scan_feed(&s,"FLASH_V123 FLASH512_V130",23);assert(cb_save_scan_result(&s).kind==CB_SAVE_FLASH64);
}
int main(void){
 signatures();struct cb_save_report r;
 const unsigned sizes[]={512,8192,32768,65536,131072};
 const enum cb_save_kind kinds[]={CB_SAVE_EEPROM,CB_SAVE_EEPROM,CB_SAVE_SRAM,CB_SAVE_FLASH64,CB_SAVE_FLASH128};
 const unsigned chunks[]={1,31,512,32768};
 for(unsigned i=0;i<5;++i)for(unsigned c=0;c<4;++c){struct fake f={0};assert(!run(&f,kinds[i],sizes[i],chunks[c],&r));assert(r.written==sizes[i]&&r.compared==sizes[i]);assert(f.restored==1&&f.created==1&&!f.aborted);assert(!memcmp(snapshot,output,sizes[i]));}
 struct fake baseline={0};assert(!run(&baseline,CB_SAVE_EEPROM,512,31,&r));
 for(unsigned fail=1;fail<=baseline.calls;++fail){struct fake f={.fail=fail};assert(run(&f,CB_SAVE_EEPROM,512,31,&r)<0);assert(f.restored==1);assert(r.detail==-99||r.restore_rc==-99);}
 struct fake changed={.change=1};assert(run(&changed,CB_SAVE_SRAM,32768,32768,&r)==CB_SAVE_CHANGED);assert(!changed.created);
 struct fake corrupt={.corrupt=1};assert(run(&corrupt,CB_SAVE_FLASH128,131072,32768,&r)==CB_SAVE_VERIFY);assert(corrupt.aborted==1);
 struct fake cancelled={.cancel=1};assert(run(&cancelled,CB_SAVE_SRAM,32768,512,&r)==CB_SAVE_CANCELLED);assert(cancelled.restored==1&&!cancelled.created);
 struct fake invalid={0};assert(run(&invalid,CB_SAVE_EEPROM,1024,512,&r)==CB_SAVE_ARGUMENT);assert(!invalid.calls);
 struct cb_save_ops o=ops(&invalid);assert(cb_save_backup(&o,CB_SAVE_SRAM,32768,snapshot,sizeof(snapshot),snapshot+1,32,&r)==CB_SAVE_ARGUMENT);
 puts("save-core PASS: streaming signatures, five sizes, chunk edges, every callback fault, mutation, cancellation, output corruption");
}
