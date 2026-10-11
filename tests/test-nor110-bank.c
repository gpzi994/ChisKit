// SPDX-License-Identifier: AGPL-3.0-only
#include "bank110.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 unsigned cases=0;
 for(int kind=CB_BANK110_AMD_BASIC;kind<=CB_BANK110_TYPE1;kind++)
  for(uint32_t bytes=CB_BANK110_WINDOW;bytes<=CB_BANK110_LIMIT;bytes*=2){
   for(unsigned bank=0;bank<bytes/CB_BANK110_WINDOW;bank++){
    struct cb_bank110_write w[3];memset(w,0xff,sizeof(w));
    assert(cb_bank110_select(kind,bytes,bank,w)==(kind==CB_BANK110_TYPE1?3u:2u));
    assert(w[0].bus==3&&w[0].address==2&&w[0].value==(bank<<4));
    assert(w[1].bus==3&&w[1].address==3&&w[1].value==0x40);
    if(kind==CB_BANK110_TYPE1)assert(w[2].bus==3&&w[2].address==4&&w[2].value==0);
    else assert(w[2].bus==0&&w[2].address==0&&w[2].value==0);
    const uint32_t offsets[]={0,1,0x100,CB_BANK110_WINDOW-2,CB_BANK110_WINDOW-1};
    for(unsigned i=0;i<sizeof(offsets)/sizeof(*offsets);i++){
     unsigned b=99;uint32_t local=99;
     assert(!cb_bank110_map(kind,bytes,bank*CB_BANK110_WINDOW+offsets[i],&b,&local));
     assert(b==bank&&local==offsets[i]);cases++;
    }
   }
   struct cb_bank110_write w[3],zero[3]={0};memset(w,0xff,sizeof(w));
   assert(!cb_bank110_select(kind,bytes,bytes/CB_BANK110_WINDOW,w));
   assert(!memcmp(w,zero,sizeof(w)));
   unsigned b=99;uint32_t local=99;
   assert(cb_bank110_map(kind,bytes,bytes,&b,&local)<0&&b==0&&local==0);
  }
 const uint32_t invalid[]={0,1,0x1000000,0x3000000,0x10000001,0x20000000,UINT32_MAX};
 for(unsigned i=0;i<sizeof(invalid)/sizeof(*invalid);i++){
  struct cb_bank110_write w[3],zero[3]={0};memset(w,0xff,sizeof(w));
  assert(!cb_bank110_select(CB_BANK110_TYPE1,invalid[i],0,w));
  assert(!memcmp(w,zero,sizeof(w)));
 }
 for(int kind=-1;kind<=3;kind+=4){
  struct cb_bank110_write w[3];
  assert(!cb_bank110_select(kind,CB_BANK110_WINDOW,0,w));
 }
 assert(!cb_bank110_select(CB_BANK110_NONE,CB_BANK110_WINDOW,0,0));
 assert(cb_bank110_map(CB_BANK110_TYPE1,CB_BANK110_WINDOW,0,0,0)<0);
 printf("PASS bank110: %u address cases; raw Mori control order, bounds and cleared failures\n",cases);
 return 0;
}
