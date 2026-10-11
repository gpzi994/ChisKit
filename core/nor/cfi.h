// SPDX-License-Identifier: AGPL-3.0-only
// Pocket-side bounded decoder, adapted from Mori's GBA CFI geometry flow.
#ifndef CHISBURNER_CFI_H
#define CHISBURNER_CFI_H
#include "mori/chisburner_nor_db.h"
#define CB_NOR_REGIONS 8
struct cb_nor_region { uint32_t start, count, sector_bytes; };
struct cb_nor_cfi {
 uint32_t bytes, buffer_bytes;
 uint16_t primary_id, pri_address;
 uint8_t region_count, lane, boot, pri_major, pri_minor;
 bool pri_valid;
 chisburner_nor_cmdset_t cmdset;
 struct cb_nor_region region[CB_NOR_REGIONS];
};
/* Input is raw x16 CFI words at logical CFI addresses, already corrected for
 * board D0/D1 wiring. No bus writes, profile activation or write permission.
 * A complete 0x200-word capture is required. Output is cleared on failure. */
int cb_nor_decode_cfi(const uint16_t *words, size_t count, struct cb_nor_cfi *out);
/* Strict ordinary-device consistency check, NOT a programmer authorization.
 * Special/dual-die/profile overrides require a separate executor adapter. */
int cb_nor_match_cfi(const chisburner_nor_entry_t *entry, const struct cb_nor_cfi *cfi);
#endif
