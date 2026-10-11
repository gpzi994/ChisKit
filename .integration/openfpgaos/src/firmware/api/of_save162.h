#ifndef OF_SAVE162_H
#define OF_SAVE162_H
#include "of_cart.h"
#include <stdint.h>
typedef struct {
 int32_t rc,restore_rc,cleanup_rc;
 uint32_t stage,attempted,verified,restore_attempted,restore_verified;
} of_save162_report;
static inline int of_save162_restore(const void *source,void *disk_backup,
 uint32_t bytes,const uint8_t expected_id[16],of_save162_report *report){
 if(!source||!disk_backup||!expected_id||!report)return OF_ERR_INVALID_PARAM;
 *report=(of_save162_report){0};
#ifndef OF_PC
 if(!OF_CART_SVC_HAS(eeprom162_restore))return OF_ERR_NOT_SUPPORTED;
 return OF_SVC->eeprom162_restore(source,disk_backup,bytes,expected_id,
  report,sizeof(*report));
#else
 (void)bytes;return OF_ERR_NOT_SUPPORTED;
#endif
}
#endif
