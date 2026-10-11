/* OS-private Flash128 read adapter. Never use the SRAM138 ROM bank latch.
 * Validate the live device with the unchanged save probe before bank commands.
 * Fixed commands only: no program/erase/data-write service is exposed. */
static int flash166_bank(unsigned bank){
 if(bank>1)return CB_NOR_ARGUMENT;
 int rc=save139_write(NULL,0x5555,0xaa);
 if(!rc)rc=save139_write(NULL,0x2aaa,0x55);
 if(!rc)rc=save139_write(NULL,0x5555,0xb0);
 if(!rc)rc=save139_write(NULL,0,(uint8_t)bank);
 if(!rc)rc=save139_delay(NULL,10);
 return rc;
}
int flash166_read(uint32_t address,void *dst,uint32_t length,uint32_t timeout_us){
 uintptr_t p=(uintptr_t)dst;uint32_t off=address&0x1ffffu;
 if((address&0xfffe0000u)!=0x20000000u||!dst||!length||length>32768u||
    length>131072u-off||timeout_us<1000u||timeout_us>10000000u||
    p<0x10000000u||p>=0x14000000u||length>0x14000000u-p)return CB_NOR_ARGUMENT;
 if(busy)return CB_NOR_IO;
 busy=true;job.authorized=false;job_token=0;mapper_kind=0;
 int rc=enter(2),clean=0;bool touched=false;
 uint32_t start=now_ms(NULL);
 if(!rc&&((CART_CAPS>>16)!=1u||(CART_CAPS&0x3fu)!=0x3fu))rc=CB_NOR_UNSUPPORTED;
 if(!rc){
  const struct cb_save139_flash_ops ops={{NULL,save139_read,save139_write,save139_alive},save139_delay};
  struct cb_save139_flash_report r=cb_save139_probe_flash(&ops);
  rc=r.cleanup_rc?r.cleanup_rc:r.rc;
  if(!rc&&(!r.exit_verified||!r.id_read||(r.id!=0xc209&&r.id!=0x6213)))rc=CB_NOR_UNSUPPORTED;
 }
 unsigned bank=UINT32_MAX;
 for(uint32_t i=0;i<length&&!rc;i++){
  if(!alive(NULL)){rc=CB_NOR_CANCELLED;break;}
  if((uint32_t)(now_ms(NULL)-start)>=(timeout_us+999u)/1000u){rc=CB_NOR_TIMEOUT;break;}
  if(bank!=((off+i)>>16)){
   touched=true;bank=(off+i)>>16;rc=flash166_bank(bank);if(rc)break;
  }
  rc=save139_read(NULL,(off+i)&0xffffu,(uint8_t*)dst+i);
 }
 /* Reset a partially issued command before restoring bank0. Never issue
  * recovery writes after lifecycle loss; failed cleanup is never PASS. */
 if(touched){
  if(!alive(NULL))clean=CB_NOR_CANCELLED;
  else{
   clean=save139_write(NULL,0,0xf0);
   if(!clean)clean=save139_delay(NULL,10);
   if(!clean)clean=flash166_bank(0);
  }
 }
 int end=private_owned?leave():0;busy=false;
 if(rc||clean||end)memset(dst,0,length);
 return rc?rc:clean?clean:end;
}
