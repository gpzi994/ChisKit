// SPDX-License-Identifier: AGPL-3.0-only
// Adapted from Mori's command-cache and geometry routing. See mori/PROVENANCE.md.
#ifndef CHISBURNER_NOR_RECIPE_H
#define CHISBURNER_NOR_RECIPE_H
#include "cfi.h"
enum cb_nor_method {
 CB_NOR_AMD_AAA, CB_NOR_AMD_555, CB_NOR_AMD_A9, CB_NOR_MSP54,
 CB_NOR_MSP55_AAA, CB_NOR_MSP55_5555, CB_NOR_INTEL, CB_NOR_INTEL_DUAL,
 CB_NOR_SHARP, CB_NOR_METHOD_COUNT
};
struct cb_nor_recipe {
 enum cb_nor_method method;
 uint32_t unlock0, unlock1; /* physical BYTE addresses, not word addresses */
 uint16_t aa, bb, id, word, buffer, confirm, erase, sector, reset;
 uint16_t ready_mask, error_mask;
 uint8_t unit, swapped, raw_count;
};
struct cb_nor_plan {
 const chisburner_nor_entry_t *entry;
 struct cb_nor_recipe recipe;
 struct cb_nor_cfi geometry;
 uint32_t flags, window_bytes, accessible_bytes;
 uint32_t reset_stride; /* Explicit qualified board override, 0 = family default. */
 bool skip_block_unlock; /* J3 Flash2Advance boards use direct 20/D0 erase. */
};
/* Pure planning only. No plan, database match or method implies hardware
 * authorization. OS must bind fresh repeated ID/CFI and board routing to a job.
 * A mapper adapter must support the whole requested address range. */
int cb_nor_recipe_make(enum cb_nor_method method, unsigned unit, bool swapped,
                      struct cb_nor_recipe *out);
bool cb_nor_method_matches(const chisburner_nor_entry_t *entry,
                           enum cb_nor_method method);
int cb_nor_plan_make(const chisburner_nor_entry_t *entry,
                     enum cb_nor_method method, bool swapped,
                     const struct cb_nor_cfi *cfi, struct cb_nor_plan *out);
int cb_nor_sector_at(const struct cb_nor_plan *plan,uint32_t address,
                     uint32_t *start,uint32_t *length);
size_t cb_nor_chunk(const struct cb_nor_plan *plan,uint32_t address,size_t requested);
uint16_t cb_nor_count(const struct cb_nor_recipe *recipe,unsigned units);
#endif
