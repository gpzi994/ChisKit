#ifndef CB_HEADER_DIAG_H
#define CB_HEADER_DIAG_H
#include <stdint.h>
/* Last GB header read evidence only; never authorizes cartridge operations. */
struct cb_header_diag { uint8_t bytes[96]; unsigned mask; int rc; };
extern struct cb_header_diag cb_header_diag;
/* Bounded trace of the existing header reads, including discarded dummy reads.
 * Capture only: no additional cartridge transactions or PSRAM allocation. */
struct cb_header150_read { unsigned bus, address, length; int rc; uint8_t bytes[32]; };
struct cb_header150_trace { unsigned count; struct cb_header150_read reads[64]; };
extern struct cb_header150_trace cb_header150_trace;
struct cb_size151_trace { unsigned count; struct cb_header150_read last[2]; };
extern struct cb_size151_trace cb_size151_trace;
#endif
