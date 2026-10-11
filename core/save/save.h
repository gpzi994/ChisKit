#ifndef CB_SAVE_H
#define CB_SAVE_H
#include <stddef.h>
#include <stdint.h>

/* ROM evidence is NOT a hardware capability or write authorization. */
enum cb_save_kind { CB_SAVE_UNKNOWN, CB_SAVE_SRAM, CB_SAVE_FLASH64,
 CB_SAVE_FLASH128, CB_SAVE_EEPROM, CB_SAVE_CONFLICT };
struct cb_save_scan { uint8_t tail[13]; unsigned used, matches; };
struct cb_save_hint { enum cb_save_kind kind; uint32_t bytes; };
void cb_save_scan_init(struct cb_save_scan *);
void cb_save_scan_feed(struct cb_save_scan *, const void *, size_t);
struct cb_save_hint cb_save_scan_result(const struct cb_save_scan *);

enum cb_save_rc { CB_SAVE_OK=0, CB_SAVE_ARGUMENT=-1, CB_SAVE_UNCONFIRMED=-2,
 CB_SAVE_READ=-3, CB_SAVE_CHANGED=-4, CB_SAVE_OUTPUT=-5,
 CB_SAVE_VERIFY=-6, CB_SAVE_RESTORE=-7, CB_SAVE_CANCELLED=-8 };
#define CB_SAVE_MAX_BYTES 131072u
/* Hardware adapter confirms protocol and size independently of ROM hints.
 * It must enforce address limits, mapper/bank selection and timeouts.
 * All buffers supplied here MUST reside in SDRAM on Pocket, never PSRAM.
 * Output adapter: create NEW file only, exact writes/reads, durable finish.
 * Failed create_new must release any partially acquired output handle itself.
 * Successful prepare must freshly check cartridge identity/capabilities;
 * restore must be safe even when prepare failed midway.
 * Incomplete output must remain identifiable; abort must not delete old files.
 */
struct cb_save_ops {
 void *ctx;
 int (*prepare)(void *, enum cb_save_kind, uint32_t);
 int (*read)(void *, uint32_t, void *, uint32_t);
 int (*restore)(void *);
 int (*create_new)(void *, uint32_t);
 int (*write)(void *, uint32_t, const void *, uint32_t);
 int (*finish)(void *);
 int (*read_output)(void *, uint32_t, void *, uint32_t);
 void (*abort_output)(void *);
 int (*cancelled)(void *);
};
struct cb_save_report { int rc, detail, restore_rc; uint32_t compared, written; };
/* Two complete cartridge reads must agree BEFORE output creation.
 * Caller supplies snapshot[bytes], scratch[chunk], disjoint SDRAM buffers.
 * No save-memory writes or RTC changes are exposed by this interface. */
int cb_save_backup(const struct cb_save_ops *, enum cb_save_kind, uint32_t,
 void *snapshot, size_t snapshot_size, void *scratch, uint32_t chunk,
 struct cb_save_report *);
#endif
