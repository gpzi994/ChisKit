#include "model.h"
#include <string.h>
#include <stdio.h>
#include "../gba_db55/sha1_55.h"
const uint8_t cb_gb_logo[48]={
 0xce,0xed,0x66,0x66,0xcc,0x0d,0,0x0b,3,0x73,0,0x83,0,0x0c,0,0x0d,
 0,8,0x11,0x1f,0x88,0x89,0,0x0e,0xdc,0xcc,0x6e,0xe6,0xdd,0xdd,0xd9,0x99,
 0xbb,0xbb,0x67,0x63,0x6e,0x0e,0xec,0xcc,0xdd,0xdc,0x99,0x9f,0xbb,0xb9,0x33,0x3e};
static void title_copy(char *d,const uint8_t *s,unsigned n){
 unsigned i; for(i=0;i<n&&s[i];++i)d[i]=(s[i]>=32&&s[i]<127)?(char)s[i]:'?'; d[i]=0;
 while(i&&d[i-1]==' ')d[--i]=0;
 if(!i)d[0]=0;
}
const char *cb_platform_name(enum cb_platform p){return p==CB_GB?"GB/GBC":p==CB_GBA?"GBA":"Unknown";}
const char *cb_result_name(enum cb_result r){
 static const char *s[]={"Header verified","No valid header","Read unstable","Header invalid","Read failed","Select Play Cartridge"};
 return (unsigned)r<6?s[r]:"Unknown";
}
int cb_parse_gb(struct cb_cart *c,const uint8_t *h){
 uint8_t sum=0; unsigned i;
 for(i=0x34;i<=0x4c;i++)sum=(uint8_t)(sum-h[i]-1);
 if(memcmp(h+4,cb_gb_logo,48)||sum!=h[0x4d])return 0;
 c->platform=CB_GB; c->result=CB_READY;c->type=h[0x47];c->cgb=h[0x43];
 c->revision=h[0x4c];c->checksum=sum;c->global_checksum=(uint16_t)((h[0x4e]<<8)|h[0x4f]);
 title_copy(c->title,h+0x34,(c->cgb&0x80)?15:16);
 const char *m="Other / unsupported";
 if(c->type==0||c->type==8||c->type==9)m="ROM only";
 else if(c->type>=1&&c->type<=3)m="MBC1";
 else if(c->type==5||c->type==6)m="MBC2";
 else if(c->type>=0x0f&&c->type<=0x13)m="MBC3";
 else if(c->type>=0x19&&c->type<=0x1e)m="MBC5";
 snprintf(c->mapper,sizeof(c->mapper),"%s",m);
 c->rom_bytes=h[0x48]<=8?(32768u<<h[0x48]):0;
 if(h[0x48]>=0x52&&h[0x48]<=0x54){static const unsigned banks[]={72,80,96};c->rom_bytes=banks[h[0x48]-0x52]*16384u;}
 static const uint32_t ram[]={0,2048,8192,32768,131072,65536};
 c->ram_bytes=h[0x49]<6?ram[h[0x49]]:0;
 if(c->rom_bytes)snprintf(c->rom,sizeof(c->rom),"%lu KiB (header)",(unsigned long)(c->rom_bytes/1024));
 else strcpy(c->rom,"Unknown size code");
 if(c->type==5||c->type==6)strcpy(c->save,"MBC2 512 x 4 bit");
 else if(h[0x49]>=6)strcpy(c->save,"Unknown size code");
 else if(c->ram_bytes)snprintf(c->save,sizeof(c->save),"RAM %lu KiB (header)",(unsigned long)(c->ram_bytes/1024));
 else strcpy(c->save,"None (header)");
 strcpy(c->rtc,(c->type==0x0f||c->type==0x10)?"Declared; read unsupported":"Not declared; not probed");
 return 1;
}
int cb_parse_gba(struct cb_cart *c,const uint8_t *h){
 static const uint8_t expected[]={0x17,0xda,0xa0,0xfe,0xc0,0x2f,0xc3,0x3c,0x0f,0x6a,0xbb,0x54,0x9a,0x8b,0x80,0xb6,0x61,0x3b,0x48,0xee};
 uint8_t digest[20],sum=0;unsigned i;
 sha1_55(h+4,156,digest);
 for(i=0xa0;i<=0xbc;i++)sum=(uint8_t)(sum+h[i]);
 sum=(uint8_t)(0-sum-0x19);
 if(memcmp(digest,expected,20)||h[0xb2]!=0x96||sum!=h[0xbd])return 0;
 c->platform=CB_GBA;c->result=CB_READY;c->revision=h[0xbc];c->checksum=sum;
 title_copy(c->title,h+0xa0,12);
 char game[5],maker[3];title_copy(game,h+0xac,4);title_copy(maker,h+0xb0,2);
 snprintf(c->code,sizeof(c->code),"%.4s / %.2s",game,maker);
 strcpy(c->rom,"Needs full analysis");strcpy(c->save,"Needs full analysis");strcpy(c->rtc,"Read not supported yet");
 return 1;
}
static int header_read(const struct cb_probe_ops *o,unsigned bus,uint8_t *h){
 unsigned length=bus==1?96:192,base=bus==1?0x100:0;uint8_t dummy[32];
 int rc=o->read(o->ctx,bus,base,dummy,32);if(rc)return rc;
 for(unsigned i=0;i<length;i+=32){rc=o->read(o->ctx,bus,base+i,h+i,32);if(rc)return rc;}
 return 0;
}
void cb_probe(struct cb_cart *c,const struct cb_probe_ops *o){
 uint8_t a[192],b[192];int saw_io=0,saw_unstable=0,saw_invalid=0;
 memset(c,0,sizeof(*c));c->result=CB_EMPTY;
 c->safe_rc=o->safe(o->ctx);if(c->safe_rc){c->result=CB_IO;return;}
 for(unsigned bus=1;bus<=2;bus++){
  if(o->progress)o->progress(o->ctx,bus==1?"Reading GB/GBC header":"Reading GBA header");
  for(unsigned attempt=0;attempt<2;attempt++){
   c->rc=header_read(o,bus,a);if(!c->rc)c->rc=header_read(o,bus,b);
   if(c->rc)saw_io=1;
   else if(memcmp(a,b,bus==1?96:192))saw_unstable=1;
   else{
    if(bus==1?cb_parse_gb(c,a):cb_parse_gba(c,a))goto done;
    saw_invalid=1;
    /* A recognizable but damaged GB header must not be driven as GBA. */
    if(bus==1){
     unsigned same=0;for(unsigned i=0;i<48;i++)same+=a[i+4]==cb_gb_logo[i];
     if(same>=24){c->result=CB_BAD_HEADER;goto done;}
    }
    break;
   }
   /* One complete retry contains a transient router or contact failure. */
   if(attempt==0){c->safe_rc=o->safe(o->ctx);if(c->safe_rc){c->result=CB_IO;goto done;}}
  }
  if(bus==1){c->safe_rc=o->safe(o->ctx);if(c->safe_rc){c->result=CB_IO;goto done;}}
 }
 c->platform=CB_UNKNOWN;
 c->result=saw_invalid?CB_EMPTY:saw_unstable?CB_UNSTABLE:saw_io?CB_IO:CB_EMPTY;
done:
 c->safe_rc=o->safe(o->ctx);
 if(c->safe_rc){c->platform=CB_UNKNOWN;c->result=CB_IO;}
}
