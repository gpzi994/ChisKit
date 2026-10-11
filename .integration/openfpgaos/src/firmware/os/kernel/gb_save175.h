/* Included only in nor108.c: private owner is shared, protocols are not. */
#include <gb_save175/gb_save175.h>
static struct gb175_plan gb175_active_plan;
static int gb177_active;
#if CART_GB_RESTORE180
static int gb180_active;
#endif
static unsigned gb179_serial_cs;
static uint8_t gb175_scratch[512]; /* Kernel SDRAM, not PSRAM. */
static int gb175_sdram(const void *p,uint32_t n){
 uintptr_t a=(uintptr_t)p;
 return p&&n&&a>=0x10000000u&&a<0x14000000u&&n<=0x14000000u-a;
}
static int gb175_acquire(void *ctx){
 (void)ctx;mapper_kind=0;
 int rc=enter(1);
 if(!rc&&((CART_CAPS>>16)!=1u||!(CART_CAPS&0x100u)))rc=CB_NOR_UNSUPPORTED;
 if(!rc&&gb177_active&&!(CART_CAPS&0x200u))rc=CB_NOR_UNSUPPORTED;
 if(!rc&&gb175_active_plan.mapper==GB179_MBC7&&!(CART_CAPS&0x400u))rc=CB_NOR_UNSUPPORTED;
 if(!rc){gb179_serial_cs=0;CART_BUS=gb175_active_plan.mapper==GB179_MBC7?9:8;fence();}
#if CART_GB_RESTORE180
 if(!rc&&gb180_active){
  if(gb175_active_plan.mapper==GB179_MBC7){
#if CART_GB_RESTORE185
   if(!(CART_CAPS&0x1000u))rc=CB_NOR_UNSUPPORTED;
   else {CART_BUS=11;fence();}
#else
   rc=CB_NOR_UNSUPPORTED;
#endif
  }else if(!(CART_CAPS&0x800u))rc=CB_NOR_UNSUPPORTED;
  else {CART_BUS=10;fence();}
 }
#endif
 if(rc&&private_owned)(void)leave();
 return rc;
}
static int gb175_live(void *ctx){(void)ctx;return alive(NULL);}
static int gb175_cancel(void *ctx){(void)ctx;return 0;}
static int gb175_header(void *ctx,uint8_t h[80]){
 (void)ctx;
 for(unsigned i=0;i<80;i++){
  uint16_t v=0;int rc=transaction(0x100u+i,&v,false);if(rc)return rc;h[i]=(uint8_t)v;
 }
 return 0;
}
static int gb175_control(void *ctx,uint16_t a,uint8_t v){
 (void)ctx;
 if(gb175_active_plan.mapper==GB179_MBC7){
  if(!((a==0&&(v==0||v==10))||(a==0x4000&&(v==0||v==64))||
       (a==0xa080&&(v==0||v==0x80||v==0x82||v==0xc0||v==0xc2))))return CB_NOR_ARGUMENT;
  uint16_t word=v;int rc=transaction(a,&word,true);if(rc)return rc;
  if(a==0xa080){
   unsigned rising=(v&0x80)&&!gb179_serial_cs;gb179_serial_cs=v&0x80;
   /* FlashGBX LK.c reinit waits 15 ms after CS rises. Preserve that
    * settling interval here without changing any GB RAM or GBA timing. */
   if(rising){
    uint32_t start=now_ms(NULL);unsigned bounded=0;
    while((uint32_t)(now_ms(NULL)-start)<15){
     if(!alive(NULL))return GB175_CANCELLED;
     if(++bounded>=1000000u)return GB175_IO;
    }
   }
  }
  return 0;
 }
 if(gb175_active_plan.mapper==GB177_PLAIN)return CB_NOR_ARGUMENT;
 if(!((a==0&&(v==0||v==10))||(gb175_active_plan.mapper!=GB178_MBC2&&a==0x4000&&v<gb175_active_plan.banks)||
 (gb177_active&&gb175_active_plan.mapper==GB177_MBC1&&a==0x6000&&v<=1)))return CB_NOR_ARGUMENT;
 uint16_t word=v;return transaction(a,&word,true);
}
static int gb175_ram(void *ctx,uint16_t a,void *out,uint32_t n){
 (void)ctx;
 if(gb175_active_plan.mapper==GB179_MBC7&&(a!=0xa080||n!=1))return CB_NOR_ARGUMENT;
 if(a<0xa000u||a>=0xc000u||!n||n>0xc000u-a)return CB_NOR_ARGUMENT;
 if(gb175_active_plan.mapper==GB178_MBC2&&(a>=0xa200u||n>0xa200u-a))return CB_NOR_ARGUMENT;
 for(uint32_t i=0;i<n;i++){
  uint16_t v=0;int rc=transaction(a+i,&v,false);if(rc)return rc;((uint8_t *)out)[i]=(uint8_t)v;
 }
 return 0;
}
static int gb175_release(void *ctx){(void)ctx;return private_owned?leave():0;}
static int gb_capture_service(const uint8_t h[80],void *snapshot,uint32_t capacity,void *out,uint32_t size,int extended,uint32_t selected){
 if(size!=sizeof(struct gb175_report)||!gb175_sdram(h,80)||
    !gb175_sdram(snapshot,capacity)||!gb175_sdram(out,size))return CB_NOR_ARGUMENT;
 uintptr_t a=(uintptr_t)snapshot,b=(uintptr_t)out;
 if(a<=b?b-a<capacity:a-b<size)return CB_NOR_ARGUMENT;
 uint8_t expected[80];memcpy(expected,h,80);
 struct gb175_report r={0};struct gb175_plan plan;
 int rc=extended==4?gb179_plan(1,expected,selected,&plan):extended==3?gb178_plan(1,expected,selected,&plan):extended==2?gb177_plan(1,expected,selected,&plan):extended?gb176_plan(1,expected,selected,&plan):gb175_plan_header(1,expected,&plan);
 if(!rc&&capacity!=plan.bytes)rc=GB175_ARGUMENT;
 if(rc){r.rc=rc;memcpy(out,&r,size);return rc;}
 if(busy){r.rc=GB175_IO;memcpy(out,&r,size);return r.rc;}
 busy=true;job.authorized=false;job_token=0;gb175_active_plan=plan;
 gb177_active=extended>=2;
 const struct gb175_bus ops={NULL,gb175_acquire,gb175_live,gb175_cancel,
  gb175_header,gb175_control,gb175_ram,gb175_release};
 rc=extended==4?gb179_capture(&ops,1,expected,selected,snapshot,capacity,gb175_scratch,sizeof gb175_scratch,&r):
  extended==3?gb178_capture(&ops,1,expected,selected,snapshot,capacity,gb175_scratch,sizeof gb175_scratch,&r):
  extended==2?gb177_capture(&ops,1,expected,selected,snapshot,capacity,gb175_scratch,sizeof gb175_scratch,&r):
  extended?gb176_capture(&ops,1,expected,selected,snapshot,capacity,gb175_scratch,sizeof gb175_scratch,&r):
  gb175_capture(&ops,1,expected,snapshot,capacity,gb175_scratch,sizeof gb175_scratch,&r);
 busy=false;gb177_active=0;memset(&gb175_active_plan,0,sizeof gb175_active_plan);
 memcpy(out,&r,size);return rc;
}
int gb175_capture_service(const uint8_t h[80],void *snapshot,uint32_t capacity,void *out,uint32_t size){
 return gb_capture_service(h,snapshot,capacity,out,size,0,0);
}
int gb176_capture_service(const uint8_t h[80],uint32_t selected,void *snapshot,uint32_t capacity,void *out,uint32_t size){
 return gb_capture_service(h,snapshot,capacity,out,size,1,selected);
}
int gb177_capture_service(const uint8_t h[80],uint32_t selected,void *snapshot,uint32_t capacity,void *out,uint32_t size){
 return gb_capture_service(h,snapshot,capacity,out,size,2,selected);
}
int gb178_capture_service(const uint8_t h[80],uint32_t selected,void *snapshot,uint32_t capacity,void *out,uint32_t size){
 return gb_capture_service(h,snapshot,capacity,out,size,3,selected);
}
int gb179_capture_service(const uint8_t h[80],uint32_t selected,void *snapshot,uint32_t capacity,void *out,uint32_t size){
 return gb_capture_service(h,snapshot,capacity,out,size,4,selected);
}
