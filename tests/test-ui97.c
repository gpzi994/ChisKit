#include "ui.h"
#include "of_input_types.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct mock {uint8_t gb[96],gba[192];int fail,unstable,damaged,empty,safefail;unsigned reads[3],safes;};
static int safe(void *v){struct mock *m=v;m->safes++;return m->safefail?-7:0;}
static int read_cart(void *v,unsigned bus,uint32_t a,void *p,unsigned n){
 struct mock *m=v;assert(n==32);assert(bus==1||bus==2);
 assert(bus==1?(a>=0x100&&a+32<=0x160):(a+32<=192));
 m->reads[bus]++;if(m->fail)return -9;
 memcpy(p,(bus==1?m->gb:m->gba)+(bus==1?a-0x100:a),32);
 if(m->empty)memset(p,255,n);
 if(m->unstable&&bus==1&&m->reads[bus]==6)((uint8_t*)p)[5]^=1;
 /* Each pass discards its first transaction; corrupt those deliberately. */
 if(m->reads[bus]==1||m->reads[bus]==(bus==1?5:8))memset(p,0x55,n);
 return 0;
}
static void init(struct mock *m){
 memset(m,0,sizeof(*m));memcpy(m->gb+4,cb_gb_logo,48);memcpy(m->gb+0x34,"ZELDA",5);
 m->gb[0x43]=0x80;m->gb[0x47]=0x1b;m->gb[0x48]=6;m->gb[0x49]=3;
 m->gb[0x4e]=0x37;m->gb[0x4f]=0x28;
 for(unsigned i=0x34;i<=0x4c;i++)m->gb[0x4d]-=m->gb[i]+1;
}
static void probe(struct cb_cart *c,struct mock *m){const struct cb_probe_ops o={m,read_cart,safe,0};cb_probe(c,&o);}
static void render(const char *name,struct cb_ui *u){
 uint8_t *b=malloc(CB_W*CB_H+32);assert(b);memset(b,0xa5,CB_W*CB_H+32);cb_render(b+16,u);
 for(unsigned i=0;i<16;i++){assert(b[i]==0xa5);assert(b[CB_W*CB_H+16+i]==0xa5);}
 if(name){char path[256];snprintf(path,sizeof(path),"build/ui97-preview/%s.ppm",name);FILE*f=fopen(path,"wb");assert(f);
  fprintf(f,"P6\n%d %d\n255\n",CB_W,CB_H);for(unsigned i=0;i<CB_W*CB_H;i++){assert(b[i+16]<16);uint32_t p=cb_palette[b[i+16]];fputc(p>>16,f);fputc(p>>8,f);fputc(p,f);}fclose(f);}
 free(b);
}
int main(int argc,char **argv){
 assert(argc==2);struct mock m;struct cb_cart c;init(&m);probe(&c,&m);
 assert(c.platform==CB_GB&&c.result==CB_READY&&m.reads[1]==8&&m.reads[2]==0&&m.safes==2);
 assert(!strcmp(c.mapper,"MBC5")&&c.rom_bytes==2097152&&c.ram_bytes==32768);
 struct cb_ui u={.cart=c};render("01-functions",&u);u.page=CB_INFO;render("02-gb-info",&u);
 init(&m);m.gb[0x47]=0x10;m.gb[0x4d]+=0x0b;probe(&c,&m);assert(c.platform==CB_GB&&!strcmp(c.mapper,"MBC3"));
 init(&m);m.gb[0x4d]^=1;probe(&c,&m);assert(c.result==CB_BAD_HEADER&&m.reads[2]==0);
 init(&m);m.unstable=1;probe(&c,&m);assert(c.platform==CB_GB&&c.result==CB_READY&&m.reads[1]==16&&m.reads[2]==0);
 init(&m);m.fail=1;probe(&c,&m);assert(c.result==CB_IO&&m.safes==5&&m.reads[1]==2&&m.reads[2]==2);
 init(&m);m.safefail=1;probe(&c,&m);assert(c.result==CB_IO&&!m.reads[1]);
 init(&m);m.empty=1;probe(&c,&m);assert(c.platform==CB_UNKNOWN&&c.result==CB_EMPTY);
 init(&m);memset(m.gb,255,96);FILE *f=fopen(argv[1],"rb");assert(f);assert(fread(m.gba,1,192,f)==192);fclose(f);
 probe(&c,&m);assert(c.platform==CB_GBA&&c.result==CB_READY&&m.reads[1]==8&&m.reads[2]==14);
 u.cart=c;u.page=CB_INFO;render("03-gba-info",&u);u.page=CB_TRANSFER;render("04-transfer",&u);
 m.gba[0xbd]^=1;probe(&c,&m);assert(c.platform==CB_UNKNOWN);
 u.page=CB_FUNCTIONS;u.options=1;render("05-options",&u);cb_key(&u,OF_BTN_B);assert(!u.options);
 for(unsigned i=2;i<6;i++){u.focus=i;assert(cb_key(&u,OF_BTN_A)==0);assert(u.page==CB_FUNCTIONS&&u.notice);}
 u.focus=0;assert(cb_key(&u,OF_BTN_A)==1);u.focus=1;assert(cb_key(&u,OF_BTN_A)==2);
 cb_key(&u,OF_BTN_R1);assert(u.page==CB_INFO);cb_key(&u,OF_BTN_R1);assert(u.page==CB_TRANSFER);cb_key(&u,OF_BTN_B);assert(u.page==CB_FUNCTIONS);
 for(unsigned p=0;p<3;p++)for(unsigned r=0;r<6;r++)for(unsigned fcs=0;fcs<8;fcs++){
  u.page=p;u.focus=fcs;u.cart.result=r;u.cart.platform=CB_UNKNOWN;render(0,&u);}
 puts("PASS: header parsing/probe, damaged/unstable/I/O/empty guards, UI bounds, keys, transfers disabled");return 0;
}
