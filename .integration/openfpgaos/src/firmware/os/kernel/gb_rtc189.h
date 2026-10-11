#include <gb_save175/rtc189.h>
static unsigned gb189_bank;
/* FlashGBX clocks the cartridge between MBC3 RTC accesses. Dedicated FPGA
 * command generates 50 full CLK pulses with RD/WR/CS inactive. Not GBA GPIO. */
static int gb191_clocks(void){
 return gb193_clocks();
}
static int gb189_acquire(void *ctx){
 int rc=gb186_acquire(ctx);gb189_bank=0;
 if(!rc&&(CART_CAPS&0x2800u)!=0x2800u)rc=CB_NOR_UNSUPPORTED;
 if(!rc){CART_BUS=10;fence();}
 if(rc&&private_owned)(void)leave();
 return rc;
}
static int gb189_control(void *ctx,uint16_t a,uint8_t v){
 if(!((a==0&&(v==0||v==10))||(a==0x4000&&(v==0||(v>=8&&v<=12)))||(a==0x6000&&v<=1)))return CB_NOR_ARGUMENT;
 int rc=gb186_control(ctx,a,v);if(!rc&&a==0x4000)gb189_bank=v;return rc;
}
static int gb189_data(void *ctx,uint8_t v){
 (void)ctx;if(!private_owned||gb189_bank<8||gb189_bank>12)return CB_NOR_ARGUMENT;
 int rc=gb191_clocks();uint16_t x=v;return rc?rc:transaction(0xa000,&x,true);
}
static int gb191_window(void *ctx,uint16_t a,void *out,uint32_t n){
 (void)ctx;if(a!=0xa880||n!=256)return CB_NOR_ARGUMENT;
 for(unsigned i=0;i<n;i++){
  int rc=gb191_clocks();uint16_t x=0;if(!rc)rc=transaction(a+i,&x,false);
  if(rc)return rc;
  ((uint8_t*)out)[i]=(uint8_t)x;
 }return 0;
}
int gb189_write_service(const uint8_t h[80],const uint8_t target[5],void *out,uint32_t size){
 if(size!=sizeof(struct gb189_report)||!gb175_sdram(h,80)||!gb175_sdram(target,5)||!gb175_sdram(out,size))return CB_NOR_ARGUMENT;
 uintptr_t a=(uintptr_t)h,b=(uintptr_t)target,z=(uintptr_t)out;
 if((a<=z?z-a<80:a-z<size)||(b<=z?z-b<5:b-z<size))return CB_NOR_ARGUMENT;
 uint8_t expected[80],raw[5];memcpy(expected,h,80);memcpy(raw,target,5);
 struct gb189_report r={0};if(busy){r.rc=GB175_IO;memcpy(out,&r,size);return r.rc;}
 busy=true;job.authorized=false;job_token=0;
 const struct gb189_bus ops={{NULL,gb189_acquire,gb175_live,gb175_cancel,gb175_header,gb189_control,gb191_window,gb175_release},gb189_data};
 int rc=gb189_write(&ops,expected,raw,&r);busy=false;gb189_bank=0;memcpy(out,&r,size);return rc;
}
