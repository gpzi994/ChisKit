#ifndef OF_SAVE139_H
#define OF_SAVE139_H
#include "of_cart.h"
#include <string.h>
/* Fixed-width, detection-only ABI. No raw EEPROM data or write entry. */
typedef struct { int32_t type,rc; uint32_t bytes,basis; } of_save139_result;
typedef struct {
 uint32_t completed,differences,first_round,first_address;
 int32_t rc,cleanup_rc;
 uint8_t block[64][32],single[64];
} of_save143_readonly;
typedef struct {of_save143_readonly sample;uint32_t scanned,sum,head_diff;uint8_t head[32];} of_save144_readonly;
typedef struct {of_save143_readonly sample;uint32_t scanned[2],sum[2],head_diff;int32_t bank_rc[2],restore_rc;uint8_t head[2][32];} of_save145_banks;
static inline int of_save145_read(of_save145_banks *out){
 if(!out)return OF_ERR_INVALID_PARAM;
 memset(out,0,sizeof(*out));
#ifndef OF_PC
 if(OF_CART_SVC_HAS(eeprom139_detect))return OF_SVC->eeprom139_detect(out,sizeof(*out));
#endif
 return OF_ERR_NOT_SUPPORTED;
}
static inline int of_save144_read(of_save144_readonly *out){
 if(!out)return OF_ERR_INVALID_PARAM;
 memset(out,0,sizeof(*out));
#ifndef OF_PC
 if(OF_CART_SVC_HAS(eeprom139_detect))return OF_SVC->eeprom139_detect(out,sizeof(*out));
#endif
 return OF_ERR_NOT_SUPPORTED;
}
static inline int of_save143_read(of_save143_readonly *out){
 if(!out)return OF_ERR_INVALID_PARAM;
 memset(out,0,sizeof(*out));
#ifndef OF_PC
 if(OF_CART_SVC_HAS(eeprom139_detect))return OF_SVC->eeprom139_detect(out,sizeof(*out));
#endif
 return OF_ERR_NOT_SUPPORTED;
}
typedef struct {
 of_save139_result result;
 uint32_t stage,id,id_read;
 int32_t probe_rc,cleanup_rc,direct_rc,restore_rc;
 uint32_t original,target,observed,restored,mismatch_address,expected,actual;
 int32_t leave_rc;
} of_save141_diagnostic;
static inline int of_save141_detect(of_save141_diagnostic *out){
 if(!out)return OF_ERR_INVALID_PARAM;
 *out=(of_save141_diagnostic){.result={.type=-1,.rc=OF_ERR_NOT_SUPPORTED}};
#ifndef OF_PC
 if(OF_CART_SVC_HAS(eeprom139_detect))return OF_SVC->eeprom139_detect(out,sizeof(*out));
#endif
 return OF_ERR_NOT_SUPPORTED;
}
/* Read-only diagnostic snapshot; raw modes, not an authorized save backup.
 * state is the content heuristic, never a physical capacity guarantee. */
typedef struct {
 int32_t rc,cleanup_rc;
 uint32_t state,bytes_read,first_difference,prefix_equal;
 uint8_t small[512],large[8192];
 uint32_t address_bits,pass,block,stage;
 uint32_t stable_mask,mismatch_mask,mode_difference[2];
} of_eeprom139_snapshot;
static inline int of_eeprom139_capture(of_eeprom139_snapshot *out){
 if(!out)return OF_ERR_INVALID_PARAM;
#ifndef OF_PC
 if(!OF_CART_SVC_HAS(eeprom139_capture))return OF_ERR_NOT_SUPPORTED;
 return OF_SVC->eeprom139_capture(out,sizeof(*out));
#else
 return OF_ERR_NOT_SUPPORTED;
#endif
}
/* Historical ABI name: now performs automatic GBA save classification.
 * May temporarily write/restore one SRAM byte and issue Flash ID/reset.
 * Never grants save write access. Raw capture remains read-only. */
static inline int of_eeprom139_detect(of_save139_result *out){
 if(!out)return OF_ERR_INVALID_PARAM;
 *out=(of_save139_result){.type=-1,.rc=OF_ERR_NOT_SUPPORTED};
#ifndef OF_PC
 if(!OF_CART_SVC_HAS(eeprom139_detect))return OF_ERR_NOT_SUPPORTED;
 return OF_SVC->eeprom139_detect(out,sizeof(*out));
#else
 return OF_ERR_NOT_SUPPORTED;
#endif
}
#endif
