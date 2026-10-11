#define OF_PC
#include "../app/ui98/ui.h"
#include "of_save139.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static struct cb_ui state,*ui=&state;
static uint8_t buffer[8192],disk[8192];
static unsigned opens,writes,reads,captures,fault,at,size;
static uint32_t began;
static void paint(void){}
static uint32_t now(void){return 123;}
static int confirm(void){return fault!=1;}
static int capture(of_eeprom139_snapshot *p){
 captures++;if(fault==2){
  p->address_bits=14;p->pass=2;p->block=1023;p->stage=4;
  p->first_difference=8703;p->cleanup_rc=0;return -3;
 }
 memset(p,0,sizeof *p);
 p->stable_mask=fault==8?2:fault==9?1:fault==10?0:3;
 p->mismatch_mask=3^p->stable_mask;
 for(unsigned i=0;i<512;i++)p->small[i]=(uint8_t)(i*17);
 for(unsigned i=0;i<8192;i++)p->large[i]=(uint8_t)(i*31+i/256);
 return 0;
}
#define of_eeprom139_capture capture
static int of_output_open(const char *p,uint32_t n){
 opens++;assert(n==((fault==8||opens==2)?8192u:512u));
 assert(strstr(p,(fault==8||opens==2)?"-large.bin":"-small.bin"));
 size=n;return fault==3&&opens==at?-3:0;
}
static long file_size(const char *p){assert(!strcmp(p,"slot:5"));return fault==4&&opens==at?size-1:size;}
static struct {long (*file_size)(const char*);} services={file_size};
#undef OF_SVC
#define OF_SVC (&services)
static int of_output_write(uint32_t off,const void *p,uint32_t n){
 writes++;assert(!off&&n==size);if(fault==5&&opens==at)return -3;
 memcpy(disk,p,n);return 0;
}
static int backup_read(uint32_t off,void *p,uint32_t n){
 reads++;assert(!off&&n==size);if(fault==6&&opens==at)return -3;
 memcpy(p,disk,n);if(fault==7&&opens==at)((uint8_t*)p)[n-1]^=1;return 0;
}
#include "../app/ui98/eeprom139_transfer.h"
int main(void){unsigned tests=0;
 for(fault=0;fault<=7;fault++)for(at=1;at<=2;at++){
  memset(&state,0,sizeof state);opens=writes=reads=captures=0;
  int rc=capture_eeprom139();tests++;
  if(!fault){assert(!rc&&opens==2&&writes==2&&reads==2&&state.done==8704);}
  else{
   assert(rc&&(!state.notice||strcmp(state.notice,"Files verified; compare on PC")));
   if(fault==1)assert(!captures&&!opens);
   else if(fault==2){
    assert(captures==1&&!opens&&!writes&&!reads);
    assert(!strcmp(state.io_diag,"Bits 14 pass 2 block 1023"));
    assert(!strcmp(state.io_regs,"S4 diff 000021ff clean 0"));
    assert(!strcmp(state.notice,"Capture failed; no new files"));
   }
   else assert(opens==at&&state.done==(at==1?0u:512u));
  }
 }
 for(fault=8;fault<=10;fault++){
  memset(&state,0,sizeof state);opens=writes=reads=captures=0;
  int rc=capture_eeprom139();tests++;
  if(fault==10)assert(rc&&!opens&&!writes&&!reads);
  else {assert(!rc&&opens==1&&writes==1&&reads==1);
   assert(state.done==(fault==8?8192u:512u));
   assert(!strcmp(state.notice,"Stable mode saved; compare on PC"));}
 }
 printf("EEPROM export PASS: %u cases, cancel/capture/create/size/write/read/corruption; production helper\n",tests);
}
