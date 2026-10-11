/* GB SRAM / MBC2 only. Separate service and hardware aperture. */
#include <gb_save175/restore180.h>
#if CART_GB_RESTORE185
#include <gb_save175/mbc7_restore185.h>
#endif
static int gb180_write(void *ctx,uint16_t a,const void *data,uint32_t n){
 (void)ctx;
 if(!gb180_active||gb175_active_plan.mapper==GB179_MBC7||a<0xa000||a>=0xc000||!n||n>0xc000u-a)return CB_NOR_ARGUMENT;
 if(gb175_active_plan.mapper==GB178_MBC2&&(a>=0xa200||n>0xa200u-a))return CB_NOR_ARGUMENT;
 for(uint32_t i=0;i<n;i++){
  uint16_t v=((const uint8_t*)data)[i];
  if(gb175_active_plan.mapper==GB178_MBC2)v&=15;
  int rc=transaction(a+i,&v,true);if(rc)return rc;
 }
 return 0;
}
static int gb180_overlap(uintptr_t a,uint32_t an,uintptr_t b,uint32_t bn){
 return a<=b?b-a<an:a-b<bn;
}
int gb180_restore_service(const uint8_t h[80],const void *original,const void *source,
 uint32_t bytes,void *out,uint32_t size){
 if(size!=sizeof(struct gb175_report)||!gb175_sdram(h,80)||!gb175_sdram(original,bytes)||
    !gb175_sdram(source,bytes)||!gb175_sdram(out,size))return CB_NOR_ARGUMENT;
 if(gb180_overlap((uintptr_t)original,bytes,(uintptr_t)source,bytes)||
    gb180_overlap((uintptr_t)original,bytes,(uintptr_t)out,size)||
    gb180_overlap((uintptr_t)source,bytes,(uintptr_t)out,size))return CB_NOR_ARGUMENT;
 uint8_t expected[80];memcpy(expected,h,80);
 struct gb175_report r={0};struct gb175_plan plan;
 int rc=gb_restore_plan(expected,bytes,&plan);
 if(rc){r.rc=rc;memcpy(out,&r,size);return rc;}
 if(busy){r.rc=GB175_IO;memcpy(out,&r,size);return r.rc;}
 busy=true;job.authorized=false;job_token=0;gb175_active_plan=plan;gb177_active=gb180_active=1;
 const struct gb180_bus ops={{NULL,gb175_acquire,gb175_live,gb175_cancel,
  gb175_header,gb175_control,gb175_ram,gb175_release},gb180_write};
#if CART_GB_RESTORE185
 if(plan.mapper==GB179_MBC7){
  const struct gb185_bus ee={ops.read,now_ms};
  rc=gb185_restore(&ee,expected,original,source,bytes,&r);
 }else
#endif
 rc=gb180_restore(&ops,expected,original,source,bytes,gb175_scratch,sizeof gb175_scratch,&r);
 gb180_active=gb177_active=0;busy=false;memset(&gb175_active_plan,0,sizeof gb175_active_plan);
 memcpy(out,&r,size);return rc;
}
