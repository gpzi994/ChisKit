// SPDX-License-Identifier: AGPL-3.0-only
// Board parameters/commands: FlashGBX fc_AGB_Flash2Advance_{128M,256M,Ultra_2G}.txt
// and Flashcart.py SelectBankROM(type=2), local upstream snapshot v5.1.
#include "board109.h"
#include <string.h>
int cb_board109_profile(enum cb_board109_kind k,struct cb_board109_profile*out){
 struct cb_board109_profile p={0};
 if(!out)return -1;
 memset(out,0,sizeof(*out));
 if(k<CB_BOARD109_F2A128||k>CB_BOARD109_F2AULTRA)return -1;
 p.kind=k;p.window_bytes=0x2000000;p.banks=k==CB_BOARD109_F2AULTRA?8:1;
 p.bytes=k==CB_BOARD109_F2A128?0x1000000:k==CB_BOARD109_F2A256?0x2000000:0x10000000;
 p.reset_stride=k==CB_BOARD109_F2AULTRA?0x400000:0;
 p.geometry.bytes=p.bytes;p.geometry.buffer_bytes=64;
 p.geometry.cmdset=CHISBURNER_NOR_CMDSET_INTEL;
 if(k!=CB_BOARD109_F2AULTRA){
  p.geometry.region_count=1;
  p.geometry.region[0]=(struct cb_nor_region){0,p.bytes/0x40000,0x40000};
 }else{
  uint32_t start=0;
  p.geometry.region_count=8;
  for(unsigned n=0;n<8;n++){
   bool small=n==0||n==3||n==4||n==7;
   uint32_t count=small?4:255,sector=small?0x10000:0x40000;
   p.geometry.region[n]=(struct cb_nor_region){start,count,sector};
   start+=count*sector;
  }
  if(start!=p.bytes)return -1;
 }
 *out=p;return 0;
}
int cb_board109_bank_id(enum cb_board109_kind k,unsigned bank,uint8_t out[8]){
 struct cb_board109_profile p;
 if(!out)return -1;
 memset(out,0,8);
 if(cb_board109_profile(k,&p)||bank>=p.banks)return -1;
 uint16_t device=k==CB_BOARD109_F2A128?0x17:k==CB_BOARD109_F2A256?0x18:
  (bank%4<2?0x8810:0x880d);
 out[0]=0x89;out[2]=0x89;
 out[4]=out[6]=(uint8_t)device;out[5]=out[7]=(uint8_t)(device>>8);
 return 0;
}
static bool clear(struct cb_board109_write*out,size_t capacity){
 if(!out||capacity>16)return false;
 memset(out,0,capacity*sizeof(*out));return true;
}
size_t cb_board109_unlock(enum cb_board109_kind k,struct cb_board109_write*out,size_t cap){
 struct cb_board109_profile p;
 if(!clear(out,cap)||cap<4||cb_board109_profile(k,&p))return 0;
 const struct cb_board109_write seq[]={
  {0x130eca8,0x5354,CB_BOARD109_ROM},{0x1c2468a,0xa55a,CB_BOARD109_ROM},
  {0x130eca8,0x5354,CB_BOARD109_ROM},{0x1e2468a,0x9413,CB_BOARD109_ROM}};
 memcpy(out,seq,sizeof(seq));return 4;
}
size_t cb_board109_select(enum cb_board109_kind k,unsigned bank,struct cb_board109_write*out,size_t cap){
 struct cb_board109_profile p;
 if(!clear(out,cap)||cb_board109_profile(k,&p)||bank>=p.banks)return 0;
 /* Non-banked boards need no select operation; distinguish via profile.banks. */
 if(k!=CB_BOARD109_F2AULTRA||cap<7)return 0;
 const struct cb_board109_write seq[]={
  {0x130eca8,0x5354,CB_BOARD109_ROM},{0x1c2468a,0xa55a,CB_BOARD109_ROM},
  {6,bank<4?0:0x10,CB_BOARD109_CONTROL},{0x130eca8,0x5354,CB_BOARD109_ROM},
  {0x16b592e,(uint16_t)(bank%4*0x400),CB_BOARD109_ROM},
  {0x130eca8,0x5354,CB_BOARD109_ROM},{0x1e2468a,0x9413,CB_BOARD109_ROM}};
 memcpy(out,seq,sizeof(seq));return 7;
}
int cb_board109_map(enum cb_board109_kind k,uint32_t address,unsigned*bank,uint32_t*physical){
 struct cb_board109_profile p;
 if(bank)*bank=0;
 if(physical)*physical=0;
 if(!bank||!physical||cb_board109_profile(k,&p)||address>=p.bytes)return -1;
 *bank=address/p.window_bytes;*physical=address%p.window_bytes;return 0;
}
bool cb_board109_ids_match(enum cb_board109_kind k,const uint8_t*a,const uint8_t*b,size_t count){
 struct cb_board109_profile p;uint8_t expected[8];
 if(!a||!b||cb_board109_profile(k,&p)||count!=p.banks)return false;
 for(unsigned n=0;n<p.banks;n++){
  if(cb_board109_bank_id(k,n,expected)||memcmp(a+n*8,expected,8)||memcmp(b+n*8,expected,8))return false;
 }
 return true;
}
int cb_board109_match_die_cfi(enum cb_board109_kind k,unsigned bank,const struct cb_nor_cfi*c){
 struct cb_board109_profile p;
 if(!c||cb_board109_profile(k,&p)||bank>=p.banks||c->cmdset!=CHISBURNER_NOR_CMDSET_INTEL||
    (c->primary_id!=1&&c->primary_id!=3)||c->lane||!c->bytes||c->bytes>p.bytes/2||
    (c->bytes&(c->bytes-1))||!c->region_count||c->region_count>CB_NOR_REGIONS||
    c->buffer_bytes<32||(c->buffer_bytes&(c->buffer_bytes-1)))return -1;
 uint32_t span=c->bytes*2,window=p.window_bytes<p.bytes?p.window_bytes:p.bytes;
 if(span<window||p.bytes%span)return -1;
 uint32_t base=(bank*p.window_bytes/span)*span,total=0;
 for(unsigned n=0;n<c->region_count;n++){
  const struct cb_nor_region*r=c->region+n;
  if(r->start!=total||!r->count||!r->sector_bytes||r->sector_bytes>c->bytes||
     r->count>(c->bytes-total)/r->sector_bytes)return -1;
  uint32_t bytes=r->count*r->sector_bytes,a=base+total*2,end=a+bytes*2;
  while(a<end){
   bool found=false;
   for(unsigned j=0;j<p.geometry.region_count;j++){
    const struct cb_nor_region*g=p.geometry.region+j;
    uint32_t ge=g->start+g->count*g->sector_bytes;
    if(a>=g->start&&a<ge){
     if(g->sector_bytes!=r->sector_bytes*2||(a-g->start)%g->sector_bytes)return -1;
     uint32_t next=end<ge?end:ge;
     if((next-a)%g->sector_bytes)return -1;
     a=next;found=true;break;
    }
   }
   if(!found)return -1;
  }
  total+=bytes;
 }
 return total==c->bytes?0:-1;
}
