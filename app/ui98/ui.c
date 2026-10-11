#include "ui.h"
#include "dump_plan.h"
#include <stdio.h>
#include <string.h>
#include "of_input_types.h"
#include "version.h"
#include "header_diag.h"
#include "save_backup161.h"
#ifdef CB_GB_SAVE176
#include "gb_selection176.h"
#endif
#ifdef CB_SAVE171
#include "save_selection171.h"
void cb_save171_update(struct cb_ui *u){
 u->save_selection=cb_ui_save171_selection(u,(int)u->save_choice-1);
}
static int save171_pending(const struct cb_ui *u){
 const struct cb_save171_selection *s=&u->save_selection;
 return s->source!=CB_SAVE171_UNKNOWN&&(s->conflict||s->io_blocked||
  !s->supported||s->type!=u->save_detected_type||s->bytes!=u->save_detected_bytes);
}
static const char *save171_source(enum cb_save171_source s){
 static const char *const names[]={"Unknown","Hardware","Database reference",
  "Flashcart default","Manual selection"};
 return s<=CB_SAVE171_MANUAL?names[s]:"Unknown";
}
#endif
#ifdef CB_GBA_HEADER140
#include "gba_header140.h"
#ifdef CB_STAGES141
#include "of_save139.h"
#include "stages141.h"
#include "save_read142.h"
#endif
#endif
#ifdef CB_CAMERA136
#include "camera136.h"
#endif
extern const uint8_t cb_font[2048];
const uint32_t cb_palette[16]={0xf3f7fc,0xffffff,0x172f49,0x62778d,0xc8d6e5,0x1875e8,0xe3efff,0x0a805f,0xe6f5ef,0xb54731,0xffeee8,0xf5c64a,0xe6ecf3,0x9baabd,0xd6e4f3,0x101f31};
static uint8_t *pixels;
const uint32_t cb_palette_dark[16]={0x111318,0x191d23,0xf1f3f5,0xadb6c2,0x48515d,0x65adff,0x20354e,0x69dcaa,0x17372c,0xff907e,0x402721,0xffd447,0x20252c,0x717b89,0x171c23,0xffffff};
#ifdef CB_LANG174
#include "lang174.h"
#include "font174.h"
static int chinese174;
#endif
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
#ifdef CB_LANG174
 if(chinese174)s=cb_translate174(s);
 const unsigned char *p=(const unsigned char *)s;
 int start=x,right=x+(int)limit*8;if(right>CB_W)right=CB_W;
 y=y*CB_H/240;
 while(*p){
  unsigned c=cb_utf174(&p);
  if(c=='\n'){x=start;y+=16;continue;}
  int width=c<128?8:12;
  if(x+width>right)break;
  const struct cb_glyph174 *g=0;
  if(c>=128)for(unsigned i=0;i<sizeof cb_font174/sizeof *cb_font174;i++)if(cb_font174[i].code==c){g=cb_font174+i;break;}
  for(int r=0;r<(c<128?8:14);r++)for(int col=0;col<width;col++){
   int set=c<128?(cb_font[c*8+r]&(128>>col)):
    g?(g->rows[r]&(1u<<(11-col))):(r==1||r==12||col==0||col==11);
   int py=y+r+(c<128&&chinese174?3:0);
   if(set&&x+col>=0&&x+col<CB_W&&py>=0&&py<CB_H)pixels[py*CB_W+x+col]=(uint8_t)color;
  }
  x+=width;
 }
#else
 y=y*CB_H/240;
 for(unsigned n=0;*s&&n<limit;n++,s++,x+=8){
  const uint8_t *g=cb_font+(uint8_t)*s*8;
  for(int row=0;row<8;row++)for(int col=0;col<8;col++)
   if((g[row]&(0x80>>col))&&x+col>=0&&x+col<CB_W&&y+row>=0&&y+row<CB_H)
    pixels[(y+row)*CB_W+x+col]=(uint8_t)color;
 }
