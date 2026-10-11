#include "probe.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
static bool busy,private_owned,live;
static struct {bool authorized;} job;
static unsigned job_token,mapper_kind,timer,commands,cancels,leaves,mode,enters;
static uint32_t caps,desc,control,mock_status,mock_high,mock_low;
#define CART_EEPROM162 1
#define CART_EEPROM_WRITE_COMMAND 0x57524954u
#define CART_CAPS caps
#define CART_EEPROM_DESC desc
#define CART_EEPROM_CONTROL control
#define CART_EEPROM_STATUS mock_status
#define CART_EEPROM_HIGH mock_high
#define CART_EEPROM_LOW mock_low
static bool alive(void *p){(void)p;return private_owned&&live;}
static uint32_t now_ms(void *p){(void)p;return timer++;}
static int enter(unsigned b){assert(b==2);enters++;private_owned=true;return 0;}
static int leave(void){leaves++;private_owned=false;return mode==4?CB_NOR_IO:0;}
static void fence(void){
 if(control==1){commands++;control=0;mock_status=mode==1||mode==2?1:mode==3?4:2;if(mode==5)live=false;}
 if(control==CART_EEPROM_WRITE_COMMAND){commands++;control=0;mock_status=2;}
 if(mode==6&&commands==129)live=false;
 if(mode==7&&commands==65)mock_high=0;
 if(mode==8)mock_high=mock_low=0;
 if(mode==9)mock_high=desc&0x4000?0x12345678:0x01234567;
 if(mode>=10){
  mock_high=0x12340000u+(desc&1023);
  if(mode==11&&!(desc&0x4000))mock_high^=0x10000000u;
  if(mode==12&&commands==65)mock_high^=1;
 }
 if(control==2){cancels++;control=0;if(mode!=2)mock_status=4;}
}
#include "../.integration/openfpgaos/src/firmware/os/kernel/eeprom139.h"
static void reset(void){busy=false;private_owned=false;live=true;job.authorized=true;
 job_token=1;mapper_kind=1;timer=commands=cancels=leaves=mode=enters=0;caps=0x10040;desc=control=mock_status=0;
 mock_high=0x01234567;mock_low=0x89abcdef;}
