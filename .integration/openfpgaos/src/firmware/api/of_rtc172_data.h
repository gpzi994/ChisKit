#ifndef OF_RTC172_DATA_H
#define OF_RTC172_DATA_H
#include "of_rtc167_data.h"
typedef struct {
 uint32_t version,stage,attempted,verified;
 int32_t rc,cleanup_rc;
 of_rtc167_report before,after;
} of_rtc172_report;
#endif
