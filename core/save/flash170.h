#ifndef CB_FLASH170_H
#define CB_FLASH170_H
#include <stdint.h>
#include <string.h>
/* FlashGBX LK_Device restore: 4 KiB sectors, AA/55/B0 banks.
 * LK.c lk_agb_cart_write_flash: AT29LV512 128-byte pages, 10 ms settle.
 * No RTC, ROM bus, PSRAM, or raw application write entry point. */
struct cb_flash170_ops {
 void *ctx;
 int (*read)(void *,uint32_t,uint8_t *);
 int (*write)(void *,uint32_t,uint8_t);
 int (*delay_us)(void *,unsigned);
 uint32_t (*milliseconds)(void *);
};
struct cb_flash170_report {int32_t rc,cleanup_rc;uint32_t id,stage,address,attempted,verified;};
static inline uint32_t cb_flash170_size(unsigned id){
 switch(id){case 0x1f3d:case 0xbfd4:case 0xc21c:case 0x321b:return 65536;
 case 0xc209:case 0x6213:return 131072;default:return 0;}
}
static int cb_flash170_cmd(const struct cb_flash170_ops *o,uint8_t cmd){
 int r=o->write(o->ctx,0x5555,0xaa);if(!r)r=o->write(o->ctx,0x2aaa,0x55);
 return r?r:o->write(o->ctx,0x5555,cmd);
}
static int cb_flash170_bank(const struct cb_flash170_ops *o,unsigned bank){
 int r=cb_flash170_cmd(o,0xb0);if(!r)r=o->write(o->ctx,0,(uint8_t)bank);
 return r?r:o->delay_us(o->ctx,10000);
}
static int cb_flash170_poll(const struct cb_flash170_ops *o,uint32_t a,uint8_t value,unsigned ms){
 uint32_t start=o->milliseconds(o->ctx);unsigned stable=0;
 do{uint8_t v;int r=o->read(o->ctx,a,&v);if(r)return r;
  stable=v==value?stable+1:0;if(stable==2)return 0;
  r=o->delay_us(o->ctx,20);if(r)return r;
 }while((uint32_t)(o->milliseconds(o->ctx)-start)<ms);
 return -5;
}
static int cb_flash170_run(const struct cb_flash170_ops *o,unsigned id,const uint8_t *source,
 const uint8_t *original,uint32_t size,struct cb_flash170_report *r){
 memset(r,0,sizeof *r);r->id=id;
 if(!o||!o->read||!o->write||!o->delay_us||!o->milliseconds||!source||!original||
    !size||size!=cb_flash170_size(id))return r->rc=-1;
 unsigned banks=size/65536u,page=id==0x1f3d?128u:4096u;
 r->stage=1;
 /* Fresh comparison of persistent backup before ANY erase/program. */
 for(unsigned b=0;b<banks&&!r->rc;b++){
  if(banks==2)r->rc=cb_flash170_bank(o,b);
  for(unsigned a=0;a<65536&&!r->rc;a++){
   uint8_t v;r->rc=o->read(o->ctx,a,&v);r->address=b*65536+a;
   if(!r->rc&&v!=original[r->address])r->rc=-2;
  }
 }
 for(unsigned b=0;b<banks&&!r->rc;b++){
  if(banks==2)r->rc=cb_flash170_bank(o,b);
  for(unsigned a=0;a<65536&&!r->rc;a+=page){
   unsigned off=b*65536+a;r->address=off;
   if(!memcmp(source+off,original+off,page))continue;
   r->attempted+=page;r->stage=2;
   if(id!=0x1f3d){
    r->rc=cb_flash170_cmd(o,0x80);
    if(!r->rc)r->rc=o->write(o->ctx,0x5555,0xaa);
    if(!r->rc)r->rc=o->write(o->ctx,0x2aaa,0x55);
    if(!r->rc)r->rc=o->write(o->ctx,a,0x30);
    if(!r->rc)r->rc=o->delay_us(o->ctx,10000);
    if(!r->rc)r->rc=cb_flash170_poll(o,a,0xff,500);
   }
   r->stage=3;
   if(id==0x1f3d){
    if(!r->rc)r->rc=cb_flash170_cmd(o,0xa0);
    for(unsigned i=0;i<128&&!r->rc;i++)r->rc=o->write(o->ctx,a+i,source[off+i]);
    if(!r->rc)r->rc=o->delay_us(o->ctx,10000);
    if(!r->rc)r->rc=cb_flash170_poll(o,a+127,source[off+127],100);
   }else for(unsigned i=0;i<page&&!r->rc;i++){
    if(source[off+i]==0xff)continue;
    r->rc=cb_flash170_cmd(o,0xa0);
    if(!r->rc)r->rc=o->write(o->ctx,a+i,source[off+i]);
    if(!r->rc)r->rc=o->delay_us(o->ctx,20);
    if(!r->rc)r->rc=cb_flash170_poll(o,a+i,source[off+i],100);
   }
  }
 }
 if(!r->rc)r->stage=4;
 for(unsigned b=0;b<banks&&!r->rc;b++){
  if(banks==2)r->rc=cb_flash170_bank(o,b);
  for(unsigned a=0;a<65536&&!r->rc;a++){
   uint8_t v;r->address=b*65536+a;r->rc=o->read(o->ctx,a,&v);
   if(!r->rc&&v!=source[r->address])r->rc=-2;
   if(!r->rc)r->verified++;
  }
 }
 /* Adapter blocks all writes after cartridge lifecycle loss. */
 r->cleanup_rc=o->write(o->ctx,0,0xf0);
 if(!r->cleanup_rc)r->cleanup_rc=o->delay_us(o->ctx,10000);
 if(!r->cleanup_rc&&banks==2)r->cleanup_rc=cb_flash170_bank(o,0);
 if(!r->rc)r->rc=r->cleanup_rc;
 if(!r->rc)r->stage=5;
 return r->rc;
}
#endif
