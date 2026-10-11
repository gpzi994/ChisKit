#ifndef OF_GB175_H
#define OF_GB175_H
#include "of_cart.h"
/* GB/GBC only, RAM capture only. No GBA save enums or RTC footer. */
typedef struct { int32_t rc,detail,cleanup_rc;uint32_t compared,written,verified; } of_gb175_report;
static inline int of_gb175_capture(const uint8_t header[80],void *snapshot,
 uint32_t capacity,of_gb175_report *report){
 if(!OF_CART_SVC_HAS(gb175_capture))return OF_ERR_NOT_SUPPORTED;
 return OF_SVC->gb175_capture(header,snapshot,capacity,report,sizeof *report);
}
static inline int of_gb176_capture(const uint8_t header[80],uint32_t selected,
 void *snapshot,uint32_t capacity,of_gb175_report *report){
 if(!OF_CART_SVC_HAS(gb176_capture))return OF_ERR_NOT_SUPPORTED;
 return OF_SVC->gb176_capture(header,selected,snapshot,capacity,report,sizeof *report);
}
static inline int of_gb177_capture(const uint8_t header[80],uint32_t selected,
 void *snapshot,uint32_t capacity,of_gb175_report *report){
 if(!OF_CART_SVC_HAS(gb177_capture))return OF_ERR_NOT_SUPPORTED;
 return OF_SVC->gb177_capture(header,selected,snapshot,capacity,report,sizeof *report);
}
static inline int of_gb178_capture(const uint8_t header[80],uint32_t selected,
 void *snapshot,uint32_t capacity,of_gb175_report *report){
 if(!OF_CART_SVC_HAS(gb178_capture))return OF_ERR_NOT_SUPPORTED;
 return OF_SVC->gb178_capture(header,selected,snapshot,capacity,report,sizeof *report);
}
static inline int of_gb179_capture(const uint8_t header[80],uint32_t selected,
 void *snapshot,uint32_t capacity,of_gb175_report *report){
 if(!OF_CART_SVC_HAS(gb179_capture))return OF_ERR_NOT_SUPPORTED;
 return OF_SVC->gb179_capture(header,selected,snapshot,capacity,report,sizeof *report);
}
static inline int of_gb180_restore(const uint8_t header[80],const void *original,
 const void *source,uint32_t bytes,of_gb175_report *report){
 if(!OF_CART_SVC_HAS(gb180_restore))return OF_ERR_NOT_SUPPORTED;
 return OF_SVC->gb180_restore(header,original,source,bytes,report,sizeof *report);
}
#endif
