/* Hardware evidence is distinct from ROM SDK hints and write permission. */
#ifndef CB_SAVE_HARDWARE138_H
#define CB_SAVE_HARDWARE138_H
#include <stddef.h>
#include <stdint.h>
enum cb_save_hw138_profile {
 CB_SAVE_HW138_UNKNOWN,
 CB_SAVE_HW138_PARALLEL32_CONFIRMED,
 CB_SAVE_HW138_CHISFLASH128_CONFIRMED,
 /* Generic 1M bank protocol snapshot, not a confirmed physical chip. */
 CB_SAVE_HW138_PARALLEL_PROBE
};
enum cb_save_hw138_state {
 CB_SAVE_HW138_BAD_INPUT,
 CB_SAVE_HW138_PROFILE_REQUIRED,
 CB_SAVE_HW138_UNSTABLE,
 CB_SAVE_HW138_UNIFORM,
 CB_SAVE_HW138_STABLE
};
struct cb_save_hw138_evidence {
 enum cb_save_hw138_state state;
 uint32_t captured_bytes;
 uint32_t backup_bytes;
 uint32_t first_difference;
 uint32_t repeating_bytes; /* Data repetition, NEVER physical capacity proof. */
 unsigned bank0_read,bank1_read;
};
/* Input snapshots must be independently acquired using the specified profile.
 * ChisFlash: explicit bank0 then bank1, 64 KiB each, restore bank0 afterwards.
 * profile must come from separately confirmed board/protocol, not ROM or NOR ID.
 * Uniform data remains inconclusive, but confirmed profiles retain full backup
 * length. Generic probe repetition is an estimate, never a chip ID/size proof.
 * No cartridge writes are authorized here. */
struct cb_save_hw138_evidence cb_save_hw138_compare(
 enum cb_save_hw138_profile profile,const uint8_t *a,const uint8_t *b,size_t n,
 unsigned bank0_read,unsigned bank1_read);
#endif
