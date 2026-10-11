/* Independent GB read-only RTC adapter. GBA RTC adapters are not called. */
#include <gb_save175/rtc186.h>
#ifdef CART_GB_RTC189
/* Both read and write must clock MBC3 commands through. Preserve bus8's
 * read-only data guard; never return a reader on the writable bus10. */
static int gb193_clocks(void){
 if(!private_owned||!gb175_live(NULL))return GB175_CANCELLED;
 unsigned bus=CART_BUS;if(bus!=8&&bus!=10)return CB_NOR_ARGUMENT;
 if(!(CART_CAPS&0x2000u))return CB_NOR_UNSUPPORTED;
 CART_BUS=12;CART_ARM=0x43424e31u;CART_ARM=0x43424e32u;CART_COMMAND=8;fence();
 int rc=wait_idle(20,true);CART_BUS=bus;fence();
 return rc?rc:gb175_live(NULL)?0:GB175_CANCELLED;
}
#endif
static int gb186_acquire(void *ctx){
 (void)ctx;mapper_kind=0;int rc=enter(1);
 if(!rc&&((CART_CAPS>>16)!=1u||(CART_CAPS&0x300u)!=0x300u))rc=CB_NOR_UNSUPPORTED;
 if(!rc){CART_BUS=8;fence();}
 if(rc&&private_owned)(void)leave();
 return rc;
}
static int gb186_control(void *ctx,uint16_t a,uint8_t v){
 (void)ctx;
 if(!((a==0&&(v==0||v==10))||(a==0x4000&&(v==0||(v>=8&&v<=12)))||(a==0x6000&&v<=1)))return CB_NOR_ARGUMENT;
 uint16_t x=v;
#ifdef CART_GB_RTC189
 int rc=gb193_clocks();if(!rc)rc=transaction(a,&x,true);
 return rc?rc:gb193_clocks(); /* Complete the final command before release. */
#else
 return transaction(a,&x,true);
#endif
}
static int gb186_window(void *ctx,uint16_t a,void *out,uint32_t n){
 (void)ctx;if(a!=0xa880||n!=256)return CB_NOR_ARGUMENT;
 for(unsigned i=0;i<n;i++){uint16_t x=0;int rc=0;
#ifdef CART_GB_RTC189
  rc=gb193_clocks();
#endif
  if(!rc)rc=transaction(a+i,&x,false);
  if(rc)return rc;
  ((uint8_t *)out)[i]=(uint8_t)x;
 }
 return 0;
}
int gb186_read_service(const uint8_t h[80],void *out,uint32_t size){
 if(size!=sizeof(struct gb186_report)||!gb175_sdram(h,80)||!gb175_sdram(out,size))return CB_NOR_ARGUMENT;
 uintptr_t a=(uintptr_t)h,z=(uintptr_t)out;if(a<=z?z-a<80:a-z<size)return CB_NOR_ARGUMENT;
 uint8_t expected[80];memcpy(expected,h,80);struct gb186_report r={0};
 int rc=gb186_header(expected);if(rc){r.rc=rc;memcpy(out,&r,size);return rc;}
 if(busy){r.rc=GB175_IO;memcpy(out,&r,size);return r.rc;}
 busy=true;job.authorized=false;job_token=0;
 const struct gb175_bus b={NULL,gb186_acquire,gb175_live,gb175_cancel,gb175_header,gb186_control,gb186_window,gb175_release};
 rc=gb186_read(&b,expected,&r);busy=false;memcpy(out,&r,size);return rc;
}
