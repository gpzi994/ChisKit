#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../app/ui98/ui.h"
#include "../app/ui98/gb_selection176.h"
#include "../app/ui98/gb_fallback181.h"
static uint8_t rom[384];
static int reads,safes,read_error,safe_error,changed;
static int read_rom(void *ctx,uint32_t a,void *out,unsigned n){
 (void)ctx;assert(n==32&&a+n<=384);++reads;
 if(read_error)return -4;
 memcpy(out,rom+a,n);if(changed&&reads==13)((uint8_t*)out)[0]^=1;
 return 0;
}
static int safe_rom(void *ctx){(void)ctx;++safes;return safe_error;}
static const struct gb181_ops ops={0,read_rom,safe_rom};
static struct cb_ui u;
static void header(unsigned type,unsigned ram){
 memset(&u,0,sizeof u);u.cart.platform=CB_GB;u.cart.result=CB_READY;
 uint8_t *h=u.gb_save175_header;memcpy(h+4,cb_gb_logo,48);h[0x47]=type;h[0x49]=ram;
 if(type==0x22){h[0x48]=5;memcpy(h+0x34,"KORO2 KIRBYKKKJ",15);}
 for(unsigned i=0x34;i<=0x4c;i++)h[0x4d]-=h[i]+1;
}
static uint32_t resolve(uint32_t content,int probe,int analysis,int writable,int db){
 u.gb_probe_rc=probe;
 u.gb_detected_bytes=gb181_capacity(u.gb_save175_header,content,probe,analysis,writable,db,&u.gb_capacity_source);
 gb176_selection(&u);return u.gb_save175_bytes;
}
int main(int argc,char **argv){
 header(0x1b,3);assert(resolve(0,0,0,1,0)==131072&&u.gb_capacity_source==2);
 assert(u.gb_choice==0&&strstr(u.cart.save,"default"));
 u.gb_choice=5;gb176_selection(&u);assert(u.gb_save175_bytes==32768&&strstr(u.cart.save,"manual"));
 u.gb_choice=0;gb176_selection(&u);assert(u.gb_save175_bytes==131072);
 header(0x1b,3);assert(resolve(32768,0,0,1,1)==32768&&u.gb_capacity_source==1);
 header(3,3);assert(resolve(0,0,0,0,1)==32768&&u.gb_capacity_source==3);
 assert(strstr(u.cart.save,"DB/header"));
 header(0x13,3);assert(resolve(0,0,0,0,1)==32768);
 header(0x1b,2);assert(resolve(0,0,0,0,1)==8192);
 header(9,2);assert(resolve(0,0,0,0,1)==8192);
 header(0x1b,3);assert(!resolve(0,0,0,0,0)&&!u.gb_save175_ready);
 u.gb_choice=5;gb176_selection(&u);assert(u.gb_save175_ready&&u.gb_save175_bytes==32768);
 header(0x1b,3);assert(!resolve(0,-4,0,1,1));u.gb_choice=7;gb176_selection(&u);assert(!u.gb_save175_ready);
 header(0x1b,3);assert(!resolve(0,0,-4,1,1));
 header(0x10,5);assert(!resolve(0,0,0,1,1)&&u.gb_detected_bytes==131072);
 assert(strstr(u.cart.save,"mapper limit"));u.gb_choice=6;gb176_selection(&u);assert(u.gb_save175_bytes==65536);
 header(0x13,3);assert(!resolve(0,0,0,1,1));u.gb_choice=5;gb176_selection(&u);assert(u.gb_save175_bytes==32768);
 header(6,0);assert(resolve(512,0,0,1,1)==512);assert(!resolve(0,0,0,1,1));
 header(0x22,0);assert(resolve(256,0,0,1,1)==256);assert(!resolve(0,0,0,1,1));
 header(0x20,4);assert(!resolve(0,0,0,1,1));
 header(3,1);assert(!resolve(0,0,0,0,1));
 header(0x1b,3);u.gb_save175_header[0x4d]^=1;assert(!resolve(0,0,0,1,1));
 header(0x1b,3);u.cart.platform=CB_GBA;assert(!resolve(0,0,0,1,1)&&!u.gb_save175_ready);
 for(int i=1;i<argc;i++){
  FILE *f=fopen(argv[i],"rb");assert(f);assert(fread(rom,1,sizeof rom,f)==sizeof rom);fclose(f);
  assert(gb181_database_match(rom)==1);
  reads=safes=0;assert(gb181_database_header(rom+0x100,&ops)==1);assert(reads==24&&safes==3);
  changed=1;reads=safes=0;assert(gb181_database_header(rom+0x100,&ops)<0&&safes==3);changed=0;
  read_error=1;reads=safes=0;assert(gb181_database_header(rom+0x100,&ops)<0&&safes==2);read_error=0;
  safe_error=-7;reads=safes=0;assert(gb181_database_header(rom+0x100,&ops)==-7&&!reads);safe_error=0;
  uint8_t expected[80];memcpy(expected,rom+0x100,80);expected[0x34]^=1;
  assert(gb181_database_header(expected,&ops)<0);
  printf("PASS actual database identity: %s\n",argv[i]);
 }
 memset(rom,0,sizeof rom);assert(!gb181_database_match(rom));
 puts("PASS GB181: blank defaults, DB/header, manual override, mapper limits, GBA isolation, failures, repeated identity reads");
}
