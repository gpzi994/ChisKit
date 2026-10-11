#include "capacity164.h"
#include <string.h>
#include <stdio.h>
#include "../../core/nor/write_policy112.h"
/* Pinned complete FlashGBX index; compiled into application SDRAM. */
#include "../gba_db55/db55.c"
#include "../gba_db55/db55_data.c"

static int header164(const struct cb_probe_ops *o,uint8_t *h){
 uint8_t block[32];
 int rc=o->read(o->ctx,2,0,block,32);if(rc)return rc;
 for(unsigned pass=0;pass<2;pass++)for(unsigned off=0;off<DB55_HEADER_BYTES;off+=32){
  rc=o->read(o->ctx,2,off,block,32);if(rc)return rc;
  if(!pass)memcpy(h+off,block,32);
  else if(memcmp(h+off,block,32))return OF_ERR_IO;
 }
 return 0;
}
int cb_gba_analyze164(struct cb_cart *c,struct cb_analysis *a,const struct cb_capacity164_ops *o){
 uint8_t header[DB55_HEADER_BYTES],after[DB55_HEADER_BYTES];
 struct cb_cart parsed={0};struct db55_result db;
 int rc=0,sr,probe_rc;
 memset(a,0,sizeof *a);a->ran=1;
 strcpy(a->stage,"Header / NOR / capacity");strcpy(a->probe,"Repeated header read");
 c->rom_bytes=0;c->rom_source=0;memset(c->rom_header_sha1,0,20);
 strcpy(c->rom,"Capacity pending");
 if(!o||!o->rom.read||!o->rom.safe||!o->nor){rc=OF_ERR_INVALID_PARAM;goto failed;}
 rc=o->rom.safe(o->rom.ctx);if(rc)goto failed;
 rc=header164(&o->rom,header);if(rc)goto cleanup;
 if(!cb_parse_gba(&parsed,header)||strcmp(parsed.title,c->title)||
    strcmp(parsed.code,c->code)||parsed.revision!=c->revision||parsed.checksum!=c->checksum){
  rc=OF_ERR_IO;goto cleanup;
 }
 a->header_ok=1;
 if(o->rom.progress)o->rom.progress(o->rom.ctx,"Identifying GBA NOR configuration");
 probe_rc=o->nor(o->rom.ctx,&a->nor);a->nor_rc=probe_rc;
 sr=o->rom.safe(o->rom.ctx);if(sr){rc=sr;goto failed;}
 if(probe_rc&&probe_rc!=-2){rc=probe_rc;goto failed;}
 const of_nor_info_t *n=&a->nor;
 /* -2 is a completed no-match only when returned by a valid NOR service.
  * A missing service must never be confused with a retail cartridge. */
 if(n->version!=OF_NOR_VERSION||n->struct_size!=sizeof *n||n->bus!=2||
    n->probe_rc!=probe_rc||n->region_count>8){rc=OF_ERR_IO;goto failed;}
 rc=header164(&o->rom,after);if(rc)goto cleanup;
 if(memcmp(header,after,sizeof header)){rc=OF_ERR_IO;goto cleanup;}
 sr=o->rom.safe(o->rom.ctx);if(sr){rc=sr;goto failed;}
 sha1_55(header,sizeof header,c->rom_header_sha1);
 if(!probe_rc){
  if(!(n->flags&OF_NOR_ID_OK)){rc=OF_ERR_IO;goto failed;}
  a->matched=1;a->bytes=n->flash_bytes;a->buffer=n->buffer_bytes;
  snprintf(a->model,sizeof a->model,"%.27s",n->model);
  snprintf(a->profile,sizeof a->profile,"Mori %lu / %s",(unsigned long)n->method,n->swapped?"D01 swap":"normal");
  snprintf(a->id,sizeof a->id,"%02X%02X %02X%02X %02X%02X %02X%02X",n->id[0],n->id[1],n->id[2],n->id[3],n->id[4],n->id[5],n->id[6],n->id[7]);
  a->region_count=n->region_count;
  for(unsigned i=0;i<n->region_count;i++)a->region[i]=(struct cb_region){n->regions[i].start,n->regions[i].count,n->regions[i].sector_bytes};
  snprintf(a->pri,sizeof a->pri,"%c regions / %s",(int)('0'+n->region_count),n->flags&OF_NOR_CFI_OK?"CFI":"profile");
  /* Read limits are independent of the write whitelist. Banked >32 MiB
   * routes are not supported by this linear dump implementation. */
  if(!n->flash_bytes||!n->accessible_bytes||n->accessible_bytes>n->flash_bytes||
     n->accessible_bytes>0x2000000u||(n->accessible_bytes&65535u)){
   rc=OF_ERR_BAD_RANGE;goto failed;
  }
  c->rom_bytes=n->accessible_bytes;c->rom_source=1;
  a->nor_rc=n->plan_rc;
  a->writable=(n->flags&OF_NOR_WRITE_OK)&&!n->plan_rc&&cb_nor_write_capacity112(2,n->flash_bytes);
  snprintf(c->rom,sizeof c->rom,"%lu MiB (NOR)",(unsigned long)(c->rom_bytes>>20));
  strcpy(a->status,"NOR capacity selected for backup");
 }else{
  if(n->flags&OF_NOR_ID_OK){rc=OF_ERR_IO;goto failed;}
  db55_identify_header(header,&db);
  if(!db.logo_ok||!db.fixed96_ok||!db.checksum_ok){rc=OF_ERR_IO;goto failed;}
  if(db.entry){
   if(db.entry->rom_size>0x2000000u||(db.entry->flags&DB55_FLAG_3D)){rc=OF_ERR_NOT_SUPPORTED;goto failed;}
   c->rom_bytes=db.entry->rom_size;c->rom_source=2;
  }else{c->rom_bytes=0x2000000u;c->rom_source=3;}
  snprintf(c->rom,sizeof c->rom,"%lu MiB (%s)",(unsigned long)(c->rom_bytes>>20),c->rom_source==2?"database":"window");
  strcpy(a->model,"No supported NOR detected");
  strcpy(a->profile,"No NOR match / ROM backup");
  strcpy(a->status,c->rom_source==2?"Database ROM size selected":"DB miss / full 32 MiB window");
 }
 c->rc=0;c->safe_rc=0;a->rc=0;return 0;
cleanup:
 sr=o->rom.safe(o->rom.ctx);if(sr){c->safe_rc=sr;if(!rc)rc=sr;}
failed:
 c->rom_bytes=0;c->rom_source=0;c->rc=rc;c->result=CB_IO;
 a->rc=rc;a->writable=0;
 strcpy(c->rom,"Capacity probe failed");
 snprintf(a->status,sizeof a->status,"Capacity probe failed rc %d",rc);
 return rc;
}
