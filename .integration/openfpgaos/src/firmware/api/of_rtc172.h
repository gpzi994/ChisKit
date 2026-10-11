#ifndef OF_RTC172_H
#define OF_RTC172_H
#include "of_cart.h"
#include "of_rtc172_data.h"
/* target = seven BCD date bytes plus status, never the backup timestamp. */
static inline int of_rtc172_write(const uint8_t target[8],const uint8_t id[16],of_rtc172_report *r){
 if(!target||!id||!r)return OF_ERR_INVALID_PARAM;
 *r=(of_rtc172_report){0};
#ifndef OF_PC
 if(!OF_CART_SVC_HAS(rtc172_write))return OF_ERR_NOT_SUPPORTED;
 return OF_SVC->rtc172_write(target,id,r,sizeof *r);
#else
 return OF_ERR_NOT_SUPPORTED;
#endif
}
#endif
