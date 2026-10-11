/* MBC7 read-only EEPROM protocol. No EWEN, WRITE or ERASE command exists here.
 * FlashGBX Mapper.py: dual enable; LK_Device.py: 512-byte mirror probe.
 * Pan Docs MBC7: start + READ(10) + eight address bits, MSB first.
 * FlashGBX_LK_Firmware LK.c: lk_dmg_mbc7_read_eeprom / lk_conn_send_u16
 * emit high byte first. Do not use Sanni's different little-endian format.
 */
#include "gb_save175.h"
#include <string.h>
int gb179_plan(unsigned bus,const uint8_t h[80],uint32_t selected,struct gb175_plan *p){
 if(!p)return GB175_ARGUMENT;
 memset(p,0,sizeof *p);
 if(bus!=1||!h)return GB175_UNSUPPORTED;
 if(h[0x47]!=0x22){
  if(selected==2048||h[0x49]==1)return GB175_UNSUPPORTED;
  return gb178_plan(bus,h,selected,p);
 }
 /* Reuse the existing GB checksum/logo validation, with a supported type
  * in a local copy. Nothing is sent to the cartridge until validation ends. */
 uint8_t checked[80];memcpy(checked,h,80);
 checked[0x47]=6;checked[0x4d]=(uint8_t)(checked[0x4d]+0x22-6);
 struct gb175_plan unused;
 int rc=gb178_plan(bus,checked,0,&unused);if(rc)return rc;
 /* FlashGBX CLI's known 256-byte retail profiles. Content repetition alone
  * cannot distinguish a 512-byte device containing duplicated data. */
 if(h[0x48]!=5||h[0x49]!=0||
   (memcmp(h+0x34,"KORO2 KIRBYKKKJ",15)&&memcmp(h+0x34,"KIRBY TNT_KTNE",14)))return GB175_UNSUPPORTED;
 if(selected&&selected!=256)return GB175_UNSUPPORTED;
 p->mapper=GB179_MBC7;p->bytes=256;p->banks=1;return 0;
}
static int ee179_stop(const struct gb175_bus *b){
 return !b->alive(b->ctx)||b->cancelled(b->ctx);
}
static int ee179_write(const struct gb175_bus *b,uint16_t a,uint8_t v){
 if(ee179_stop(b))return GB175_CANCELLED;
 return b->control(b->ctx,a,v);
}
static int ee179_bit(const struct gb175_bus *b,unsigned bit){
 uint8_t v=(uint8_t)(0x80|(bit?2:0));
 int rc=ee179_write(b,0xa080,v);
 return rc?rc:ee179_write(b,0xa080,(uint8_t)(v|0x40));
}
static int ee179_read(const struct gb175_bus *b,uint8_t *v){
 if(ee179_stop(b))return GB175_CANCELLED;
 return b->read_ram(b->ctx,0xa080,v,1);
}
static int ee179_word(const struct gb175_bus *b,unsigned address,uint16_t *word){
 int rc=ee179_write(b,0xa080,0);if(rc)return rc;
 rc=ee179_write(b,0xa080,0x80);if(rc)return rc;
 unsigned command=0x600u|address; /* 1(start),10(READ),8 address bits */
 for(int bit=10;bit>=0;bit--){rc=ee179_bit(b,(command>>bit)&1);if(rc)return rc;}
 uint8_t v=0;
 rc=ee179_read(b,&v);if(rc)return rc;
 /* READ dummy zero distinguishes a responding EEPROM from a floating bus. */
 if(v&1)return GB175_IO;
 *word=0;
 for(unsigned bit=0;bit<16;bit++){
  rc=ee179_bit(b,0);if(rc)return rc;
  rc=ee179_read(b,&v);if(rc)return rc;
  *word=(uint16_t)((*word<<1)|(v&1));
 }
 return ee179_write(b,0xa080,0);
}
int gb179_capture(const struct gb175_bus *b,unsigned bus,const uint8_t h[80],uint32_t selected,
 void *snapshot,size_t cap,void *scratch,uint32_t chunk,struct gb175_report *r){
 if(!r)return GB175_ARGUMENT;
 memset(r,0,sizeof *r);
 struct gb175_plan p;int rc=gb179_plan(bus,h,selected,&p);if(rc)return r->rc=rc;
 if(p.mapper!=GB179_MBC7)return gb178_capture(b,bus,h,selected,snapshot,cap,scratch,chunk,r);
 if(!b||!b->acquire||!b->alive||!b->cancelled||!b->read_header||!b->control||
    !b->read_ram||!b->release||!snapshot||cap<256)return r->rc=GB175_ARGUMENT;
 uint8_t expected[80],current[80],first[512],second[512];memcpy(expected,h,80);
 rc=b->acquire(b->ctx);if(rc){r->detail=rc;return r->rc=GB175_IO;}
 int touched=0;
 for(unsigned pass=0;pass<2&&!rc;pass++){
  if(ee179_stop(b)){rc=GB175_CANCELLED;break;}
  rc=b->read_header(b->ctx,current);if(rc)break;
  if(memcmp(current,expected,80)){rc=GB175_IDENTITY;break;}
  touched=1;rc=ee179_write(b,0,10);if(rc)break;
  rc=ee179_write(b,0x4000,0x40);if(rc)break;
  uint8_t *data=pass?second:first;
  for(unsigned w=0;w<256&&!rc;w++){
   uint16_t value=0;rc=ee179_word(b,w,&value);
   data[2*w]=(uint8_t)(value>>8);data[2*w+1]=(uint8_t)value;
  }
 }
 if(!rc&&memcmp(first,second,512))rc=GB175_CHANGED;
 if(!rc&&memcmp(first,first+256,256))rc=GB175_UNSUPPORTED;
 r->detail=rc;r->rc=rc;
 /* Cleanup always drops serial CS, then both mapper enable gates. */
 if(touched){
  const uint16_t addr[]={0xa080,0x4000,0};
  for(unsigned i=0;i<3&&b->alive(b->ctx);i++){
   int cr=b->control(b->ctx,addr[i],0);if(cr&&!r->cleanup_rc)r->cleanup_rc=cr;
  }
 }
 if(!r->rc){
  if(ee179_stop(b))r->rc=GB175_CANCELLED;
  else if((rc=b->read_header(b->ctx,current)))r->rc=rc;
  else if(memcmp(current,expected,80))r->rc=GB175_IDENTITY;
 }
 rc=b->release(b->ctx);if(rc&&!r->cleanup_rc)r->cleanup_rc=rc;
 if(!r->rc&&r->cleanup_rc)r->rc=GB175_CLEANUP;
 if(!r->rc){memcpy(snapshot,first,256);r->compared=256;}
 return r->rc;
}
