// SPDX-License-Identifier: AGPL-3.0-only
#ifndef CHISBURNER_NOR_BOARD109_H
#define CHISBURNER_NOR_BOARD109_H
#include "cfi.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/* Exact board profiles, not a generic chip-ID => bank wiring inference.
 * Candidate selection does not authorize bus writes. OS must qualify the
 * selected board route and bind all bank identities to a lifecycle epoch. */
enum cb_board109_kind { CB_BOARD109_NONE, CB_BOARD109_F2A128,
 CB_BOARD109_F2A256, CB_BOARD109_F2AULTRA };
enum cb_board109_bus { CB_BOARD109_ROM=2, CB_BOARD109_CONTROL=3 };
struct cb_board109_write { uint32_t address; uint16_t value; uint8_t bus; };
struct cb_board109_profile {
 enum cb_board109_kind kind;
 uint32_t bytes,window_bytes,reset_stride;
 unsigned banks;
 struct cb_nor_cfi geometry;
};
int cb_board109_profile(enum cb_board109_kind,struct cb_board109_profile*);
int cb_board109_bank_id(enum cb_board109_kind,unsigned,uint8_t[8]);
/* Returns exact ordered raw BYTE-addressed writes, no hardware I/O.
 * Error returns 0 and clears the complete caller buffer. No partial sequence. */
size_t cb_board109_unlock(enum cb_board109_kind,struct cb_board109_write*,size_t);
size_t cb_board109_select(enum cb_board109_kind,unsigned,struct cb_board109_write*,size_t);
int cb_board109_map(enum cb_board109_kind,uint32_t,unsigned*,uint32_t*);
/* Compare an independently decoded single-die x16 CFI map with the exact
 * profile slice exposed at a window. Interleaving doubles byte/sector size,
 * not sector counts. Never infer missing CFI from the profile. */
int cb_board109_match_die_cfi(enum cb_board109_kind,unsigned,const struct cb_nor_cfi*);
/* All physical windows must be sampled twice; caller supplies IDs AFTER
 * returning each window to ID mode. This is a gate component, not proof of
 * independent physical storage (alias/CFI qualification remains required). */
bool cb_board109_ids_match(enum cb_board109_kind,const uint8_t*,const uint8_t*,size_t);
#endif
