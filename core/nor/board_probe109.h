// SPDX-License-Identifier: AGPL-3.0-only
#ifndef CB_BOARD_PROBE109_H
#define CB_BOARD_PROBE109_H
#include "board109.h"
#include "execute.h"
struct cb_board109_io {
 void*ctx;
 int (*write)(void*,unsigned bus,uint32_t byte_address,uint16_t raw_value);
 int (*read)(void*,uint32_t byte_address,uint16_t*);
 bool (*alive)(void*);
 uint32_t (*epoch)(void*);
 uint32_t (*now_ms)(void*);
 void (*yield)(void*);
 void (*release)(void*);
};
struct cb_board109_capture {
 struct cb_board109_profile profile;
 uint32_t epoch;
 uint8_t id[8][8];
 uint16_t normal[8][16];
 bool ids_valid,restored;
 struct cb_nor_cfi cfi[8];
 bool geometry_valid;
 int geometry_rc;
};
struct cb_board109_scratch {uint16_t first[512],repeat[512];};
/* Candidate must originate from a repeated dual-direct ID probe. Exact seed
 * match is checked before ANY board-control write. Only flash mode and board
 * latch commands are sent. Success qualifies repeated IDs and restoration,
 * NOT CFI geometry, storage non-aliasing or erase/program authorization. */
int cb_board109_capture_ids(const struct cb_board109_io*,enum cb_board109_kind,
 const uint8_t repeated_seed[8],struct cb_board109_capture*);
int cb_board109_capture_full(const struct cb_board109_io*,enum cb_board109_kind,
 const uint8_t repeated_seed[8],struct cb_board109_scratch*,struct cb_board109_capture*);
#endif
