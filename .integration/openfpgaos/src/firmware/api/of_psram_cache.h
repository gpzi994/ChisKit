// SPDX-License-Identifier: AGPL-3.0-only
#ifndef OF_PSRAM_CACHE_H
#define OF_PSRAM_CACHE_H
#include "of_cart.h"
#ifndef OF_PC
static inline int of_psram_cache_available(void){
 return OF_CART_SVC_HAS(psram_cache_load)&&OF_CART_SVC_HAS(psram_cache_read);
}
static inline int of_psram_cache_load(uint32_t slot,uint32_t slot_offset,
 uint32_t cache_offset,uint32_t length){
 return OF_CART_SVC_HAS(psram_cache_load)?OF_SVC->psram_cache_load(slot,slot_offset,cache_offset,length):-2;
}
static inline int of_psram_cache_read(uint32_t cache_offset,void*dst,uint32_t length){
 return OF_CART_SVC_HAS(psram_cache_read)?OF_SVC->psram_cache_read(cache_offset,dst,length):-2;
}
#endif
#endif
