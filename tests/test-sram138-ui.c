#include "../app/ui98/ui.h"
#include "of_input_types.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
static uint8_t fb[CB_W*CB_H+32];
int main(void){
 struct cb_ui u={0};u.cart.platform=CB_GBA;u.cart.result=CB_READY;
 strcpy(u.cart.save,"Unknown (ROM hint)");
 strcpy(u.cart.title,"WARIO LAND");
 strcpy(u.cart.rtc,"Read not supported yet");
 u.page=CB_INFO;u.detail=0;
 cb_render(fb+16,&u);
 assert(!strcmp(u.cart.title,"WARIO LAND"));
 assert(!strcmp(u.cart.rtc,"Read not supported yet"));
 assert(cb_action_enabled(&u,3));
 assert(!cb_action_enabled(&u,5));
 u.save_detected_type=3;u.save_detected_bytes=32768;
 assert(cb_action_enabled(&u,5));
 u.focus=5;u.page=CB_FUNCTIONS;assert(cb_key(&u,OF_BTN_A)==6);
 u.save_detected_type=4;assert(!cb_action_enabled(&u,5));
 u.save_detected_type=8;u.save_detected_bytes=131072;assert(cb_action_enabled(&u,5));
 u.save_detected_bytes=65536;assert(!cb_action_enabled(&u,5));
 u.save_detected_bytes=131072;u.save_rc=-4;assert(!cb_action_enabled(&u,5));
 u.save_rc=0;u.focus=0;
 u.options=1;cb_key(&u,OF_BTN_RIGHT);assert(u.save_profile==1);
 cb_key(&u,OF_BTN_RIGHT);assert(u.save_profile==2);
 cb_key(&u,OF_BTN_A);assert(!u.options);
 u.page=CB_INFO;u.detail=1;cb_key(&u,OF_BTN_DOWN);assert(u.detail==2);
 u.save_observed=131072;u.save_repeat=32768;
 memset(fb,0xa5,sizeof(fb));cb_render(fb+16,&u);
 for(unsigned i=0;i<16;i++)assert(fb[i]==0xa5&&fb[sizeof(fb)-i-1]==0xa5);
 for(unsigned i=16;i<sizeof(fb)-16;i++)assert(fb[i]<16);
 cb_key(&u,OF_BTN_DOWN);assert(u.detail==0);
 u.options=1;cb_key(&u,OF_BTN_LEFT);assert(u.save_profile==1&&!u.save_observed);
 puts("SRAM138 UI PASS: missing hint backup enabled, profile controls, hardware page, framebuffer bounds");
}
