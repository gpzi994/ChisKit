/* Hardware evidence classifier. No ROM SDK hint authorizes a backup.
 * Type numbers follow the local FlashGBX AgbSaveTypes reference.
 * Acquisition and cleanup MUST finish before calling this classifier. */
#ifndef CB_SAVE_DETECT139_H
#define CB_SAVE_DETECT139_H
#include <stdint.h>
#include <stddef.h>
enum cb_save139_type {
 CB_SAVE139_UNKNOWN=-1, CB_SAVE139_NONE=0, CB_SAVE139_EEPROM512=1,
 CB_SAVE139_EEPROM8K=2, CB_SAVE139_SRAM32=3, CB_SAVE139_FLASH64=4,
 CB_SAVE139_FLASH128=5, CB_SAVE139_DACS=6, CB_SAVE139_SRAM64=7,
 CB_SAVE139_SRAM128=8, CB_SAVE139_BATTERYLESS=9
};
enum cb_save139_basis { CB_SAVE139_UNCONFIRMED, CB_SAVE139_PARALLEL_PROBE,
 CB_SAVE139_FLASH_ID, CB_SAVE139_SERIAL_PROBE, CB_SAVE139_SPECIAL_SIGNATURE,
 CB_SAVE139_HEADER_MARKER };
struct cb_save139_input {
 int io_rc, restore_rc;
 unsigned stable, cleanup_verified;
 unsigned parallel_writable; /* Test byte changed AND original restored/verified. */
 uint32_t repeating_bytes; /* Observed repetition, not physical chip capacity. */
 unsigned bank0_read, bank1_read;
 unsigned flash_id_valid; /* ID-mode response established; not just raw 0xffff. */
 uint16_t flash_id;
 unsigned parallel_all_ff, banks_equal;
 unsigned eeprom_tested, eeprom_uniform, eeprom_prefix_equal;
 unsigned dacs_signature, batteryless_signature;
 uint32_t batteryless_bytes;
};
struct cb_save139_result {
 enum cb_save139_type type;
 enum cb_save139_basis basis;
 uint32_t bytes;
 uint16_t flash_id;
 int rc;
 unsigned backup_allowed, write_allowed;
};
struct cb_save139_diag {
 uint32_t stage,id,id_read;
 int32_t probe_rc,cleanup_rc,direct_rc,restore_rc;
 uint32_t original,target,observed,restored,mismatch_address,expected,actual;
 int32_t leave_rc;
};
struct cb_save139_result cb_save139_classify(const struct cb_save139_input *in);
/* Upstream content-period estimate, minimum 32 bytes (not a chip ID). */
uint32_t cb_save139_repeat(const uint8_t *data,size_t size);
/* True only after successful parallel/Flash exclusion; no ROM hint gate. */
int cb_save139_needs_eeprom(const struct cb_save139_input *in);
const char *cb_save139_name(enum cb_save139_type type);
/* Returns a declaration only: never authorizes backup or writing by itself.
 * A valid marker can intentionally leave capacity zero (unresolved). */
struct cb_save139_result cb_save139_header_marker(const uint8_t *header,size_t n);
#endif
