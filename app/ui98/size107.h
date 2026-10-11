#ifndef CB_SIZE107_H
#define CB_SIZE107_H
/* GBA has no ROM-size header field. Read-only mirror estimate, not file length.
 * Like FlashGBX, probe powers of two from 64 KiB to the 32 MiB window.
 * Additional samples/repeats reject coincidental header matches and instability.
 */
static int cb_gba_size107(const struct cb_probe_ops *o,uint32_t *size){
 static const uint32_t points[]={0xa0,0,0x1000,0x3fe0};
 uint8_t base[4][32],a[32],b[32];int rc;
 /* A preceding SAFE resets the router/engine. Keep the accepted first-read
  * discard here so both Refresh and Backup callers use the same rule. */
 rc=o->read(o->ctx,2,points[0],a,32);if(rc)return rc;
 for(unsigned p=0;p<4;p++){
  rc=o->read(o->ctx,2,points[p],base[p],32);if(rc)return rc;
  rc=o->read(o->ctx,2,points[p],b,32);if(rc)return rc;
  if(memcmp(base[p],b,32))return -13;
 }
 *size=0x2000000;
 for(uint32_t n=65536;n<0x2000000;n<<=1){
  int equal=1;
 for(unsigned p=0;p<4;p++){
   rc=o->read(o->ctx,2,n+points[p],a,32);if(rc)return rc;
   rc=o->read(o->ctx,2,n+points[p],b,32);if(rc)return rc;
   if(p==0){
    /* The high half of a <=16 MiB EEPROM cartridge is not ROM. Both reads
     * may differ there, including in the first 16 bytes. If neither has
     * the header key, this candidate cannot be a ROM mirror. A possible
     * mirror still requires two identical 32-byte samples. */
    int amatch=!memcmp(a,base[0],16),bmatch=!memcmp(b,base[0],16);
    if(!amatch&&!bmatch){equal=0;break;}
    if(!amatch||!bmatch||memcmp(a,b,32))return -13;
   }else{
    if(memcmp(a,b,32))return -13;
    if(memcmp(a,base[p],32)){equal=0;break;}
   }
  }
  if(equal){*size=n;break;}
 }
 return 0;
}
#endif
