/* GB MBC3/MBC30 only. No GBA GPIO or RTC save-footer dependencies. */
#ifndef GB_RTC186_H
#define GB_RTC186_H
#include "gb_save175.h"
struct gb186_report {int32_t rc,detail,cleanup_rc;uint32_t supported,days,hours,minutes,seconds,halt,carry;uint8_t raw[5],reserved[3];};
int gb186_header(const uint8_t h[80]);
int gb186_read(const struct gb175_bus *,const uint8_t h[80],struct gb186_report *);
#endif
