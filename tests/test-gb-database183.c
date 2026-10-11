#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../app/ui98/gb_database183.h"
#include "../app/ui98/gb_fallback181.h"
static struct cb_cart cart;
static unsigned next,reads,safes,fail_read,fail_safe;
static int safe(void *ctx){(void)ctx;next=0;return ++safes==fail_safe?-7:0;}
static int read_block(void *ctx,uint32_t off,void *out,unsigned n){
 uint32_t a;assert(!gb183_address(ctx,off,n,&a));
 /* Model the private IDENTIFY aperture and RTL sequential guard. */
 assert((a>>24)==0x11&&(a&0xffffff)==next&&n==32);
 next+=32;if(++reads==fail_read)return -13;
 memset(out,0,n);return 0;
}
int main(void){
 uint32_t a;cart.platform=CB_GB;cart.result=CB_READY;cart.rom_bytes=32768;
 const unsigned types[]={9,3,6,0x13,0x10,0x1b,0x22,0xfc};
 for(unsigned mode=0;mode<8;mode++){
  cart.type=types[mode];cart.ram_bytes=mode==4?65536:32768;
  for(unsigned off=0;off<384;off+=32){assert(!gb183_address(&cart,off,32,&a));assert(a==0x10000000u+(mode<<24)+off);}
 }
 assert(gb183_address(&cart,384,32,&a));assert(gb183_address(&cart,1,32,&a));
 assert(gb183_address(&cart,0,64,&a));assert(gb183_address(&cart,0xffffffff,32,&a));
 cart.platform=CB_GBA;assert(gb183_address(&cart,0,32,&a));cart.platform=CB_GB;
 cart.type=0x20;assert(gb183_address(&cart,0,32,&a));cart.type=3;
 cart.result=CB_BAD_HEADER;assert(gb183_address(&cart,0,32,&a));cart.result=CB_READY;
 uint8_t expected[80]={0};struct gb181_ops ops={&cart,read_block,safe};
 assert(gb181_database_header(expected,&ops)==0&&reads==24&&safes==3);
 for(unsigned f=1;f<=24;f++){
  reads=safes=0;fail_read=f;assert(gb181_database_header(expected,&ops)==-13);assert(next==0);
 }
 fail_read=0;
 for(unsigned f=1;f<=3;f++){
  reads=safes=0;fail_safe=f;assert(gb181_database_header(expected,&ops)==-7);assert(next==0);
 }
 puts("PASS GB183: all mapper apertures, bounded identity window, sequential restart, read and cleanup failures");
}
