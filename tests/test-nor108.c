// SPDX-License-Identifier: AGPL-3.0-only
#include "cfi.h"
#include "mori/chisburner_nor_db_data.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void uniform(uint16_t w[512]) {
 memset(w,0,1024);w[0x10]='Q';w[0x11]='R';w[0x12]='Y';
 w[0x13]=2;w[0x15]=0x40;w[0x27]=25;w[0x2a]=10;
 w[0x2c]=1;w[0x2d]=255;w[0x30]=2;
 w[0x40]='P';w[0x41]='R';w[0x42]='I';w[0x43]='1';w[0x44]='3';w[0x5e]=2;
}
static void bad(uint16_t *w,size_t n) {
 struct cb_nor_cfi c,zero={0};memset(&c,0xa5,sizeof(c));
 assert(cb_nor_decode_cfi(w,n,&c)!=0);assert(!memcmp(&c,&zero,sizeof(c)));
}
static void table(const chisburner_nor_entry_t *t,size_t n,unsigned width) {
 for(size_t i=0;i<n;i++) {
  assert(t[i].id_len==width);
  const chisburner_nor_entry_t *e=width==8?chisburner_nor_db_lookup_gba(t[i].id):chisburner_nor_db_lookup_mbc5(t[i].id);
  assert(e);assert(strcmp(chisburner_nor_entry_name(e),"unknown"));
  /* Exercise masked-ID matching with all ignored bits inverted. */
  uint8_t id[8];for(unsigned j=0;j<8;j++)id[j]=t[i].id[j] ^ (uint8_t)~t[i].id_mask[j];
  e=width==8?chisburner_nor_db_lookup_gba(id):chisburner_nor_db_lookup_mbc5(id);
  assert(e);
 }
}
int main(void) {
 table(g_chisburner_nor_db_gba,g_chisburner_nor_db_gba_count,8);
 table(g_chisburner_nor_db_mbc5,g_chisburner_nor_db_mbc5_count,4);
 assert(!chisburner_nor_db_lookup_gba(NULL));
 uint8_t unknown[8]={0};assert(!chisburner_nor_db_lookup_gba(unknown));
 uint8_t js[]={0x89,0,0x7e,0x22,0x22,0x22,1,0x22};
 const chisburner_nor_entry_t *e=chisburner_nor_db_lookup_gba(js);assert(e);
 assert(chisburner_nor_entry_device_size(e)==0x2000000);
 assert(chisburner_nor_entry_buffer_write_bytes(e)==1024);
 assert(chisburner_nor_entry_cmdset(e)==CHISBURNER_NOR_CMDSET_AMD);
 uint16_t w[512];struct cb_nor_cfi c;uniform(w);
 assert(!cb_nor_decode_cfi(w,512,&c));assert(c.bytes==0x2000000 && c.buffer_bytes==1024);
 assert(!cb_nor_match_cfi(e,&c));
 uniform(w);w[0x2a]=1;assert(!cb_nor_decode_cfi(w,512,&c)&&!c.buffer_bytes);
 c.cmdset=CHISBURNER_NOR_CMDSET_INTEL;assert(cb_nor_match_cfi(e,&c));
 uniform(w);for(unsigned i=0;i<512;i++)w[i]<<=8;
 assert(!cb_nor_decode_cfi(w,512,&c));assert(c.lane==1);
 uniform(w);for(unsigned i=0;i<512;i++)w[i]|=w[i]<<8;
 assert(!cb_nor_decode_cfi(w,512,&c));w[0x27]^=0x100;bad(w,512);
 uniform(w);bad(w,511);w[0x10]=0;bad(w,512);
 uniform(w);w[0x27]=31;bad(w,512);
 uniform(w);w[0x2b]=1;bad(w,512);
 uniform(w);w[0x2c]=9;bad(w,512);
 uniform(w);w[0x2d]=254;bad(w,512);
 uniform(w);w[0x30]=0;bad(w,512);
 /* 8 MiB boot layout: eight 8 KiB sectors plus 127 64 KiB sectors. */
 uniform(w);w[0x27]=23;w[0x2a]=5;w[0x2c]=2;
 w[0x2d]=7;w[0x2f]=32;w[0x30]=0;
 w[0x31]=126;w[0x32]=0;w[0x33]=0;w[0x34]=1;
 assert(!cb_nor_decode_cfi(w,512,&c));assert(c.region[1].start==65536);
 w[0x5e]=3;assert(!cb_nor_decode_cfi(w,512,&c));
 assert(c.region[0].sector_bytes==65536 && c.region[1].start==127u*65536u);
 assert(cb_nor_match_cfi(e,&c));
 w[0x13]=1;assert(!cb_nor_decode_cfi(w,512,&c));assert(c.region[0].sector_bytes==8192);
 /* Deterministic malformed-input stress under sanitizers. */
 uint32_t seed=108;
 for(unsigned k=0;k<20000;k++) {
  for(unsigned i=0;i<512;i++){seed=seed*1664525u+1013904223u;w[i]=(uint16_t)(seed>>8);}
  if(k&1){w[0x10]='Q';w[0x11]='R';w[0x12]='Y';}
  (void)cb_nor_decode_cfi(w,512,&c);
 }
 printf("PASS: GBA=%zu GB/MBC5=%zu database records; CFI bounds, lanes, geometry, conflict tests\n",g_chisburner_nor_db_gba_count,g_chisburner_nor_db_mbc5_count);
 return 0;
}
