#include "ui.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 struct cb_ui u={0};u.cart.platform=CB_GBA;u.cart.result=CB_READY;
 assert(cb_action_enabled(&u,3));
#ifdef CB_EEPROM139_DIAG
 u.save_serial_candidate=1;
 assert(cb_action_enabled(&u,3));
 u.analysis.matched=u.analysis.writable=1;
 assert(!cb_action_enabled(&u,4)&&!cb_action_enabled(&u,5));
 u.busy=1;assert(!cb_action_enabled(&u,3));u.busy=0;
 u.cart.platform=CB_GB;assert(!cb_action_enabled(&u,3));
 u.cart.platform=CB_GBA;u.cart.result=CB_BAD_HEADER;assert(!cb_action_enabled(&u,3));
 puts("EEPROM diagnostic UI PASS: raw capture allowed, ROM/save writes denied, busy/platform/header guards");
#else
 u.save_serial_candidate=1;
 assert(!cb_action_enabled(&u,3));
 assert(cb_action_enabled(&u,2)); /* ROM backup unaffected. */
 for(unsigned p=0;p<3;p++){u.save_profile=p;assert(!cb_action_enabled(&u,3));}
#ifdef CB_EEPROM139
 for(int t=1;t<=2;t++){
  u.save_detected_type=t;u.save_detected_bytes=t==1?512:8192;
  assert(cb_action_enabled(&u,3)&&!cb_action_enabled(&u,5));
  u.save_rc=-3;assert(!cb_action_enabled(&u,3));u.save_rc=0;
  u.busy=1;assert(!cb_action_enabled(&u,3));u.busy=0;
  u.save_detected_bytes=131072;assert(!cb_action_enabled(&u,3));
 }
 for(int t=4;t<=5;t++){u.save_detected_type=t;u.save_detected_bytes=t==4?65536:131072;assert(!cb_action_enabled(&u,3));}
#endif
 puts("EEPROM UI PASS: serial candidate blocks save backup, profile cannot override, ROM backup preserved");
#endif
}
