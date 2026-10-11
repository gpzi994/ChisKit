#ifndef OF_RTC167_DATA_H
#define OF_RTC167_DATA_H
#include <stdint.h>
#define OF_RTC167_GPIO 1u
#define OF_RTC167_TIME_OK 2u
#define OF_RTC167_POWER_FLAG 4u
typedef struct {
 uint32_t version,flags,stage;
 int32_t rc,cleanup_rc;
 uint8_t date[7],status;
 uint16_t rom[3],mapped[3];
} of_rtc167_report;
#endif
