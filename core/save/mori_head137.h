/* Adapted from MoriBunrnner-for-ESP32-S3 burner_gba_analysis.c:
 * burner_detect_gba_save_type_in_span + header game-code fallback.
 * Preserve signature precedence; this is ROM evidence, not hardware ID.
 * Use ChisBurner's existing SDRAM buffer, not Mori's SPIRAM allocation. */
#ifndef CB_MORI_HEAD137_H
#define CB_MORI_HEAD137_H
#include "save.h"
#include <string.h>
#define CB_MORI_HEAD_BYTES 65536u
static int cb_mori_contains(const uint8_t *p,size_t n,const char *s){
 size_t k=strlen(s);
 if(n<k)return 0;
 for(size_t i=0;i<=n-k;i++)if(p[i]==(uint8_t)s[0]&&!memcmp(p+i,s,k))return 1;
 return 0;
}
static struct cb_save_hint cb_mori_head137(const uint8_t *p,size_t n,int *by_code){
 *by_code=0;
 if(cb_mori_contains(p,n,"FLASH1M_V"))return (struct cb_save_hint){CB_SAVE_FLASH128,131072};
 if(cb_mori_contains(p,n,"FLASH512_V")||cb_mori_contains(p,n,"FLASH_V"))return (struct cb_save_hint){CB_SAVE_FLASH64,65536};
 if(cb_mori_contains(p,n,"EEPROM_V"))return (struct cb_save_hint){CB_SAVE_EEPROM,8192};
 if(cb_mori_contains(p,n,"SRAM_V"))return (struct cb_save_hint){CB_SAVE_SRAM,32768};
 if(n>=0xb0&&(!memcmp(p+0xac,"RZWE",4)||!memcmp(p+0xac,"RZWJ",4))){
  *by_code=1;return (struct cb_save_hint){CB_SAVE_SRAM,32768};
 }
 return (struct cb_save_hint){CB_SAVE_UNKNOWN,0};
}
#endif
