#include <assert.h>
#include <stdio.h>
#include "../core/save/flash170.h"
static uint8_t mem[131072],original[131072],source[131072];
static unsigned bank,state,page_count,page_addr,device,tick,erase_count,program_count,writes,fail_at;
static int dead,stuck;
static int wr(void*c,uint32_t a,uint8_t v){
 (void)c;assert(a<65536);if(dead)return -4;
 if(++writes==fail_at){dead=1;return -4;}
 if(state==4){bank=v;assert(a==0&&v<=1);state=0;return 0;}
 if(state==3){
  if(device==0x1f3d){
   if(!page_count){page_addr=a;assert(!(a&127));}
   assert(a==page_addr+page_count);mem[a]=v;page_count++;
   if(page_count==128){page_count=0;state=0;program_count++;}
  }else{mem[bank*65536+a]&=v;program_count++;state=0;}return 0;
 }
 if(a==0&&v==0xf0){state=0;return 0;}
 if(!state){assert(a==0x5555&&v==0xaa);state=1;}
 else if(state==1){assert(a==0x2aaa&&v==0x55);state=2;}
 else if(state==2){assert(a==0x5555);if(v==0xa0)state=3;else if(v==0xb0)state=4;else{assert(v==0x80&&device!=0x1f3d);state=5;}}
 else if(state==5){assert(a==0x5555&&v==0xaa);state=6;}
 else if(state==6){assert(a==0x2aaa&&v==0x55);state=7;}
 else{assert(state==7&&v==0x30&&!(a&4095));memset(mem+bank*65536+a,0xff,4096);erase_count++;state=0;}
 return 0;
}
static int rd(void*c,uint32_t a,uint8_t*v){(void)c;assert(a<65536);if(dead)return -4;assert(!state);*v=stuck&&erase_count?0:mem[bank*65536+a];return 0;}
static int delay(void*c,unsigned us){(void)c;tick+=us;return dead?-4:0;}
static uint32_t ms(void*c){(void)c;return tick/1000;}
static const struct cb_flash170_ops ops={NULL,rd,wr,delay,ms};
static void reset(unsigned id){
 device=id;bank=state=page_count=tick=erase_count=program_count=writes=fail_at=0;dead=stuck=0;
 for(unsigned i=0;i<sizeof mem;i++)mem[i]=original[i]=source[i]=(uint8_t)(i*31+i/257);
}
int main(void){
 struct cb_flash170_report r;unsigned ids[]={0x1f3d,0xbfd4,0xc21c,0x321b,0xc209,0x6213};
 for(unsigned j=0;j<6;j++){
  unsigned id=ids[j],n=cb_flash170_size(id);reset(id);
  assert(!cb_flash170_run(&ops,id,source,original,n,&r)&&r.verified==n&&!r.attempted&&!program_count&&!erase_count&&!bank);
  reset(id);for(unsigned i=0;i<n;i++)source[i]^=0x5a;
  assert(!cb_flash170_run(&ops,id,source,original,n,&r)&&r.verified==n&&!memcmp(mem,source,n)&&!bank);
  assert(id==0x1f3d?!erase_count&&program_count==512:erase_count==n/4096);
 }
 reset(0xc209);source[65535]^=1;source[65536]^=1;
 assert(!cb_flash170_run(&ops,device,source,original,131072,&r)&&erase_count==2&&!memcmp(mem,source,131072)&&!bank);
 reset(0x1f3d);original[42]^=1;assert(cb_flash170_run(&ops,device,source,original,65536,&r)&&!program_count&&!erase_count);
 reset(0xc209);assert(cb_flash170_run(&ops,device,source,original,65536,&r)&&!writes);
 reset(0xffff);assert(cb_flash170_run(&ops,device,source,original,131072,&r)&&!writes);
 reset(0xc21c);source[0]^=1;stuck=1;assert(cb_flash170_run(&ops,device,source,original,65536,&r)==-5&&!program_count);
 for(unsigned f=1;f<=20;f++){
  reset(0xc209);source[0]^=1;fail_at=f;
  assert(cb_flash170_run(&ops,device,source,original,131072,&r));assert(writes==fail_at&&dead);
 }
 puts("Flash170 PASS: six IDs, no-op, Atmel pages, sector byte programming, both banks/boundary, backup mismatch, size/ID rejection, timeout, lifecycle faults; mock bus only");
 return 0;
}
