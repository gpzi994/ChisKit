#ifndef CB_SAVE_EEPROM139_H
#define CB_SAVE_EEPROM139_H
#include <stddef.h>
#include <stdint.h>
enum cb_eeprom139_state { CB_EEPROM139_UNKNOWN,CB_EEPROM139_512,
 CB_EEPROM139_8K,CB_EEPROM139_UNSTABLE,CB_EEPROM139_BLANK };
struct cb_eeprom139_ops {
 void *ctx;
 /* Whole read transaction: 11 + 6/14 address bits + stop 0, direction
  * turnaround, four dummy clocks then 64 bits MSB-first. No write opcode.
  * Adapter must release CS on success/error and maintain exclusive ownership. */
 int (*read_block)(void *,unsigned address_bits,unsigned block,uint8_t out[8]);
 int (*same_cart)(void *);
 int (*release)(void *);
};
struct cb_eeprom139_report {
 enum cb_eeprom139_state state;
 int rc,cleanup_rc;
 uint32_t bytes_read,first_difference;
 unsigned prefix_equal;
 /* Last attempted transaction; pass is one-based. Stage: 1=pre-epoch,
  * 2=transport, 3=post-epoch, 4=repeat mismatch, 5=release-only failure. */
 unsigned address_bits,pass,block,stage;
 unsigned stable_mask,mismatch_mask;
 uint32_t mode_difference[2];
};
/* Scratch supplied by OS in SDRAM: 512-byte small result, 8192-byte large.
 * Contents retained for diagnosis. Capacity result is a content heuristic. */
struct cb_eeprom139_report cb_eeprom139_probe(const struct cb_eeprom139_ops *,
 uint8_t *small,size_t small_n,uint8_t *large,size_t large_n);
/* Diagnostic only: continue after repeat mismatch, never after transport or
 * epoch failure. Per-mode flags do not authorize automatic classification. */
struct cb_eeprom139_report cb_eeprom139_capture(const struct cb_eeprom139_ops *,
 uint8_t *small,size_t small_n,uint8_t *large,size_t large_n);
/* FlashGBX detection: one full read per address mode. No repeat gate. */
struct cb_eeprom139_report cb_eeprom139_detect(const struct cb_eeprom139_ops *,
 uint8_t *small,size_t small_n,uint8_t *large,size_t large_n);
/* Read-content estimate only; never authorizes writes or backup. */
enum cb_eeprom139_state cb_eeprom139_estimate(const struct cb_eeprom139_report *,
 const uint8_t *small,const uint8_t *large);
#endif
