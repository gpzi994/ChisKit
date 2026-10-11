#include <rtc/write172.h>
static int rtc172_sdram(const void *p,uint32_t n){
 uintptr_t a=(uintptr_t)p;return p&&a>=0x10000000u&&a<0x14000000u&&n<=0x14000000u-a;
}
static int rtc172_identity(void *ctx){
 const uint8_t *id=ctx;
 for(unsigned i=0;i<16;i+=2){
  uint16_t v=0;if(!alive(NULL))return CB_NOR_CANCELLED;
  CART_BUS=2;fence();int rc=transaction(0xa0u+i,&v,false);if(rc)return rc;
  if((uint8_t)v!=id[i]||(uint8_t)(v>>8)!=id[i+1])return CB_NOR_IO;
 }
 return 0;
}
int rtc172_write_service(const void *target,const uint8_t expected_id[16],void *out,uint32_t size){
 if(size!=sizeof(of_rtc172_report)||!rtc172_sdram(target,8)||
    !rtc172_sdram(expected_id,16)||!rtc172_sdram(out,size))return CB_NOR_ARGUMENT;
 /* Copy inputs before writing output, including when application ranges overlap. */
 uint8_t data[8],id[16];memcpy(data,target,8);memcpy(id,expected_id,16);
 of_rtc172_report r={0};r.version=172;
 if(busy){r.rc=CB_NOR_IO;memcpy(out,&r,size);return r.rc;}
 busy=true;job.authorized=false;job_token=0;mapper_kind=0;
 int rc=enter(2);
 if(!rc&&((CART_CAPS>>16)!=1u||(CART_CAPS&7u)!=7u))rc=CB_NOR_UNSUPPORTED;
 if(!rc)rc=rtc172_identity(id);
 if(!rc){
  const struct cb_rtc167_ops ops={NULL,rtc167_bus_read,rtc167_bus_write,rtc167_live};
  rc=cb_rtc172_write(&ops,data,rtc172_identity,id,&r);
 }
 int clean=private_owned?leave():0;busy=false;
 if(clean&&!r.cleanup_rc)r.cleanup_rc=clean;
 r.rc=rc?rc:clean;if(r.rc||r.cleanup_rc)r.verified=0;
 memcpy(out,&r,size);return r.rc;
}
