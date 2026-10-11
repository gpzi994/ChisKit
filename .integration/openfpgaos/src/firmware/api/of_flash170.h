#ifndef OF_FLASH170_H
#define OF_FLASH170_H
#include "of_cart.h"
typedef struct {int32_t rc,cleanup_rc;uint32_t id,stage,address,attempted,verified;} of_flash170_report;
static inline int of_flash170_restore(const void *source,void *backup,uint32_t n,
 const uint8_t id[16],of_flash170_report *r){
 if(!source||!backup||!id||!r)return OF_ERR_INVALID_PARAM;
 *r=(of_flash170_report){0};
 if(!OF_CART_SVC_HAS(flash170_restore))return OF_ERR_NOT_SUPPORTED;
 return OF_SVC->flash170_restore(source,backup,n,id,r,sizeof *r);
}
#endif
