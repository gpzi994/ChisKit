// SPDX-License-Identifier: AGPL-3.0-only
#ifndef KERNEL_NOR108_H
#define KERNEL_NOR108_H
#include <stdint.h>
#include <stddef.h>
int nor108_probe(uint32_t bus,void*out,uint32_t out_size);
int nor108_begin(uint32_t generation,uint32_t bytes,uint32_t*token);
int nor108_erase(uint32_t token,uint32_t address,uint32_t*length);
int nor108_program(uint32_t token,uint32_t address,const void*data,uint32_t length);
int nor108_finish(uint32_t token);
int nor108_verify_read(uint32_t token,uint32_t address,void*dst,uint32_t length);
int nor130_begin_full(uint32_t generation,uint32_t program_bytes,uint32_t erase_bytes,uint32_t*token);
int nor130_erase_poll(uint32_t token,uint32_t address,uint32_t*length);
int nor130_program_poll(uint32_t token,uint32_t address,const void*data,uint32_t length);
int nor130_verify_full_read(uint32_t token,uint32_t address,void*dst,uint32_t length);
/* Kernel-only one-chunk PSRAM programming. 1=pending, 0=success, <0=failure.
 * Once start returns 1, keep calling step (cancel=1 to abort) until drained.
 * A stuck physical adapter intentionally retains the operation lock. */
int nor131_program_start(uint32_t token,uint32_t source,uint32_t address,uint32_t length);
int nor131_program_step(uint32_t token,int cancel);
int nor131_program_next(uint32_t token,uint32_t address,uint32_t maximum,uint32_t *length);
int nor131_program_pending(void);
int nor131_compare_start(uint32_t token,uint32_t address,uint32_t length);
int nor131_compare_result(uint32_t token,uint32_t *out);
/* Legacy cartridge calls revoke active jobs. Reject reentry during NOR IO. */
int nor108_legacy_touch(void);
int sram137_read(uint32_t address,void *dst,uint32_t length,uint32_t timeout_us);
int flash165_read(uint32_t address,void *dst,uint32_t length,uint32_t timeout_us);
int flash166_read(uint32_t address,void *dst,uint32_t length,uint32_t timeout_us);
int flash170_restore_service(const void *,void *,uint32_t,const uint8_t [16],void *,uint32_t);
int save171_restore_service(uint32_t,const void *,void *,uint32_t,const uint8_t [16],void *,uint32_t);
int rtc167_read(uint32_t address,void *dst,uint32_t length,uint32_t timeout_us);
int rtc172_write_service(const void *,const uint8_t [16],void *,uint32_t);
int gb175_capture_service(const uint8_t [80],void *,uint32_t,void *,uint32_t);
int gb176_capture_service(const uint8_t [80],uint32_t,void *,uint32_t,void *,uint32_t);
int gb177_capture_service(const uint8_t [80],uint32_t,void *,uint32_t,void *,uint32_t);
int gb178_capture_service(const uint8_t [80],uint32_t,void *,uint32_t,void *,uint32_t);
int gb179_capture_service(const uint8_t [80],uint32_t,void *,uint32_t,void *,uint32_t);
int gb180_restore_service(const uint8_t [80],const void *,const void *,uint32_t,void *,uint32_t);
int gb186_read_service(const uint8_t [80],void *,uint32_t);
int gb189_write_service(const uint8_t [80],const uint8_t [5],void *,uint32_t);
int sram138_read(uint32_t address,void *dst,uint32_t length,uint32_t timeout_us);
struct cb_save139_flash_report;
int save139_probe_kernel(struct cb_save139_flash_report *out);
/* Kernel-owned eight-byte output only. A drain timeout retains the lock. */
int eeprom139_read_block_kernel(unsigned address_bits,unsigned block,uint8_t *out);
struct cb_eeprom139_report;
struct cb_save139_result;
/* Detection only; kernel-owned SDRAM scratch, no save backup/write grant. */
struct cb_save139_result eeprom139_detect_kernel(void);
int eeprom139_detect_service(void *out,uint32_t size);
int eeprom139_capture_service(void *out,uint32_t size);
#if CART_EEPROM162
int eeprom162_restore_service(const void *source,void *disk_backup,uint32_t bytes,
 const uint8_t expected_id[16],void *report,uint32_t report_size);
#endif
#if CART_SAVE158
struct cb_save158_report;
/* OS-private until backup orchestration and the app confirmation gate exist. */
int sram158_restore_kernel(const void *source,const void *disk_backup,
 uint32_t bytes,const uint8_t expected_id[16],struct cb_save158_report *report);
/* App-visible entry is linked only into explicit CART_SAVE158 builds. The
 * kernel independently reads the newly created output file from slot 5. */
int sram160_restore_service(const void *source,void *disk_backup,uint32_t bytes,
 const uint8_t expected_id[16],void *report,uint32_t report_size);
#endif
/* Scratch must be kernel-owned SDRAM, non-overlapping, 512 and 8192 bytes. */
struct cb_eeprom139_report eeprom139_probe_kernel(uint8_t *small,size_t small_n,
 uint8_t *large,size_t large_n);
#endif
