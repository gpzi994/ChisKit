// SPDX-License-Identifier: AGPL-3.0-only
#ifndef CHISBURNER_NOR_EXECUTE_H
#define CHISBURNER_NOR_EXECUTE_H
#include "recipe.h"
enum cb_nor_result { CB_NOR_OK=0,CB_NOR_ARGUMENT=-1,CB_NOR_UNSUPPORTED=-2,
 CB_NOR_CANCELLED=-3,CB_NOR_TIMEOUT=-4,CB_NOR_IO=-5,CB_NOR_STATUS=-6,
 CB_NOR_VERIFY=-7,CB_NOR_UNARMED=-8 };
/* OS-internal only. select() maps a full flash BYTE address to a physical
 * window; it must reject unverified board mappings, never silently truncate.
 * read/write access physical bus BYTE addresses in that selected window.
 * Clock is monotonic modulo 2^32 ms. alive() is the lifecycle gate, distinct
 * from user cancellation. release() must always leave the pins inert. */
struct cb_nor_io {
 void *ctx;
 int (*select)(void*,uint32_t);
 int (*read)(void*,uint32_t,uint16_t*);
 int (*write)(void*,uint32_t,uint16_t);
 uint32_t (*now_ms)(void*);
 void (*yield)(void*);
 bool (*alive)(void*);
 bool (*cancelled)(void*);
 void (*release)(void*);
};
struct cb_nor_job {
 const struct cb_nor_plan *plan;
 struct cb_nor_io io;
 bool authorized; /* Set only by OS after fresh matching identity/geometry. */
 uint16_t last_status;
 uint32_t failed_address;
};
int cb_nor_erase(struct cb_nor_job *job,uint32_t address,uint32_t timeout_ms);
int cb_nor_erase_poll(struct cb_nor_job *job,uint32_t address,uint32_t timeout_ms);
int cb_nor_program(struct cb_nor_job *job,uint32_t address,const uint8_t *data,
                   size_t length,uint32_t timeout_ms);
int cb_nor_program_poll(struct cb_nor_job *job,uint32_t address,const uint8_t *data,
                   size_t length,uint32_t timeout_ms);
int cb_nor_finish(struct cb_nor_job *job);
#endif
