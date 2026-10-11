// SPDX-License-Identifier: AGPL-3.0-only
#ifndef CHISBURNER_NOR_PROBE_H
#define CHISBURNER_NOR_PROBE_H
#include "execute.h"
struct cb_nor_probe {
 const chisburner_nor_entry_t *entry;
 struct cb_nor_plan plan;
 enum cb_nor_method method;
 uint8_t id[8];
 uint16_t baseline[16];
 int plan_rc;
 bool swapped, repeated, restored, cfi_valid;
};
struct cb_nor_probe_scratch {uint16_t cfi[512],repeat[512];};
/* No ERASE/PROGRAM commands. io.select(0) must first select a verified board
 * route. Scratch belongs to OS; this function is not reentrant on one bus.
 * Success means stable recognized ID and restored ROM; plan_rc may still
 * block writing. Dual Intel captures both interleaved CFI lanes repeatedly;
 * access beyond the CPU window still requires a verified board adapter. */
int cb_nor_probe_bus(const struct cb_nor_io *io,unsigned unit,
                    struct cb_nor_probe_scratch *scratch,struct cb_nor_probe *out);
#endif
