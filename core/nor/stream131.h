// SPDX-License-Identifier: AGPL-3.0-only
#ifndef CHISBURNER_STREAM131_H
#define CHISBURNER_STREAM131_H
#include "recipe.h"
/* OS-private descriptor. This is NOT a write authorization or public API.
 * Word order is fixed; all unused words must remain zero. */
enum cb_stream131_word {
 CB_S131_FLAGS, CB_S131_SOURCE, CB_S131_PHYSICAL, CB_S131_LENGTH,
 CB_S131_BUFFER, CB_S131_UNLOCK0, CB_S131_UNLOCK1, CB_S131_AA_BB,
 CB_S131_BUFFER_CONFIRM, CB_S131_RESET_WORD, CB_S131_TIMEOUT,
 CB_S131_WORDS=16
};
struct cb_stream131_descriptor { uint32_t words[CB_S131_WORDS]; };
/* GB byte-program batches retain ownership, but still poll every byte.
 * Bound each batch by sector, mapper window, and 256 bytes. */
size_t cb_stream131_chunk(const struct cb_nor_plan *plan,uint32_t address,size_t maximum);
/* Development-only private register indices, not CPU byte addresses.
 * Write every descriptor word (including zero reserved words), then KEY1,
 * KEY2, SUBMIT. Every submit attempt consumes arm and completeness.
 * Configuration writes revoke arm; busy writes are rejected without mutation.
 * Parent OS integration must still enforce fresh identification/job authority.
 */
enum cb_stream131_register {
 CB_S131_REG_INDEX=20, CB_S131_REG_DATA=21, CB_S131_REG_CONTROL=22,
 CB_S131_REG_STATUS=23, CB_S131_REG_KEY=24, CB_S131_REG_LEASE=25
};
/* LEASE: write 1 requests both PSRAM ports, write 0 releases after drain.
 * Read bits 0=requested, 1=handshake busy, 2=granted.
 * A lease is NOT fresh-NOR identification or write authorization. */
#define CB_S131_KEY1 UINT32_C(0x43425331)
#define CB_S131_KEY2 UINT32_C(0x43425332)
#define CB_S131_SUBMIT UINT32_C(1)
#define CB_S131_CANCEL UINT32_C(2)
#define CB_S131_STATUS_BUSY UINT32_C(1)
#define CB_S131_STATUS_DONE UINT32_C(2)
#define CB_S131_STATUS_ARMED UINT32_C(4)
#define CB_S131_STATUS_REJECTED UINT32_C(8)
/* Status error is bits 7:4; KEY reads return the 16-bit written-word mask.
 * Frontend failures: 1 malformed/incomplete, 3 unauthorized, 8 adapters busy.
 * Once accepted, error is the program engine result (same numeric namespace).
 */
/* Pure conversion only: no register access, no fresh-ID checks, no pin writes.
 * Caller must first validate the job token and select the verified bank route.
 * Returns -1 invalid extent/arguments, -2 unsupported hardware mode/capacity. */
int cb_stream131_build(const struct cb_nor_plan *plan,uint32_t source,
 uint32_t address,uint32_t length,uint32_t timeout_cycles,
 struct cb_stream131_descriptor *out);
#endif
