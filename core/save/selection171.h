#ifndef CB_SAVE_SELECTION171_H
#define CB_SAVE_SELECTION171_H
#include <stdint.h>

/* Selection is a request, NEVER hardware evidence or write authorization.
 * Keep the detector result immutable. The OS must validate the selected
 * protocol and fresh cartridge identity before touching a blank save. */
enum cb_save171_source {
 CB_SAVE171_UNKNOWN, CB_SAVE171_HARDWARE, CB_SAVE171_DATABASE,
 CB_SAVE171_FLASHCART_DEFAULT, CB_SAVE171_MANUAL
};
struct cb_save171_input {
 int analyzed, header_valid, cart_rc, cleanup_rc;
 int nor_matched, nor_writable, nor_completed;
 int hardware_type, hardware_rc;
 uint32_t hardware_bytes;
 int database_hit, database_type;
 uint32_t database_bytes;
 /* -1 = automatic; 0..9 use the existing FlashGBX type numbers. */
 int manual_type;
};
struct cb_save171_selection {
 int type;
 uint32_t bytes;
 enum cb_save171_source source;
 int conflict, io_blocked, supported;
};
static inline uint32_t cb_save171_bytes(int type){
 static const uint32_t sizes[]={0,512,8192,32768,65536,131072,
                               1048576,65536,131072,0};
 return type>=0&&type<=9?sizes[type]:0;
}
static inline const char *cb_save171_name(int type){
 static const char *const names[]={"None","EEPROM 512 B","EEPROM 8 KiB",
  "SRAM/FRAM 32 KiB","Flash 64 KiB","Flash 128 KiB","DACS 1 MiB",
  "Unlicensed SRAM 64 KiB","Unlicensed SRAM 128 KiB","Batteryless SRAM"};
 return type>=0&&type<=9?names[type]:"Unknown / select type";
}
static inline int cb_save171_valid(int type,uint32_t bytes){
 return type>=0&&type<=9&&bytes==cb_save171_bytes(type);
}
static inline struct cb_save171_selection cb_save171_select(
 const struct cb_save171_input *i){
 struct cb_save171_selection s={-1,0,CB_SAVE171_UNKNOWN,0,0,0};
 if(!i||!i->analyzed||!i->header_valid)return s;
 s.io_blocked=!!(i->cart_rc||i->cleanup_rc||i->hardware_rc);
 int hw=!i->hardware_rc&&i->hardware_type>0&&
        cb_save171_valid(i->hardware_type,i->hardware_bytes);
 if(i->manual_type>=0){
  if(i->manual_type>9)return s;
  s.type=i->manual_type;s.source=CB_SAVE171_MANUAL;
  /* A manual choice must never defeat a positive hardware classification,
   * especially a known Flash ID. Retain it as a visible conflict instead. */
  s.conflict=hw&&i->hardware_type!=s.type;
 }else if(hw){
  s.type=i->hardware_type;s.source=CB_SAVE171_HARDWARE;
 }else if(i->nor_completed&&i->nor_matched&&i->nor_writable){
  s.type=8;s.source=CB_SAVE171_FLASHCART_DEFAULT;
 }else if(i->nor_completed&&!i->nor_matched&&i->database_hit&&
          cb_save171_valid(i->database_type,i->database_bytes)){
  s.type=i->database_type;s.source=CB_SAVE171_DATABASE;
 }
 s.bytes=cb_save171_bytes(s.type);
 /* Current backends: no 64 KiB unlicensed SRAM, DACS or batteryless writes.
  * supported describes backend availability, NOT permission to write. */
 s.supported=(s.type>=1&&s.type<=5)||s.type==8;
 return s;
}
#endif
