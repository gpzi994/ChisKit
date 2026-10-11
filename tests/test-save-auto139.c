#include "probe.h"
#include "save/probe139.h"
#include "save/detect139.h"
#include "save/eeprom139.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static bool busy,private_owned,live=true;
static struct {bool authorized;} job;
static unsigned job_token,mapper_kind,enters,leaves,reads,probes,serial_reads,banks;
static uint32_t caps=0x1007f,period=32;
static uint32_t CART_BUS;
#define fence() ((void)0)
static unsigned floating_bus,prime_value,prime_error;
static int bank,fail_read,fail_restore,fail_leave;
static uint8_t header[192];
static struct cb_save139_flash_report flash;
#define CART_CAPS caps
static int enter(unsigned b){assert(b==2);enters++;private_owned=true;return 0;}
static int alive(void *x){(void)x;return live&&private_owned;}
static int leave(void){leaves++;private_owned=false;return fail_leave?-7:0;}
static int read_bus(void*x,uint32_t a,uint16_t*v){
 if(a==0xaa0000u||a==0x1540000u){
  assert(CART_BUS==2);prime_value=(a>>17)&255;
  if(prime_error)return -19;
 }
 (void)x;*v=a<192?(uint16_t)(header[a]|header[a+1]<<8):0;return 0;
}
static int sram138_bank(unsigned b){assert(alive(0));bank=b;banks++;return fail_restore&&banks==3?-8:0;}
static int save139_read(void*x,uint32_t a,uint8_t*v){
 (void)x;assert(alive(0));reads++;unsigned pos=(a+bank*65536)%period;
 *v=floating_bus?(uint8_t)prime_value:(uint8_t)(pos^(pos>>8)^(pos>>16));
 if(floating_bus==2&&reads==3)*v^=1;
 return fail_read&&reads==10?-9:0;
}
static int save139_write(void*x,uint32_t a,uint8_t v){(void)x;(void)a;(void)v;return 0;}
static int save139_alive(void*x){return alive(x);}
static int save139_delay(void*x,unsigned ms){(void)x;(void)ms;return 0;}
struct cb_save139_flash_report cb_save139_probe_flash(const struct cb_save139_flash_ops *o){
 assert(o&&alive(0)&&bank==0);probes++;return flash;
}
struct eeprom139_session {bool stuck;};
static int eeprom139_read_owned(void*x,unsigned bits,unsigned b,uint8_t*out){
 (void)x;(void)bits;assert(alive(0));serial_reads++;
 for(unsigned i=0;i<8;i++)out[i]=(uint8_t)(b*8+i);
 return 0;
}
static int eeprom139_same_cart(void*x){return alive(x);}
static int eeprom139_release_owned(void*x){(void)x;busy=false;return leave();}
static uint8_t eeprom139_small[512],eeprom139_large[8192];
static struct cb_save139_result eeprom139_result(struct cb_eeprom139_report p){
 struct cb_save139_result r={.type=CB_SAVE139_UNKNOWN,.rc=p.cleanup_rc?p.cleanup_rc:p.rc};
 if(!r.rc){r.type=p.state==CB_EEPROM139_8K?CB_SAVE139_EEPROM8K:CB_SAVE139_EEPROM512;r.bytes=r.type==2?8192:512;}
 return r;
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/save_auto139.h"
static unsigned owned_calls;
static int owned_fail;
static int owned_check(const struct cb_save139_result *r,const uint8_t *h,
 const uint8_t *original,void *ctx){
 assert(ctx==&owned_calls&&alive(0)&&busy&&bank==0);
 assert(r->type==CB_SAVE139_SRAM128&&r->bytes==131072&&r->backup_allowed);
 assert(!memcmp(h,header,sizeof header)&&original[0]==0&&original[65536]==1);
 owned_calls++;return owned_fail?-77:0;
}
static void reset(void){
 busy=private_owned=false;live=true;bank=0;
 enters=leaves=reads=probes=serial_reads=banks=0;period=32;
 floating_bus=prime_value=prime_error=0;
 fail_read=fail_restore=fail_leave=0;memset(header,0,sizeof header);
 flash=(struct cb_save139_flash_report){.direct={.state=CB_SAVE139_PROBE_UNCHANGED,.restore_verified=1},.exit_verified=1,.id_read=1};
}
int main(void){
 reset();struct cb_save139_result r=save_auto139_kernel();
 assert(r.type==2&&r.bytes==8192&&reads==131076&&probes==1&&serial_reads==1088);
 assert(enters==1&&leaves==1&&!busy&&!private_owned);
 for(unsigned n=32768;n<=131072;n*=2){
  reset();period=n;flash.direct.state=CB_SAVE139_PROBE_SRAM_LIKE;flash.id_read=0;
  r=save_auto139_kernel();assert(r.type==(n==32768?3:n==65536?7:8)&&r.bytes==n&&!serial_reads);
 }
 reset();period=131072;flash.direct.state=CB_SAVE139_PROBE_SRAM_LIKE;flash.id_read=0;
 owned_calls=owned_fail=0;
 r=save_auto139_kernel_owned(owned_check,&owned_calls);
 assert(!r.rc&&r.type==CB_SAVE139_SRAM128&&owned_calls==1&&
  leaves==1&&!busy&&!private_owned);
 reset();period=131072;flash.direct.state=CB_SAVE139_PROBE_SRAM_LIKE;flash.id_read=0;
 owned_calls=0;owned_fail=1;
 r=save_auto139_kernel_owned(owned_check,&owned_calls);
 assert(r.rc==-77&&owned_calls==1&&leaves==1&&!busy);
 reset();owned_calls=owned_fail=0;flash.id=0xc209;
 r=save_auto139_kernel_owned(owned_check,&owned_calls);
 assert(r.rc&&owned_calls==0&&leaves==1&&!busy);
 reset();owned_calls=0;header[0xbe]=0x4c;header[0xbf]=0x10;
 r=save_auto139_kernel_owned(owned_check,&owned_calls);
 assert(r.rc&&owned_calls==0&&!serial_reads&&leaves==1&&!busy);
 reset();flash.id=0xc209;r=save_auto139_kernel();assert(r.type==5&&!serial_reads);
 reset();flash.id=0x1234;r=save_auto139_kernel();assert(r.type==-1&&!serial_reads);
 reset();flash.id=0xffff;r=save_auto139_kernel();assert(r.type==4&&!serial_reads);
 reset();period=512;r=save_auto139_kernel();assert(!serial_reads);
 reset();period=32768;r=save_auto139_kernel();
 assert(r.type==3&&!r.backup_allowed&&!serial_reads); /* ID=0, observed span only */
 reset();flash.direct.state=CB_SAVE139_PROBE_SRAM_LIKE;flash.id_read=0;
 r=save_auto139_kernel();assert(r.type==2&&serial_reads==1088); /* short repeat fallback */
 for(unsigned m=0;m<3;m++){
  reset();fail_read=m==0;fail_restore=m==1;flash.cleanup_rc=m==2?-6:0;
  r=save_auto139_kernel();assert(r.rc&&r.type==-1&&!serial_reads&&!busy);
 }
 reset();header[0xbe]=0x4c;header[0xbf]=0x10; /* EEPROM, unknown size */
 r=save_auto139_kernel();assert(r.type==1&&!r.bytes&&!reads&&!probes&&!serial_reads);
 reset();memcpy(header+0xa0,"NGC-HIKARU3",10);memcpy(header+0xac,"GHTJ",4);header[0xbd]=0xb3;
 r=save_auto139_kernel();assert(r.type==6&&r.bytes==1048576&&!probes);
 reset();busy=true;r=save_auto139_kernel();assert(r.rc&&!enters);
 assert(save_auto139_diag.stage==1);
 reset();flash.direct.restore_rc=-4;flash.direct.original=0x12;
 flash.direct.target=0xed;flash.direct.observed=0x12;flash.direct.restored=0x34;
 r=save_auto139_kernel();assert(r.rc==-4&&save_auto139_diag.stage==7);
 assert(save_auto139_diag.restore_rc==-4&&save_auto139_diag.original==0x12&&save_auto139_diag.restored==0x34);
 reset();flash.id=0x1f3d;flash.cleanup_rc=-4;flash.mismatch_address=0x5555;
 flash.mismatch_expected=0x56;flash.mismatch_actual=0x78;
 r=save_auto139_kernel();assert(r.rc==-4&&save_auto139_diag.stage==8&&save_auto139_diag.id==0x1f3d);
 assert(save_auto139_diag.cleanup_rc==-4&&save_auto139_diag.mismatch_address==0x5555&&save_auto139_diag.actual==0x78);
 reset();fail_leave=1;r=save_auto139_kernel();assert(r.rc==-7);
 reset();flash.id=0x1f3d;fail_leave=1;r=save_auto139_kernel();
 assert(r.rc==-7&&save_auto139_diag.stage==10&&save_auto139_diag.leave_rc==-7);
 for(unsigned variant=0;variant<7;variant++){
  reset();flash=(struct cb_save139_flash_report){.rc=-3,.direct={.rc=-3,.state=CB_SAVE139_PROBE_INCONCLUSIVE,.restore_verified=1}};
  if(variant==1)flash.direct.state=CB_SAVE139_PROBE_UNKNOWN; /* transport -3 */
  if(variant==2)flash.direct.state=CB_SAVE139_PROBE_UNSTABLE; /* initial mismatch */
  if(variant==3)flash.direct.restore_rc=-4;
  if(variant==4)flash.direct.restore_verified=0;
  if(variant==5)period=32768;
  if(variant==6)flash.id_read=1;
  r=save_auto139_kernel();
  if(!variant)assert(!r.rc&&serial_reads==1088&&save_auto139_diag.stage==9&&save_auto139_diag.direct_rc==-3);
  else assert(r.rc&&!serial_reads);
  assert(!busy&&!private_owned&&leaves==1);
 }
 reset();floating_bus=1;period=131072;
 flash.direct.state=CB_SAVE139_PROBE_SRAM_LIKE;flash.id_read=0;
 r=save_auto139_kernel();
 assert(!r.rc&&r.type==CB_SAVE139_EEPROM8K&&serial_reads==1088&&!probes&&!banks&&reads==4);
 reset();floating_bus=1;owned_calls=0;
 r=save_auto139_kernel_owned(owned_check,&owned_calls);
 assert(r.rc&&!owned_calls&&!probes&&!serial_reads&&leaves==1&&!busy);
 reset();prime_error=1;r=save_auto139_kernel();
 assert(r.rc==-19&&!serial_reads&&!probes&&leaves==1&&!busy);
 reset();floating_bus=2;r=save_auto139_kernel();
 assert(r.rc==CB_NOR_IO&&!serial_reads&&!probes&&leaves==1&&!busy);
 puts("Save auto PASS: floating-bus SRAM echo rejected before writes; serial fallback, driver/transport errors, real SRAM/Flash and restore guards");
}
