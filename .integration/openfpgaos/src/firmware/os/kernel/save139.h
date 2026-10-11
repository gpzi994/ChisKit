/* Kernel-only adapter. No raw write syscall is added. Link probe139.c and
 * flash139.c with this feature. The caller must not pass application pointers. */
#include <save/probe139.h>
static int save139_alive(void *ctx){(void)ctx;return alive(NULL);}
static int save139_read(void *ctx,uint32_t address,uint8_t *value){
 (void)ctx;
 if(!value||address>=65536)return CB_NOR_ARGUMENT;
 if(!alive(NULL))return CB_NOR_CANCELLED;
 if(CART_STATUS&CART_ST_BUSY)return CB_NOR_IO;
 CART_BUS=4;CART_ADDRESS=address;CART_COMMAND=2;fence();
 int rc=wait_idle(20,true);
 if(rc){release(NULL);return rc;}
 if(!alive(NULL)||CART_RX_LEVEL!=1)return CB_NOR_IO;
 *value=(uint8_t)CART_RX_DATA;return 0;
}
static int save139_write(void *ctx,uint32_t address,uint8_t value){
 (void)ctx;
 if(address!=0&&address!=4&&address!=0x5555&&address!=0x2aaa)return CB_NOR_ARGUMENT;
 if(!alive(NULL))return CB_NOR_CANCELLED;
 CART_BUS=6;fence();uint16_t word=value;
 return transaction(address,&word,true);
}
static int save139_delay(void *ctx,unsigned ms){
 (void)ctx;if(ms>20)return CB_NOR_ARGUMENT;
 uint32_t start=now_ms(NULL);
 while((uint32_t)(now_ms(NULL)-start)<ms)if(!alive(NULL))return CB_NOR_CANCELLED;
 return alive(NULL)?0:CB_NOR_CANCELLED;
}
int save139_probe_kernel(struct cb_save139_flash_report *out){
 if(!out)return CB_NOR_ARGUMENT;
 memset(out,0,sizeof(*out));
 if(busy){out->rc=CB_NOR_IO;return out->rc;}
 busy=true;job.authorized=false;job_token=0;mapper_kind=0;
 int rc=enter(2);
 // ABI 1 with all required probe bits. New optional capabilities (EEPROM)
 // must not invalidate the already-supported parallel probe path.
 if(!rc&&((CART_CAPS>>16)!=1u||(CART_CAPS&0x3fu)!=0x3fu))rc=CB_NOR_UNSUPPORTED;
 if(!rc){
  const struct cb_save139_flash_ops ops={{NULL,save139_read,save139_write,save139_alive},save139_delay};
  *out=cb_save139_probe_flash(&ops);
  rc=out->cleanup_rc?out->cleanup_rc:out->rc;
 }else out->rc=rc;
 int end_rc=private_owned?leave():0;
 if(end_rc&&!out->cleanup_rc)out->cleanup_rc=end_rc;
 busy=false;return rc?rc:end_rc;
}
