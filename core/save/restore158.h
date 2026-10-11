/* OS-owned SRAM restore after save_auto139 has proved writable SRAM in one
 * cartridge lifecycle epoch. No file I/O, PSRAM, NOR, Flash or EEPROM access. */
#ifndef CB_SAVE_RESTORE158_H
#define CB_SAVE_RESTORE158_H
#include <stdint.h>
#include <save/detect139.h>

struct cb_save158_ops {
 void *ctx;
 int (*alive)(void *ctx);
 int (*bank)(void *ctx,unsigned bank);
 int (*read)(void *ctx,uint32_t address,uint8_t *value);
 int (*write)(void *ctx,uint32_t address,uint8_t value);
};
struct cb_save158_report {
 int rc,restore_rc,bank0_rc;
 uint32_t attempted,changed,verified;
 unsigned restore_attempted,restore_verified;
};
int cb_save158_restore(const struct cb_save139_result *detected,
 const uint8_t header[192],const uint8_t expected_id[16],
 const uint8_t *original,const uint8_t *disk_backup,
 const uint8_t *source,uint32_t bytes,const struct cb_save158_ops *ops,
 struct cb_save158_report *report);
#endif
