#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "model.h"
#include "size107.h"
struct mock {int mode;unsigned hits;};
static int rd(void *v,unsigned bus,uint32_t addr,void *dst,unsigned n){
 struct mock *m=v;uint8_t *p=dst;assert(bus==2&&n==32);
 memset(p,0x42,n);
 if(addr>=65536)memset(p,0x11,n);
 if(addr==0x8000a0){
  unsigned hit=m->hits++;
  if(m->mode==3)memset(p,0x42,n); /* possible mirror must stay strict */
  if(m->mode==5&&hit==0)memset(p,0x42,n); /* one matching read is ambiguous */
  if(m->mode==4&&hit==1)return -9;
  if(hit==0&&m->mode<=3)p[m->mode==2?5:20]^=8;
 }
 if(addr==0x10000a0&&m->mode==6){
  /* Minish Cap trace: the 16 MiB candidate is outside the ROM, and even
   * its first 16 bytes vary. Neither read contains the original header. */
  unsigned hit=m->hits++;
  memset(p,0,n);p[2]=1;p[4]=hit?2:0;
 }
 return 0;
}
int main(void){
 for(int mode=1;mode<=6;mode++){
  struct mock m={mode,0};struct cb_probe_ops o={&m,rd,0,0};uint32_t size=0;
  int rc=cb_gba_size107(&o,&size);
  assert(rc==(mode==3||mode==5?-13:mode==4?-9:0));
  if(!rc)assert(size==0x2000000);
 }
 puts("PASS size152: non-mirror instability tolerated; possible mirrors and transport errors retained");
}
