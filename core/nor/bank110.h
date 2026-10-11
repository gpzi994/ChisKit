// SPDX-License-Identifier: AGPL-3.0-only
#ifndef CHISBURNER_NOR_BANK110_H
#define CHISBURNER_NOR_BANK110_H
#include <stddef.h>
#include <stdint.h>
/* Pure command planner. A chip's capacity is NOT evidence of board wiring.
 * No hardware authorization: callers must qualify the route independently. */
enum cb_bank110_kind { CB_BANK110_NONE, CB_BANK110_AMD_BASIC, CB_BANK110_TYPE1 };
#define CB_BANK110_WINDOW UINT32_C(0x02000000)
#define CB_BANK110_LIMIT UINT32_C(0x10000000)
struct cb_bank110_write { uint32_t address; uint8_t value, bus; };
int cb_bank110_map(enum cb_bank110_kind,uint32_t bytes,uint32_t address,
                   unsigned *bank,uint32_t *local);
/* Output is always cleared, including errors. Commands are raw control-bus
 * bytes, never subject to NOR D0/D1 swapping. Exact sequence, no partial output. */
size_t cb_bank110_select(enum cb_bank110_kind,uint32_t bytes,unsigned bank,
                         struct cb_bank110_write out[3]);
#endif
