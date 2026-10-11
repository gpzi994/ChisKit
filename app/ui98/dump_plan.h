#ifndef CB_DUMP_PLAN_H
#define CB_DUMP_PLAN_H
#include "model.h"
#include <string.h>
/* Private UI107 spaces 16..23. Capacity is a header declaration, never NOR. */
static inline int cb_dump_mode(const struct cb_cart *c){
 if(c->platform!=CB_GB||c->result!=CB_READY)return -1;
 int mode=-1;uint32_t max=0;
 switch(c->type){
 case 0:case 8:case 9:mode=0;max=32768;break;
 case 1:case 2:case 3:
  /* Known multicart wiring is not ordinary MBC1. Do not silently misdump. */
  if(!strcmp(c->title,"MOMOCOL")||!strcmp(c->title,"BOMCOL")||!strcmp(c->title,"BOMSEL")||
     !strcmp(c->title,"GENCOL")||!strcmp(c->title,"SUPERCHINESE 123")||
     !strcmp(c->title,"MORTALKOMBATI&II")||!strcmp(c->title,"MORTALKOMBAT DUO"))return -1;
  mode=1;max=2097152;break;
 case 5:case 6:mode=2;max=262144;break;
 case 0x0f:case 0x10:case 0x11:case 0x12:case 0x13:
  mode=c->type==0x10&&c->ram_bytes==65536?4:3;max=mode==4?4194304:2097152;break;
 case 0x19:case 0x1a:case 0x1b:case 0x1c:case 0x1d:case 0x1e:mode=5;max=8388608;break;
 case 0x22:mode=6;max=4194304;break;
 case 0xfc:mode=7;max=1048576;break;
 }
 return c->rom_bytes>=32768&&c->rom_bytes<=max&&!(c->rom_bytes&16383)?mode:-1;
}
#endif
