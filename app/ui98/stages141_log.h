#ifndef CB_STAGES141_LOG_H
#define CB_STAGES141_LOG_H
#include <stdarg.h>
/* The create-only API returns -4 for an existing path (also other denials).
 * Try bounded alternative names, never overwrite or retry IO/timeouts. */
static int cb_log141_open(int (*create)(const char *,uint32_t),char *path,size_t cap,
 unsigned long tick,unsigned *sequence,uint32_t size){
 for(unsigned tries=0;tries<16;tries++){
  int n=snprintf(path,cap,"/Saves/chiskit/common/st141-%08lx-%04x.txt",tick,++*sequence);
  if(n<0||(size_t)n>=cap)return -11;
  int rc=create(path,size);if(rc!=-4)return rc;
 }
 return -4;
}
/* Pure formatter: no cartridge, MMIO, PSRAM or filesystem operations. */
static int cb_log141_add(char *out,size_t cap,size_t *used,const char *fmt,...){
 if(*used>=cap)return -1;
 va_list ap;va_start(ap,fmt);int n=vsnprintf(out+*used,cap-*used,fmt,ap);va_end(ap);
 if(n<0||(size_t)n>=cap-*used)return -1;
 *used+=(size_t)n;return 0;
}
static int cb_log141_format(char *out,size_t cap,unsigned run,int executed,int rc,int safe_rc,
 const struct cb_gba_header140 *before,const struct cb_gba_header140 *after,
 const of_save141_diagnostic *d){
 size_t used=0;
#define LOG141(...) do{if(cb_log141_add(out,cap,&used,__VA_ARGS__))return -1;}while(0)
 LOG141("chiskit diag.146-wait\nmode=%u (0=Baseline 4=Save read 5=Session read 6=64K then read 7=Bank compare 8=Save probe wait) executed=%d op_rc=%d safe_rc=%d\n",run,executed,rc,safe_rc);
 LOG141("save: type=%ld rc=%ld bytes=%lu basis=%lu stage=%lu id=%04lX id_read=%lu\n",
 (long)d->result.type,(long)d->result.rc,(unsigned long)d->result.bytes,(unsigned long)d->result.basis,
 (unsigned long)d->stage,(unsigned long)d->id,(unsigned long)d->id_read);
 LOG141("probe_rc=%ld cleanup_rc=%ld direct_rc=%ld restore_rc=%ld leave_rc=%ld\n",
 (long)d->probe_rc,(long)d->cleanup_rc,(long)d->direct_rc,(long)d->restore_rc,(long)d->leave_rc);
 LOG141("byte@0004 original=%02lX target=%02lX observed=%02lX restored=%02lX\nexit_address=%08lX expected=%02lX actual=%02lX\n",
 (unsigned long)d->original,(unsigned long)d->target,(unsigned long)d->observed,(unsigned long)d->restored,
 (unsigned long)d->mismatch_address,(unsigned long)d->expected,(unsigned long)d->actual);
 LOG141("Raw bytes valid only within completed blocks. If executed=0, after is the retained pre-operation snapshot.\n");
 for(unsigned side=0;side<2;side++){
  const struct cb_gba_header140 *r=side?after:before;
  LOG141("%s safe_before=%d safe_after=%d\n",side?"AFTER":"BEFORE",r->before,r->after);
  for(unsigned p=0;p<4;p++){
   unsigned diff=0;for(unsigned i=0;i<192;i++)diff+=r->data[p][i]!=before->data[0][i];
   int comparable=r->rc[p]==0&&r->blocks[p]==6&&before->rc[0]==0&&before->blocks[0]==6;
   LOG141("page=%u rc=%d blocks=%u/6 diff_from_before0=%u comparable=%d\n",p+1,r->rc[p],r->blocks[p],diff,comparable);
   for(unsigned i=0;i<192;i++)LOG141("%02X%s",r->data[p][i],i%32==31?"\n":" ");
  }
 }
 LOG141("END diag.141-log\n");
#undef LOG141
 return (int)used;
}
#endif
