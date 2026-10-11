#include "ui.h"
#include "of_input_types.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const uint8_t geometry[32]={
 0xc2,0x7e,0x10,0,7,0,32,0,126,0,0,1,7,0,32,0,126,0,0,1,
 'P','R','I','1','3',0x14,2,2,23,5,0,2};
static of_cart_flash_caps_t flash(void){
 of_cart_flash_caps_t f={0};f.struct_size=sizeof(f);f.version=OF_CART_FLASH_CAPS_VERSION;
 f.id_prefix=0x227e0001;f.cfi_signature=0x00520051;f.command_set=OF_CART_FLASH_CMDSET_AMD;
 f.profile_status=1;f.flash_size_bytes=0x2000000;f.sector_size_bytes=0x20000;f.program_buffer_bytes=512;
 return f;
}
static void preview(const char *name,struct cb_ui *u){
 uint8_t *b=malloc(CB_W*CB_H+32);assert(b);memset(b,0xa5,CB_W*CB_H+32);cb_render(b+16,u);
 for(int i=0;i<16;i++)assert(b[i]==0xa5&&b[CB_W*CB_H+16+i]==0xa5);
 char p[128];snprintf(p,sizeof(p),"build/ui98-preview/%s.ppm",name);FILE *f=fopen(p,"wb");assert(f);
 fprintf(f,"P6\n%d %d\n255\n",CB_W,CB_H);
 for(unsigned i=0;i<CB_W*CB_H;i++){assert(b[i+16]<16);uint32_t c=cb_palette[b[i+16]];fputc(c>>16,f);fputc(c>>8,f);fputc(c,f);}
 fclose(f);free(b);
}
struct mock {uint8_t samples[352];int reads,seq,safes,fail_at,fail_always,safe_fail,flash_rc,alias,nor_rc,bad_id,bad_geom;};
static int safe(void *v){struct mock*m=v;m->seq=0;return ++m->safes==m->safe_fail?-7:0;}
static int read_space(void *v,unsigned bus,unsigned space,uint32_t off,void *p,unsigned n){
 struct mock*m=v;static const unsigned req[]={0,32,0x4000,0x4020,0,32,0x4000,0x4020,0x100,0x120,0x140};
 assert(bus==1&&space==4&&n==32);int i=m->seq++,call=++m->reads;if(m->fail_always||call==m->fail_at)return OF_ERR_IO;
 if(i<11){assert(off==0x10000+req[i]);memcpy(p,m->samples+32*i,32);}
 else{assert(i==11&&off==0);if(m->nor_rc)return m->nor_rc;memcpy(p,geometry,32);if(m->bad_id)((uint8_t*)p)[0]^=1;if(m->bad_geom)((uint8_t*)p)[4]^=1;}return 0;
}
static int fc(void*v,of_cart_flash_caps_t*f){struct mock*m=v;if(m->flash_rc)return m->flash_rc;*f=flash();return 0;}
static int cc(void*v,of_cart_caps_t*c){(void)v;memset(c,0,sizeof(*c));c->cart_profile_raw=0x17;return 0;}
static void init(struct mock*m,struct cb_cart*c,int type,int alias){
 memset(m,0,sizeof(*m));for(int i=0;i<64;i++)m->samples[i]=(uint8_t)(i+3);
 for(int i=0;i<64;i++)m->samples[64+i]=m->samples[i]^(alias?0:0x55);
 memcpy(m->samples+128,m->samples,128);uint8_t *h=m->samples+256;memcpy(h+4,cb_gb_logo,48);
 memcpy(h+0x34,"ZELDA",5);h[0x43]=0x80;h[0x47]=(uint8_t)type;h[0x48]=6;h[0x49]=3;
 for(unsigned i=0x34;i<=0x4c;i++)h[0x4d]-=h[i]+1;
 memset(c,0,sizeof(*c));assert(cb_parse_gb(c,h));c->result=CB_READY;
}
void analysis_tests(void){
 struct cb_analysis a={0};uint8_t s[32];memcpy(s,geometry,32);
 assert(!cb_gb_geometry(&a,s,0x17)&&a.matched&&!a.writable&&a.bytes==8388608);
 assert(a.region[1].start==65536&&a.region[1].count==127&&a.buffer==32);
 struct cb_ui u={.cart={.platform=CB_GB,.result=CB_READY},.analysis=a,.page=CB_INFO,.detail=1};
 strcpy(u.analysis.status,"NOR matched / GB write not validated");u.analysis.ran=1;u.analysis.mapper_ok=1;
 preview("06-gb-nor",&u);assert(!cb_action_enabled(&u,2)&&!cb_action_enabled(&u,4));
 s[24]='1';memset(&a,0,sizeof(a));assert(!cb_gb_geometry(&a,s,0x17));
 for(unsigned i=0;i<32;i++){
  memcpy(s,geometry,32);s[i]^=0xff;memset(&a,0,sizeof(a));
  if(i!=25&&i!=26)assert(cb_gb_geometry(&a,s,0x17)!=0&&!a.writable);
 }
 memcpy(s,geometry,32);memset(&a,0,sizeof(a));assert(cb_gb_geometry(&a,s,0x13)==0);
 memset(&a,0,sizeof(a));assert(cb_gb_geometry(&a,s,0x11)!=0);
 /* Synthetic datasheet LV snapshot, not a hardware capture. */
 uint8_t lv[32];memcpy(lv,geometry,32);lv[1]=0xcb;lv[2]=0;lv[3]=8;
 lv[24]='1';lv[25]=0;lv[29]=0;
 memset(&a,0,sizeof(a));assert(!cb_gb_geometry(&a,lv,0x17));
 assert(a.matched&&!a.writable&&a.bytes==8388608&&!a.buffer);
 assert(a.region[0].count==8&&a.region[0].bytes==8192&&a.region[1].start==65536);
 u.analysis=a;u.analysis.ran=1;strcpy(u.analysis.status,"NOR matched / GB write not validated");
 preview("18-lv-nor-readonly",&u);assert(!cb_action_enabled(&u,4));
 for(unsigned i=0;i<32;i++){
  memcpy(s,lv,32);s[i]^=255;memset(&a,0,sizeof(a));
  int rc=cb_gb_geometry(&a,s,0x17);
  if(i==2||i==3||i==25||i==26)assert(!rc&&a.matched&&!a.writable);
  else assert(rc&&!a.matched&&!a.writable);
 }
 for(unsigned bit=0;bit<5;bit++)if((1u<<bit)&0x13){
  memset(&a,0,sizeof(a));assert(cb_gb_geometry(&a,lv,0x17&~(1u<<bit))!=0&&!a.matched);
 }
 of_cart_flash_caps_t f=flash();memset(&a,0,sizeof(a));assert(!cb_gba_geometry(&a,&f));
 assert(a.bytes==33554432&&a.region[0].count==256&&a.buffer==512&&a.writable);
 u.cart.platform=CB_GBA;u.analysis=a;u.analysis.ran=1;strcpy(u.analysis.status,"NOR matched / GBA transfer ready");
 preview("07-gba-nor",&u);u.page=CB_FUNCTIONS;preview("08-gba-actions",&u);
 assert(cb_action_enabled(&u,2)&&cb_action_enabled(&u,4)&&!cb_action_enabled(&u,3)&&!cb_action_enabled(&u,5));
 u.focus=4;assert(cb_key(&u,OF_BTN_A)==5);u.busy=1;assert(!cb_key(&u,OF_BTN_A));assert(!cb_key(&u,OF_BTN_R1));u.busy=0;
 u.cart.result=CB_EMPTY;assert(!cb_action_enabled(&u,4));u.cart.result=CB_READY;
 u.analysis.writable=0;assert(cb_action_enabled(&u,2)&&!cb_action_enabled(&u,4));u.analysis.writable=1;
 f.program_buffer_bytes=32;memset(&a,0,sizeof(a));assert(cb_gba_geometry(&a,&f)!=0&&!a.writable);
 f=flash();f.id_prefix^=1;assert(cb_gba_geometry(&a,&f)!=0);
 struct mock m;struct cb_cart c;struct cb_analysis_ops o={&m,read_space,safe,fc,cc};
 init(&m,&c,0x1b,1);cb_analyze(&a,&c,&o);assert(!a.rc&&a.mapper_ok&&a.matched&&!a.writable&&m.reads==12&&m.safes==2);
 init(&m,&c,0x10,1);cb_analyze(&a,&c,&o);assert(!a.rc&&!a.mapper_ok&&a.matched&&m.reads==12);
 assert(a.header_ok&&a.mapper_warning&&!a.writable&&!strcmp(a.probe,"Bank0 samples equal")&&a.id[0]);
 init(&m,&c,0x10,0);cb_analyze(&a,&c,&o);assert(!a.rc&&a.mapper_ok&&a.matched&&!a.writable&&m.reads==12);
 init(&m,&c,0x1b,0);cb_analyze(&a,&c,&o);assert(!a.rc&&!a.mapper_ok&&a.matched&&a.mapper_warning&&!a.writable&&m.reads==12);
 init(&m,&c,0x03,0);cb_analyze(&a,&c,&o);assert(!a.rc&&a.header_ok&&!a.matched&&!a.writable&&!m.reads&&m.safes==2);
 init(&m,&c,0x10,1);m.nor_rc=OF_ERR_NOT_SUPPORTED;cb_analyze(&a,&c,&o);
 assert(!a.rc&&a.mapper_warning&&!a.matched&&!a.writable&&a.nor_rc==OF_ERR_NOT_SUPPORTED&&m.safes==2);
 init(&m,&c,0x10,1);m.bad_id=1;cb_analyze(&a,&c,&o);assert(!a.rc&&!a.matched&&!a.writable&&a.id[0]);
 init(&m,&c,0x10,1);m.bad_geom=1;cb_analyze(&a,&c,&o);assert(a.rc==OF_ERR_DENIED&&!a.matched&&!a.writable);
 init(&m,&c,0x10,1);cb_analyze(&a,&c,&o);u.cart=c;u.analysis=a;u.page=CB_INFO;u.detail=1;
 preview("14-mapper-warning",&u);assert(!cb_action_enabled(&u,4));
 for(int fail=1;fail<=12;fail++){init(&m,&c,0x1b,1);m.fail_at=fail;cb_analyze(&a,&c,&o);assert(!a.rc&&a.matched&&!a.writable&&m.safes==3);}
 init(&m,&c,0x1b,1);m.fail_always=1;cb_analyze(&a,&c,&o);assert(a.rc==OF_ERR_IO&&!a.matched&&!a.writable&&m.safes==4);
 init(&m,&c,0x1b,1);m.samples[128]^=1;cb_analyze(&a,&c,&o);assert(a.rc&&m.reads==11);
 assert(!strcmp(a.stage,"Repeat samples"));
 init(&m,&c,0x10,0);c.checksum^=1;cb_analyze(&a,&c,&o);assert(a.rc&&!strcmp(a.stage,"Header recheck")&&m.reads==11);
 u.cart=c;u.analysis=a;u.page=CB_INFO;u.detail=1;preview("12-gb-blocked-stage",&u);
 init(&m,&c,0x1b,1);m.safe_fail=2;cb_analyze(&a,&c,&o);assert(a.rc&&!a.matched);
 init(&m,&c,0x1b,1);c.platform=CB_GBA;cb_analyze(&a,&c,&o);assert(!a.rc&&a.writable&&!m.reads&&m.safes==2);
 init(&m,&c,0x1b,1);c.platform=CB_GBA;m.flash_rc=OF_ERR_NOT_SUPPORTED;cb_analyze(&a,&c,&o);
 assert(!a.rc&&!a.matched&&!a.writable&&!strcmp(a.status,"GBA readable / writing disabled"));
 init(&m,&c,0x1b,1);c.result=CB_EMPTY;cb_analyze(&a,&c,&o);assert(a.rc&&!a.writable&&!m.reads&&!m.safes);
 u.page=CB_TRANSFER;u.busy=1;u.task="Write ROM (erases NOR 32 MiB)";u.phase="Program ROM";
 u.done=8388608;u.total=33554432;u.elapsed_ms=20000;u.phase_ms=8000;preview("09-transfer-live",&u);
 u.confirm=1;u.phase="Press A again to confirm";strcpy(u.source_title,"POKEMON EMER");preview("10-confirm",&u);
 u.confirm=0;u.busy=0;u.finished=1;u.done=u.total;strcpy(u.hash,"0123456789abcdef0123456789abcdef");preview("11-complete",&u);
 puts("PASS: NOR geometry/PRI 1.1+1.3, mismatches, mapper header authority, I/O cleanup, UI gates and previews");
}
