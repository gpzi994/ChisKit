#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <save/restore158.h>

static uint8_t cart[131072],original[131072],backup[131072],source[131072];
static uint8_t header[192],id[16];
static unsigned bank,writes,reads,fail_write,fail_read,drop_epoch;
static int epoch;
static int alive(void *ctx){(void)ctx;return epoch;}
static int set_bank(void *ctx,unsigned b){(void)ctx;assert(epoch&&b<2);bank=b;return 0;}
static int read_byte(void *ctx,uint32_t a,uint8_t *v){
 (void)ctx;assert(epoch&&a<65536);reads++;*v=cart[bank*65536+a];
 if(fail_read&&reads==fail_read){*v^=1;fail_read=0;}
 return 0;
}
static int write_byte(void *ctx,uint32_t a,uint8_t v){
 (void)ctx;assert(epoch&&a<65536);writes++;cart[bank*65536+a]=v;
 if(drop_epoch&&writes==drop_epoch){epoch=0;return -91;}
 if(fail_write&&writes==fail_write){fail_write=0;return -92;}
 return 0;
}
static const struct cb_save158_ops ops={NULL,alive,set_bank,read_byte,write_byte};
static struct cb_save139_result detected;
static struct cb_save158_report report;
static void reset(unsigned n){
 for(unsigned i=0;i<sizeof cart;i++)original[i]=(uint8_t)(i*19u+(i>>16)*73u);
 memcpy(cart,original,sizeof cart);memcpy(backup,original,sizeof backup);
 memcpy(source,original,sizeof source);
 memset(header,0,sizeof header);memcpy(header+0xa0,"POKEMON EMERBPEE",16);
 memcpy(id,header+0xa0,16);
 detected=(struct cb_save139_result){
  .type=n==32768?CB_SAVE139_SRAM32:CB_SAVE139_SRAM128,
  .bytes=n,.backup_allowed=1,.basis=CB_SAVE139_PARALLEL_PROBE};
 bank=writes=reads=fail_write=fail_read=drop_epoch=0;epoch=1;
 memset(&report,0,sizeof report);
}
static int run(unsigned n){
 return cb_save158_restore(&detected,header,id,original,backup,source,n,&ops,&report);
}
int main(void){
 reset(32768);source[0]=0xab;source[32767]=0xcd;
 assert(!run(32768)&&report.attempted==2&&report.changed==2&&
  report.verified==32768&&bank==0&&!memcmp(cart,source,32768));
 reset(131072);source[0]^=1;source[65536]^=2;source[131071]^=4;
 assert(!run(131072)&&report.attempted==3&&report.verified==131072&&
  bank==0&&!memcmp(cart,source,131072));
 reset(131072);assert(!run(131072)&&!writes&&report.verified==131072);
 reset(131072);id[0]^=1;assert(run(131072)&&!writes&&!reads);
 reset(131072);backup[17]^=1;assert(run(131072)&&!writes&&!reads);
 reset(131072);detected.type=CB_SAVE139_FLASH128;
 assert(run(131072)&&!writes&&!reads);
 reset(131072);detected.backup_allowed=0;
 assert(run(131072)&&!writes&&!reads);
 reset(131072);assert(run(32768)&&!writes&&!reads);
 reset(131072);source[0]^=1;fail_write=1;
 assert(run(131072)&&report.attempted==1&&!report.changed&&
  report.restore_attempted&&report.restore_verified&&
  !memcmp(cart,original,131072));
 reset(131072);source[0]^=1;source[65536]^=2;fail_read=1;
 assert(run(131072)&&report.attempted==2&&report.restore_attempted&&
  report.restore_verified&&!memcmp(cart,original,131072));
 reset(131072);source[0]^=1;drop_epoch=1;
 assert(run(131072)&&report.attempted==1&&!report.restore_attempted&&
  report.bank0_rc&&cart[0]!=original[0]);
 puts("SAVE158 restore PASS: 32/128 KiB, unchanged skip, identity/backup/type guards, full verify, post-pulse recovery, epoch loss");
}
