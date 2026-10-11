#ifndef OF_RTC167_H
#define OF_RTC167_H
#include "of_cart.h"
#include "of_rtc167_data.h"
static inline int of_rtc167_read(of_rtc167_report *r){
 return of_cart_read_space(OF_CART_BUS_GBA,OF_CART_SPACE_RTC_GPIO,0,r,sizeof(*r),1000000);
}
#endif
