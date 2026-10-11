// NOR108 adapter. Legacy read-only analysis remains available without new OS.
#include "../../core/nor/write_policy112.h"
static void analyze_nor108(struct cb_analysis*a,const struct cb_cart*c){
#ifdef CB_GB_IDENTIFY134
 if(c->platform==CB_GB){
  a->legacy_programmer=0;
  if(a->rc)return; /* Never overwrite sample/repeat/header/safe failure. */
  if(!((c->type>=0x0f&&c->type<=0x13)||(c->type>=0x19&&c->type<=0x1e)))return;
  if(!of_nor_available()){
   a->writable=0;a->matched=0;a->rc=OF_ERR_NOT_SUPPORTED;
   strcpy(a->status,"Dynamic NOR service unavailable");return;
  }
 }
#endif
#ifdef CB_GBA_IDENTIFY133
 if(c->platform==CB_GBA){
  a->legacy_programmer=0;
  if(a->rc)return; /* Do not override a failed safe-idle check. */
  if(!of_nor_available()){
   a->writable=0;a->matched=0;a->rc=OF_ERR_NOT_SUPPORTED;
   strcpy(a->status,"Dynamic NOR service unavailable");return;
  }
 }
#endif
 a->legacy_programmer=c->platform==CB_GBA&&a->matched&&a->writable;
 if(!of_nor_available())return;
 of_nor_info_t n={0};int rc=of_nor_probe(c->platform==CB_GBA?2:1,&n);
 a->nor=n;a->writable=0;a->nor_rc=rc?rc:n.plan_rc;
 if(rc){
#ifdef CB_GBA_IDENTIFY133
  if(c->platform==CB_GBA&&rc==OF_ERR_NOT_SUPPORTED){
   a->matched=0;strcpy(a->model,"No supported NOR detected");
   strcpy(a->profile,"GBA ROM / backup only");
   strcpy(a->status,"GBA readable / writing disabled");
  }
#endif
  // Retail/unsupported cards retain normal header and dump support.
  if(rc!=-2){a->rc=rc;snprintf(a->status,sizeof(a->status),"NOR probe failed rc %d",rc);}
  return;
 }
 if(n.version!=OF_NOR_VERSION||n.struct_size!=sizeof(n)||n.region_count>8){a->rc=-1;return;}
 a->rc=0;a->matched=(n.flags&OF_NOR_ID_OK)!=0;
 snprintf(a->model,sizeof(a->model),"%.27s",n.model);
 snprintf(a->id,sizeof(a->id),"%02X%02X %02X%02X %02X%02X %02X%02X",n.id[0],n.id[1],n.id[2],n.id[3],n.id[4],n.id[5],n.id[6],n.id[7]);
 snprintf(a->profile,sizeof(a->profile),"Mori method %lu / D01 %lu",(unsigned long)n.method,(unsigned long)n.swapped);
 a->bytes=n.flash_bytes;a->buffer=n.buffer_bytes;a->region_count=n.region_count;
 for(unsigned i=0;i<n.region_count;i++)a->region[i]=(struct cb_region){n.regions[i].start,n.regions[i].count,n.regions[i].sector_bytes};
 snprintf(a->pri,sizeof(a->pri),"%lu regions / %s",(unsigned long)n.region_count,n.flags&OF_NOR_CFI_OK?"CFI":"profile");
 a->writable=(n.flags&OF_NOR_WRITE_OK)&&!n.plan_rc&&cb_nor_write_capacity112(n.bus,n.flash_bytes);
 if(c->platform==CB_GBA&&n.flash_bytes>CB_ROM_CACHE112_BYTES){
  a->legacy_programmer=0;
  snprintf(a->status,sizeof(a->status),"NOR >32 MiB / write excluded");return;
 }
 snprintf(a->status,sizeof(a->status),"%s",a->writable?"NOR matched / test write available":"NOR matched / geometry blocked");
}
