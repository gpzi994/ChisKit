// SPDX-License-Identifier: AGPL-3.0-only
#include "mbcx111.h"
#include <string.h>
size_t cb_mbcx111_select(uint32_t bytes,uint32_t address,struct cb_mbcx111_write out[7]){
 if(!out)return 0;
 memset(out,0,7*sizeof(*out));
 if(bytes<0x8000||bytes>CB_MBCX111_MAX_BYTES||(bytes&(bytes-1))||address>=bytes)return 0;
 uint32_t bank=address/0x4000;
 /* Always reselect the outer bank; no stale cached-board assumption. */
 out[0]=(struct cb_mbcx111_write){0x0000,0x05,100};
 out[1]=(struct cb_mbcx111_write){0x4000,0x82,100};
 out[2]=(struct cb_mbcx111_write){0xa000,(uint8_t)(bank/512),100};
 out[3]=(struct cb_mbcx111_write){0x0000,0x00,100};
 out[4]=(struct cb_mbcx111_write){0x3000,(uint8_t)((bank%512)>>8),0};
 out[5]=(struct cb_mbcx111_write){0x2100,(uint8_t)bank,0};
 out[6]=(struct cb_mbcx111_write){0x4000,0x00,0};
 return 7;
}
