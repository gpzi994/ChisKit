#include "../app/ui98/ui.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned char pixels[CB_W*CB_H+32];
int main(void){
 struct cb_ui u={0};u.cart.platform=CB_GBA;u.cart.result=CB_READY;
 strcpy(u.cart.title,"OugonTaiyo_A");strcpy(u.cart.rtc,"Read not supported yet");
 const char *names[]={"SRAM/FRAM 32 KiB","Unlicensed SRAM 128 KiB","EEPROM 512 B (estimate)","EEPROM 8 KiB","Flash 64 KiB","Flash 128 KiB"};
 for(unsigned i=0;i<6;i++){
  strcpy(u.cart.save,names[i]);u.save_serial_candidate=i>=2;
  assert(cb_action_enabled(&u,3)==(i<2));assert(!cb_action_enabled(&u,5));
  for(unsigned page=0;page<3;page++){
   u.page=page;memset(pixels,0xa5,sizeof pixels);cb_render(pixels+16,&u);
   for(unsigned j=0;j<16;j++)assert(pixels[j]==0xa5&&pixels[sizeof pixels-j-1]==0xa5);
  }
  assert(!strcmp(u.cart.title,"OugonTaiyo_A"));
 }
 u.save_serial_candidate=1;u.save_rc=-4;assert(!cb_action_enabled(&u,3));
 puts("PASS normal UI: SRAM backup only, EEPROM/Flash locked, save writes locked, bounds and title preserved");
}
