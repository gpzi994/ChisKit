#ifndef CB_SAVE_BLANK171_H
#define CB_SAVE_BLANK171_H
#include "detect139.h"
/* Explicit SRAM profile only. No result is inferred from uniform data alone:
 * the fresh probe must have proved writable parallel memory AND restoration. */
static inline int cb_save171_blank_sram(const struct cb_save139_input *p,
 const uint8_t *snapshot,uint32_t captured,uint32_t selected_bytes){
 if(!p||!snapshot||captured!=131072u||
    (selected_bytes!=32768u&&selected_bytes!=131072u)||
    p->io_rc||p->restore_rc||!p->stable||!p->cleanup_verified||
    !p->parallel_writable||!p->bank0_read||!p->bank1_read||p->flash_id_valid||
    p->dacs_signature||p->batteryless_signature)return 0;
 uint8_t fill=snapshot[0];if(fill!=0&&fill!=255)return 0;
 for(uint32_t i=1;i<captured;i++)if(snapshot[i]!=fill)return 0;
 return 1;
}
#endif
