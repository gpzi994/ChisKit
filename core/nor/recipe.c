// SPDX-License-Identifier: AGPL-3.0-only
// Adapted from Mori burner_gba_lowlevel.c command cache and geometry helpers.
#include "recipe.h"
#include <string.h>
static uint16_t swap01(uint16_t v){return (uint16_t)((v&0xfffcu)|((v&1u)<<1)|((v&2u)>>1));}
int cb_nor_recipe_make(enum cb_nor_method method,unsigned unit,bool swapped,struct cb_nor_recipe *out){
 struct cb_nor_recipe r={0};
 if(!out)return -1;
 memset(out,0,sizeof(*out));
 if((unit!=1&&unit!=2)||method<0||method>=CB_NOR_METHOD_COUNT)return -1;
 if(unit==1&&method!=CB_NOR_AMD_AAA)return -1;
 r.method=method;r.unit=(uint8_t)unit;r.swapped=swapped;
 r.unlock0=0xaaa*unit;r.unlock1=0x555*unit;
 /* Database profile strings are labels, not bus-address units. Mori's
  * ordinary GBA host uses WORD 555/2AA (BYTE AAA/554); the special raw
  * A9/MSP methods explicitly use WORD AAA/555. GB uses BYTE AAA/555. */
 if(unit==2&&method==CB_NOR_AMD_AAA){r.unlock0=0xaaa;r.unlock1=0x554;}
 r.aa=0xaa;r.bb=0x55;r.id=0x90;r.word=0xa0;r.buffer=0x25;
 r.confirm=0x29;r.erase=0x80;r.sector=0x30;r.reset=0xf0;
 r.ready_mask=0x80;r.error_mask=0x20;
 switch(method){
 case CB_NOR_AMD_555:r.unlock0=0x555*unit;r.unlock1=0x2aa*unit;break;
 case CB_NOR_AMD_A9:r.aa=0xa9;r.bb=0x56;r.buffer=0x26;r.confirm=0x2a;break;
 case CB_NOR_MSP54:
  r.aa=0x69;r.bb=0x96;r.id=0x50;r.word=0x60;r.buffer=0x26;
  r.confirm=0x2a;r.erase=0x40;r.raw_count=1;break;
 case CB_NOR_MSP55_AAA:case CB_NOR_MSP55_5555:
  if(method==CB_NOR_MSP55_5555){r.unlock0=0x5555*unit;r.unlock1=0x2aaa*unit;}
  r.aa=0xaaa9;r.bb=0x5556;r.id=0x9090;r.word=0xa0a0;r.buffer=0x2526;
  r.confirm=0x292a;r.erase=0x8080;r.sector=0x3030;r.reset=0xf0f0;
  r.ready_mask=0x8080;r.error_mask=0x2020;
  r.raw_count=1;break;
 case CB_NOR_INTEL:case CB_NOR_INTEL_DUAL:case CB_NOR_SHARP:
  r.unlock0=0;r.unlock1=0;r.aa=0;r.bb=0;r.word=0x40;r.buffer=0xe8;
  r.confirm=0xd0;r.erase=0x20;r.sector=0xd0;r.reset=0xff;r.error_mask=0x3a;break;
 default:break;
 }
 /* Special raw MSP sequences are deliberate, not generic D0/D1 transforms.
  * Data payloads remain raw bus bytes, exactly as in Mori. */
 if(swapped&&(method==CB_NOR_AMD_AAA||method==CB_NOR_AMD_555||method>=CB_NOR_INTEL)){
  r.aa=swap01(r.aa);r.bb=swap01(r.bb);r.id=swap01(r.id);r.word=swap01(r.word);
  r.buffer=swap01(r.buffer);r.confirm=swap01(r.confirm);r.erase=swap01(r.erase);
  r.sector=swap01(r.sector);r.reset=swap01(r.reset);
 }
 *out=r;return 0;
}
bool cb_nor_method_matches(const chisburner_nor_entry_t *e,enum cb_nor_method m){
 const char *s;
 if(!e||m<0||m>=CB_NOR_METHOD_COUNT)return false;
 if(e->bus==CHISBURNER_NOR_IDBUS_MBC5)
  return m==CB_NOR_AMD_AAA&&chisburner_nor_entry_cmdset(e)==CHISBURNER_NOR_CMDSET_AMD;
 if(e->bus!=CHISBURNER_NOR_IDBUS_GBA)return false;
 s=e->profile?e->profile:e->family?e->family->gba_profile:NULL;
 if(!s)return chisburner_nor_entry_cmdset(e)==CHISBURNER_NOR_CMDSET_AMD?
  m==CB_NOR_AMD_AAA:m==CB_NOR_INTEL;
 if(!strcmp(s,"AGB:AMD:AAA/555:AA/55"))return m==CB_NOR_AMD_AAA;
 if(!strcmp(s,"AGB:AMD:555/2AA:AA/55"))return m==CB_NOR_AMD_555;
 if(!strcmp(s,"AGB:AMD:AAA/555:A9/56"))return m==CB_NOR_AMD_A9;
 if(!strcmp(s,"AGB:AMD:MSP54LV:69/96/50"))return m==CB_NOR_MSP54;
 if(!strcmp(s,"AGB:AMD:MSP55LV100:wide-id"))return m==CB_NOR_MSP55_AAA||m==CB_NOR_MSP55_5555;
 if(!strcmp(s,"AGB:INTEL:direct-id"))return m==CB_NOR_INTEL;
 if(!strcmp(s,"AGB:INTEL:dual-direct-id"))return m==CB_NOR_INTEL_DUAL;
 if(!strcmp(s,"AGB:SHARP:direct-id"))return m==CB_NOR_SHARP;
 return false;
}
static bool geometry_valid(const struct cb_nor_cfi *g,unsigned unit){
 uint64_t total=0;
 if(unit!=1&&unit!=2)return false;
 if(!g->bytes||g->bytes>0x10000000u||!g->region_count||g->region_count>CB_NOR_REGIONS)return false;
 if(g->buffer_bytes&&(g->buffer_bytes<unit||(g->buffer_bytes&(g->buffer_bytes-1))))return false;
 for(unsigned i=0;i<g->region_count;i++){
  const struct cb_nor_region *r=&g->region[i];
  if(r->start!=total||!r->count||r->sector_bytes<unit||r->sector_bytes%unit)return false;
  total+=(uint64_t)r->count*r->sector_bytes;
  if(total>g->bytes)return false;
 }
 return total==g->bytes;
}
int cb_nor_plan_make(const chisburner_nor_entry_t *e,enum cb_nor_method m,bool swapped,
 const struct cb_nor_cfi *cfi,struct cb_nor_plan *out){
 struct cb_nor_plan p={0};
 uint32_t size,sector,buffer;
 unsigned unit;
 if(!out)return -1;
 memset(out,0,sizeof(*out));
 if(!e||!cb_nor_method_matches(e,m))return -1;
 unit=e->bus==CHISBURNER_NOR_IDBUS_GBA?2:1;
 if(cb_nor_recipe_make(m,unit,swapped,&p.recipe))return -1;
 p.entry=e;p.flags=chisburner_nor_entry_flags(e);
 size=chisburner_nor_entry_device_size(e);sector=chisburner_nor_entry_sector_size(e);
 buffer=chisburner_nor_entry_buffer_write_bytes(e);
 bool gl640=unit==1&&e->id[0]==0xc2&&e->id[1]==0x7e&&e->id[2]==0x10&&e->id[3]==0;
 /* This local extension has no uniform-sector fallback. A correct ID alone
  * must not turn missing/wrong CFI into write authority. */
 if(gl640&&(!cfi||cfi->bytes!=0x800000||cfi->buffer_bytes!=32||
    cfi->region_count!=2||cfi->region[0].count!=8||cfi->region[0].sector_bytes!=8192||
    cfi->region[1].count!=127||cfi->region[1].sector_bytes!=65536))return -2;
 if(p.flags&~31u)return -1;
 if(cfi){
  if(!geometry_valid(cfi,unit)||cfi->cmdset!=chisburner_nor_entry_cmdset(e))return -2;
  p.geometry=*cfi;
  if(unit==1&&e->id[0]==0xc2&&(e->id[1]==0xcb||e->id[1]==0xc9)){
   unsigned boot=e->id[1]==0xcb?0:1,large=1-boot;
   if(cfi->region_count!=2||cfi->region[boot].count!=8||
      cfi->region[boot].sector_bytes!=8192||cfi->region[large].count!=127||
      cfi->region[large].sector_bytes!=65536)return -2;
  }
  if(sector){
   uint32_t largest=0;
   for(unsigned i=0;i<cfi->region_count;i++)
    if(cfi->region[i].sector_bytes>largest)largest=cfi->region[i].sector_bytes;
   if(m==CB_NOR_INTEL_DUAL&&size==2*cfi->bytes)largest*=2;
   if(largest!=sector)return -2;
  }
  /* AMD stacked dies concatenate. Intel dual-direct boards interleave
   * halfwords: each region/sector spans twice the single-die byte count.
   * Acquisition must separately verify both dies before using this plan. */
  if(size&&size!=cfi->bytes){
   if(!(p.flags&CHISBURNER_NOR_FLAG_DUAL_DIE)||cfi->bytes!=size/2||
      (m!=CB_NOR_INTEL_DUAL&&cfi->region_count>CB_NOR_REGIONS/2))return -2;
   if(m==CB_NOR_INTEL_DUAL){
    for(unsigned i=0;i<cfi->region_count;i++){
     p.geometry.region[i].start*=2;p.geometry.region[i].sector_bytes*=2;
    }
    p.geometry.buffer_bytes*=2;
   }else{
    for(unsigned i=0;i<cfi->region_count;i++){
     p.geometry.region[i+cfi->region_count]=cfi->region[i];
     p.geometry.region[i+cfi->region_count].start+=cfi->bytes;
    }
    p.geometry.region_count*=2;
   }
   p.geometry.bytes=size;
  }
  /* Buffer limits are maxima, never enlarged beyond either source. */
  if(buffer&&p.geometry.buffer_bytes>buffer)p.geometry.buffer_bytes=buffer;
 }else{
  if(chisburner_nor_entry_cmdset(e)==CHISBURNER_NOR_CMDSET_INTEL)return -2;
  /* Mixed boot chips require CFI, except Mori's explicit MX29LV640 EB/ET layout. */
  if(unit==1&&e->id[0]==0xc2&&(e->id[1]==0xcb||e->id[1]==0xc9)){
   p.geometry.region_count=2;p.geometry.bytes=0x800000;
   if(e->id[1]==0xcb){
    p.geometry.region[0]=(struct cb_nor_region){0,8,8192};
    p.geometry.region[1]=(struct cb_nor_region){65536,127,65536};
   }else{
    p.geometry.region[0]=(struct cb_nor_region){0,127,65536};
    p.geometry.region[1]=(struct cb_nor_region){127*65536,8,8192};
   }
  }else{
   /* Exact Mori burner_gba_probe_load_entry_geometry fallback: a known
    * non-Intel entry with explicit device AND sector sizes is declared
    * uniform by that source. Zero/unknown sizes must never be guessed.
    * The probe must distinguish absent CFI from responding invalid CFI;
    * this branch must not override a contradictory query. */
   if(!size||!sector||sector>size||size%sector)return -2;
   p.geometry.bytes=size;p.geometry.region_count=1;
   p.geometry.region[0]=(struct cb_nor_region){0,size/sector,sector};
  }
  p.geometry.cmdset=chisburner_nor_entry_cmdset(e);p.geometry.buffer_bytes=buffer;
 }
 if(p.flags&CHISBURNER_NOR_FLAG_DISABLE_BUFFER_PROGRAM)p.geometry.buffer_bytes=0;
 if(p.geometry.buffer_bytes>1024)p.geometry.buffer_bytes=1024;
 if(p.flags&CHISBURNER_NOR_FLAG_INTEL_88B0)
  p.recipe.buffer=swapped?swap01(0xe9):0xe9;
 p.window_bytes=unit==2?0x2000000:0x4000;
 if(!geometry_valid(&p.geometry,unit))return -2;
 p.accessible_bytes=p.geometry.bytes; /* OS narrows to its verified route. */
 *out=p;return 0;
}
int cb_nor_sector_at(const struct cb_nor_plan *p,uint32_t a,uint32_t *start,uint32_t *length){
 if(!p||!start||!length||!geometry_valid(&p->geometry,p->recipe.unit)||a>=p->geometry.bytes)return -1;
 for(unsigned i=0;i<p->geometry.region_count;i++){
  const struct cb_nor_region *r=&p->geometry.region[i];
  uint32_t span=r->count*r->sector_bytes;
  if(a>=r->start&&a-r->start<span){
   *start=r->start+(a-r->start)/r->sector_bytes*r->sector_bytes;
   *length=r->sector_bytes;return 0;
  }
 }
 return -1;
}
size_t cb_nor_chunk(const struct cb_nor_plan *p,uint32_t a,size_t requested){
 uint32_t start,len,limit,remain;
 if(!p||!requested||a>=p->accessible_bytes||p->accessible_bytes>p->geometry.bytes||
    (p->recipe.unit!=1&&p->recipe.unit!=2)||
    a%p->recipe.unit||cb_nor_sector_at(p,a,&start,&len)||
    (p->window_bytes!=0x2000000&&p->window_bytes!=0x4000))return 0;
 limit=p->geometry.buffer_bytes?p->geometry.buffer_bytes:p->recipe.unit;
 if(requested>p->accessible_bytes-a)requested=p->accessible_bytes-a;
 remain=limit-a%limit;
 if(requested>remain)requested=remain;
 remain=len-(a-start);if(requested>remain)requested=remain;
 remain=p->window_bytes-a%p->window_bytes;if(requested>remain)requested=remain;
 return requested-requested%p->recipe.unit;
}
uint16_t cb_nor_count(const struct cb_nor_recipe *r,unsigned units){
 uint16_t n=(uint16_t)(units-1);
 return r->swapped&&!r->raw_count?swap01(n):n;
}
