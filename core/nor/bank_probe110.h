// SPDX-License-Identifier: AGPL-3.0-only
#ifndef CB_BANK_PROBE110_H
#define CB_BANK_PROBE110_H
#include "bank110.h"
#include "probe.h"
struct cb_bank110_capture {
 enum cb_bank110_kind kind;
 uint32_t bytes,epoch,reset_stride;
 unsigned banks;
 uint8_t id[8][8];
 uint16_t normal[8][16];
 bool qualified,restored;
};
struct cb_bank110_io {
 void *ctx;
 int (*control)(void *,uint32_t,uint8_t);
 /* Run full repeated ID/CFI probe in CURRENT selected window, restoring ROM.
  * Its select(0) adapter MUST NOT change the selected physical bank. */
 int (*probe)(void *,struct cb_nor_probe *);
 bool (*alive)(void *);
 uint32_t (*epoch)(void *);
 uint32_t (*now_ms)(void *);
 void (*yield)(void *);
 void (*release)(void *);
};
/* Qualification of explicit source-backed candidates only. Equal IDs do not
 * prove independent physical storage; hardware alias tests remain necessary.
 * No erase/program commands. This result alone never grants a write token. */
int cb_bank110_capture(const struct cb_bank110_io *,const struct cb_nor_probe *,
 struct cb_nor_probe *scratch,struct cb_bank110_capture *);
#endif
