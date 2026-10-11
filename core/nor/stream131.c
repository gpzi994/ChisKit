// SPDX-License-Identifier: AGPL-3.0-only
#include "stream131.h"
#include <string.h>
size_t cb_stream131_chunk(const struct cb_nor_plan *p,uint32_t a,size_t maximum){
 if(!p)return 0;
 if(p->recipe.unit!=1||p->geometry.buffer_bytes||p->flags)return cb_nor_chunk(p,a,maximum);
 /* Reuse all range/sector/window checks without changing the physical NOR
  * programming geometry or the non-stream executor's byte granularity. */
 struct cb_nor_plan batch=*p;
 batch.geometry.buffer_bytes=256;
 return cb_nor_chunk(&batch,a,maximum);
}
int cb_stream131_build(const struct cb_nor_plan *p,uint32_t source,
 uint32_t address,uint32_t length,uint32_t timeout,
 struct cb_stream131_descriptor *out){
 if(!out)return -1;
 memset(out,0,sizeof(*out));
 if(!p||!timeout||!length||source>=0x02000000u||length>0x02000000u-source||
    (p->recipe.unit!=1&&p->recipe.unit!=2)||source%p->recipe.unit||
    cb_stream131_chunk(p,address,length)!=length)return -1;
 if(p->geometry.bytes>0x02000000u||p->accessible_bytes>0x02000000u||
    p->geometry.buffer_bytes>65536u||length>65536u)return -2;
 const struct cb_nor_recipe *r=&p->recipe;
 bool intel=p->geometry.cmdset==CHISBURNER_NOR_CMDSET_INTEL;
 bool dual=r->method==CB_NOR_INTEL_DUAL;
 bool wide=r->method==CB_NOR_MSP55_AAA||r->method==CB_NOR_MSP55_5555;
 bool special=(p->flags&CHISBURNER_NOR_FLAG_DISABLE_BUFFER_PROGRAM)!=0;
 bool single=special||p->geometry.buffer_bytes==0;
 if((single&&((r->unit!=1&&length!=r->unit)||dual))||
    (dual&&(!intel||r->unit!=2||(address&3)||(length&3)))||
    (intel&&(wide||r->unit!=2))||(wide&&r->unit!=2)||
    (special&&(!intel||r->unit!=2||dual||wide)))return -2;
 uint32_t *w=out->words;
 w[CB_S131_FLAGS]=(r->unit==2?1u:0u)|(intel?2u:0u)|(dual?4u:0u)|
  (wide?8u:0u)|(r->swapped?16u:0u)|(r->raw_count?32u:0u)|(special?64u:0u);
 w[CB_S131_SOURCE]=source;
 w[CB_S131_PHYSICAL]=r->unit==1?(address<0x4000u?address:0x4000u+(address&0x3fffu)):address;
 w[CB_S131_LENGTH]=length;w[CB_S131_BUFFER]=p->geometry.buffer_bytes;
 w[CB_S131_UNLOCK0]=r->unlock0;w[CB_S131_UNLOCK1]=r->unlock1;
 w[CB_S131_AA_BB]=(uint32_t)r->aa|((uint32_t)r->bb<<16);
 w[CB_S131_BUFFER_CONFIRM]=(uint32_t)r->buffer|((uint32_t)r->confirm<<16);
 w[CB_S131_RESET_WORD]=(uint32_t)r->reset|((uint32_t)r->word<<16);
 w[CB_S131_TIMEOUT]=timeout;
 return 0;
}
