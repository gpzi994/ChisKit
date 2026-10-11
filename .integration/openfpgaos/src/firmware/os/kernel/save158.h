/* OS-private SRAM restore adapter. Never install a service entry until the
 * application has an independently verified persistent original SAV backup. */
#include <save/restore158.h>
struct save158_context {
 const uint8_t *source,*backup,*expected_id;
 uint32_t bytes;
 struct cb_save158_report *report;
};
static int save158_live(void *ctx){(void)ctx;return alive(NULL);}
static int save158_bank(void *ctx,unsigned bank){(void)ctx;return sram138_bank(bank);}
static int save158_read(void *ctx,uint32_t address,uint8_t *value){
 (void)ctx;return save139_read(NULL,address,value);
}
static int save158_write(void *ctx,uint32_t address,uint8_t value){
 (void)ctx;
 if(address>=65536u||!alive(NULL))return CB_NOR_ARGUMENT;
 CART_BUS=7;fence();uint16_t word=value;
 return transaction(address,&word,true);
}
static int save158_apply_owned(const struct cb_save139_result *detected,
 const uint8_t *header,const uint8_t *original,void *opaque){
 struct save158_context *c=opaque;
 if(!alive(NULL)||(CART_CAPS&0x00010080u)!=0x00010080u)
  return CB_NOR_UNSUPPORTED;
 const struct cb_save158_ops ops={NULL,save158_live,save158_bank,
  save158_read,save158_write};
 return cb_save158_restore(detected,header,c->expected_id,original,
  c->backup,c->source,c->bytes,&ops,c->report);
}
static int save158_sdram_range(const void *p,uint32_t n){
 uintptr_t a=(uintptr_t)p;
 return p&&a>=0x10000000u&&a<0x14000000u&&n<=0x14000000u-a;
}
static int save158_overlaps(const void *a,uint32_t an,const void *b,uint32_t bn){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 return x<y+bn&&y<x+an;
}
static int sram158_restore_impl(const void *source,const void *disk_backup,
 uint32_t bytes,const uint8_t expected_id[16],struct cb_save158_report *report,
 int selected){
 if(!report)return CB_NOR_ARGUMENT;
 memset(report,0,sizeof(*report));
 if((bytes!=32768u&&bytes!=131072u)||
    !save158_sdram_range(source,bytes)||
    !save158_sdram_range(disk_backup,bytes)||
    !expected_id)return report->rc=CB_NOR_ARGUMENT;
 if(save158_overlaps(source,bytes,disk_backup,bytes))
  return report->rc=CB_NOR_ARGUMENT;
 /* This synchronous service owns the caller's two disjoint SDRAM snapshots
  * until it returns. No app thread or file DMA may reuse them meanwhile. */
 struct save158_context c={source,disk_backup,expected_id,bytes,report};
 struct cb_save139_result detected;
#ifdef CB_SAVE171
 if(selected)detected=save_auto139_kernel_owned_impl(save158_apply_owned,&c,bytes);
 else
#else
 (void)selected;
#endif
 detected=save_auto139_kernel_owned(save158_apply_owned,&c);
 if(detected.rc){report->rc=detected.rc;return detected.rc;}
 return report->rc;
}
int sram158_restore_kernel(const void *source,const void *disk_backup,
 uint32_t bytes,const uint8_t expected_id[16],struct cb_save158_report *report){
 return sram158_restore_impl(source,disk_backup,bytes,expected_id,report,0);
}
static int sram160_restore_impl(const void *source,void *disk_backup,uint32_t bytes,
 const uint8_t expected_id[16],void *out,uint32_t out_size,int selected){
 struct cb_save158_report report={0};
 if(out_size!=sizeof report||!save158_sdram_range(out,out_size)||
    !save158_sdram_range(source,bytes)||
    !save158_sdram_range(disk_backup,bytes)||
    !save158_sdram_range(expected_id,16u)||
    (bytes!=32768u&&bytes!=131072u))return CB_NOR_ARGUMENT;
 if(save158_overlaps(source,bytes,disk_backup,bytes)||
    save158_overlaps(source,bytes,out,out_size)||
    save158_overlaps(disk_backup,bytes,out,out_size))return CB_NOR_ARGUMENT;
 /* CART_CAPS belongs to the currently selected engine. Check SAVE158 only
  * inside save158_apply_owned, after entering the private save engine. */
 /* No cartridge bus ownership while reading the host file. The file must
  * match the fresh OS-owned cartridge snapshot inside the later epoch. */
 if(of_file_size(5u)!=(long)bytes)return CB_NOR_ARGUMENT;
 uint8_t id[16];memcpy(id,expected_id,sizeof id);
 for(uint32_t off=0;off<bytes;off+=32768u){
  int rc=of_file_read(5u,off,(uint8_t *)disk_backup+off,32768u);
  if(rc){report.rc=rc;memcpy(out,&report,sizeof report);return rc;}
 }
 int rc=sram158_restore_impl(source,disk_backup,bytes,id,&report,selected);
 memcpy(out,&report,sizeof report);
 return rc;
}
int sram160_restore_service(const void *source,void *disk_backup,uint32_t bytes,
 const uint8_t expected_id[16],void *out,uint32_t out_size){
 return sram160_restore_impl(source,disk_backup,bytes,expected_id,out,out_size,0);
}
