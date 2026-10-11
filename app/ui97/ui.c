#include "ui.h"
#include <stdio.h>
#include <string.h>
#include "of_input_types.h"
extern const uint8_t cb_font[2048];
const uint32_t cb_palette[16]={0xf3f7fc,0xffffff,0x172f49,0x62778d,0xc8d6e5,0x1875e8,0xe3efff,0x0a805f,0xe6f5ef,0xb54731,0xffeee8,0xf5c64a,0xe6ecf3,0x9baabd,0xd6e4f3,0x101f31};
static uint8_t *pixels;
static void box(int x,int y,int w,int h,unsigned c){
 /* Layout is authored on a 240-line grid; use the existing 10:9 scaler.
  * Text remains native 8x8: only positions/panel spacing expand vertically. */
 int bottom=(y+h)*CB_H/240;y=y*CB_H/240;h=bottom-y;
 if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
 if(x+w>CB_W)w=CB_W-x;
 if(y+h>CB_H)h=CB_H-y;
 if(w<=0||h<=0)return;
 for(int row=0;row<h;row++)memset(pixels+(y+row)*CB_W+x,c,(size_t)w);
}
static void panel(int x,int y,int w,int h,unsigned fill,unsigned border){
 box(x+1,y,w-2,h,border);box(x,y+1,w,h-2,border);box(x+1,y+1,w-2,h-2,fill);
}
static void text(int x,int y,const char *s,unsigned color,unsigned limit){
 if(!s)return;
 y=y*CB_H/240;
 for(unsigned n=0;*s&&n<limit;n++,s++,x+=8){
  const uint8_t *g=cb_font+(uint8_t)*s*8;
  for(int row=0;row<8;row++)for(int col=0;col<8;col++)
   if((g[row]&(0x80>>col))&&x+col>=0&&x+col<CB_W&&y+row>=0&&y+row<CB_H)
    pixels[(y+row)*CB_W+x+col]=(uint8_t)color;
 }
}
static void icon(int x,int y,unsigned kind,unsigned c){
 /* Small purpose-drawn pixel icons share a 16 x 16 grid. */
 if(kind==0){text(x,y,"R",c,1);box(x+9,y+1,4,2,c);box(x+11,y+3,2,7,c);box(x+4,y+12,9,2,c);}
 else if(kind==1||kind==6){panel(x+2,y,12,16,1,c);text(x+4,y+4,kind==1?"?":"i",c,1);}
 else if(kind==7){box(x+5,y,6,16,c);box(x,y+5,16,6,c);box(x+5,y+5,6,6,1);}
 else {panel(x,y+3,16,12,1,c);box(x+4,y,8,5,c);box(x+4,y+9,8,4,c);}
}
static void row(unsigned i,const char *label,const char *value){
 int y=48+(int)i*13;
 if(i&1)box(7,y-2,306,13,0);
 text(12,y,label,3,11);text(108,y,value,2,25);
}
static void footer(const char *hint){box(0,222,320,18,14);text(8,227,hint,2,38);}
static void functions(const struct cb_ui *u){
 text(9,29,"Platform: Auto",3,20);text(9,43,cb_platform_name(u->cart.platform),5,10);
 text(91,43,u->probing?"Detecting...":cb_result_name(u->cart.result),u->cart.result==CB_READY?7:3,27);
 static const char *labels[]={"Refresh","Analyze Cart","Backup ROM","Backup Save","Write ROM","Write Save","Cart Info","Options"};
 for(unsigned i=0;i<8;i++){
  int x=7+(int)(i%2)*156,y=62+(int)(i/2)*35;
  int enabled=(i<2||i>=6),selected=i==u->focus;
  panel(x,y,150,31,selected?6:enabled?1:12,selected?5:4);
  if(selected)box(x+1,y+3,3,25,5);
  icon(x+9,y+7,i,enabled?2:13);text(x+30,y+11,labels[i],enabled?2:3,14);
 }
 text(9,207,u->notice?u->notice:"Stage 1 / read-only",3,37);
 footer("D-pad Select  A OK  L/R Pages  B Back");
 if(u->options){
  panel(23,70,274,118,1,5);text(35,81,"Options / smoke.97",2,30);
  text(35,102,"Platform: Auto",3,29);text(35,117,"Display: 320 x 288",3,29);
  text(35,132,"Transfers: Not enabled",3,29);text(35,165,"B: Close",5,29);
 }
}
static void info(const struct cb_ui *u){
 const struct cb_cart *c=&u->cart;char buf[32];
 text(9,29,c->platform==CB_GBA?"GBA Cartridge Info":c->platform==CB_GB?"GB/GBC Cartridge Info":"Cartridge Info",2,37);
 panel(6,43,308,159,1,4);
 if(c->platform==CB_UNKNOWN){
  text(18,63,cb_result_name(c->result),3,34);
  text(18,89,"A: Analyze cartridge",5,34);
  snprintf(buf,sizeof(buf),"Read rc %d / safe rc %d",c->rc,c->safe_rc);text(18,115,buf,3,34);
  text(18,141,"NOR: Not probed",3,34);
 }else{
  row(0,"ROM title",c->title[0]?c->title:"(untitled)");
  if(c->platform==CB_GBA){
   row(1,"Code/Maker",c->code);
   snprintf(buf,sizeof(buf),"%u",c->revision);row(2,"Revision",buf);
  }else{
   snprintf(buf,sizeof(buf),"%s / %02X",c->mapper,c->type);row(1,"Mapper",buf);
   snprintf(buf,sizeof(buf),"%u / %s",c->revision,(c->cgb&0x80)?"CGB":"GB");row(2,"Rev/Color",buf);
  }
  row(3,"Logo","Nintendo / valid");
  snprintf(buf,sizeof(buf),"Valid (0x%02X)",c->checksum);row(4,"Header sum",buf);
  if(c->platform==CB_GB)snprintf(buf,sizeof(buf),"0x%04X (stored only)",c->global_checksum);
  else strcpy(buf,"Not computed");
  row(5,"ROM sum",buf);row(6,"ROM size",c->rom);row(7,"Save",c->save);row(8,"RTC",c->rtc);
  row(9,"NOR model","Not probed (stage 1)");row(10,"NOR size","Unknown");row(11,"Profile","Read-only header");
 }
 text(9,208,c->platform!=CB_UNKNOWN?"Header OK / NOR analysis pending":"Analysis not complete",c->platform!=CB_UNKNOWN?7:3,38);
 footer("A Analyze    L/R Pages    B Functions");
}
static void transfer(const struct cb_ui *u){
 (void)u;text(9,29,"Transfer Status",2,37);
 panel(7,45,306,44,1,4);text(17,55,"No active task",2,34);text(17,72,"Backup / ROM / Save",3,34);
 panel(7,96,306,91,1,4);
 text(17,105,"Transferred",3,16);text(201,105,"-- / --",2,12);
 text(17,122,"Speed",3,16);text(201,122,"-- KiB/s",2,12);
 text(17,139,"Elapsed",3,16);text(201,139,"--:--:--",2,12);
 text(17,156,"ETA",3,16);text(201,156,"--:--:--",2,12);
 panel(17,172,260,7,12,4);text(285,171,"--",3,2);
 text(9,197,"Transfers unavailable in stage 1",3,38);
 text(9,210,"No transfer has been started",3,38);
 footer("L/R Pages                B Functions");
}
void cb_render(uint8_t *fb,const struct cb_ui *u){
 pixels=fb;memset(fb,0,CB_W*CB_H);box(0,0,320,21,14);
 /* Cartridge-shaped yellow mark, rendered as geometry. */
 panel(7,5,13,12,11,2);box(10,3,7,3,11);box(10,13,2,3,2);box(15,13,2,3,2);
 text(27,7,"ChisBurner",2,15);text(226,7,"smoke.97",3,11);
 if(u->page==CB_FUNCTIONS)functions(u);else if(u->page==CB_INFO)info(u);else transfer(u);
}
int cb_key(struct cb_ui *u,uint32_t key){
 if(u->probing)return 0;
 if(u->options){if(key&(OF_BTN_B|OF_BTN_A))u->options=0;return 0;}
 if(key&OF_BTN_B){u->page=CB_FUNCTIONS;u->notice=0;return 0;}
 if(key&OF_BTN_L1){u->page=(enum cb_page)((u->page+2)%3);return 0;}
 if(key&OF_BTN_R1){u->page=(enum cb_page)((u->page+1)%3);return 0;}
 if(u->page==CB_INFO)return(key&OF_BTN_A)?2:0;
 if(u->page!=CB_FUNCTIONS)return 0;
 if(key&OF_BTN_UP)u->focus=(u->focus+6)%8;
 else if(key&OF_BTN_DOWN)u->focus=(u->focus+2)%8;
 else if(key&(OF_BTN_LEFT|OF_BTN_RIGHT))u->focus^=1;
 else if(key&OF_BTN_A){
  u->notice=0;
  if(u->focus<2)return (int)u->focus+1;
  if(u->focus==6)u->page=CB_INFO;
  else if(u->focus==7)u->options=1;
  else u->notice="Transfer not enabled in this build";
 }
 return 0;
}
