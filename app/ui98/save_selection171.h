#ifndef CB_UI_SAVE_SELECTION171_H
#define CB_UI_SAVE_SELECTION171_H
#include "ui.h"
#include "../../core/save/selection171.h"
#include "../gba_db55/db55.h"

/* The ROM164 header digest already identifies the pinned full database.
 * No ROM rescan, save probe, bank write, or PSRAM allocation here. */
static inline struct cb_save171_selection cb_ui_save171_selection(
 const struct cb_ui *u,int manual_type){
 struct cb_save171_input i={0};
 i.manual_type=manual_type;i.hardware_type=-1;
#ifdef CB_ROM164
 if(u&&u->cart.platform==CB_GBA){
  i.analyzed=u->analysis.ran;
  i.header_valid=u->cart.header_valid&&u->cart.result==CB_READY;
  i.cart_rc=u->cart.rc;i.cleanup_rc=u->cart.safe_rc;
  i.nor_completed=u->cart.rom_source!=0&&!u->analysis.rc;
  i.nor_matched=u->analysis.matched;i.nor_writable=u->analysis.writable;
  i.hardware_type=u->save_detected_type;i.hardware_bytes=u->save_detected_bytes;
  i.hardware_rc=u->save_rc;
  const struct db55_entry *entry=db55_lookup(u->cart.rom_header_sha1);
  if(entry){i.database_hit=1;i.database_type=entry->save_type;
            i.database_bytes=entry->save_size;}
 }
#else
 (void)u;
#endif
 return cb_save171_select(&i);
}
#endif
