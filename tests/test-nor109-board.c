#include "board109.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 for(unsigned k=CB_BOARD109_F2A128;k<=CB_BOARD109_F2AULTRA;k++){
  struct cb_board109_profile p;
  struct cb_board109_write seq[8];
  uint8_t ids[8][8],repeat[8][8];
  assert(!cb_board109_profile(k,&p));
  uint32_t total=0;
  for(unsigned n=0;n<p.geometry.region_count;n++){
   assert(p.geometry.region[n].start==total);
   total+=p.geometry.region[n].count*p.geometry.region[n].sector_bytes;
  }
  assert(total==p.bytes&&p.geometry.buffer_bytes==64);
  if(k==CB_BOARD109_F2AULTRA){
   const uint32_t starts[]={0,0x40000,0x4000000,0x7fc0000,0x8000000,0x8040000,0xc000000,0xffc0000};
   assert(p.geometry.region_count==8&&p.reset_stride==0x400000);
   for(unsigned n=0;n<8;n++){
    bool small=n==0||n==3||n==4||n==7;
    assert(p.geometry.region[n].start==starts[n]);
    assert(p.geometry.region[n].count==(small?4u:255u));
    assert(p.geometry.region[n].sector_bytes==(small?0x10000u:0x40000u));
   }
  }
  assert(cb_board109_unlock(k,seq,8)==4);
  assert(seq[0].address==0x130eca8&&seq[1].value==0xa55a&&seq[3].value==0x9413);
  for(unsigned n=0;n<p.banks;n++){
   unsigned bank;uint32_t physical;
   assert(!cb_board109_map(k,n*0x2000000,&bank,&physical)&&bank==n&&!physical);
   uint32_t end=(n+1)*0x2000000;if(end>p.bytes)end=p.bytes;
   assert(!cb_board109_map(k,end-1,&bank,&physical)&&bank==n&&physical==(end-1)%0x2000000);
   assert(!cb_board109_bank_id(k,n,ids[n]));
   memcpy(repeat[n],ids[n],8);
   if(k==CB_BOARD109_F2AULTRA){
    assert(cb_board109_select(k,n,seq,8)==7);
    assert(seq[2].bus==3&&seq[2].address==6&&seq[2].value==(n<4?0:0x10));
    assert(seq[4].address==0xB5AC97*2&&seq[4].value==n%4*0x400);
    assert(ids[n][4]==(n%4<2?0x10:0x0d));
   }
  }
  assert(cb_board109_ids_match(k,&ids[0][0],&repeat[0][0],p.banks));
  repeat[p.banks-1][7]^=1;
  assert(!cb_board109_ids_match(k,&ids[0][0],&repeat[0][0],p.banks));
  assert(!cb_board109_ids_match(k,&ids[0][0],&ids[0][0],p.banks-1));
  unsigned bank=99;uint32_t physical=99;
  assert(cb_board109_map(k,p.bytes,&bank,&physical)&&!bank&&!physical);
  memset(seq,0xff,sizeof(seq));
  assert(!cb_board109_select(k,p.banks,seq,8));
  for(unsigned n=0;n<8;n++)assert(!seq[n].address&&!seq[n].value&&!seq[n].bus);
 }
 struct cb_board109_write shortseq[3];
 for(unsigned k=1;k<=3;k++)for(unsigned bank=0;bank<(k==3?8u:1u);bank++){
  struct cb_nor_cfi c={.cmdset=CHISBURNER_NOR_CMDSET_INTEL,.primary_id=1,.buffer_bytes=32};
  c.bytes=k==1?0x800000:k==2?0x1000000:0x2000000;
  if(k<3){c.region_count=1;c.region[0]=(struct cb_nor_region){0,c.bytes/0x20000,0x20000};}
  else{
   c.region_count=2;
   if(bank%4<2){c.region[0]=(struct cb_nor_region){0,4,0x8000};c.region[1]=(struct cb_nor_region){0x20000,255,0x20000};}
   else {c.region[0]=(struct cb_nor_region){0,255,0x20000};c.region[1]=(struct cb_nor_region){0x1fe0000,4,0x8000};}
  }
  assert(!cb_board109_match_die_cfi(k,bank,&c));
  struct cb_nor_cfi bad=c;bad.region[0].sector_bytes/=2;assert(cb_board109_match_die_cfi(k,bank,&bad));
  bad=c;bad.bytes/=2;assert(cb_board109_match_die_cfi(k,bank,&bad));
  bad=c;bad.region[0].count--;assert(cb_board109_match_die_cfi(k,bank,&bad));
  bad=c;bad.cmdset=CHISBURNER_NOR_CMDSET_AMD;assert(cb_board109_match_die_cfi(k,bank,&bad));
  bad=c;bad.buffer_bytes=16;assert(cb_board109_match_die_cfi(k,bank,&bad));
  if(k==3)assert(cb_board109_match_die_cfi(k,(bank+2)%8,&c));
 }
 /* A larger CFI span is accepted only if its COMPLETE map matches the
  * corresponding board slice; identical-size region splits are immaterial. */
 struct cb_nor_cfi combined={.cmdset=CHISBURNER_NOR_CMDSET_INTEL,.primary_id=3,
  .bytes=0x4000000,.buffer_bytes=32,.region_count=3,
  .region={{0,4,0x8000},{0x20000,510,0x20000},{0x3fe0000,4,0x8000}}};
 for(unsigned bank=0;bank<8;bank++)assert(!cb_board109_match_die_cfi(3,bank,&combined));
 memset(shortseq,0xff,sizeof(shortseq));
 assert(!cb_board109_unlock(CB_BOARD109_F2A128,shortseq,3));
 assert(!shortseq[2].value);
 puts("PASS board109: explicit 128M/256M/Ultra profiles; 8-bank routes and IDs; full geometry; bounds; no hardware I/O");
}
