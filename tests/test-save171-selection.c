#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../app/ui98/save_selection171.h"
static struct cb_save171_input base(void){
 struct cb_save171_input i={0};
 i.analyzed=i.header_valid=i.nor_completed=1;
 i.hardware_type=-1;i.manual_type=-1;return i;
}
int main(void){
 struct cb_save171_input i=base();struct cb_save171_selection s;
 i.nor_matched=i.nor_writable=1;
 i.database_hit=1;i.database_type=2;i.database_bytes=8192;
 s=cb_save171_select(&i);
 assert(s.type==8&&s.bytes==131072&&s.source==CB_SAVE171_FLASHCART_DEFAULT);
 for(int t=1;t<=5;t++){
  i.hardware_type=t;i.hardware_bytes=cb_save171_bytes(t);
  s=cb_save171_select(&i);assert(s.type==t&&s.source==CB_SAVE171_HARDWARE);
 }
 i.manual_type=8;s=cb_save171_select(&i);assert(s.conflict);
 i.hardware_type=-1;i.manual_type=-1;i.nor_matched=i.nor_writable=0;
 s=cb_save171_select(&i);assert(s.type==2&&s.source==CB_SAVE171_DATABASE);
 i.nor_matched=1;s=cb_save171_select(&i);assert(s.type==-1); /* read-only NOR != retail */
 i.nor_matched=0;i.database_hit=0;s=cb_save171_select(&i);assert(s.type==-1);
 for(int t=0;t<=9;t++){
  i.manual_type=t;s=cb_save171_select(&i);
  assert(s.type==t&&s.source==CB_SAVE171_MANUAL);
  assert(s.supported==((t>=1&&t<=5)||t==8));
 }
 i.manual_type=2;i.hardware_rc=-3;s=cb_save171_select(&i);assert(s.io_blocked);
 i.hardware_rc=0;i.cleanup_rc=-4;s=cb_save171_select(&i);assert(s.io_blocked);
 i.cleanup_rc=0;i.header_valid=0;s=cb_save171_select(&i);assert(s.type==-1);
 assert(cb_save171_select(NULL).type==-1);
 assert(!cb_save171_valid(8,65536)&&!cb_save171_valid(10,0));
 /* Exercise every entry in the actual shipped database, not named games. */
 for(uint32_t n=0;n<db55_entry_count;n++){
  const struct db55_entry *e=&db55_entries[n];
  struct cb_ui u={0};u.cart.platform=CB_GBA;u.cart.result=CB_READY;
  u.cart.header_valid=1;u.cart.rom_source=2;u.analysis.ran=1;
  u.save_detected_type=-1;memcpy(u.cart.rom_header_sha1,e->header_sha1,20);
  s=cb_ui_save171_selection(&u,-1);
  if(cb_save171_valid(e->save_type,e->save_size)){
   assert(s.type==e->save_type&&s.bytes==e->save_size);
   assert(s.source==CB_SAVE171_DATABASE);
  }else assert(s.type==-1);
  /* A fresh UI state cannot retain manual/default authorization. */
  memset(&u,0,sizeof u);assert(cb_ui_save171_selection(&u,-1).type==-1);
 }
 printf("save171 selection PASS: defaults, conflicts, faults, 10 choices, %u database entries\n",db55_entry_count);
}
