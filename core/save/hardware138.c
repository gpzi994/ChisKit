#include "hardware138.h"
#include <string.h>
struct cb_save_hw138_evidence cb_save_hw138_compare(
 enum cb_save_hw138_profile profile,const uint8_t *a,const uint8_t *b,size_t n,
 unsigned bank0_read,unsigned bank1_read){
 struct cb_save_hw138_evidence r={.state=CB_SAVE_HW138_BAD_INPUT,
  .first_difference=UINT32_MAX,.bank0_read=!!bank0_read,.bank1_read=!!bank1_read};
 if(!a||!b||a==b||(n!=32768&&n!=131072))return r;
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 if(x<y?y-x<n:x-y<n)return r;
 r.captured_bytes=(uint32_t)n;
 if(profile!=CB_SAVE_HW138_PARALLEL32_CONFIRMED&&
    profile!=CB_SAVE_HW138_CHISFLASH128_CONFIRMED&&
    profile!=CB_SAVE_HW138_PARALLEL_PROBE){
  r.state=CB_SAVE_HW138_PROFILE_REQUIRED;return r;
 }
 if((profile==CB_SAVE_HW138_PARALLEL32_CONFIRMED&&(n!=32768||bank1_read))||
    ((profile==CB_SAVE_HW138_CHISFLASH128_CONFIRMED||profile==CB_SAVE_HW138_PARALLEL_PROBE)&&
     (n!=131072||!bank0_read||!bank1_read)))return r;
 for(size_t i=0;i<n;i++)if(a[i]!=b[i]){
  r.state=CB_SAVE_HW138_UNSTABLE;r.first_difference=(uint32_t)i;return r;
 }
 int uniform=1;
 for(size_t i=1;i<n;i++)if(a[i]!=a[0]){uniform=0;break;}
 if(uniform){
  r.state=CB_SAVE_HW138_UNIFORM;r.repeating_bytes=1;
  if(profile!=CB_SAVE_HW138_PARALLEL_PROBE)r.backup_bytes=(uint32_t)n;
  return r;
 }
 for(size_t period=32768;period<n;period*=2){
  if(!memcmp(a,a+period,n-period)){r.repeating_bytes=(uint32_t)period;break;}
 }
 /* Do not trim a known 128 KiB board just because its content repeats. */
 r.state=CB_SAVE_HW138_STABLE;r.backup_bytes=(uint32_t)n;
 /* The caller must display this as a content-based candidate, NOT verified
  * SRAM identity. Flash save ID/EEPROM discrimination is a separate probe. */
 if(profile==CB_SAVE_HW138_PARALLEL_PROBE&&r.repeating_bytes)
  r.backup_bytes=r.repeating_bytes;
 return r;
}
