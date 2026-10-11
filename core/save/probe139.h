#ifndef CB_SAVE_PROBE139_H
#define CB_SAVE_PROBE139_H
#include <stdint.h>
/* OS adapter must retain one cartridge lifecycle/owner throughout this call.
 * same_cart must become false after reset, removal, or owner/epoch change.
 * These callbacks access SAVE bytes, never ROM words. */
struct cb_save139_probe_ops {
 void *ctx;
 int (*read)(void *,uint32_t,uint8_t *);
 int (*write)(void *,uint32_t,uint8_t);
 int (*same_cart)(void *);
};
enum cb_save139_probe_state { CB_SAVE139_PROBE_UNKNOWN, CB_SAVE139_PROBE_SRAM_LIKE,
 CB_SAVE139_PROBE_UNCHANGED, CB_SAVE139_PROBE_UNSTABLE,
 CB_SAVE139_PROBE_INCONCLUSIVE };
struct cb_save139_probe_report {
 enum cb_save139_probe_state state;
 int rc,restore_rc;
 unsigned modified_possible,restore_verified;
 uint8_t original,target,observed,restored;
};
/* A temporary data write is involved. No erase, program unlock or Flash ID
 * sequences are sent. Failures preserve the original byte in the report. */
struct cb_save139_probe_report cb_save139_probe_byte(const struct cb_save139_probe_ops *ops);
struct cb_save139_probe_report cb_save139_probe_byte_timed(const struct cb_save139_probe_ops *ops,int (*delay_ms)(void *,unsigned));
struct cb_save139_flash_ops {
 struct cb_save139_probe_ops bus;
 int (*delay_ms)(void *,unsigned);
};
struct cb_save139_flash_report {
 struct cb_save139_probe_report direct;
 int rc,cleanup_rc;
 uint16_t id;
 unsigned id_read,known_id,exit_verified,values_restored;
 uint8_t original[3]; /* 5555, 2aaa, 0000 */
 uint32_t mismatch_address;
 uint8_t mismatch_expected,mismatch_actual;
};
/* Performs direct-write screening first. SRAM-like/unstable/error results do
 * not enter Flash ID. Caller owns cartridge lifecycle and exclusive access. */
struct cb_save139_flash_report cb_save139_probe_flash(const struct cb_save139_flash_ops *ops);
#endif
