#ifndef CB_CAPACITY164_H
#define CB_CAPACITY164_H
#include "analysis.h"
struct cb_capacity164_ops {
 struct cb_probe_ops rom;
 int (*nor)(void *,of_nor_info_t *);
};
/* No save detection, erase, programming, or PSRAM access. */
int cb_gba_analyze164(struct cb_cart *,struct cb_analysis *,const struct cb_capacity164_ops *);
#endif
