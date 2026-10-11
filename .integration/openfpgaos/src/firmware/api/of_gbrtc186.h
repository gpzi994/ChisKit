#ifndef OF_GBRTC186_H
#define OF_GBRTC186_H
#include "of_cart.h"
typedef struct {int32_t rc,detail,cleanup_rc;uint32_t supported,days,hours,minutes,seconds,halt,carry;uint8_t raw[5],reserved[3];} of_gb186_report;
static inline int of_gb186_read(const uint8_t h[80],of_gb186_report *r){
 if(!OF_CART_SVC_HAS(gb186_read))return OF_ERR_NOT_SUPPORTED;
 return OF_SVC->gb186_read(h,r,sizeof *r);
}
#endif
