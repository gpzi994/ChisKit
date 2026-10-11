#include "gb_fallback181.h"
#include "../../core/gb_save175/gb_save175.h"
#include "../gba_db55/sha1_55.h" /* Stateless SHA1 utility only, no GBA save logic. */
#include <string.h>
#include "gb_db181_data.inc"
int gb181_database_match(const uint8_t header[384]){
 uint8_t hash[20];sha1_55(header,384,hash);
 unsigned lo=0,hi=sizeof gb181_hashes/sizeof gb181_hashes[0];
 while(lo<hi){unsigned mid=lo+(hi-lo)/2;int c=memcmp(hash,gb181_hashes[mid],20);
  if(!c)return 1;
  if(c<0)hi=mid;else lo=mid+1;
 }return 0;
}
int gb181_database_header(const uint8_t expected[80],const struct gb181_ops *o){
 uint8_t header[384],block[32];int rc=o->safe(o->ctx);
 for(unsigned pass=0;pass<2&&!rc;pass++){
  /* General ROM dump requires a fresh sequential session for each pass. */
  if(pass)rc=o->safe(o->ctx);
  for(unsigned off=0;off<384&&!rc;off+=32){
  rc=o->read(o->ctx,off,block,32);
  if(!rc){if(!pass)memcpy(header+off,block,32);else if(memcmp(header+off,block,32))rc=GB175_IO;}
 }
 }
 int clean=o->safe(o->ctx);if(!rc)rc=clean;
 if(rc)return rc;
 if(memcmp(header+0x100,expected,80))return GB175_IDENTITY;
 return gb181_database_match(header);
}
uint32_t gb181_capacity(const uint8_t h[80],uint32_t content,int probe_rc,
                       int analysis_rc,int writable,int db_match,unsigned *source){
 struct gb175_plan plan;*source=0;
 if(probe_rc||gb179_plan(1,h,0,&plan))return 0;
 if(content){*source=1;return content;}
 /* Dedicated MBC2/MBC7 protocols must never turn into ordinary SRAM. */
 if(plan.mapper==GB178_MBC2||plan.mapper==GB179_MBC7||analysis_rc)return 0;
 if(writable){*source=2;return 131072;}
 if(db_match!=1)return 0;
 static const uint32_t bytes[]={0,2048,8192,32768,131072,65536};
 unsigned r=h[0x49];uint32_t n=r<6?bytes[r]:0;
 if(!n||n==2048||gb179_plan(1,h,n,&plan))return 0;
 *source=3;return n;
}
