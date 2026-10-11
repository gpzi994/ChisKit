#include "../core/save/detect139.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 struct cb_save139_input p={.stable=1,.cleanup_verified=1,.bank0_read=1,.bank1_read=1};
 const unsigned sizes[]={32768,65536,131072};
 const int types[]={3,7,8};
 for(unsigned i=0;i<3;i++){
  p.parallel_writable=1;p.repeating_bytes=sizes[i];
  struct cb_save139_result r=cb_save139_classify(&p);
  assert((int)r.type==types[i]&&r.bytes==sizes[i]&&r.backup_allowed&&!r.write_allowed);
 }
 p.repeating_bytes=1;assert(cb_save139_classify(&p).type==CB_SAVE139_UNKNOWN);
 p.parallel_writable=0;p.flash_id_valid=1;
 const unsigned ids[]={0xbfd4,0x1f3d,0xc21c,0x321b,0xc209,0x6213,0xbf4b,0xbf5b,0xbf6d,0xffff};
 for(unsigned i=0;i<10;i++){
  p.flash_id=ids[i];struct cb_save139_result r=cb_save139_classify(&p);
  assert(r.bytes==(i<4?65536:131072)&&!r.backup_allowed&&!r.write_allowed);
 }
 p.flash_id=0x1234;assert(cb_save139_classify(&p).type==CB_SAVE139_UNKNOWN);
 p.flash_id=0xffff;p.banks_equal=1;assert(cb_save139_classify(&p).bytes==65536);
 p.parallel_all_ff=1;assert(cb_save139_classify(&p).bytes==131072);
 p.flash_id_valid=0;p.eeprom_tested=1;
 assert(cb_save139_classify(&p).type==CB_SAVE139_EEPROM512);
 p.eeprom_prefix_equal=1;assert(cb_save139_classify(&p).bytes==8192);
 p.eeprom_uniform=1;assert(cb_save139_classify(&p).type==CB_SAVE139_UNKNOWN);
 p.dacs_signature=1;assert(cb_save139_classify(&p).type==CB_SAVE139_DACS);
 p.batteryless_signature=1;p.batteryless_bytes=131072;
 assert(cb_save139_classify(&p).type==CB_SAVE139_BATTERYLESS);
 assert(!cb_save139_classify(&p).backup_allowed);
 p.restore_rc=-9;assert(cb_save139_classify(&p).rc==-9);
 p.restore_rc=0;p.cleanup_verified=0;assert(cb_save139_classify(&p).type==CB_SAVE139_UNKNOWN);
 assert(cb_save139_classify(0).rc);
 puts("detect139 PASS: SRAM 32/64/128, 10 Flash IDs, EEPROM sizes, special types, restore gate; host classifier only");
}
