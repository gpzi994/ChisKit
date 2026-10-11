#include "ui.h"
#include "header_diag.h"
#include "of_input_types.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct mock {uint8_t gb[96],gba[192];int fail,unstable,damaged,empty,safefail,sizebad;unsigned reads[3],safes;};
static int safe(void *v){struct mock *m=v;m->safes++;return m->safefail?-7:0;}
static int read_cart(void *v,unsigned bus,uint32_t a,void *p,unsigned n){
 struct mock *m=v;assert(n==32);assert(bus==1||bus==2);
 assert(bus==1?(a>=0x100&&a+32<=0x160):(a+32<=0x2000000));
 m->reads[bus]++;if(m->fail)return -9;
 if(bus==2&&m->sizebad==2&&m->reads[bus]==17)return -9;
 if(bus==2&&a>=192){for(unsigned i=0;i<n;i++)((uint8_t*)p)[i]=(uint8_t)((a+i)^((a+i)>>8)^((a+i)>>16)^((a+i)>>24));}
 else memcpy(p,(bus==1?m->gb:m->gba)+(bus==1?a-0x100:a),32);
 if(m->empty)memset(p,255,n);
 if(m->unstable&&bus==1&&m->reads[bus]==6)((uint8_t*)p)[5]^=1;
 if(bus==2&&m->sizebad==1&&m->reads[bus]==17)((uint8_t*)p)[5]^=1;
 /* Each pass discards its first transaction; corrupt those deliberately. */
 if(m->reads[bus]==1||m->reads[bus]==(bus==1?5:8)||(bus==2&&m->reads[bus]==15))memset(p,0x55,n);
 return 0;
}
static void init(struct mock *m){
 memset(m,0,sizeof(*m));memcpy(m->gb+4,cb_gb_logo,48);memcpy(m->gb+0x34,"ZELDA",5);
 m->gb[0x43]=0x80;m->gb[0x47]=0x1b;m->gb[0x48]=6;m->gb[0x49]=3;
 m->gb[0x4e]=0x37;m->gb[0x4f]=0x28;
 for(unsigned i=0x34;i<=0x4c;i++)m->gb[0x4d]-=m->gb[i]+1;
}
static void probe(struct cb_cart *c,struct mock *m){
 const struct cb_probe_ops o={m,read_cart,safe,0};cb_probe(c,&o);
 assert(cb_header150_trace.count<=64);
 if(m->safefail)assert(cb_header150_trace.count==0);
 else{
  assert(cb_header150_trace.count>0);
  assert(cb_header150_trace.reads[0].bus==1);
  assert(cb_header150_trace.reads[0].address==0x100);
  assert(cb_header150_trace.reads[0].rc==(m->fail?-9:0));
 }
}
static void render(const char *name,struct cb_ui *u){
 uint8_t *b=malloc(CB_W*CB_H+32);assert(b);memset(b,0xa5,CB_W*CB_H+32);cb_render(b+16,u);
 for(unsigned i=0;i<16;i++){assert(b[i]==0xa5);assert(b[CB_W*CB_H+16+i]==0xa5);}
 if(name){char path[256];snprintf(path,sizeof(path),"build/ui98-preview/%s.ppm",name);FILE*f=fopen(path,"wb");assert(f);
  fprintf(f,"P6\n%d %d\n255\n",CB_W,CB_H);for(unsigned i=0;i<CB_W*CB_H;i++){assert(b[i+16]<16);uint32_t p=cb_palette[b[i+16]];fputc(p>>16,f);fputc(p>>8,f);fputc(p,f);}fclose(f);}
 free(b);
}
void analysis_tests(void);
int main(int argc,char **argv){
 analysis_tests();
 assert(argc==2);struct mock m;struct cb_cart c;init(&m);probe(&c,&m);
 assert(c.platform==CB_GB&&c.result==CB_READY&&m.reads[1]==8&&m.reads[2]==0&&m.safes==2);
 assert(!strcmp(c.mapper,"MBC5")&&c.rom_bytes==2097152&&c.ram_bytes==32768);
 struct cb_ui u={.cart=c};render("01-functions",&u);u.page=CB_INFO;render("02-gb-info",&u);
 init(&m);m.gb[0x47]=0x10;m.gb[0x4d]+=0x0b;probe(&c,&m);assert(c.platform==CB_GB&&!strcmp(c.mapper,"MBC3"));
 for(unsigned type=0x0f;type<=0x1e;type++)for(unsigned ram=0;ram<=6;ram++){
  init(&m);m.gb[0x47]=(uint8_t)type;m.gb[0x49]=(uint8_t)ram;m.gb[0x4d]=0;
  for(unsigned i=0x34;i<=0x4c;i++)m.gb[0x4d]-=m.gb[i]+1;
  memset(&c,0,sizeof(c));assert(cb_parse_gb(&c,m.gb));
  assert((!strcmp(c.mapper,"MBC30"))==(type==0x10&&ram==5));
  assert(c.type==type);
  if(type==0x10&&ram==5){
   assert(c.ram_bytes==65536u);probe(&c,&m);assert(!strcmp(c.mapper,"MBC30"));
   u.cart=c;u.page=CB_INFO;render("13-mbc30-header",&u);
   assert(!cb_action_enabled(&u,4));
  }
 }
 for(unsigned j=0;j<2;j++){
  init(&m);m.gb[0x47]=j?0xfc:0x22;m.gb[0x4d]=0;
  for(unsigned i=0x34;i<=0x4c;i++)m.gb[0x4d]-=m.gb[i]+1;
  probe(&c,&m);assert(c.result==CB_READY);
  assert(!strcmp(c.mapper,j?"Camera (MBC-GBD)":"MBC7"));
  assert(cb_header_diag.mask==7&&!cb_header_diag.rc);
  assert(!memcmp(cb_header_diag.bytes,m.gb,96));
  u.cart=c;u.page=CB_INFO;render(j?"16-camera-header":"15-mbc7-header",&u);
 }
 init(&m);m.gb[0x4d]^=1;probe(&c,&m);assert(c.result==CB_BAD_HEADER&&m.reads[2]==0);
 assert(cb_header_diag.mask==7&&cb_header_diag.bytes[0x4d]==m.gb[0x4d]);
 u.cart=c;u.page=CB_INFO;render("17-invalid-header-diagnostic",&u);
 init(&m);m.unstable=1;probe(&c,&m);assert(c.platform==CB_GB&&c.result==CB_READY&&m.reads[1]==16&&m.reads[2]==0);
 init(&m);m.fail=1;probe(&c,&m);assert(c.result==CB_IO&&m.safes==5&&m.reads[1]==2&&m.reads[2]==2);
 init(&m);m.safefail=1;probe(&c,&m);assert(c.result==CB_IO&&!m.reads[1]);
 init(&m);m.empty=1;probe(&c,&m);assert(c.platform==CB_UNKNOWN&&c.result==CB_EMPTY);
 init(&m);memset(m.gb,255,96);FILE *f=fopen(argv[1],"rb");assert(f);assert(fread(m.gba,1,192,f)==192);fclose(f);
 probe(&c,&m);assert(c.platform==CB_GBA&&c.result==CB_READY&&m.reads[1]==8&&m.reads[2]==41&&c.rom_bytes==0x2000000);
 u.cart=c;u.page=CB_INFO;render("03-gba-info",&u);u.page=CB_TRANSFER;render("04-transfer",&u);
 for(int mode=1;mode<=2;mode++){
  struct mock bad=m;memset(bad.reads,0,sizeof bad.reads);bad.sizebad=mode;
  struct cb_cart failed;probe(&failed,&bad);
  assert(failed.header_valid&&failed.result==CB_IO&&failed.rom_bytes==0);
  assert(failed.rc==(mode==1?-13:-9));
  assert(cb_size151_trace.count==3&&cb_size151_trace.last[0].address==0xa0&&cb_size151_trace.last[1].address==0xa0);
  assert(cb_size151_trace.last[1].rc==(mode==1?0:-9));
  struct cb_ui blocked={0};blocked.cart=failed;blocked.page=CB_INFO;
  assert(!cb_action_enabled(&blocked,2)&&!cb_action_enabled(&blocked,4));
  render(mode==1?"18-size-mismatch":"19-size-io",&blocked);
 }
 m.gba[0xbd]^=1;probe(&c,&m);assert(c.platform==CB_UNKNOWN);
 u.page=CB_FUNCTIONS;u.options=1;render("05-options",&u);cb_key(&u,OF_BTN_B);assert(!u.options);
 /* A valid retail GBA may always be backed up; destructive writes still
  * require the separately matched writable-NOR analysis. */
 u.focus=2;assert(cb_key(&u,OF_BTN_A)==3);assert(u.page==CB_FUNCTIONS&&!u.notice);
 for(unsigned i=3;i<6;i++){u.focus=i;assert(cb_key(&u,OF_BTN_A)==0);assert(u.page==CB_FUNCTIONS&&u.notice);}
 u.focus=0;assert(cb_key(&u,OF_BTN_A)==1);u.focus=1;assert(cb_key(&u,OF_BTN_A)==2);
 cb_key(&u,OF_BTN_R1);assert(u.page==CB_INFO);cb_key(&u,OF_BTN_R1);assert(u.page==CB_TRANSFER);cb_key(&u,OF_BTN_B);assert(u.page==CB_FUNCTIONS);
 for(unsigned p=0;p<3;p++)for(unsigned r=0;r<6;r++)for(unsigned fcs=0;fcs<8;fcs++){
  u.page=p;u.focus=fcs;u.cart.result=r;u.cart.platform=CB_UNKNOWN;render(0,&u);}
 u=(struct cb_ui){0};u.cart.platform=CB_GBA;u.cart.result=CB_EMPTY;
 assert(!cb_action_enabled(&u,4));
 u.analysis.matched=u.analysis.writable=1;
 assert(!cb_action_enabled(&u,4));
 u.analysis.nor.version=OF_NOR_VERSION;u.analysis.nor.bus=2;u.analysis.nor.flags=OF_NOR_WRITE_OK;
 assert(cb_action_enabled(&u,4));assert(!cb_action_enabled(&u,2));
 u.cart.safe_rc=-1;assert(!cb_action_enabled(&u,4));u.cart.safe_rc=0;
 u.analysis.nor.plan_rc=-1;assert(!cb_action_enabled(&u,4));
 puts("PASS: header parsing/probe, damaged/unstable/I/O/empty guards, UI bounds, keys, headerless fresh-NOR write gate");return 0;
}
