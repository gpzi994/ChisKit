#ifndef CB_MBC7_RESTORE185_H
#define CB_MBC7_RESTORE185_H
#include "gb_save175.h"
struct gb185_bus {struct gb175_bus io;uint32_t (*now_ms)(void *);};
int gb185_restore(const struct gb185_bus *,const uint8_t [80],const void *,const void *,uint32_t,struct gb175_report *);
#endif
