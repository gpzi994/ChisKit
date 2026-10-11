// SPDX-License-Identifier: AGPL-3.0-only
#include "mbcx111.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 struct cb_mbcx111_write w[7],zero[7]={0};unsigned cases=0;
 for(uint32_t size=0x8000;size<=CB_MBCX111_MAX_BYTES;size*=2){
  for(uint32_t bank=0;bank<size/0x4000;bank++)for(unsigned edge=0;edge<2;edge++){
   assert(cb_mbcx111_select(size,bank*0x4000+(edge?0x3fff:0),w)==7);
   assert(w[0].address==0&&w[0].value==5);
   assert(w[1].address==0x4000&&w[1].value==0x82);
   assert(w[2].address==0xa000&&w[2].value==bank/512&&w[2].value<4);
   assert(w[3].address==0&&!w[3].value);
   assert(w[4].address==0x3000&&w[4].value==(bank%512)/256);
   assert(w[5].address==0x2100&&w[5].value==bank%256);
   assert(w[6].address==0x4000&&!w[6].value);
   for(unsigned k=0;k<7;k++)assert(w[k].delay_ms==(k<4?100:0));
   cases++;
  }
  assert(!cb_mbcx111_select(size,size,w)&&!memcmp(w,zero,sizeof(w)));
 }
 const uint32_t bad[]={0,1,0x4000,0x8001,0x3000000,0x4000000,0xffffffff};
 for(unsigned k=0;k<sizeof(bad)/sizeof(bad[0]);k++){
  memset(w,0xaa,sizeof(w));assert(!cb_mbcx111_select(bad[k],0,w));
  assert(!memcmp(w,zero,sizeof(w)));
 }
 assert(!cb_mbcx111_select(0x800000,0,NULL));
 printf("PASS MBCX pure planner: %u bank-edge cases, four outer windows, bounded capacity and cleared failures; not hardware enabled\n",cases);
}
