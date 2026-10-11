// SPDX-License-Identifier: AGPL-3.0-only
// Append-only, bounded NOR jobs. No application-visible raw bus writes.
#ifndef OF_NOR_H
#define OF_NOR_H
#include "of_cart.h"
#define OF_NOR_VERSION 1u
#define OF_NOR_REGIONS 8u
#define OF_NOR_ID_OK 1u
#define OF_NOR_CFI_OK 2u
#define OF_NOR_WRITE_OK 4u
#define OF_NOR_WINDOW_LIMITED 8u
#define OF_NOR_BOARD_IDS_OK 16u /* Diagnostic evidence, NOT write authorization. */
typedef struct of_nor_region { uint32_t start,count,sector_bytes; } of_nor_region_t;
typedef struct of_nor_info {
 uint32_t struct_size,version,generation,bus,mapper,flags;
 uint32_t flash_bytes,accessible_bytes,buffer_bytes,command_set,method,swapped;
 uint32_t region_count;
 of_nor_region_t regions[OF_NOR_REGIONS];
 uint8_t id[8];
 int32_t probe_rc,plan_rc;
 char model[64];
} of_nor_info_t;
// Return codes are cb_nor_result (0 success, -1 argument, -2 unsupported,
// -3 lifecycle/cancel, -4 timeout, -5 IO, -6 status, -7 verify, -8 unarmed).
#ifndef OF_PC
static inline int of_nor_available(void){return of_cart_available()&&OF_CART_SVC_HAS(nor_probe)&&OF_CART_SVC_HAS(nor_begin)&&OF_CART_SVC_HAS(nor_erase)&&OF_CART_SVC_HAS(nor_program)&&OF_CART_SVC_HAS(nor_finish);}
static inline int of_nor_probe(uint32_t bus,of_nor_info_t*out){
 if(!out)return -1;
 of_cart_clear_bytes(out,sizeof(*out));
 return of_nor_available()?OF_SVC->nor_probe(bus,out,sizeof(*out)):-2;
}
static inline int of_nor_begin(uint32_t generation,uint32_t bytes,uint32_t*token){
 if(!token)return -1;
 *token=0;
 return of_nor_available()?OF_SVC->nor_begin(generation,bytes,token):-2;
}
static inline int of_nor_erase(uint32_t token,uint32_t address,uint32_t*length){
 return OF_CART_SVC_HAS(nor_erase)?OF_SVC->nor_erase(token,address,length):-2;
}
static inline int of_nor_program(uint32_t token,uint32_t address,const void*data,uint32_t length){
 return OF_CART_SVC_HAS(nor_program)?OF_SVC->nor_program(token,address,data,length):-2;
}
static inline int of_nor_finish(uint32_t token){return OF_CART_SVC_HAS(nor_finish)?OF_SVC->nor_finish(token):-2;}
/* Optional append-only extension. A v108 OS remains compatible and reports
 * unavailable. This does not expose unbounded raw addresses or bus writes. */
static inline int of_nor_verify_available(void){return OF_CART_SVC_HAS(nor_verify_read);}
static inline int of_nor_verify_read(uint32_t token,uint32_t address,void*dst,uint32_t length){
 return of_nor_verify_available()?OF_SVC->nor_verify_read(token,address,dst,length):-2;
}
static inline int of_nor_pipeline_available(void){
 return OF_CART_SVC_HAS(psram_cache_load)&&OF_CART_SVC_HAS(psram_cache_read)&&
  OF_CART_SVC_HAS(nor_begin_full)&&OF_CART_SVC_HAS(nor_erase_poll)&&
  OF_CART_SVC_HAS(nor_program_poll)&&OF_CART_SVC_HAS(nor_verify_full_read);
}
static inline int of_nor_begin_full(uint32_t generation,uint32_t program_bytes,uint32_t erase_bytes,uint32_t*token){
 if(!token)return -1;
 *token=0;
 return OF_CART_SVC_HAS(nor_begin_full)?OF_SVC->nor_begin_full(generation,program_bytes,erase_bytes,token):-2;
}
static inline int of_nor_erase_poll(uint32_t token,uint32_t address,uint32_t*length){
 return OF_CART_SVC_HAS(nor_erase_poll)?OF_SVC->nor_erase_poll(token,address,length):-2;
}
static inline int of_nor_program_poll(uint32_t token,uint32_t address,const void*data,uint32_t length){
 return OF_CART_SVC_HAS(nor_program_poll)?OF_SVC->nor_program_poll(token,address,data,length):-2;
}
static inline int of_nor_verify_full_read(uint32_t token,uint32_t address,void*dst,uint32_t length){
 return OF_CART_SVC_HAS(nor_verify_full_read)?OF_SVC->nor_verify_full_read(token,address,dst,length):-2;
}
static inline int of_nor_stream_available(void){
 return OF_CART_SVC_HAS(nor_program_next)&&OF_CART_SVC_HAS(nor_program_step);
}
static inline int of_nor_program_next(uint32_t token,uint32_t address,uint32_t maximum,uint32_t *length){
 if(!length)return -1;
 *length=0;
 return of_nor_stream_available()?OF_SVC->nor_program_next(token,address,maximum,length):-2;
}
static inline int of_nor_program_step(uint32_t token,int cancel){
 return of_nor_stream_available()?OF_SVC->nor_program_step(token,cancel):-2;
}
static inline int of_nor_compare_available(void){
 return of_nor_stream_available() && OF_CART_SVC_HAS(nor_compare_start);
}
static inline int of_nor_compare_start(uint32_t token,uint32_t address,uint32_t length){
 return of_nor_compare_available()?OF_SVC->nor_compare_start(token,address,length):-2;
}
/* Four words: raw status, compared bytes, fault byte address, actual:expected. */
static inline int of_nor_compare_result(uint32_t token,uint32_t out[4]){
 if(!out)return -1;
 return OF_CART_SVC_HAS(nor_compare_result)?OF_SVC->nor_compare_result(token,out):-2;
}
#endif
#endif
