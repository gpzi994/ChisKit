#ifndef CHISBURNER_UI98_VERSION_H
#define CHISBURNER_UI98_VERSION_H

#ifdef CB_GB_RTC189
#define CB_RELEASE_NAME "0.01b"
#elif defined(CB_GB_RTC187)
#define CB_RELEASE_NAME "dev.188-rtcui"
#elif defined(CB_GB_RTC186)
#define CB_RELEASE_NAME "dev.186b-gbrtc"
#elif defined(CB_GB_RESTORE185)
#define CB_RELEASE_NAME "dev.185-mbc7"
#elif defined(CB_GB_DIAG182)
#define CB_RELEASE_NAME "dev.184-mbc2"
#elif defined(CB_GB_EMPTY181)
#define CB_RELEASE_NAME "dev.181-gb-empty"
#elif defined(CB_GB_RESTORE180)
#define CB_RELEASE_NAME "dev.180-r2"
#elif defined(CB_GB_SAVE179)
#define CB_RELEASE_NAME "dev.179-mbc7"
#elif defined(CB_GB_SAVE178)
#define CB_RELEASE_NAME "dev.178-mbc2"
#elif defined(CB_GB_SAVE177)
#define CB_RELEASE_NAME "dev.177-gb-ram"
#elif defined(CB_GB_SAVE176)
#define CB_RELEASE_NAME "dev.176-gb-size"
#elif defined(CB_GB_SAVE175)
#define CB_RELEASE_NAME "dev.175-gb-save"
#elif defined(CB_LANG174)
#define CB_RELEASE_NAME "174-cn-r5"
#elif defined(CB_RTC173)
#define CB_RELEASE_NAME "dev.173-rtc-r3"
#elif defined(CB_RTC172)
#define CB_RELEASE_NAME "dev.172-rtc"
#elif defined(CB_SAVE171)
#define CB_RELEASE_NAME "dev.171-r3"
#elif defined(CB_FLASH170)
#define CB_RELEASE_NAME "dev.170-r3"
#elif defined(CB_RTC167)
#define CB_RELEASE_NAME "dev.169-rtc-sav"
#elif defined(CB_FLASH166)
#define CB_RELEASE_NAME "dev.166-f128"
#elif defined(CB_FLASH165)
#define CB_RELEASE_NAME "dev.165-f64-r2"
#elif defined(CB_ROM164)
#define CB_RELEASE_NAME "dev.164-rom"
#elif defined(CB_EEPROM162)
#define CB_RELEASE_NAME "dev.163-size"
#elif defined(CB_SAVE160)
#define CB_RELEASE_NAME "dev.160-r6"
#elif defined(CB_SAVE148)
#define CB_RELEASE_NAME "dev.157-r3"
#elif defined(CB_SAVE147)
#define CB_RELEASE_NAME "dev.147-save"
#elif defined(CB_STAGES141)
#define CB_RELEASE_NAME "diag.146-wait"
#elif defined(CB_GBA_HEADER140)
#define CB_RELEASE_NAME "diag.140-rom"
#elif defined(CB_EEPROM139_DIAG)
#define CB_RELEASE_NAME "ee139-r6"
#elif defined(CB_SRAM138)
#define CB_RELEASE_NAME "dev.138-r2"
#elif defined(CB_SRAM137)
#define CB_RELEASE_NAME "dev.137-r2"
#elif defined(CB_CAMERA136)
#define CB_RELEASE_NAME "diag.136-cam"
#elif defined(CB_TIMERS135)
#define CB_RELEASE_NAME "dev.135-tmr"
#elif defined(CB_GB_IDENTIFY134)
#define CB_RELEASE_NAME "dev.134-gb"
#elif defined(CB_GBA_IDENTIFY133)
#define CB_RELEASE_NAME "smoke.133-gba"
#elif defined(CB_GPU_LEAN132)
#define CB_RELEASE_NAME "smoke.132-lean"
#elif defined(CB_STREAM131)
#define CB_RELEASE_NAME "smoke.131-r8"
#elif defined(CB_NOR130)
#define CB_RELEASE_NAME "smoke.130"
#elif defined(CB_NOR112)
#define CB_RELEASE_NAME "dev.112"
#elif defined(CB_NOR111)
#define CB_RELEASE_NAME "smoke.111"
#elif defined(CB_NOR110)
#define CB_RELEASE_NAME "smoke.110"
#elif defined(CB_NOR109)
#define CB_RELEASE_NAME "smoke.109"
#elif defined(CB_NOR108)
#define CB_RELEASE_NAME "smoke.108"
#else
#define CB_RELEASE_NAME "smoke.107"
#endif
#define CB_BACKUP_PREFIX "cb102"

#endif
