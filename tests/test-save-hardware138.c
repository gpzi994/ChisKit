#include "../core/save/hardware138.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint8_t a[131072],b[131072];
int main(void){
 for(unsigned i=0;i<sizeof(a);i++)a[i]=(uint8_t)(i*19+i/257);
 memcpy(b,a,sizeof(a));
 struct cb_save_hw138_evidence r=cb_save_hw138_compare(CB_SAVE_HW138_UNKNOWN,a,b,sizeof(a),1,1);
 assert(r.state==CB_SAVE_HW138_PROFILE_REQUIRED&&!r.backup_bytes);
 r=cb_save_hw138_compare(CB_SAVE_HW138_CHISFLASH128_CONFIRMED,a,b,sizeof(a),1,0);
 assert(r.state==CB_SAVE_HW138_BAD_INPUT&&!r.backup_bytes);
 r=cb_save_hw138_compare(CB_SAVE_HW138_CHISFLASH128_CONFIRMED,a,b,sizeof(a),1,1);
 assert(r.state==CB_SAVE_HW138_STABLE&&r.backup_bytes==131072);
 for(unsigned i=32768;i<sizeof(a);i++)a[i]=a[i%32768];
 memcpy(b,a,sizeof(a));
 r=cb_save_hw138_compare(CB_SAVE_HW138_CHISFLASH128_CONFIRMED,a,b,sizeof(a),1,1);
 assert(r.state==CB_SAVE_HW138_STABLE&&r.backup_bytes==131072&&r.repeating_bytes==32768);
 r=cb_save_hw138_compare(CB_SAVE_HW138_PARALLEL_PROBE,a,b,sizeof(a),1,1);
 assert(r.state==CB_SAVE_HW138_STABLE&&r.backup_bytes==32768);
 for(unsigned i=0;i<sizeof(a);i+=32767){
  b[i]^=1;r=cb_save_hw138_compare(CB_SAVE_HW138_CHISFLASH128_CONFIRMED,a,b,sizeof(a),1,1);
  assert(r.state==CB_SAVE_HW138_UNSTABLE&&r.first_difference==i&&!r.backup_bytes);b[i]^=1;
 }
 r=cb_save_hw138_compare(CB_SAVE_HW138_PARALLEL32_CONFIRMED,a,b,32768,0,0);
 assert(r.state==CB_SAVE_HW138_STABLE&&r.backup_bytes==32768);
 r=cb_save_hw138_compare(CB_SAVE_HW138_PARALLEL32_CONFIRMED,a,a,32768,0,0);
 assert(r.state==CB_SAVE_HW138_BAD_INPUT);
 memset(a,0xff,sizeof(a));memset(b,0xff,sizeof(b));
 r=cb_save_hw138_compare(CB_SAVE_HW138_CHISFLASH128_CONFIRMED,a,b,sizeof(a),1,1);
 assert(r.state==CB_SAVE_HW138_UNIFORM&&r.backup_bytes==131072);
 r=cb_save_hw138_compare(CB_SAVE_HW138_PARALLEL_PROBE,a,b,sizeof(a),1,1);
 assert(r.state==CB_SAVE_HW138_UNIFORM&&!r.backup_bytes);
 puts("PASS hardware138 evidence: unknown profile, bank coverage, full backup despite mirrors, corruption, 32 KiB, alias, uniform");
}
