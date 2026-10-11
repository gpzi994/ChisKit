#ifndef CB_ANALYSIS98_H
#define CB_ANALYSIS98_H
#include "model.h"
#include "of_cart.h"
#include "of_nor.h"
struct cb_region { uint32_t start, count, bytes; };
struct cb_analysis {
 int ran, rc, mapper_ok, bank0_alias, matched, writable;
 int header_ok, mapper_warning, nor_rc, legacy_programmer;
 uint32_t bytes, buffer, region_count;
 struct cb_region region[8];
 of_nor_info_t nor;
 char model[28], profile[28], status[39], id[28], pri[20];
 char stage[28], probe[28];
};
struct cb_analysis_ops {
 void *ctx;
 int (*read)(void *, unsigned, unsigned, uint32_t, void *, unsigned);
 int (*safe)(void *);
 int (*flash)(void *, of_cart_flash_caps_t *);
 int (*caps)(void *, of_cart_caps_t *);
};
void cb_analyze(struct cb_analysis *,const struct cb_cart *,const struct cb_analysis_ops *);
int cb_gb_geometry(struct cb_analysis *,const uint8_t *,uint32_t);
int cb_gba_geometry(struct cb_analysis *,const of_cart_flash_caps_t *);
#endif
