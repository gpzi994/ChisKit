#ifndef CB_GB_DATABASE183_H
#define CB_GB_DATABASE183_H
#include "dump_plan.h"
/* Only the 384-byte database identity, not an unrestricted ROM reader. */
static inline int gb183_address(const struct cb_cart *cart,uint32_t offset,unsigned n,uint32_t *address){
 int mode=cb_dump_mode(cart);
 if(mode<0||n!=32||(offset&31)||offset>384-32)return -1;
 *address=0x10000000u+((unsigned)mode<<24)+offset;
 return 0;
}
#endif
