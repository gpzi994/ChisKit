/* SRAM138 transport. Included by nor108.c; uses its private ownership guard.
 * Bit 31 is an explicit request for Unlicensed 1M SRAM bank protocol, NOT an
 * inferred ROM/NOR property. No save-data write command exists in this API. */
#define SRAM138_BANKED 0x80000000u
static int sram138_bank(unsigned bank){
 if(bank>1||!alive(NULL))return CB_NOR_CANCELLED;
 CART_BUS=5;fence();
 uint16_t value=(uint16_t)bank;
 int rc=transaction(0x01000000u,&value,true);
 if(rc)return rc;
 /* FlashGBX's conservative bank-latch settle; not per-byte. */
 uint32_t start=now_ms(NULL);
 while((uint32_t)(now_ms(NULL)-start)<50u)
  if(!alive(NULL))return CB_NOR_CANCELLED;
 CART_BUS=4;fence();return 0;
}
int sram138_read(uint32_t address,void *dst,uint32_t length,uint32_t timeout_us){
 bool banked=(address&SRAM138_BANKED)!=0;
 uint32_t off=address&~SRAM138_BANKED,limit=banked?131072u:65536u;
 uintptr_t p=(uintptr_t)dst;
 if(!dst||!length||length>32768u||off>=limit||length>limit-off||
    timeout_us<1000u||timeout_us>10000000u||p<0x10000000u||
    p>=0x14000000u||length>0x14000000u-p)return CB_NOR_ARGUMENT;
 if(busy)return CB_NOR_IO;
 busy=true;job.authorized=false;job_token=0;mapper_kind=0;
 int rc=enter(2),cleanup=0;bool touched=false;
 if(!rc&&(CART_CAPS&0x00010018u)!=0x00010018u)rc=CB_NOR_UNSUPPORTED;
 unsigned bank=UINT32_MAX;
 uint32_t start=now_ms(NULL);
 for(uint32_t i=0;i<length&&!rc;i++){
  if((uint32_t)(now_ms(NULL)-start)>=(timeout_us+999u)/1000u){rc=CB_NOR_TIMEOUT;break;}
  if(!alive(NULL)){rc=CB_NOR_CANCELLED;break;}
  if(banked&&bank!=((off+i)>>16)){
   touched=true;bank=(off+i)>>16;rc=sram138_bank(bank);if(rc)break;
  }else if(!i){CART_BUS=4;fence();}
  CART_ADDRESS=(off+i)&0xffffu;CART_COMMAND=2;fence();
  rc=wait_idle(20,true);
  if(!rc&&(!alive(NULL)||CART_RX_LEVEL!=1))rc=CB_NOR_IO;
  if(!rc)((uint8_t*)dst)[i]=(uint8_t)CART_RX_DATA;
 }
 /* Never send a recovery write to a different lifecycle/cart. If the latch
  * command failed, still attempt bank0 on the same live owner. */
 if(touched){
  if(alive(NULL))cleanup=sram138_bank(0);
  else cleanup=CB_NOR_CANCELLED;
 }
 int leave_rc=private_owned?leave():0;busy=false;
 return rc?rc:cleanup?cleanup:leave_rc;
}
/* Separate opt-in preserves the legacy 32 KiB SRAM contract. Reject combined
 * flags before any ownership or register access; this path never calls bank(). */
int flash165_read(uint32_t address,void *dst,uint32_t length,uint32_t timeout_us){
 if((address&0xffff0000u)!=0x40000000u)return CB_NOR_ARGUMENT;
 return sram138_read(address&0xffffu,dst,length,timeout_us);
}
