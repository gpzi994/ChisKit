#define OF_PC 1
#include <stdio.h>
#include "ui.h"
#include "of_save139.h"
#include "backup_name.h"
#include "save_backup161.h"
#include "../app/gba_dump53/md5.h"
#include <assert.h>
#include <stdio.h>
static struct cb_ui state,*ui=&state;
static uint8_t sram137_header[192],buffer[65536],disk[8192];
static uint32_t began;
static unsigned opens,writes,captures,stable=3,mismatch;
static int fault,identity_fail,cancelled;
static uint32_t now(void){return 123;}
static void paint(void){}
static int safe(void){return fault==1?-21:0;}
static int cart_read(uint32_t a,void*p,uint32_t n,uint32_t t){
 (void)a;(void)t;memset(p,0,n);return fault==2?-22:0;
}
static int parse(struct cb_cart*c,const uint8_t*h){
 (void)h;*c=ui->cart;if(identity_fail)c->code[0]='Z';return 1;
}
static int sram137_cancel(void*p){(void)p;return cancelled;}
static int sram137_restore(void*p){(void)p;return fault==4?-24:0;}
static int capture(of_eeprom139_snapshot*s){
 captures++;s->stable_mask=stable;s->mismatch_mask=mismatch;
 for(unsigned i=0;i<512;i++)s->small[i]=(uint8_t)(i^0x55);
 for(unsigned i=0;i<8192;i++)s->large[i]=(uint8_t)(i^0xaa);
 if(fault==8)s->cleanup_rc=-28;
 return fault==3?-23:0;
}
static int output_open(const char*p,uint32_t n){
 assert(strstr(p,"/Saves/chisburner/common/GBA-TEST-ABCD-")&&strstr(p,".sav"));
 assert(n==ui->save_detected_bytes);opens++;return fault==5?-25:0;
}
static long file_size(const char*p){assert(!strcmp(p,"slot:5"));return fault==9?0:ui->save_detected_bytes;}
static struct {long(*file_size)(const char*);} service={file_size};
#undef OF_SVC
#define OF_SVC (&service)
static int output_write(uint32_t a,const void*p,uint32_t n){assert(a==0);writes++;memcpy(disk,p,n);return fault==6?-26:0;}
static int backup_read(uint32_t a,void*p,uint32_t n){assert(a==0);memcpy(p,disk,n);if(fault==7)((uint8_t*)p)[0]^=1;return fault==10?-30:0;}
#define of_cart_enter_safe_idle safe
#define cb_parse_gba parse
#define of_eeprom139_capture capture
#define of_output_open output_open
#define of_output_write output_write
#include "../app/ui98/eeprom161_transfer.h"
static void reset(unsigned type){
 state=(struct cb_ui){.cart={.platform=CB_GBA,.result=CB_READY,.title="TEST",.code="ABCD"},
  .save_detected_type=(int)type,.save_detected_bytes=type==1?512:8192};
 opens=writes=captures=0;fault=identity_fail=cancelled=0;stable=3;mismatch=0;
}
int main(void){
 for(unsigned type=1;type<=2;type++){
  reset(type);assert(!backup_eeprom161()&&opens==1&&writes==1&&captures==1&&ui->done==ui->save_detected_bytes);
  assert(disk[0]==(type==1?0x55:0xaa)&&strlen(ui->hash)==32);
  reset(type);stable=type==1?1:2;assert(!backup_eeprom161()); /* Other mode need not be stable. */
  reset(type);stable=type==1?2:1;assert(backup_eeprom161()&&!opens);
  reset(type);mismatch=type==1?1:2;assert(backup_eeprom161()&&!opens);
  for(int f=1;f<=10;f++){reset(type);fault=f;assert(backup_eeprom161());if(f<=4||f==8)assert(!opens);}
  reset(type);identity_fail=1;assert(backup_eeprom161()&&!captures&&!opens);
  reset(type);cancelled=1;assert(backup_eeprom161()&&!captures&&!opens);
  reset(type);ui->save_rc=-3;assert(backup_eeprom161()&&!captures);
  reset(type);ui->save_detected_bytes=131072;assert(backup_eeprom161()&&!captures);
 }
 puts("EEPROM backup PASS: both sizes, selected stable mode, identity/cancel/error guards, single named SAV and full file compare (host mocks)");
}
