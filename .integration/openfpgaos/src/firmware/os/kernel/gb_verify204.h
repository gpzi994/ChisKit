/* GB/GBC final ROM verification only. Included by nor108.c after its transport.
 * Keep the private engine, physical pulse widths, mapper and cleanup unchanged.
 * Consecutive completed reads need no redundant pre-command status transaction.
 * The short polling path avoids a 64-bit clock conversion for every GB byte;
 * a busy/stalled engine still uses the original timed wait and hardware watchdog.
 */
static int gb_verify204_read(uint32_t address,uint8_t *out,uint32_t length){
 if(bus_kind!=1)return CB_NOR_ARGUMENT;
 if(!alive(NULL))return CB_NOR_CANCELLED;
 if(CART_STATUS&CART_ST_BUSY)return CB_NOR_IO;
 while(length){
  uint32_t span=0x4000u-(address&0x3fffu);
  if(span>length)span=length;
  int rc=select_bus(NULL,address);
  if(rc){mapper_kind=0;return rc;}
  uint32_t pa=address<0x4000u?address:0x4000u+(address&0x3fffu);
  for(uint32_t n=0;n<span;n++){
   if(!alive(NULL))return CB_NOR_CANCELLED;
   CART_ADDRESS=pa+n;CART_COMMAND=2;fence();
   uint32_t status=0;
   for(unsigned poll=0;poll<32;poll++){
    status=CART_STATUS;
    if(!(status&CART_ST_BUSY))break;
   }
   if(status&CART_ST_BUSY)rc=wait_idle(20,true);
   else rc=((status&(CART_ST_ERROR|CART_ST_TIMEOUT|CART_ST_ABORTED))||
            !(status&CART_ST_DONE))?CB_NOR_IO:0;
   if(rc){release(NULL);return rc;}
   if(CART_RX_LEVEL!=1)return CB_NOR_IO;
   out[n]=(uint8_t)CART_RX_DATA;
  }
  address+=span;out+=span;length-=span;
 }
 return alive(NULL)?0:CB_NOR_CANCELLED;
}
