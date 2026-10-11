#ifndef CB_UI98_MODEL_H
#define CB_UI98_MODEL_H
#include <stdint.h>
#include <stddef.h>
enum cb_platform { CB_UNKNOWN, CB_GB, CB_GBA };
enum cb_result { CB_READY, CB_EMPTY, CB_UNSTABLE, CB_BAD_HEADER, CB_IO, CB_LOCKED };
struct cb_cart {
 enum cb_platform platform;
 enum cb_result result;
 char title[17], code[16], mapper[24], rom[28], save[28], rtc[28];
 uint8_t type, cgb, revision, checksum;
 uint16_t global_checksum;
 uint32_t rom_bytes, ram_bytes;
 int rc, safe_rc;
 /* Header validity is independent of subsequent capacity/cleanup failures. */
 int header_valid;
#ifdef CB_ROM164
 /* 1=NOR accessible range, 2=header database, 3=32 MiB fallback. */
 unsigned rom_source;
 uint8_t rom_header_sha1[20];
#endif
};
struct cb_probe_ops {
 void *ctx;
 int (*read)(void *, unsigned, uint32_t, void *, unsigned);
 int (*safe)(void *);
 void (*progress)(void *, const char *);
};
int cb_parse_gb(struct cb_cart *, const uint8_t *);
int cb_parse_gba(struct cb_cart *, const uint8_t *);
void cb_probe(struct cb_cart *, const struct cb_probe_ops *);
const char *cb_platform_name(enum cb_platform);
const char *cb_result_name(enum cb_result);
/* Fixed logo bytes used for header validation; no game payload is bundled. */
extern const uint8_t cb_gb_logo[48];
#endif
