#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <save/blank171.h>
int main(void){
 static uint8_t snapshot[131072];
 struct cb_save139_input good={.stable=1,.cleanup_verified=1,
  .parallel_writable=1,.bank0_read=1,.bank1_read=1};
 for(unsigned fill=0;fill<=255;fill+=255){
  memset(snapshot,fill,sizeof snapshot);
  assert(cb_save171_blank_sram(&good,snapshot,sizeof snapshot,32768));
  assert(cb_save171_blank_sram(&good,snapshot,sizeof snapshot,131072));
  assert(!cb_save171_blank_sram(&good,snapshot,sizeof snapshot,65536));
  snapshot[131071]^=1;
  assert(!cb_save171_blank_sram(&good,snapshot,sizeof snapshot,32768));
 }
 memset(snapshot,255,sizeof snapshot);
 for(unsigned n=0;n<10;n++){
  struct cb_save139_input p=good;
  switch(n){case 0:p.io_rc=-3;break;case 1:p.restore_rc=-4;break;
   case 2:p.stable=0;break;case 3:p.cleanup_verified=0;break;
   case 4:p.parallel_writable=0;break;case 5:p.bank0_read=0;break;
   case 6:p.bank1_read=0;break;case 7:p.flash_id_valid=1;break;
   case 8:p.dacs_signature=1;break;case 9:p.batteryless_signature=1;break;}
  assert(!cb_save171_blank_sram(&p,snapshot,sizeof snapshot,131072));
 }
 puts("SRAM171 blank evidence PASS: 00/FF, 32/128 KiB, whole capture and protocol exclusions");
}
