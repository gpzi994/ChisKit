// SPDX-License-Identifier: AGPL-3.0-only
#ifndef CB_MBCX111_H
#define CB_MBCX111_H
#include <stdint.h>
#include <stddef.h>
#define CB_MBCX111_MAX_BYTES UINT32_C(0x2000000)
#define CB_MBCX111_WINDOW_BYTES UINT32_C(0x800000)
struct cb_mbcx111_write {uint16_t address;uint8_t value;uint16_t delay_ms;};
/* Pure planner, not board identification or write authorization. Adapted from
 * FlashGBX Mapper.py DMG_Unlicensed_MBCX (local upstream v5.1).
 * Includes the ROM-write bank-switch separator. Caller must separately prove
 * the board protocol, run the delays, revoke on partial failure, and restore
 * known state. Do NOT send its A000 access through a generic SRAM writer. */
size_t cb_mbcx111_select(uint32_t bytes,uint32_t address,
                        struct cb_mbcx111_write out[7]);
#endif
