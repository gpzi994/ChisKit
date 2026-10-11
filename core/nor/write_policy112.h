// SPDX-License-Identifier: AGPL-3.0-only
#ifndef CHISBURNER_WRITE_POLICY112_H
#define CHISBURNER_WRITE_POLICY112_H
#include <stdbool.h>
#include <stdint.h>
#define CB_ROM_CACHE112_BYTES UINT32_C(0x02000000)
/* Whole-device scope, not an address mask. A small image on a larger GBA
 * NOR must not bypass this gate. Existing GB mapper limits stay unchanged. */
static inline bool cb_nor_write_capacity112(unsigned bus, uint32_t flash_bytes) {
 return flash_bytes && (bus==1 || (bus==2 && flash_bytes<=CB_ROM_CACHE112_BYTES));
}
#endif
