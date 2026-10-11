#ifndef GB_RTC189_H
#define GB_RTC189_H
#include "rtc186.h"
struct gb189_value {uint8_t raw[5];uint64_t timestamp;};
int gb189_parse(const uint8_t tail[48],struct gb189_value *);
int gb189_advance(struct gb189_value *,uint64_t now);
struct gb189_bus {struct gb175_bus read;int (*write_rtc)(void *,uint8_t);};
struct gb189_report {int32_t rc,detail,cleanup_rc;uint32_t written,verified;uint8_t raw[5],reserved[3];};
int gb189_write(const struct gb189_bus *,const uint8_t header[80],const uint8_t raw[5],struct gb189_report *);
#endif
