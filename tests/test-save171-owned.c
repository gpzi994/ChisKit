#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <save/detect139.h>
#include <save/probe139.h>
#include <save/eeprom139.h>
#include <save/restore158.h>
#define CB_NOR_IO (-3)
#define CB_NOR_CANCELLED (-9)
#define CB_NOR_UNSUPPORTED (-2)
static bool busy,private_owned;
static struct {bool authorized;} job;
static unsigned job_token,mapper_kind,CART_BUS,CART_CAPS=0x1007f;
#define fence() ((void)0)
static uint8_t cart[131072],eeprom139_small[512],eeprom139_large[8192];
static unsigned bank,reads,calls,fail_read,change_read,bank_failure;
static int probe_error,cleanup_error,flash;
static uint8_t source[131072],backup[131072],id[16];
static unsigned write_mode,writes;
static struct cb_save158_report write_report;
struct eeprom139_session {bool stuck;};
static int alive(void *p){(void)p;return private_owned;}
static int enter(unsigned b){assert(b==2);private_owned=true;return 0;}
static int leave(void){private_owned=false;return 0;}
static int read_bus(void *p,uint32_t a,uint16_t *v){(void)p;(void)a;*v=0;return 0;}
static int sram138_bank(unsigned b){bank=b;return bank_failure&&reads>131072?-3:0;}
static int save139_read(void *p,uint32_t a,uint8_t *v){
 (void)p;reads++;*v=cart[bank*65536+a];
 if(change_read&&reads==change_read)*v^=1;
 return reads==fail_read?-3:0;
}
static int save139_write(void *p,uint32_t a,uint8_t v){(void)p;assert(write_mode&&private_owned);cart[bank*65536+a]=v;writes++;return 0;}
static int bank_op(void *ctx,unsigned b){(void)ctx;return sram138_bank(b);}
static int save139_alive(void *p){return alive(p);}
static int save139_delay(void *p,unsigned n){(void)p;(void)n;return 0;}
struct cb_save139_flash_report cb_save139_probe_flash(const struct cb_save139_flash_ops *o){
 (void)o;struct cb_save139_flash_report f={0};
 f.direct.state=flash?CB_SAVE139_PROBE_UNCHANGED:CB_SAVE139_PROBE_SRAM_LIKE;
 f.direct.restore_verified=1;f.rc=probe_error;f.cleanup_rc=cleanup_error;
 if(flash){f.id_read=1;f.id=0xc209;f.exit_verified=1;}return f;
}
static int eeprom139_read_owned(void *p,unsigned b,unsigned n,uint8_t *out){
 (void)p;(void)b;(void)n;(void)out;assert(0);return -3;
}
static int eeprom139_same_cart(void *p){return alive(p);}
static int eeprom139_release_owned(void *p){(void)p;return leave();}
struct cb_eeprom139_report cb_eeprom139_detect(const struct cb_eeprom139_ops *o,
 uint8_t *s,size_t sn,uint8_t *l,size_t ln){
 (void)s;(void)sn;(void)l;(void)ln;
 o->release(o->ctx);busy=false;
 return (struct cb_eeprom139_report){0};
}
static struct cb_save139_result eeprom139_result(struct cb_eeprom139_report r){
 return (struct cb_save139_result){.type=-1,.rc=r.rc};
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/save_auto139.h"
static int continuation(const struct cb_save139_result *r,const uint8_t *h,
 const uint8_t *original,void *ctx){
 (void)h;assert(private_owned&&busy&&bank==0);
 assert(r->backup_allowed&&r->bytes==*(unsigned*)ctx);
 assert(!memcmp(original,cart,131072));calls++;
 if(write_mode){
  const struct cb_save158_ops ops={NULL,alive,bank_op,save139_read,save139_write};
  return cb_save158_restore(r,h,id,original,backup,source,r->bytes,&ops,&write_report);
 }
 return 0;
}
static void reset(void){
 memset(cart,255,sizeof cart);busy=private_owned=false;
 reads=calls=fail_read=change_read=bank_failure=bank=0;
 probe_error=cleanup_error=flash=0;
 write_mode=writes=0;memset(id,0,sizeof id);
}
int main(void){
 for(unsigned n=32768;n<=131072;n*=4){
  reset();assert(save_auto139_kernel().type==CB_SAVE139_UNKNOWN);
  reset();assert(save_auto139_kernel_owned(continuation,&n).rc&&!calls);
  reset();assert(!save_auto139_kernel_owned_impl(continuation,&n,n).rc&&calls==1);
  assert(!busy&&!private_owned);
  reset();write_mode=1;memcpy(backup,cart,sizeof cart);memcpy(source,cart,sizeof cart);
  source[0]=1;source[n-1]=2;
  assert(!save_auto139_kernel_owned_impl(continuation,&n,n).rc);
  assert(writes==2&&write_report.verified==n&&!memcmp(cart,source,n));
  reset();write_mode=1;memcpy(backup,cart,sizeof cart);backup[0]^=1;
  assert(save_auto139_kernel_owned_impl(continuation,&n,n).rc&&!writes);
  for(unsigned f=0;f<6;f++){
   reset();
   if(f==0)fail_read=131080;
   if(f==1)change_read=131080;
   if(f==2)probe_error=-3;
   if(f==3)cleanup_error=-4;
   if(f==4)bank_failure=1;
   if(f==5)flash=1;
   assert(save_auto139_kernel_owned_impl(continuation,&n,n).rc&&!calls);
   assert(!busy&&!private_owned);
  }
 }
 puts("SRAM171 owned PASS: actual detector continuation, repeated capture, probe/cleanup/read/bank/conflict faults");
}
