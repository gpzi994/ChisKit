#include "detect139.h"
#include <stddef.h>
#include <string.h>
uint32_t cb_save139_repeat(const uint8_t *data,size_t size){
 if(!data||size!=131072)return 0;
 while(size>32&&!memcmp(data,data+size/2,size/2))size/=2;
 return (uint32_t)size;
}
int cb_save139_needs_eeprom(const struct cb_save139_input *p){
 return p&&!p->io_rc&&!p->restore_rc&&p->stable&&p->cleanup_verified&&
  p->bank0_read&&p->bank1_read&&!p->flash_id_valid&&
  !p->dacs_signature&&!p->batteryless_signature&&
  p->repeating_bytes>=32&&p->repeating_bytes<=256;
}
struct cb_save139_result cb_save139_header_marker(const uint8_t *h,size_t n){
 struct cb_save139_result r={.type=CB_SAVE139_UNKNOWN};
 static const uint32_t sizes[]={0,512,8192,32768,65536,131072};
 if(!h||n<192||h[0xbe]!=0x4c)return r;
 unsigned flags=h[0xbf],code=flags&7,medium=(flags>>3)&3,bl=flags&32;
 if(code>=6)return r;
 uint32_t bytes=sizes[code];
 if(medium==0){if(bytes||bl)return r;r.type=CB_SAVE139_NONE;}
 else if(medium==1){
  if(bl){if(!bytes)return r;r.type=CB_SAVE139_BATTERYLESS;}
  else r.type=bytes<=32768?CB_SAVE139_SRAM32:bytes==65536?CB_SAVE139_SRAM64:CB_SAVE139_SRAM128;
 }else{
  if(bl)return r;
  if(medium==2){
   if(bytes!=0&&bytes!=512&&bytes!=8192)return r;
   r.type=bytes==8192?CB_SAVE139_EEPROM8K:CB_SAVE139_EEPROM512;
  }else r.type=bytes==131072?CB_SAVE139_FLASH128:CB_SAVE139_FLASH64;
 }
 r.bytes=bytes;r.basis=CB_SAVE139_HEADER_MARKER;return r;
}
const char *cb_save139_name(enum cb_save139_type t){
 static const char *const names[]={"None", "4K EEPROM", "64K EEPROM",
  "256K SRAM/FRAM", "512K FLASH", "1M FLASH", "8M DACS",
  "Unlicensed 512K SRAM", "Unlicensed 1M SRAM", "Unlicensed Batteryless SRAM"};
 return t>=0&&t<=9?names[t]:"Hardware unconfirmed";
}
struct cb_save139_result cb_save139_classify(const struct cb_save139_input *p){
 struct cb_save139_result r={.type=CB_SAVE139_UNKNOWN};
 if(!p){r.rc=-1;return r;}
 if(p->io_rc||p->restore_rc){r.rc=p->restore_rc?p->restore_rc:p->io_rc;return r;}
 if(!p->stable||!p->cleanup_verified)return r;
 /* Special saves live in different backing storage. Never authorize ordinary
  * SRAM backup merely because their running game exposes writable SRAM. */
 if(p->batteryless_signature){
  uint32_t n=p->batteryless_bytes;
  if(n!=512&&n!=8192&&n!=32768&&n!=65536&&n!=131072)return r;
  r.type=CB_SAVE139_BATTERYLESS;r.bytes=n;r.basis=CB_SAVE139_SPECIAL_SIGNATURE;return r;
 }
 if(p->dacs_signature){r.type=CB_SAVE139_DACS;r.bytes=1048576;r.basis=CB_SAVE139_SPECIAL_SIGNATURE;return r;}
 if(p->flash_id_valid){
  if(p->parallel_writable)return r; /* contradictory evidence */
  r.flash_id=p->flash_id;
  switch(p->flash_id){
   case 0xbfd4:case 0x1f3d:case 0xc21c:case 0x321b:r.bytes=65536;break;
   case 0xc209:case 0x6213:r.bytes=131072;break;
   case 0xbf4b:case 0xbf5b:case 0xbf6d:case 0xffff:
    if(!p->bank0_read||!p->bank1_read)return r;
    r.bytes=p->parallel_all_ff||!p->banks_equal?131072:65536;break;
   default:return r;
  }
  r.type=r.bytes==65536?CB_SAVE139_FLASH64:CB_SAVE139_FLASH128;
  r.basis=CB_SAVE139_FLASH_ID;return r;
 }
 if(p->parallel_writable||p->repeating_bytes>256){
  if(!p->bank0_read||!p->bank1_read)return r;
  switch(p->repeating_bytes){
   case 512:r.type=CB_SAVE139_EEPROM512;break;
   case 8192:r.type=CB_SAVE139_EEPROM8K;break;
   case 32768:r.type=CB_SAVE139_SRAM32;break;
   case 65536:r.type=CB_SAVE139_SRAM64;break;
   case 131072:r.type=CB_SAVE139_SRAM128;break;
   default:return r; /* uniform/short repetition cannot prove capacity */
  }
  r.bytes=p->repeating_bytes;r.basis=CB_SAVE139_PARALLEL_PROBE;
  /* Match upstream classification by observed span, but never authorize
   * SRAM backup without a restored, verified direct-write observation. */
  r.backup_allowed=p->parallel_writable&&
   (r.type==CB_SAVE139_SRAM32||r.type==CB_SAVE139_SRAM64||r.type==CB_SAVE139_SRAM128);
  return r;
 }
 if(p->eeprom_tested&&!p->eeprom_uniform){
  r.type=p->eeprom_prefix_equal?CB_SAVE139_EEPROM8K:CB_SAVE139_EEPROM512;
  r.bytes=p->eeprom_prefix_equal?8192:512;r.basis=CB_SAVE139_SERIAL_PROBE;
 }
 /* Uniform EEPROM or an open bus is UNKNOWN, not positive proof of no save. */
 return r;
}
