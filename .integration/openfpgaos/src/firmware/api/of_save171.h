#ifndef OF_SAVE171_H
#define OF_SAVE171_H
#include "of_cart.h"
typedef struct {
 uint32_t version;
 int32_t rc,restore_rc,cleanup_rc;
 uint32_t attempted,verified,restore_verified;
} of_save171_report;
static inline int of_save171_restore(unsigned type,const void *source,void *backup,
 uint32_t bytes,const uint8_t id[16],of_save171_report *r){
 if(!source||!backup||!id||!r)return OF_ERR_INVALID_PARAM;
 *r=(of_save171_report){0};
#ifndef OF_PC
 if(!OF_CART_SVC_HAS(save171_restore))return OF_ERR_NOT_SUPPORTED;
 return OF_SVC->save171_restore(type,source,backup,bytes,id,r,sizeof *r);
#else
 (void)type;(void)bytes;return OF_ERR_NOT_SUPPORTED;
#endif
}
#endif
