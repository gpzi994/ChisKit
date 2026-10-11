// SPDX-License-Identifier: AGPL-3.0-only
#ifndef CB_SUBMIT131_H
#define CB_SUBMIT131_H
#include "stream131.h"
/* Kernel-only transport. check must validate the existing OS token, lifecycle
 * epoch and operation bounds; descriptor keys do NOT replace that policy.
 * step returns 1 while active, 0 success, negative failure AFTER lease drain.
 * A permanently stuck adapter deliberately remains active, retaining ownership.
 * Initialize the state struct to zero. MMIO callbacks must preserve ordering:
 * a read after SUBMIT must observe its acceptance, not the previous sticky DONE.
 * The kernel adapter must supply fences/queued-write acknowledgment as needed.
 */
struct cb_submit131_io {
 void *ctx;
 uint32_t (*read)(void *,unsigned);
 void (*write)(void *,unsigned,uint32_t);
 int (*check)(void *);
};
struct cb_submit131 {
 struct cb_submit131_io io;
 struct cb_stream131_descriptor descriptor;
 uint32_t started,timeout;
 unsigned state;
 int result;
};
int cb_submit131_begin(struct cb_submit131 *,const struct cb_submit131_io *,
 const struct cb_stream131_descriptor *,uint32_t now,uint32_t timeout);
int cb_submit131_step(struct cb_submit131 *,uint32_t now,int cancel);
#endif
