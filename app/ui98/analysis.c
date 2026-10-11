// Independent UI adapter; validated cartridge engines are not modified.
#include "analysis.h"
#include <string.h>
#include <stdio.h>
static uint32_t le16(const uint8_t *p){return p[0]|((uint32_t)p[1]<<8);}
int cb_gb_geometry(struct cb_analysis *a,const uint8_t *s,uint32_t profile){
 static const uint8_t id[]={0xc2,0x7e,0x10,0};
 /* MX29LV640E PM1328 rev1.7, tables 4-3/4-4. LV uses a two-byte
  * silicon ID; offsets 0x1c/0x1e are NOT its extended device ID.
  * C2/CB is shared with compatible parts: geometry gates remain mandatory.
  * User board marking is MX29LV640EBT. No board/write profile is inferred. */
 int lv=s[0]==0xc2&&s[1]==0xcb;
 snprintf(a->id,sizeof(a->id),"%02X %02X %02X %02X",s[0],s[1],s[2],s[3]);
 if(!lv&&memcmp(s,id,4)){strcpy(a->model,"Unknown / unsupported");return OF_ERR_NOT_SUPPORTED;}
 strcpy(a->model,lv?"MX29LV640EB compatible":"MX29GL640EB");
 snprintf(a->pri,sizeof(a->pri),"PRI %c.%c / boot %02X",s[23],s[24],s[27]);
 /* Mori: PRI signature + boot indicator, not a hard-coded PRI 1.1 table.
  * Keep strict ID, repeated geometry, complete size and return-to-ROM gates.
  * This matches a read-only profile; it NEVER authorizes GB programming. */
 if(memcmp(s+4,s+12,8)||memcmp(s+20,"PRI",3)||s[23]!='1'||
    (lv?s[24]!='1':(s[24]!='1'&&s[24]!='3'))||s[27]!=2||
    s[28]!=23||s[29]!=(lv?0:5)||s[30]!=0||s[31]!=2||
    (profile&0x13u)!=0x13u)return OF_ERR_DENIED;
 uint64_t total=0;
 for(unsigned i=0;i<2;i++){
  struct cb_region *r=&a->region[i];
  r->start=(uint32_t)total;r->count=le16(s+4+i*4)+1;
  r->bytes=le16(s+6+i*4)*256u;
  total+=(uint64_t)r->count*r->bytes;
 }
 if(total!=0x800000u||a->region[0].count!=8||a->region[0].bytes!=8192||
    a->region[1].count!=127||a->region[1].bytes!=65536)return OF_ERR_DENIED;
 a->bytes=(uint32_t)total;a->buffer=lv?0:32;a->region_count=2;a->matched=1;
 strcpy(a->profile,lv?"LV640EB / read only":"MX29GL640EB / read only");return 0;
}
int cb_gba_geometry(struct cb_analysis *a,const of_cart_flash_caps_t *f){
 snprintf(a->id,sizeof(a->id),"%08lX %08lX",(unsigned long)f->id_prefix,(unsigned long)f->id_tail);
 if(f->struct_size!=sizeof(*f)||f->version!=OF_CART_FLASH_CAPS_VERSION||
    f->id_prefix!=0x227e0001u||f->cfi_signature!=0x00520051u||
    f->command_set!=OF_CART_FLASH_CMDSET_AMD||f->profile_status!=1||
    f->flash_size_bytes!=0x2000000u||f->sector_size_bytes!=0x20000u||
    f->program_buffer_bytes!=512)return OF_ERR_DENIED;
 strcpy(a->model,"S29GL256S");strcpy(a->profile,"S29GL256S / buffer512");
 a->bytes=f->flash_size_bytes;a->buffer=f->program_buffer_bytes;
 a->region_count=1;a->region[0]=(struct cb_region){0,256,0x20000};
 a->matched=1;a->writable=1;strcpy(a->pri,"Uniform sectors");return 0;
}
static int transient(int rc){
 return rc==OF_ERR_IO||rc==OF_ERR_BUSY||rc==OF_ERR_TIMEOUT;
}
static int analyze_gb_once(struct cb_analysis *a,const struct cb_cart *c,
                           const struct cb_analysis_ops *o){
 static const uint32_t addr[11]={0,32,0x4000,0x4020,0,32,0x4000,0x4020,0x100,0x120,0x140};
 uint8_t sample[352];struct cb_cart check={0};
#ifndef CB_GB_IDENTIFY134
 uint8_t s[32];of_cart_caps_t caps={0};
#endif
 int rc;
 for(unsigned i=0;i<11;i++){
  snprintf(a->stage,sizeof(a->stage),"Sample %u / 11",i+1);
  rc=o->read(o->ctx,1,4,0x10000u+addr[i],sample+32*i,32);
  if(rc)return rc;
 }
 strcpy(a->stage,"Repeat samples");
 if(memcmp(sample,sample+128,128))return OF_ERR_DENIED;
 strcpy(a->stage,"Header recheck");
 if(!cb_parse_gb(&check,sample+256)||
    check.type!=c->type||check.checksum!=c->checksum||strcmp(check.title,c->title)||
    check.rom_bytes!=c->rom_bytes||check.ram_bytes!=c->ram_bytes||
    check.cgb!=c->cgb||check.revision!=c->revision)return OF_ERR_DENIED;
 int nonzero=0,nonff=0;for(unsigned i=0;i<128;i++){nonzero|=sample[i];nonff|=sample[i]^255;}
 strcpy(a->stage,"Nonblank samples");
 if(!nonzero||!nonff)return OF_ERR_DENIED;
 a->bank0_alias=!memcmp(sample,sample+64,64);
 strcpy(a->probe,a->bank0_alias?"Bank0 samples equal":"Bank0 samples differ");
 strcpy(a->stage,"Mapper agreement");
 int mbc3=c->type>=0x0f&&c->type<=0x13,mbc5=c->type>=0x19&&c->type<=0x1e;
 a->mapper_ok=(mbc3&&!a->bank0_alias&&c->rom_bytes<=0x200000u)||
              (mbc5&&a->bank0_alias);
 /* A ROM header describes the software requirement, not the flashcart's
  * physical mapper. Keep this heuristic as evidence, never as NOR authority.
  * Only the existing bounded identify engine is used; GB writes stay off. */
 a->mapper_warning=!a->mapper_ok;
#ifdef CB_GB_IDENTIFY134
 /* Mapper evidence remains in engine2; NOR authority belongs to OS. */
 strcpy(a->stage,"Samples ready / dynamic NOR next");
 strcpy(a->status,"GB header OK / NOR pending");
 return 0;
#else
 strcpy(a->stage,"NOR query");
 rc=o->read(o->ctx,1,4,0,s,32);a->nor_rc=rc;
 if(rc==OF_ERR_NOT_SUPPORTED){
  strcpy(a->model,"No supported NOR detected");
  strcpy(a->status,"Header OK / NOR unconfirmed");return 0;
 }
 if(rc)return rc;
 strcpy(a->stage,"NOR capabilities");
 rc=o->caps(o->ctx,&caps);if(rc)return rc;
 strcpy(a->stage,"NOR geometry");
 rc=cb_gb_geometry(a,s,caps.cart_profile_raw);a->nor_rc=rc;
 if(rc==OF_ERR_NOT_SUPPORTED){
  strcpy(a->status,"Header OK / NOR unconfirmed");return 0;
 }
 return rc;
#endif
}
void cb_analyze(struct cb_analysis *a,const struct cb_cart *c,const struct cb_analysis_ops *o){
 memset(a,0,sizeof(*a));a->ran=1;a->rc=OF_ERR_DENIED;
 strcpy(a->model,"Unknown / unsupported");strcpy(a->profile,"No verified match");
 strcpy(a->status,"Analysis blocked");
 strcpy(a->stage,"Preconditions");strcpy(a->probe,"Not sampled");
 if(c->result!=CB_READY||!o||!o->read||!o->safe||!o->caps||!o->flash)return;
 a->header_ok=1;
 strcpy(a->stage,"Initial safe idle");
 a->rc=o->safe(o->ctx);if(a->rc)goto done;
 if(c->platform==CB_GBA){
#ifdef CB_GBA_IDENTIFY133
  /* This variant delegates NOR probing to the OS dynamic service only.
   * Keep both safe-idle checks; header validity is not write authority. */
  strcpy(a->model,"NOR not yet probed");
  strcpy(a->profile,"Dynamic NOR service");
  strcpy(a->stage,"Header ready / dynamic NOR next");
  strcpy(a->status,"GBA header OK / NOR pending");
#else
  of_cart_flash_caps_t f={0};a->rc=o->flash(o->ctx,&f);
  if(a->rc==OF_ERR_NOT_SUPPORTED){
   a->rc=0;strcpy(a->model,"No supported writable NOR");
   strcpy(a->profile,"GBA ROM / backup only");
   strcpy(a->status,"GBA readable / writing disabled");
  }else if(!a->rc)a->rc=cb_gba_geometry(a,&f);
#endif
 }else if(c->platform==CB_GB){
  /* Retail/unsupported mappers still have valid information. Do not send
   * an MBC3/5 flash sequence to arbitrary mapper or save/RTC registers. */
  if(!((c->type>=0x0f&&c->type<=0x13)||(c->type>=0x19&&c->type<=0x1e))){
   a->rc=0;strcpy(a->stage,"Header only");
   strcpy(a->model,"Not probed for this mapper");
   strcpy(a->status,"Header OK / NOR probe unavailable");goto done;
  }
  for(unsigned attempt=0;attempt<3;attempt++){
   if(attempt){a->rc=o->safe(o->ctx);if(a->rc)break;}
   a->mapper_ok=a->bank0_alias=a->matched=a->writable=0;
   a->mapper_warning=a->nor_rc=0;
   a->rc=analyze_gb_once(a,c,o);
   if(!transient(a->rc))break;
  }
 }
done:
 {int rc=o->safe(o->ctx);if(rc){a->rc=rc;strcpy(a->stage,"Final safe idle");}}
 if(a->rc){a->matched=0;a->writable=0;}
 else if(a->matched)strcpy(a->status,a->writable?"NOR matched / GBA transfer ready":"NOR matched / GB write not validated");
 if(!a->rc&&a->mapper_warning)
  strcpy(a->status,a->matched?"NOR matched / probe inconclusive":"Header OK / NOR unconfirmed");
}
