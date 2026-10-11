/* GB/GBC RAM only, including MBC2 low-nibble RAM. Never GBA or EEPROM. */
#ifndef GB_RESTORE180_H
#define GB_RESTORE180_H
#include "gb_save175.h"
static inline int gb180_plan(const uint8_t h[80],uint32_t bytes,struct gb175_plan *p){
 if(bytes!=512&&bytes!=8192&&bytes!=32768&&bytes!=65536&&bytes!=131072)return GB175_UNSUPPORTED;
 int rc=gb179_plan(1,h,bytes,p);if(rc)return rc;
 return p->mapper==GB178_MBC2||p->mapper==GB177_PLAIN||p->mapper==GB177_MBC1||p->mapper==GB175_MBC3||
  p->mapper==GB175_MBC30||p->mapper==GB175_MBC5?0:GB175_UNSUPPORTED;
}
struct gb180_bus {struct gb175_bus read;int (*write_ram)(void *,uint16_t,const void *,uint32_t);};
static inline int gb_restore_plan(const uint8_t h[80],uint32_t bytes,struct gb175_plan *p){
#if defined(CB_GB_RESTORE185) || CART_GB_RESTORE185
 if(bytes==256){int rc=gb179_plan(1,h,bytes,p);return rc?rc:p->mapper==GB179_MBC7?0:GB175_UNSUPPORTED;}
#endif
 return gb180_plan(h,bytes,p);
}
int gb180_restore(const struct gb180_bus *,const uint8_t [80],const void *original,
 const void *source,uint32_t bytes,void *scratch,uint32_t chunk,struct gb175_report *);
#endif
