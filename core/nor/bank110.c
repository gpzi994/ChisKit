// SPDX-License-Identifier: AGPL-3.0-only
// Mori burner_gba_lowlevel.c burner_bacon_gba_rom_switch_bank.
#include "bank110.h"
#include <string.h>
static int valid(enum cb_bank110_kind kind,uint32_t bytes){
 return (kind==CB_BANK110_AMD_BASIC||kind==CB_BANK110_TYPE1)&&
  bytes>=CB_BANK110_WINDOW&&bytes<=CB_BANK110_LIMIT&&!(bytes&(bytes-1));
}
int cb_bank110_map(enum cb_bank110_kind kind,uint32_t bytes,uint32_t address,
 unsigned *bank,uint32_t *local){
 if(bank)*bank=0;
 if(local)*local=0;
 if(!bank||!local||!valid(kind,bytes)||address>=bytes)return -1;
 *bank=address/CB_BANK110_WINDOW;*local=address%CB_BANK110_WINDOW;
 return 0;
}
size_t cb_bank110_select(enum cb_bank110_kind kind,uint32_t bytes,unsigned bank,
 struct cb_bank110_write out[3]){
 if(!out)return 0;
 memset(out,0,3*sizeof(*out));
 if(!valid(kind,bytes)||bank>=bytes/CB_BANK110_WINDOW)return 0;
 out[0]=(struct cb_bank110_write){2,(uint8_t)(bank<<4),3};
 out[1]=(struct cb_bank110_write){3,0x40,3};
 if(kind==CB_BANK110_TYPE1){
  out[2]=(struct cb_bank110_write){4,0,3};return 3;
 }
 return 2;
}
