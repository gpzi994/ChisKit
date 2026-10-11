#include <rtc/read167.h>
static void rtc167_delay(void){
 uint64_t start=read_cycles();
 while(read_cycles()-start<CPU_FREQ_HZ/200000u){}
}
static int rtc167_bus_read(void *ctx,uint32_t a,uint16_t *v){
 (void)ctx;if(a!=0xc4&&a!=0xc6&&a!=0xc8)return CB_NOR_ARGUMENT;
 CART_BUS=2;fence();int rc=transaction(a,v,false);rtc167_delay();return rc;
}
static int rtc167_bus_write(void *ctx,uint32_t a,uint16_t v){
 (void)ctx;
 if(!((a==0xc4&&v<=7)||(a==0xc6&&(v==0||v==5||v==7))||(a==0xc8&&v<=1)))return CB_NOR_ARGUMENT;
 CART_BUS=2;fence();int rc=transaction(a,&v,true);rtc167_delay();return rc;
}
static int rtc167_live(void *ctx){return alive(ctx);}
int rtc167_read(uint32_t address,void *dst,uint32_t length,uint32_t timeout_us){
 uintptr_t p=(uintptr_t)dst;
 if(address||!dst||length!=sizeof(of_rtc167_report)||timeout_us!=1000000u||
    p<0x10000000u||p>=0x14000000u||length>0x14000000u-p)return CB_NOR_ARGUMENT;
 of_rtc167_report r={0};r.version=167;
 if(busy){r.rc=CB_NOR_IO;memcpy(dst,&r,sizeof r);return r.rc;}
 busy=true;job.authorized=false;job_token=0;mapper_kind=0;
 int rc=enter(2);
 if(!rc&&((CART_CAPS>>16)!=1u||(CART_CAPS&7u)!=7u))rc=CB_NOR_UNSUPPORTED;
 if(!rc){
  const struct cb_rtc167_ops ops={NULL,rtc167_bus_read,rtc167_bus_write,rtc167_live};
  rc=cb_rtc167_capture(&ops,&r);
 }
 int clean=private_owned?leave():0;busy=false;
 if(clean&&!r.cleanup_rc)r.cleanup_rc=clean;
 r.rc=rc?rc:clean;
 if(r.rc)r.flags&=~OF_RTC167_TIME_OK;
 memcpy(dst,&r,sizeof r);return r.rc;
}