#endif
}
static void icon(int x,int y,unsigned kind,unsigned c){
 /* Native square-pixel masks: scale the layout origin, never one bitmap axis.
  * Reset and gear both occupy a centered 14x14 footprint. */
 static const uint16_t masks[8][16]={
 {0,0x03c0,0x0ff0,0x1c38,0x300c,0x6006,0x6006,0x603e,0x601c,0x6008,0x6000,0x300c,0x1c38,0x0ff0,0x03c0,0},
 {0x7fc0,0x4040,0x5f40,0x5f40,0x4040,0x47c0,0x4c60,0x4830,0x4830,0x4830,0x4c60,0x47e0,0x0030,0x0018,0x000c,0},
 {0x7ff0,0x4010,0x5fd0,0x5fd0,0x4010,0x4010,0x4110,0x4390,0x47d0,0x4110,0x4110,0x4010,0x7ff0,0,0,0},
 {0x7ff0,0x5090,0x5090,0x5f90,0x4010,0x4110,0x4390,0x47d0,0x4110,0x4110,0x4010,0x5fd0,0x5050,0x7ff0,0,0},
 {0x7ff0,0x4010,0x5fd0,0x5fd0,0x4010,0x4110,0x4110,0x47d0,0x4390,0x4110,0x4010,0x4010,0x7ff0,0,0,0},
 {0x7ff0,0x5090,0x5090,0x5f90,0x4010,0x4110,0x4110,0x47d0,0x4390,0x4110,0x4010,0x5fd0,0x5050,0x7ff0,0,0},
 {0x7ff0,0x4010,0x5fd0,0x5fd0,0x4010,0x4310,0x4310,0x4010,0x4710,0x4310,0x4310,0x4790,0x4010,0x7ff0,0,0},
 {0,0x03c0,0x03c0,0x1bd8,0x3ffc,0x1e78,0x0c30,0x7c3e,0x7c3e,0x0c30,0x1e78,0x3ffc,0x1bd8,0x03c0,0x03c0,0}};
 y=y*CB_H/240;
 for(int r=0;r<16;r++)for(int col=0;col<16;col++)
  if(masks[kind%8][r]&(0x8000u>>col))pixels[(y+r)*CB_W+x+col]=(uint8_t)c;
}
static void row(unsigned i,const char *label,const char *value){
 int y=48+(int)i*13;
 if(i&1)box(7,y-2,306,13,0);
 text(12,y,label,3,11);text(108,y,value,2,25);
}
static void footer(const char *hint){box(0,222,320,18,14);text(8,227,hint,2,38);}
#ifdef CB_ROM164
void cb_reset164(struct cb_ui *u){
 unsigned language=u->language,theme=u->theme;
 memset(u,0,sizeof *u);u->cart.result=CB_LOCKED;
 u->language=u->language_draft=language;
 u->theme=u->theme_draft=theme;
 u->save_detected_type=-1;u->save_serial_candidate=1;
#ifdef CB_SAVE171
 u->save_selection.type=-1;
#endif
 u->notice="Cleared; select Analyze Cart";
}
#endif
#ifdef CB_GB_RESTORE180
#include "../../core/gb_save175/restore180.h"
#endif
int cb_action_enabled(const struct cb_ui *u,unsigned i){
 if(i<2||i>=6)return 1;
#ifdef CB_GB_SAVE175
 if(u->cart.platform==CB_GB&&(i==3||i==5)){
#ifdef CB_GB_RESTORE180
  if(i==5){struct gb175_plan p;return u->gb_save175_ready&&u->cart.result==CB_READY&&!u->busy&&
   !gb_restore_plan(u->gb_save175_header,u->gb_save175_bytes,&p);}
#endif
  return i==3&&u->gb_save175_ready&&u->cart.result==CB_READY&&!u->busy;
 }
#endif
#ifdef CB_SAVE171
 /* Selection is not hardware proof. Overrides use only the new OS service,
  * never the old automatically classified backend. */
 if((i==3||i==5)&&save171_pending(u)){
  const struct cb_save171_selection *s=&u->save_selection;
  return i==5&&u->cart.platform==CB_GBA&&u->cart.result==CB_READY&&!u->busy&&
   !s->conflict&&!s->io_blocked&&s->supported&&
   (s->type==1||s->type==2||s->type==3||s->type==8);
 }
#endif
#ifdef CB_EEPROM139_DIAG
 if(i==3)return u->cart.platform==CB_GBA&&u->cart.result==CB_READY&&!u->busy;
 if(i==4)return 0;
#endif
#ifdef CB_SRAM138
 if(i==5)return u->cart.platform==CB_GBA&&u->cart.result==CB_READY&&!u->busy&&
  !u->save_rc&&(
#ifdef CB_FLASH170
  (u->save_detected_type==4&&u->save_detected_bytes==65536u)||
  (u->save_detected_type==5&&u->save_detected_bytes==131072u)||
#endif
#ifdef CB_EEPROM162
  cb_eeprom161_supported(u->save_detected_type,u->save_detected_bytes)||
#endif
  (u->save_detected_type==3&&u->save_detected_bytes==32768u)||
  ((u->save_detected_type==7||u->save_detected_type==8)&&u->save_detected_bytes==131072u));
#else
 if(i==5)return 0;
#endif
#ifdef CB_SRAM137
#ifdef CB_FLASH165
 if(i==3&&(u->save_detected_type==4||u->save_detected_type==5))
  return ((u->save_detected_type==4&&u->save_detected_bytes==65536u)
#ifdef CB_FLASH166
   ||(u->save_detected_type==5&&u->save_detected_bytes==131072u)
#endif
   )&&
   u->cart.platform==CB_GBA&&u->cart.result==CB_READY&&!u->busy&&!u->save_rc;
#endif
#ifdef CB_EEPROM139
 if(i==3&&cb_eeprom161_supported(u->save_detected_type,u->save_detected_bytes))
  return u->cart.platform==CB_GBA&&u->cart.result==CB_READY&&!u->busy&&!u->save_rc;
#endif
 if(i==3)return u->cart.platform==CB_GBA&&u->cart.result==CB_READY&&!u->busy&&!u->save_serial_candidate;
#endif
 if(i==2)return u->cart.result==CB_READY&&!u->busy&&
#ifdef CB_ROM164
  (u->cart.platform!=CB_GBA||(u->cart.rom_source&&u->cart.rom_bytes))&&
#endif
  (u->cart.platform==CB_GBA||cb_dump_mode(&u->cart)>=0);
 int headerless=u->cart.platform==CB_GBA&&
  (u->cart.result==CB_EMPTY||u->cart.result==CB_BAD_HEADER)&&
  !u->cart.rc&&!u->cart.safe_rc&&u->analysis.nor.version==OF_NOR_VERSION&&
  u->analysis.nor.bus==2&&(u->analysis.nor.flags&OF_NOR_WRITE_OK)&&!u->analysis.nor.plan_rc;
 return i==4&&(u->cart.platform==CB_GBA||u->analysis.nor.version==OF_NOR_VERSION)&&(u->cart.result==CB_READY||headerless)&&
        u->analysis.matched&&u->analysis.writable&&!u->busy;
}
static void functions(const struct cb_ui *u){
 text(9,29,"Platform: Auto",3,20);text(9,43,cb_platform_name(u->cart.platform),5,10);
 text(91,43,u->probing?"Detecting...":
#ifdef CB_ROM164
 u->cart.result==CB_LOCKED?"Not analyzed":
#endif
 cb_result_name(u->cart.result),u->cart.result==CB_READY?7:3,27);
 static const char *labels[]={"Reset","Analyze Cart","Backup ROM","Backup Save","Write ROM","Write Save","Cart Info","Options"};
 for(unsigned i=0;i<8;i++){
  int x=7+(int)(i%2)*156,y=62+(int)(i/2)*35;
  int enabled=cb_action_enabled(u,i),selected=i==u->focus;
  panel(x,y,150,31,selected?6:enabled?1:12,selected?5:4);
  if(selected)box(x+1,y+3,3,25,5);
  const char *label=labels[i];
#ifdef CB_EEPROM139_DIAG
  if(i==3)label="EEPROM capture";
#endif
#ifdef CB_SRAM138
  if(i==5){
#ifdef CB_SAVE160
   label="Write SAV";
#elif defined(CB_SAVE159)
   label="Prep SAV";
#else
   label="Check SAV";
#endif
  }
#endif
  icon(x+9,y+7,i,enabled?2:13);text(x+30,y+11,label,enabled?2:3,14);
 }
 text(9,207,u->notice?u->notice:"Analyze NOR before write; backup is read only",3,37);
#ifdef CB_RTC173
 footer("D-pad Select  A OK  X RTC settings");
#else
 footer("D-pad Select  A OK  L/R Pages  B Back");
#endif
 if(u->options){
#ifdef CB_LANG174
  if(u->options==1){
   box(0,22,320,199,0);
   footer("A Apply/Open   B Cancel");
   panel(8,51,304,153,1,5);
   text(20,60,"Options",2,34);
   text(20,84,"Language",u->settings_focus==0?5:2,12);
   text(125,84,u->language_draft?"English":"中文",5,22);
   if(u->cart.platform==CB_GBA
#ifdef CB_GB_SAVE176
      ||u->cart.platform==CB_GB
#endif
     )
    text(20,108,"Save type settings",u->settings_focus==1?5:2,34);
   text(20,128,"Theme",u->settings_focus==2?5:2,12);
   text(125,128,u->theme_draft?"Dark":"Light",u->settings_focus==2?5:2,20);
   text(20,148,"Up/Down Select; Left/Right Change",3,35);
   text(20,161,"A Apply/Open   B Cancel",2,35);
   text(20,184,"Developed by Alius",3,35);
   if(u->notice)text(20,199,u->notice,9,36);
   return;
  }
#endif
#ifdef CB_GB_SAVE176
  if(u->cart.platform==CB_GB){
   struct cb_ui candidate=*u;candidate.gb_choice=u->gb_choice_draft;gb176_selection(&candidate);
   panel(16,57,288,157,1,5);text(27,68,"GB save type / Left-Right",2,33);
   text(27,90,gb176_choice_name(u->gb_choice_draft),5,33);
   char sizes[40];
   if(u->gb_detected_bytes&&u->gb_detected_bytes<1024)snprintf(sizes,sizeof sizes,"Probe %lu bytes",(unsigned long)u->gb_detected_bytes);
   else snprintf(sizes,sizeof sizes,"Header %lu / Probe %lu KiB",(unsigned long)(u->cart.ram_bytes/1024),(unsigned long)(u->gb_detected_bytes/1024));
   text(27,112,sizes,2,33);
#ifdef CB_GB_RESTORE180
   struct gb175_plan restore_plan;
   const char *available=gb_restore_plan(candidate.gb_save175_header,candidate.gb_save175_bytes,&restore_plan)?
    "Read-only backup available":"Backup / SRAM restore available";
#else
   const char *available="Read-only backup available";
#endif
   text(27,134,u->gb_probe_rc?"Probe error: analyze again":candidate.gb_save175_ready?available:"Unsupported / unknown size",candidate.gb_save175_ready?3:9,33);
#ifdef CB_GB_RESTORE180
   text(27,158,"SRAM restore; RTC unchanged",3,33);
#else
   text(27,158,"GB restore / RTC remain disabled",3,33);
#endif
   text(27,180,"A Apply   B Cancel",5,33);
   text(27,196,"Reset clears selection",3,33);return;
  }
#endif
#ifdef CB_SAVE171
  if(u->cart.platform==CB_GBA){
   struct cb_save171_selection s=cb_ui_save171_selection(u,(int)u->save_choice_draft-1);
   panel(16,57,288,157,1,5);text(27,68,"Save type / Left-Right",2,33);
   text(27,87,u->save_choice_draft?cb_save171_name((int)u->save_choice_draft-1):"Automatic",5,33);
   text(27,105,cb_save171_name(s.type),2,33);
   text(27,122,save171_source(s.source),3,33);
   text(27,139,s.io_blocked?"I/O error: analyze again":s.conflict?"Conflicts with hardware":
    !s.supported?"No supported write backend":s.type!=u->save_detected_type||s.bytes!=u->save_detected_bytes?
    "Confirm type before restore":"Existing hardware path",s.io_blocked||s.conflict?9:3,33);
   text(27,159,"ROM / RTC settings unchanged",3,33);
   text(27,180,"A Apply   B Cancel",5,33);
   text(27,196,"Reset clears selection",3,33);return;
  }
#endif
  panel(23,70,274,118,1,5);text(35,81,"Options / " CB_RELEASE_NAME,2,30);
  text(35,102,"Platform: Auto",3,29);
#ifdef CB_SRAM138
  text(35,117,"Save profile: Left/Right",3,29);
  text(35,132,u->save_profile==2?"ChisFlash FRAM 128 KiB":u->save_profile==1?"Retail SRAM/FRAM 32 KiB":"Parallel probe (estimate)",3,29);
#else
  text(35,117,"Display: 320 x 288",3,29);
  text(35,132,"GBA: verified engine / UI98",3,29);
#endif
  text(35,145,"Save/RTC writes: locked",3,29);text(35,165,"B: Close",5,29);
 }
}
static void info(const struct cb_ui *u){
#ifdef CB_STAGES141
 static const char *const names[]={"Baseline","disabled","disabled","disabled","Save read","Session read","64K then read","Bank compare","Save probe + wait"};
 const struct cb_gba_header140 *r=&cb_gba_header140,*b=&cb_stage141_before;
 unsigned p=u->detail%4,diff=0;char line[64];struct cb_cart parsed={0};
 int complete=!r->rc[p]&&r->blocks[p]==6,valid=complete&&cb_parse_gba(&parsed,r->data[p]);
 snprintf(line,sizeof line,"Select: %s (A runs)",names[cb_stage141_selected]);text(8,46,line,5,38);
 snprintf(line,sizeof line,"Ran: %s %s",names[cb_stage141_ran],cb_stage141_executed?"":"SKIPPED");text(8,60,line,3,38);
 snprintf(line,sizeof line,"Op rc %d safe %d",cb_stage141_rc,cb_stage141_safe);text(8,74,line,3,38);
 snprintf(line,sizeof line,"After %u/4 rc %d blocks %u/6",p+1,r->rc[p],r->blocks[p]);text(8,88,line,3,38);
 text(8,102,valid?"Header VALID":complete?"Header INVALID":"Read INCOMPLETE",5,38);
 if(complete&&!b->rc[0]&&b->blocks[0]==6){for(unsigned i=0;i<192;i++)diff+=b->data[0][i]!=r->data[p][i];snprintf(line,sizeof line,"Before/after diff %u",diff);}
 else snprintf(line,sizeof line,"Before/after unavailable");
 text(8,116,line,3,38);
 const of_save141_diagnostic *d=&cb_stage141_save;
 (void)d;
 if(cb_stage141_ran==8){
 snprintf(line,sizeof line,"Save stage %lu ID %04lX valid %lu",(unsigned long)d->stage,(unsigned long)d->id,(unsigned long)d->id_read);text(8,134,line,3,38);
 snprintf(line,sizeof line,"Probe %ld clean %ld leave %ld",(long)d->probe_rc,(long)d->cleanup_rc,(long)d->leave_rc);text(8,148,line,3,38);
 snprintf(line,sizeof line,"Byte rc %ld restore %ld",(long)d->direct_rc,(long)d->restore_rc);text(8,162,line,3,38);
 snprintf(line,sizeof line,"Byte %02lX > %02lX read %02lX back %02lX",(unsigned long)d->original,(unsigned long)d->target,(unsigned long)d->observed,(unsigned long)d->restored);text(8,176,line,3,38);
 snprintf(line,sizeof line,"Exit %04lX expected %02lX got %02lX",(unsigned long)d->mismatch_address,(unsigned long)d->expected,(unsigned long)d->actual);text(8,190,line,3,38);
 }else{
 snprintf(line,sizeof line,"Read rounds %u/64 rc %d",cb_save_read142.completed,cb_save_read142.rc);text(8,134,line,3,38);
 snprintf(line,sizeof line,"Differences %u",cb_save_read142.differences);text(8,148,line,cb_save_read142.differences?9:7,38);
 if(cb_save_read142.differences){snprintf(line,sizeof line,"First round %u addr %04X",cb_save_read142.first_round,cb_save_read142.first_address);text(8,162,line,9,38);}
 text(8,176,"Bank commands; no data writes",3,38);
 }
 snprintf(line,sizeof line,"Log %s rc %d",cb_stage141_log_phase,cb_stage141_log_rc);text(8,210,line,cb_stage141_log_rc?9:7,38);
 footer("U/D Select  L/R Page  A Run");return;
#endif
#if defined(CB_GBA_HEADER140) && !defined(CB_STAGES141)
 const struct cb_gba_header140 *r=&cb_gba_header140;
 unsigned p=u->detail%4,diff=0;char line[64];
 struct cb_cart parsed={0};
 int complete=!r->rc[p]&&r->blocks[p]==6;
 int valid=complete&&cb_parse_gba(&parsed,r->data[p]);
 for(unsigned i=0;i<192;i++)diff+=r->data[0][i]!=r->data[p][i];
 snprintf(line,sizeof line,"GBA ONLY pass %u/4 rc %d",p+1,r->rc[p]);text(8,48,line,5,38);
 snprintf(line,sizeof line,"Blocks %u/6 safe %d/%d",r->blocks[p],r->before,r->after);text(8,64,line,3,38);
 text(8,80,valid?"Header VALID":complete?"Header INVALID":"Read INCOMPLETE",5,38);
 if(complete&&!r->rc[0]&&r->blocks[0]==6)snprintf(line,sizeof line,"Diff from pass1: %u bytes",diff);
 else snprintf(line,sizeof line,"Diff unavailable");
 text(8,96,line,3,38);
 for(unsigned j=0;j<6;j++){
  unsigned off=0x90+j*8;int n=snprintf(line,sizeof line,"%03X:",off);
  for(unsigned k=0;k<8;k++)n+=snprintf(line+n,sizeof(line)-(unsigned)n,"%02X",r->data[p][off+k]);
  text(8,116+j*14,line,2,38);
 }
 text(8,214,"NO SAVE PROBE / NO CART WRITES",5,38);
 footer("D-pad Pass  A Repeat");return;
#endif
#ifdef CB_CAMERA136
 const struct cb_camera136_report *r=&cb_camera136_report;
 unsigned p=u->detail%3,logo=0,zeros=0,ffs=0;uint8_t sum=0;
 const uint8_t *h=r->bytes[p];char line[64];
 for(unsigned i=0;i<48;i++)logo+=h[i+4]==cb_gb_logo[i];
 for(unsigned i=0;i<96;i++){zeros+=h[i]==0;ffs+=h[i]==255;}
 for(unsigned i=0x34;i<=0x4c;i++)sum=(uint8_t)(sum-h[i]-1);
 snprintf(line,sizeof(line),"GB ONLY read %u/3; done %u",p+1,r->completed);text(8,47,line,5,38);
 snprintf(line,sizeof(line),"rc %d mask %u safe %d/%d",r->rc[p],r->mask[p],r->safe_before,r->safe_after);text(8,61,line,3,38);
 snprintf(line,sizeof(line),"Type %02X logo %u/48 sum %02X/%02X",h[0x47],logo,h[0x4d],sum);text(8,75,line,3,38);
 for(unsigned row=0;row<8;row++){
  unsigned off=row*12;int n=snprintf(line,sizeof(line),"%03X:",0x100+off);
  for(unsigned col=0;col<12;col++)n+=snprintf(line+n,sizeof(line)-(unsigned)n,"%02X",h[off+col]);
  text(8,91+row*12,line,2,38);
 }
 snprintf(line,sizeof(line),"00:%u FF:%u /96",zeros,ffs);text(8,191,line,3,38);
 int complete=r->completed==3&&r->mask[0]==7&&r->mask[1]==7&&r->mask[2]==7;
 text(8,205,!complete?"INCOMPLETE; no header verdict":
  memcmp(r->bytes[0],r->bytes[1],96)||memcmp(r->bytes[0],r->bytes[2],96)?
  "Repeated reads DIFFER":"Repeated reads SAME (not validity)",5,38);
 footer("D-pad Pages  A Repeat  NO WRITES");return;
#endif
 const struct cb_cart *c=&u->cart;char buf[32];
#ifdef CB_SRAM138
 unsigned save_page=u->analysis.nor.region_count>2?1+(u->analysis.nor.region_count+1)/2:2;
 if(c->platform==CB_GBA&&u->detail==save_page){
#ifdef CB_SAVE171
  const struct cb_save171_selection *s=&u->save_selection;
  text(9,29,"Save Type / Evidence",2,37);
  row(0,"ROM title",c->title);
  row(1,"Selected",cb_save171_name(s->type));
  row(2,"Source",save171_source(s->source));
  row(3,"Hardware",cb_save171_name(u->save_detected_type));
  snprintf(buf,sizeof buf,"rc %d",u->save_rc);row(4,"Probe",buf);
  row(5,"ROM NOR",u->analysis.matched?(u->analysis.writable?"Matched / writable":"Matched / read only"):"No match");
  row(6,"Save write",cb_action_enabled(u,5)?"Available / confirmation":"Disabled");
  row(7,"Selection",s->conflict?"Hardware conflict":s->io_blocked?"I/O error":save171_pending(u)?"OS rechecks before write":"No override needed");
  row(8,"Buffer","SDRAM only");row(9,"ROM hint",u->save_rom_hint);
  row(10,"RTC",c->rtc);row(11,"Reset","Clears manual selection");
  footer("Up/Down Pages           B Functions");return;
#endif
  text(9,29,"Parallel Save / Hardware",2,37);
  row(0,"ROM title",c->title[0]?c->title:"No valid ROM header");
  row(1,"Board",u->save_profile==2?"ChisFlash FRAM 128 KiB":u->save_profile==1?"Retail SRAM/FRAM 32 KiB":"Not confirmed / probe");
  row(2,"Protocol",u->save_profile==1?"Parallel / no bank writes":u->save_profile==2?"Unlicensed 1M SRAM":"Not confirmed");
  row(3,"Chip ID","Not available / not proven");
  if(u->save_observed)snprintf(buf,sizeof(buf),"%lu KiB%s",(unsigned long)(u->save_observed>>10),u->save_profile?" (profile)":" (estimate)");
  else strcpy(buf,"Not confirmed");
  row(4,"Save size",buf);
  if(u->save_repeat)snprintf(buf,sizeof(buf),"%lu bytes",(unsigned long)u->save_repeat);else strcpy(buf,"None / not tested");
  row(5,"Repetition",buf);
  row(6,"Read check",u->save_observed&&!u->save_rc?"Two snapshots agree":"Not verified");
  row(7,"Save write",cb_action_enabled(u,5)?"Available / confirmation":"Disabled");row(8,"Buffer","SDRAM only");
  row(9,"ROM hint",u->save_rom_hint[0]?u->save_rom_hint:c->save);
  row(10,"RTC",c->rtc);
  row(11,"Detection","Parallel only / incomplete");
  text(9,207,"Options: select known board if needed",3,38);
  footer("Up/Down Pages           B Functions");return;
 }
#endif
 text(9,29,c->platform==CB_GBA?"GBA Cartridge Info":c->platform==CB_GB?"GB/GBC Cartridge Info":"Cartridge Info",2,37);
 panel(6,43,308,159,1,4);
 if(u->detail && u->analysis.ran){
  const struct cb_analysis *a=&u->analysis;
  if(a->nor.version==OF_NOR_VERSION&&(a->nor.flags&OF_NOR_ID_OK)){
   const of_nor_info_t*n=&a->nor;
   row(0,"NOR model",a->model);row(1,"JEDEC",a->id);
   snprintf(buf,sizeof(buf),"%lu KiB physical",(unsigned long)(n->flash_bytes>>10));row(2,"NOR size",buf);
   snprintf(buf,sizeof(buf),"%lu KiB mapped",(unsigned long)(n->accessible_bytes>>10));row(3,"Write limit",buf);
   row(4,"Profile",a->profile);
   snprintf(buf,sizeof(buf),"%lu bytes",(unsigned long)n->buffer_bytes);row(5,"Buffer",n->buffer_bytes?buf:"Single-unit program");
   unsigned base=(u->detail-1)*2;
   for(unsigned i=0;i<2;i++){
    char label[16];unsigned r=base+i;
    snprintf(label,sizeof(label),"Region %u",r);
    if(r<n->region_count)snprintf(buf,sizeof(buf),"%lu x %lu KiB",(unsigned long)n->regions[r].count,(unsigned long)(n->regions[r].sector_bytes>>10));
    else strcpy(buf,"--");
    row(6+i,label,buf);
   }
   if(base<n->region_count)snprintf(buf,sizeof(buf),"0x%08lX",(unsigned long)n->regions[base].start);else strcpy(buf,"--");
   row(8,"Start",buf);row(9,"Cmd set",n->command_set==1?"AMD":n->command_set==2?"Intel / Sharp":"Unconfirmed");
   row(10,"Write",a->writable?"Test / fresh ID required":"Disabled");
   snprintf(buf,sizeof(buf),"RC %ld / %ld",(long)n->probe_rc,(long)n->plan_rc);row(11,"Probe / Plan",buf);
   text(9,208,a->status,a->writable?7:9,38);
   footer("Up/Down Regions A Analyze B Functions");return;
  }
  row(0,"NOR model",a->model);row(1,"JEDEC",a->id);
  if(a->bytes)snprintf(buf,sizeof(buf),"%lu KiB",(unsigned long)(a->bytes>>10));else strcpy(buf,"Not confirmed");
  row(2,"NOR size",buf);row(3,"Profile",a->profile);
  snprintf(buf,sizeof(buf),"%lu bytes",(unsigned long)a->buffer);row(4,"Buffer",a->buffer?buf:a->matched?"No buffer programming":"Unknown");
  row(5,c->platform==CB_GB&&(a->rc||a->mapper_warning)?"Probe":"Layout",c->platform==CB_GB&&(a->rc||a->mapper_warning)?a->probe:a->pri);
  for(unsigned i=0;i<2;i++){
   if(i<a->region_count)snprintf(buf,sizeof(buf),"%lu x %lu KiB",(unsigned long)a->region[i].count,(unsigned long)(a->region[i].bytes>>10));
   else strcpy(buf,"--");
   row(6+i,i?"Region 1":"Region 0",buf);
  }
  if(a->region_count>1)snprintf(buf,sizeof(buf),"0x%06lX",(unsigned long)a->region[1].start);else strcpy(buf,"--");
  if(c->platform==CB_GB&&(a->rc||!a->matched))row(8,"NOR stage",a->stage);
  else row(8,"R1 start",buf);
  row(9,c->platform==CB_GBA?"Cmd set":"Probe state",c->platform==CB_GBA?(a->matched?"AMD / 0002":"Not confirmed"):a->mapper_warning?"Inconclusive samples":a->mapper_ok?"Sample agrees (not full)":"Header only; unverified");
  row(10,"Write",a->writable?"GBA verified profile":"Disabled");
  snprintf(buf,sizeof(buf),"rc %d",a->rc);row(11,"Analysis",buf);
  if(c->platform==CB_GB){snprintf(buf,sizeof(buf),"%d / NOR %d",a->rc,a->nor_rc);row(11,"Result rc",buf);}
  text(9,208,a->status,a->rc||a->mapper_warning?9:7,38);
  footer("Up/Down Info  A Analyze  B Functions");return;
 }
 if(c->platform==CB_UNKNOWN){
  text(18,63,cb_result_name(c->result),3,34);
  text(18,82,"GB header read diagnostics",5,34);
  snprintf(buf,sizeof(buf),"Read rc %d / safe rc %d",c->rc,c->safe_rc);text(18,115,buf,3,34);
  snprintf(buf,sizeof(buf),"GB rc %d / blocks %u/7",cb_header_diag.rc,cb_header_diag.mask);text(18,130,buf,3,34);
  if(cb_header_diag.mask==7&&!cb_header_diag.rc){
   const uint8_t *h=cb_header_diag.bytes;unsigned logo=0;uint8_t sum=0;
   for(unsigned i=0;i<48;i++)logo+=h[i+4]==cb_gb_logo[i];
   for(unsigned i=0x34;i<=0x4c;i++)sum=(uint8_t)(sum-h[i]-1);
   snprintf(buf,sizeof(buf),"Type %02X / Logo %u/48",h[0x47],logo);text(18,145,buf,3,34);
   snprintf(buf,sizeof(buf),"Sum %02X / calculated %02X",h[0x4d],sum);text(18,160,buf,3,34);
   snprintf(buf,sizeof(buf),"Start %02X %02X %02X %02X",h[0],h[1],h[2],h[3]);text(18,175,buf,3,34);
  }
  text(18,190,"NOR: Not probed; writes disabled",3,34);
 }else if(c->platform==CB_GBA&&c->result!=CB_READY){
  row(0,"ROM title",c->header_valid?c->title:"No valid ROM header");
  row(1,"Platform",c->header_valid?"GBA / header valid":"GBA / user selected");
  row(2,"NOR model",u->analysis.model);
  row(3,"Profile",u->analysis.profile);
  row(4,"Write",u->analysis.writable?"Fresh NOR match":"Disabled");
  if(c->header_valid){
   row(5,"ROM size",c->rom);
   snprintf(buf,sizeof buf,"Read %d / safe %d",c->rc,c->safe_rc);row(6,"Probe rc",buf);
  }
 }else{
  row(0,"ROM title",c->title[0]?c->title:"(untitled)");
  if(c->platform==CB_GBA){
   row(1,"Code/Maker",c->code);
   snprintf(buf,sizeof(buf),"%u",c->revision);row(2,"Revision",buf);
  }else{
   snprintf(buf,sizeof(buf),"%s / %02X",c->mapper,c->type);row(1,"Header MBC",buf);
   snprintf(buf,sizeof(buf),"%u / %s",c->revision,(c->cgb&0x80)?"CGB":"GB");row(2,"Rev/Color",buf);
  }
  row(3,"Logo","Nintendo / valid");
  snprintf(buf,sizeof(buf),"Valid (0x%02X)",c->checksum);row(4,"Header sum",buf);
  if(c->platform==CB_GB)snprintf(buf,sizeof(buf),"0x%04X (stored only)",c->global_checksum);
  else strcpy(buf,"Not computed");
  row(5,"ROM sum",buf);row(6,"ROM size",c->rom);
#ifdef CB_SAVE171
  row(7,"Save",c->platform==CB_GBA&&u->save_selection.source!=CB_SAVE171_UNKNOWN?
   cb_save171_name(u->save_selection.type):c->save);
#else
  row(7,"Save",c->save);
#endif
  row(8,"RTC",c->rtc);
  row(9,"NOR model",u->analysis.ran?u->analysis.model:"Not analyzed");
  if(u->analysis.bytes)snprintf(buf,sizeof(buf),"%lu MiB (physical)",(unsigned long)(u->analysis.bytes>>20));else strcpy(buf,"Unknown");
  row(10,"NOR size",buf);row(11,"Profile",u->analysis.ran?u->analysis.profile:"Analyze cartridge");
 }
 text(9,208,u->analysis.ran?u->analysis.status:c->result==CB_READY?"Header OK / NOR analysis pending":c->header_valid?"Header OK / capacity or cleanup failed":"Header unconfirmed / analysis pending",u->analysis.rc?9:c->result==CB_READY?7:3,38);
 footer("Up/Down NOR  A Analyze    B Functions");
}
static void transfer(const struct cb_ui *u){
 char b[48];text(9,29,"Transfer Status",2,37);
 panel(7,43,306,157,1,4);
 text(15,51,u->task?u->task:"No active task",2,36);
 text(15,67,u->phase?u->phase:"Analyze cartridge first",5,36);
  if(u->confirm==7||u->confirm==8){
   static const char *fields[]={"Day","Hour (24h)","Minute","Second"};
   snprintf(b,sizeof b,"%ud %02u:%02u:%02u",u->rtc_edit[0],u->rtc_edit[1],u->rtc_edit[2],u->rtc_edit[3]);
   text(15,90,b,5,36);
   if(u->confirm==7){
    text(15,112,fields[u->rtc_field%4],7,36);
    text(15,132,"Left/Right field; Up/Down value",2,36);
    text(15,152,u->rtc_edit[4]&0x40?"RTC halted":"RTC running",2,36);
    text(15,177,"A: Review   B: Cancel",2,36);
   }else{
    text(15,116,"CHANGES CARTRIDGE CLOCK",9,36);
    text(15,138,"Save data / ROM unchanged",2,36);
    text(15,162,"A: Write RTC   B: Edit again",2,36);
   }
  }else if(u->confirm==4||u->confirm==5){
   static const char *fields[]={"Year","Month","Day","Hour (24h)","Minute","Second"};
   snprintf(b,sizeof b,"%04u-%02u-%02u  %02u:%02u:%02u",u->rtc_edit[0],u->rtc_edit[1],u->rtc_edit[2],u->rtc_edit[3],u->rtc_edit[4],u->rtc_edit[5]);
   text(15,90,b,5,36);
   if(u->confirm==4){
    text(15,112,fields[u->rtc_field%6],7,36);
    text(15,132,"Left/Right field; Up/Down value",2,36);
    text(15,152,"X: Use Pocket current time",2,36);
    text(15,177,"A: Review   B: Cancel",2,36);
   }else{
    text(15,116,"CHANGES CARTRIDGE CLOCK",9,36);
    text(15,138,"Save data / ROM unchanged",2,36);
    text(15,162,"A: Write RTC   B: Edit again",2,36);
   }
  }else if(u->confirm==2){
   text(15,87,"RTC detected and readable",7,36);
   text(15,105,u->cart.rtc,2,36);
   text(15,125,"Backup SAV only",u->rtc_backup_choice==0?5:3,36);
   text(15,143,"Backup SAV including RTC",u->rtc_backup_choice==1?5:3,36);
   text(15,163,u->cart.platform==CB_GB?"GB RTC adds 48 bytes to SAV":"RTC adds 16 bytes to SAV",3,36);
   text(15,185,"Up/Down: Choose  A: OK  B: Back",2,36);
  }else if(u->confirm==3){
   static const char *choices[]={"Restore SAV only","Restore SAV + original RTC","Restore SAV + elapsed RTC","Cancel restore"};
   text(15,87,"Up/Down: choose RTC handling",2,36);
   for(unsigned i=0;i<4;i++)text(15,105+(int)i*18,choices[i],i==u->rtc_restore_choice?5:3,36);
   text(15,181,"A: Select   B: Cancel all",2,36);
  }else if(u->confirm){
  if(u->transfer_action==5){
   if(u->total<1024)snprintf(b,sizeof(b),"OVERWRITES SAVE: %lu bytes",(unsigned long)u->total);
   else snprintf(b,sizeof(b),"OVERWRITES SAVE: %lu KiB",(unsigned long)(u->total>>10));
   text(15,87,b,9,36);
   text(15,105,u->rtc_restore_pending?"SAV first, then update RTC":"Game ROM and RTC unchanged",2,36);
   text(15,125,"A: Confirm save restore",2,36);
   text(15,143,"Release A before confirming",3,36);
   text(15,163,"B: Cancel save restore",7,36);
  }else{
  if(u->task&&u->task[0]=='W'){
   uint32_t bytes=u->analysis.nor.accessible_bytes;
   if(bytes)snprintf(b,sizeof b,"ERASES ALL NOR: %lu KiB",(unsigned long)(bytes>>10));
   else strcpy(b,"ERASES ENTIRE CARTRIDGE NOR");
  }
  else snprintf(b,sizeof(b),"New backup: %lu KiB",(unsigned long)(u->total>>10));
  text(15,87,b,9,36);
  text(15,105,u->source_title,2,36);
  text(15,125,"Press A again to confirm",2,36);
  text(15,143,"Release first A before confirming",3,36);
  text(15,163,"B: Cancel without starting",7,36);
  }
 }else{
  if(u->cart.platform==CB_GB&&u->total&&u->total<1024)snprintf(b,sizeof(b),"%lu / %lu bytes",(unsigned long)u->done,(unsigned long)u->total);
  else snprintf(b,sizeof(b),"%lu / %lu KiB",(unsigned long)(u->done>>10),(unsigned long)(u->total>>10));
  if(u->total<1024)snprintf(b,sizeof(b),"%lu / %lu checks",(unsigned long)u->done,(unsigned long)u->total);
  text(15,88,"Progress",3,12);text(111,88,b,2,24);
  snprintf(b,sizeof(b),"%lus / %lu KiB/s",(unsigned long)(u->elapsed_ms/1000),(unsigned long)(u->phase_ms?(uint64_t)u->done*1000/u->phase_ms/1024:0));
  text(15,105,"Elapsed",3,12);text(111,105,b,2,24);
  unsigned percent=u->total?(unsigned)((uint64_t)u->done*100/u->total):0;if(percent>100)percent=100;
  panel(15,124,278,10,12,4);box(16,125,(int)(276*percent/100),8,5);
  snprintf(b,sizeof(b),"%u%%",percent);text(15,140,b,5,10);
  if(u->done&&u->phase_ms&&u->total>u->done){
   snprintf(b,sizeof(b),"ETA ~%lus",(unsigned long)((uint64_t)(u->total-u->done)*u->phase_ms/u->done/1000));
   text(111,140,b,3,24);
  }
  if(u->finished){snprintf(b,sizeof(b),"Result: %s  rc %d",u->task_rc?"STOP":"PASS",u->task_rc);text(15,158,b,u->task_rc?9:7,36);}
  else text(15,158,u->busy?"Keep power on; do not remove cart":"No transfer running",3,36);
  if(u->sha256[0]){text(15,171,"File SHA256:",3,12);text(15,181,u->sha256,2,32);text(15,192,u->sha256+32,2,32);}
  else if(u->hash[0]){text(15,174,"MD5:",3,5);text(15,187,u->hash,2,32);}
  else if(u->io_diag[0]){text(15,174,u->io_diag,3,36);text(15,187,u->io_regs,3,36);}
 }
 const char *leaf=strrchr(u->path,'/');
 text(9,205,u->notice?u->notice:u->path[0]?(leaf?leaf+1:u->path):"Saves and GB writes remain disabled",3,38);
 footer(u->busy?"Busy: refresh / removal not allowed":"L/R Pages              B Functions");
}
void cb_render(uint8_t *fb,const struct cb_ui *u){
#ifdef CB_LANG174
 chinese174=!(u->options==1?u->language_draft:u->language);
#endif
 pixels=fb;memset(fb,0,CB_W*CB_H);box(0,0,320,21,14);
 /* Yellow cheese wedge; dark holes use a fixed dark palette entry. */
 {static const uint16_t cheese[12]={0x0030,0x00f0,0x03f0,0x0ff0,0x3df0,0x7df0,0xfff0,0xf9f0,0xf9f0,0xff30,0xff30,0xfff0};
  int top=5*CB_H/240;
  for(int r=0;r<12;r++)for(int col=0;col<12;col++)
   if(cheese[r]&(0x8000u>>col))pixels[(top+r)*CB_W+7+col]=11;
 }
 text(27,7,"ChisKit",2,15);
 /* ASCII version: right align the entire label with an eight-pixel margin. */
 text(CB_W-8-(int)strlen(CB_RELEASE_NAME)*8,7,CB_RELEASE_NAME,3,(unsigned)strlen(CB_RELEASE_NAME));
 if(u->page==CB_FUNCTIONS)functions(u);else if(u->page==CB_INFO)info(u);else transfer(u);
}
int cb_key(struct cb_ui *u,uint32_t key){
 if(u->probing||u->busy)return 0;
#ifdef CB_RTC173
 if(!u->options&&u->page==CB_FUNCTIONS&&(key&OF_BTN_X)){
  if(u->cart.platform==CB_GBA&&u->cart.result==CB_READY)return 7;
#ifdef CB_GB_RTC189
  if(u->cart.platform==CB_GB&&u->cart.result==CB_READY)return 7;
#endif
  u->notice="Analyze GBA cartridge first";return 0;
 }
#endif
 if(u->options){
#ifdef CB_LANG174
  if(u->options==1){
   int has_save=u->cart.platform==CB_GBA;
#ifdef CB_GB_SAVE176
   has_save|=u->cart.platform==CB_GB;
#endif
   if(!has_save&&u->settings_focus==1)u->settings_focus=0;
   if(key&OF_BTN_B){u->options=0;u->language_draft=u->language;u->theme_draft=u->theme;return 0;}
   if(key&(OF_BTN_UP|OF_BTN_DOWN)){
    do{u->settings_focus=(u->settings_focus+((key&OF_BTN_UP)?2:1))%3;}while(!has_save&&u->settings_focus==1);
   }
   else if(u->settings_focus==2&&(key&(OF_BTN_LEFT|OF_BTN_RIGHT)))u->theme_draft^=1;
   else if(!u->settings_focus&&(key&(OF_BTN_LEFT|OF_BTN_RIGHT)))u->language_draft^=1;
   else if(key&OF_BTN_A){
    if(u->settings_focus!=1)return 8;
    if(u->cart.platform==CB_GBA)u->options=2;
#ifdef CB_GB_SAVE176
    if(u->cart.platform==CB_GB){u->gb_choice_draft=u->gb_choice;u->options=2;}
#endif
   }
   return 0;
  }
#endif
#ifdef CB_GB_SAVE176
  if(u->cart.platform==CB_GB){
   if(key&OF_BTN_B){u->options=0;u->gb_choice_draft=u->gb_choice;return 0;}
   if(key&OF_BTN_RIGHT)u->gb_choice_draft=(u->gb_choice_draft+1)%10;
   if(key&OF_BTN_LEFT)u->gb_choice_draft=(u->gb_choice_draft+9)%10;
   if(key&OF_BTN_A){u->gb_choice=u->gb_choice_draft;gb176_selection(u);u->options=0;}
   return 0;
  }
#endif
#ifdef CB_LANG174
  if(u->cart.platform!=CB_GBA){u->options=0;return 0;}
#endif
#ifdef CB_SAVE171
  if(u->cart.platform==CB_GBA){
   if(key&OF_BTN_B){u->options=0;u->save_choice_draft=u->save_choice;return 0;}
   if(key&OF_BTN_RIGHT)u->save_choice_draft=(u->save_choice_draft+1)%11;
   if(key&OF_BTN_LEFT)u->save_choice_draft=(u->save_choice_draft+10)%11;
   if(key&OF_BTN_A){u->save_choice=u->save_choice_draft;cb_save171_update(u);u->options=0;}
   return 0;
  }
#endif
#ifdef CB_SRAM138
  if(key&OF_BTN_RIGHT)u->save_profile=(u->save_profile+1)%3;
  if(key&OF_BTN_LEFT)u->save_profile=(u->save_profile+2)%3;
  if(key&(OF_BTN_LEFT|OF_BTN_RIGHT)){u->save_observed=0;u->save_repeat=0;u->save_rc=0;}
#endif
  if(key&(OF_BTN_B|OF_BTN_A))u->options=0;
  return 0;
 }
 if(key&OF_BTN_B){u->page=CB_FUNCTIONS;u->notice=0;return 0;}
 if(key&OF_BTN_L1){u->page=(enum cb_page)((u->page+2)%3);return 0;}
 if(key&OF_BTN_R1){u->page=(enum cb_page)((u->page+1)%3);return 0;}
 if(u->page==CB_INFO){
  unsigned pages=u->analysis.nor.region_count>2?1+(u->analysis.nor.region_count+1)/2:2;
#ifdef CB_SRAM138
  if(u->cart.platform==CB_GBA)pages++;
#endif
  if(key&OF_BTN_DOWN)u->detail=(u->detail+1)%pages;
  else if(key&OF_BTN_UP)u->detail=(u->detail+pages-1)%pages;
  return(key&OF_BTN_A)?2:0;
 }
 if(u->page!=CB_FUNCTIONS)return 0;
 if(key&OF_BTN_UP)u->focus=(u->focus+6)%8;
 else if(key&OF_BTN_DOWN)u->focus=(u->focus+2)%8;
 else if(key&(OF_BTN_LEFT|OF_BTN_RIGHT))u->focus^=1;
 else if(key&OF_BTN_A){
  u->notice=0;
  if(u->focus<2)return (int)u->focus+1;
  if((u->focus>=2&&u->focus<=5)&&cb_action_enabled(u,u->focus))return (int)u->focus+1;
  if(u->focus==6)u->page=CB_INFO;
  else if(u->focus==7){u->options=1;
#ifdef CB_LANG174
   u->language_draft=u->language;u->theme_draft=u->theme;u->settings_focus=0;
#endif
#ifdef CB_SAVE171
   u->save_choice_draft=u->save_choice;
#endif
  }
  else u->notice="Not validated, or analyze required";
 }
 return 0;
}
