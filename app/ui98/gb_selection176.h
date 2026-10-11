#ifndef CB_GB_SELECTION176_H
#define CB_GB_SELECTION176_H
#include <stdio.h>
#include "../../core/gb_save175/gb_save175.h"
#ifdef CB_GB_SAVE179
#define gb176_plan gb179_plan
#define gb176_content_bytes gb177_content_bytes
#define of_gb176_capture of_gb179_capture
#elif defined(CB_GB_SAVE178)
#define gb176_plan gb178_plan
#define gb176_content_bytes gb177_content_bytes
#define of_gb176_capture of_gb178_capture
#elif defined(CB_GB_SAVE177)
#define gb176_plan gb177_plan
#define gb176_content_bytes gb177_content_bytes
#define of_gb176_capture of_gb177_capture
#endif
/* GB-only choice indexes. No GBA enums or selection state. */
static inline const char *gb176_choice_name(unsigned c){
 static const char *const names[]={"Automatic","None","SRAM 512 bytes","SRAM 2 KiB",
 "SRAM 8 KiB","SRAM 32 KiB","SRAM 64 KiB","SRAM 128 KiB",
 "MBC7 EEPROM 256 bytes","MBC7 EEPROM 512 bytes"};
 return c<10?names[c]:"Unsupported";
}
static inline void gb176_selection(struct cb_ui *u){
 static const uint32_t sizes[]={0,0,512,2048,8192,32768,65536,131072,256,512};
 u->gb_save175_ready=0;u->gb_save175_bytes=0;
 if(u->cart.platform!=CB_GB||u->cart.result!=CB_READY||u->gb_probe_rc||u->gb_choice>=10)return;
 uint32_t n=u->gb_choice?sizes[u->gb_choice]:u->gb_detected_bytes;
#ifdef CB_GB_SAVE179
 if(u->gb_choice==3||u->gb_choice==9||!n)return;
 if((u->gb_save175_header[0x47]==0x22 && u->gb_choice!=0&&u->gb_choice!=8)||
    (u->gb_save175_header[0x47]!=0x22&&u->gb_choice==8))return;
#else
 if(u->gb_choice>=8||!n)return;
#endif
 struct gb175_plan p;if(gb176_plan(1,u->gb_save175_header,n,&p)){
#ifdef CB_GB_EMPTY181
  if(!u->gb_choice&&u->gb_capacity_source==2){
   snprintf(u->cart.save,sizeof u->cart.save,"128K default / mapper limit");
   u->notice="128 KiB exceeds mapper; select RAM size";
  }
#endif
  return;
 }
 u->gb_save175_bytes=n;u->gb_save175_ready=1;
 if(p.mapper==GB179_MBC7)snprintf(u->cart.save,sizeof u->cart.save,"MBC7 EEPROM 256 bytes");
 else if(p.mapper==GB178_MBC2)snprintf(u->cart.save,sizeof u->cart.save,"MBC2 512 bytes (4-bit)");
 else snprintf(u->cart.save,sizeof u->cart.save,"RAM %lu KiB (%s)",
  (unsigned long)(n/1024),u->gb_choice?"manual":
#ifdef CB_GB_EMPTY181
  u->gb_capacity_source==2?"default":u->gb_capacity_source==3?"DB/header":
#endif
  "probe");
}
#endif
