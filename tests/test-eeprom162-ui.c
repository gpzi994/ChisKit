#include "ui.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 struct cb_ui u={0};u.cart.platform=CB_GBA;u.cart.result=CB_READY;
 for(int t=1;t<=2;t++){
  u.save_detected_type=t;u.save_detected_bytes=t==1?512:8192;
  assert(cb_action_enabled(&u,3)&&cb_action_enabled(&u,5));
  u.busy=1;assert(!cb_action_enabled(&u,5));u.busy=0;
  u.save_rc=-3;assert(!cb_action_enabled(&u,5));u.save_rc=0;
  u.save_detected_bytes=t==1?8192:512;
  assert(!cb_action_enabled(&u,5));
 }
 #ifdef CB_FLASH165
 u.save_detected_type=4;u.save_detected_bytes=65536;
 assert(cb_action_enabled(&u,3));
#ifdef CB_FLASH170
 assert(cb_action_enabled(&u,5));
#else
 assert(!cb_action_enabled(&u,5));
#endif
 u.save_rc=-3;assert(!cb_action_enabled(&u,3));u.save_rc=0;
 u.save_detected_bytes=131072;assert(!cb_action_enabled(&u,3));
 u.save_detected_type=5;
#ifdef CB_FLASH166
 assert(cb_action_enabled(&u,3));
#ifdef CB_FLASH170
 assert(cb_action_enabled(&u,5));
#else
 assert(!cb_action_enabled(&u,5));
#endif
 u.busy=1;assert(!cb_action_enabled(&u,3));u.busy=0;
 u.save_rc=-3;assert(!cb_action_enabled(&u,3));u.save_rc=0;
#else
 assert(!cb_action_enabled(&u,3));
#endif
 #endif
 u.save_detected_type=3;u.save_detected_bytes=32768;
 assert(cb_action_enabled(&u,5));
 u.save_detected_type=8;u.save_detected_bytes=131072;
 assert(cb_action_enabled(&u,5));
 for(int t=4;t<=5;t++){
  u.save_detected_type=t;u.save_detected_bytes=t==4?65536:131072;
#ifdef CB_FLASH170
  assert(cb_action_enabled(&u,5));
#else
  assert(!cb_action_enabled(&u,5));
#endif
 }
 u.save_detected_type=1;u.save_detected_bytes=512;
 u.cart.result=CB_BAD_HEADER;assert(!cb_action_enabled(&u,5));
 u.cart.result=CB_READY;u.cart.platform=CB_GB;assert(!cb_action_enabled(&u,5));
#ifdef CB_FLASH170
 puts("Flash170 UI PASS: 64/128 KiB Flash enabled; EEPROM/SRAM retained; invalid states locked");
#else
 puts("EEPROM162 UI PASS: hardware-confirmed 512/8192 writable; SRAM unchanged; Flash and bad states locked");
#endif
}
