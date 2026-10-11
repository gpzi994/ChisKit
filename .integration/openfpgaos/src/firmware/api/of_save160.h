#ifndef OF_SAVE160_H
#define OF_SAVE160_H
#include "of_cart.h"
#include <stdint.h>
/* This optional service is only present in explicit SAVE158 OS builds.
 * Caller must first create and verify a new original SAV in output slot 5. */
typedef struct {
 int32_t rc,restore_rc,bank0_rc;
 uint32_t attempted,changed,verified;
 uint32_t restore_attempted,restore_verified;
} of_save160_report;
static inline int of_save160_restore(const void *source,void *disk_backup,uint32_t bytes,
 const uint8_t expected_id[16],of_save160_report *report){
 if(!source||!disk_backup||!expected_id||!report)return OF_ERR_INVALID_PARAM;
 *report=(of_save160_report){0};
#ifndef OF_PC
 if(!OF_CART_SVC_HAS(sram160_restore))return OF_ERR_NOT_SUPPORTED;
 return OF_SVC->sram160_restore(source,disk_backup,bytes,expected_id,
  report,sizeof(*report));
#else
 (void)bytes;return OF_ERR_NOT_SUPPORTED;
#endif
}
#endif