int main(void){
 uint8_t out[8],want[]={1,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};unsigned cases=0;
 {
  struct eeprom139_session session={0};
  reset();private_owned=true;
  assert(!eeprom139_write_owned(&session,6,63,want));
  assert(desc==63&&mock_high==0x01234567&&mock_low==0x89abcdef&&commands==1);
  assert(eeprom139_write_owned(&session,6,64,want)==CB_NOR_ARGUMENT&&commands==1);
  reset();private_owned=true;
  assert(!eeprom139_write_owned(&session,14,1023,want));
  assert(desc==0x43ff&&commands==1);
  reset();private_owned=true;live=false;
  assert(eeprom139_write_owned(&session,6,0,want)==CB_NOR_CANCELLED&&!commands);
 }
 for(unsigned bits=6;bits<=14;bits+=8)for(unsigned b=0;b<(bits==6?64u:1024u);b++){
  reset();int rc=eeprom139_read_block_kernel(bits,b,out);
  if(rc)fprintf(stderr,"bits=%u block=%u rc=%d status=%u live=%u\n",bits,b,rc,mock_status,live);
  assert(!rc);
  assert(!memcmp(out,want,8)&&desc==((bits==14?0x4000u:0u)|b));
  assert(commands==1&&!cancels&&leaves==1&&!busy&&!private_owned&&!job.authorized&&!job_token);cases++;
 }
 reset();assert(eeprom139_read_block_kernel(6,64,out)==CB_NOR_ARGUMENT&&!commands);
 assert(eeprom139_read_block_kernel(14,1024,out)==CB_NOR_ARGUMENT);
 assert(eeprom139_read_block_kernel(8,0,out)==CB_NOR_ARGUMENT);
 assert(eeprom139_read_block_kernel(6,0,NULL)==CB_NOR_ARGUMENT);
 reset();busy=true;assert(eeprom139_read_block_kernel(6,0,out)==CB_NOR_IO&&!commands);
 for(unsigned c=0;c<2;c++){reset();caps=c?0x20040:0x1003f;
  assert(eeprom139_read_block_kernel(6,0,out)==CB_NOR_UNSUPPORTED&&!commands&&!busy);}
 for(unsigned m=1;m<=5;m++){
  reset();mode=m;assert(eeprom139_read_block_kernel(6,0,out)<0);
  for(unsigned i=0;i<8;i++)assert(out[i]==0);
  if(m==2)assert(busy&&private_owned&&cancels==1&&!leaves);
  else assert(!busy&&!private_owned);
  if(m==5)assert(!cancels&&!leaves);
 }
 printf("EEPROM OS PASS: %u blocks, byte order, capability/range, timeout/cancel/drain lock, epoch loss, cleanup error\n",cases);
 uint8_t small[512],large[8192];
 reset();struct cb_eeprom139_report r=eeprom139_probe_kernel(small,sizeof small,large,sizeof large);
 assert(!r.rc&&!r.cleanup_rc&&r.state==CB_EEPROM139_8K);
 assert(r.bytes_read==17408&&commands==2176&&enters==1&&leaves==1&&!busy&&!private_owned);
 reset();r=eeprom139_probe_kernel(small,511,large,sizeof large);
 assert(r.rc==CB_NOR_ARGUMENT&&!commands&&!leaves);
 r=eeprom139_probe_kernel(large,512,large+8,8192);
 assert(r.rc==CB_NOR_ARGUMENT&&!commands&&!leaves);
 reset();busy=true;r=eeprom139_probe_kernel(small,512,large,8192);
 assert(r.rc==CB_NOR_IO&&busy&&!commands&&!leaves);
 for(unsigned m=1;m<=5;m++){
  reset();mode=m;r=eeprom139_probe_kernel(small,512,large,8192);
  assert((r.rc||r.cleanup_rc)&&r.state==CB_EEPROM139_UNKNOWN);
  if(m==2)assert(busy&&private_owned&&cancels==1&&!leaves);
  else assert(!busy&&!private_owned);
  if(m==5)assert(!cancels&&!leaves);
 }
 puts("EEPROM session PASS: 2176 reads, one release, invalid scratch, busy, timeout, stuck drain, epoch loss, cleanup failure");
 reset();mode=6;r=eeprom139_probe_kernel(small,512,large,8192);
 assert(r.rc&&r.state==CB_EEPROM139_UNKNOWN&&commands==129&&!cancels&&!leaves&&!busy);
 reset();mode=7;r=eeprom139_probe_kernel(small,512,large,8192);
 assert(r.rc&&r.state==CB_EEPROM139_UNSTABLE&&r.first_difference==0&&leaves==1);
 reset();mode=8;r=eeprom139_probe_kernel(small,512,large,8192);
 assert(!r.rc&&!r.cleanup_rc&&r.state==CB_EEPROM139_BLANK&&leaves==1);
 reset();mode=9;r=eeprom139_probe_kernel(small,512,large,8192);
 assert(!r.rc&&!r.cleanup_rc&&r.state==CB_EEPROM139_512&&leaves==1);
 puts("EEPROM session PASS: mid-scan epoch loss, unstable repeat, blank inconclusive, 512-byte heuristic");
 for(unsigned m=0;m<=12;m++){
  reset();mode=m;struct cb_save139_result result=eeprom139_detect_kernel();
  assert(!result.backup_allowed&&!result.write_allowed);
  if(m==0||m==7||m>=9){assert(!result.rc&&result.basis==CB_SAVE139_SERIAL_PROBE);
   int small=m==7||m==9||m==11||m==12;
   assert(result.type==(small?CB_SAVE139_EEPROM512:CB_SAVE139_EEPROM8K));
   assert(result.bytes==(small?512u:8192u));
   assert(commands==1088&&enters==1&&leaves==1);
  }else if(m==8)assert(result.type==CB_SAVE139_NONE&&!result.bytes);
  else assert(result.type==CB_SAVE139_UNKNOWN&&result.bytes==0);
 }
 puts("EEPROM detection PASS: owned scratch, classifier, failure/blank unknown, no backup or write grants");
 reset();of_save139_result wire;
 assert(eeprom139_detect_service(NULL,sizeof wire)==CB_NOR_ARGUMENT&&!commands);
 assert(eeprom139_detect_service(&wire,sizeof wire-1)==CB_NOR_ARGUMENT&&!commands);
 mode=10;assert(!eeprom139_detect_service(&wire,sizeof wire)&&wire.type==2&&wire.bytes==8192);
 reset();mode=4;assert(eeprom139_detect_service(&wire,sizeof wire)<0&&wire.type==-1&&!wire.bytes);
 reset();mode=10;
 struct {of_save141_diagnostic d;uint32_t canary;} extended;
 memset(&extended,0xa5,sizeof extended);extended.canary=0x12345678;
 assert(!eeprom139_detect_service(&extended.d,sizeof extended.d));
 assert(extended.d.result.type==2&&extended.d.result.bytes==8192&&extended.canary==0x12345678);
 /* This isolated EEPROM-only adapter has no parallel diagnostic producer. */
 assert(!extended.d.stage&&!extended.d.id&&!extended.d.leave_rc);
 reset();assert(eeprom139_detect_service(&extended.d,sizeof extended.d-1)==CB_NOR_ARGUMENT&&!commands);
 puts("EEPROM service PASS: report size/null validation and cleanup error propagation");
 static of_eeprom139_snapshot snapshot;
 reset();assert(eeprom139_capture_service(NULL,sizeof snapshot)==CB_NOR_ARGUMENT&&!commands);
 assert(eeprom139_capture_service(&snapshot,sizeof snapshot-1)==CB_NOR_ARGUMENT&&!commands);
 for(unsigned m=0;m<=9;m++){
  reset();mode=m;memset(&snapshot,0xa5,sizeof snapshot);
  int rc=eeprom139_capture_service(&snapshot,sizeof snapshot);
  if(m==0||m==8||m==9){
   assert(!rc&&snapshot.bytes_read==17408&&commands==2176&&leaves==1);
   assert(!memcmp(snapshot.small,eeprom139_small,512));
   assert(!memcmp(snapshot.large,eeprom139_large,8192));
  }else if(m==7){
   assert(!rc&&snapshot.stable_mask==2&&snapshot.mismatch_mask==1);
   assert(snapshot.mode_difference[0]==0&&snapshot.mode_difference[1]==UINT32_MAX);
   for(unsigned i=0;i<512;i++)assert(!snapshot.small[i]);
   assert(!memcmp(snapshot.large,eeprom139_large,8192));
  }else{
   assert(rc);
   for(unsigned i=0;i<512;i++)assert(!snapshot.small[i]);
   for(unsigned i=0;i<8192;i++)assert(!snapshot.large[i]);
  }
 }
 reset();mode=7;
 assert(!eeprom139_capture_service(&snapshot,sizeof snapshot));
 assert(snapshot.stable_mask==2&&snapshot.mismatch_mask==1);
 assert(snapshot.mode_difference[0]==0&&!snapshot.cleanup_rc);
 /* R2 clients retain abort-on-mismatch and no-payload semantics. */
 reset();mode=7;memset(&snapshot,0xa5,sizeof snapshot);
 assert(eeprom139_capture_service(&snapshot,offsetof(of_eeprom139_snapshot,stable_mask))<0);
 assert(snapshot.address_bits==6&&snapshot.pass==2&&snapshot.block==0&&snapshot.stage==4);
 const unsigned char *r2tail=(const unsigned char *)&snapshot.stable_mask;
 for(unsigned i=0;i<sizeof snapshot - offsetof(of_eeprom139_snapshot,stable_mask);i++)assert(r2tail[i]==0xa5);
 reset();mode=5;
 assert(eeprom139_capture_service(&snapshot,sizeof snapshot)<0);
 assert(snapshot.address_bits==6&&snapshot.pass==1&&snapshot.block==0);
 assert(snapshot.stage==2);
 reset();memset(&snapshot,0xa5,sizeof snapshot);
 assert(!eeprom139_capture_service(&snapshot,offsetof(of_eeprom139_snapshot,address_bits)));
 const unsigned char *tail=(const unsigned char *)&snapshot.address_bits;
 for(unsigned i=0;i<sizeof snapshot - offsetof(of_eeprom139_snapshot,address_bits);i++)assert(tail[i]==0xa5);
 puts("EEPROM capture PASS: raw dual-mode data, failure details, legacy-size canary, repeat/epoch/cleanup errors suppress payload; no file or PSRAM IO");
}
