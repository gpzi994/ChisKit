/* Execute production transfer orchestration with cartridge/file fault injection. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../app/ui98/ui.h"
#include "../app/ui98/backup_name.h"
#define OF_CART_SAVE_BANKED_1M 0x80000000u
#define OF_ERR_IO -3
#define OF_ERR_BAD_RANGE -4
#define OF_ERR_NOT_SUPPORTED -2
#define OF_ERR_DENIED -5
#define OF_CART_BUS_GBA 2
#define OF_CART_SPACE_SAVE_PARALLEL 1
static struct cb_ui state,*ui=&state;
static uint8_t buffer[65536],sram137_header[192],output[131072];
static uint32_t phase_began,began,ticks,output_size;
static unsigned reads,opens,writes,mode,fault_read,fault_file,fault_open,fault_write;
static int cancelled,approved=1,identity_rc;
static long selected_size;
static uint32_t selected_reads;
static int selected_fault,cart_changed,change_after_original,original_second_fault;
static unsigned restore_calls;
static int restore_fault,restore_unrecoverable;
#define of_cart_get_caps mock_caps
#define of_cart_enter_safe_idle mock_idle
#define of_cart_read_space mock_read
static void paint(void){}
static uint32_t now(void){return ++ticks;}
static int confirm(void){return approved;}
static int of_cart_get_caps(of_cart_caps_t *c){c->cart_caps_raw=0x06713f03;c->gba_space_mask=19;return 0;}
static int of_cart_enter_safe_idle(void){return 0;}
static int cart_read(uint32_t a,void *p,uint32_t n,uint32_t t){(void)a;(void)t;memset(p,0,n);return 0;}
int cb_parse_gba(struct cb_cart *c,const uint8_t *h){(void)h;*c=ui->cart;if(cart_changed||(change_after_original&&reads))strcpy(c->code,"OTHER");return 1;}
static int sram137_identity(void){return identity_rc;}
static int sram137_restore(void *c){(void)c;return identity_rc;}
static int sram137_cancel(void *c){(void)c;return cancelled;}
static uint8_t sample(uint32_t a){
 if(mode==3)return 0xff;
 if(mode==1)a%=32768;
 if(mode==2)a%=65536;
 return (uint8_t)(a*31+a/257+(a>>16)*97);
}
static int of_cart_read_space(unsigned b,unsigned s,uint32_t a,void *p,uint32_t n,uint32_t t){
 assert(b==2&&s==1&&t);reads++;
 if(fault_read==reads)return -3;
 uint32_t off=a&0x7fffffff;
 assert(off+n<=131072);
 for(unsigned i=0;i<n;i++)((uint8_t*)p)[i]=sample(off+i);
 if(mode==4&&reads==5)((uint8_t*)p)[0]^=1;
 if(original_second_fault&&reads==2)((uint8_t*)p)[0]^=1;
 return 0;
}
static int of_output_open(const char *p,uint32_t n){assert(strstr(p,"/Saves/chisburner/common/GBA-"));opens++;if(fault_open)return OF_ERR_IO;output_size=n;return 0;}
static long file_size(const char *p){assert(!strcmp(p,"slot:5")||!strcmp(p,"slot:6"));return !strcmp(p,"slot:6")?selected_size:output_size;}
static struct {long (*file_size)(const char*);} services={file_size};
#undef OF_SVC
#define OF_SVC (&services)
static int of_output_write(uint32_t a,const void *p,uint32_t n){assert(a+n<=output_size);writes++;if(fault_write==writes)return OF_ERR_IO;memcpy(output+a,p,n);return 0;}
static int backup_read(uint32_t a,void *p,uint32_t n){memcpy(p,output+a,n);if(fault_file)((uint8_t*)p)[0]^=1;return 0;}
static int file_read(unsigned slot,uint32_t a,void *p,uint32_t n){
 assert(slot==6&&a+n<=selected_size);selected_reads++;
 if(selected_fault==1)return OF_ERR_IO;
 for(unsigned i=0;i<n;i++)((uint8_t*)p)[i]=sample(a+i);
 if(selected_fault==2&&(selected_reads&1u)==0)((uint8_t*)p)[0]^=1;
 return 0;
}
/* Hash implementation itself is outside this test; verify its input extent. */
struct md5_53 {unsigned n;};
static void md5_init53(struct md5_53 *m){m->n=0;}
static void md5_add53(struct md5_53 *m,const void *p,unsigned n){assert(p);m->n+=n;}
static void md5_end53(struct md5_53 *m,uint8_t *d){assert(m->n==output_size);memset(d,0,16);}
static void md5_hex53(const uint8_t *d,char *s){(void)d;memset(s,'0',32);s[32]=0;}
#include "../app/ui98/sram138_transfer.h"
#include "../app/ui98/save_restore157.h"
#include "../app/ui98/save_backup159.h"
typedef struct {
 int32_t rc,restore_rc,bank0_rc;
 uint32_t attempted,changed,verified,restore_attempted,restore_verified;
} of_save160_report;
#undef OF_CART_SVC_HAS
#define OF_CART_SVC_HAS(field) 1
static int of_save160_restore(const void *src,void *bak,uint32_t n,
 const uint8_t id[16],of_save160_report *r){
 assert(src==save138_snapshots[0]&&bak==save138_snapshots[1]&&
  id==save159_expected_id&&n==output_size&&!memcmp(bak,output,n));
 restore_calls++;
 if(restore_fault){r->rc=restore_fault;r->attempted=1;
  r->restore_attempted=1;r->restore_verified=!restore_unrecoverable;
  return restore_fault;
 }
 r->verified=n;return 0;
}
static int confirm(void);
#include "../app/ui98/save_restore160.h"
static void reset(unsigned p,unsigned m){
 memset(&state,0,sizeof(state));state.cart.result=CB_READY;state.cart.platform=CB_GBA;
 strcpy(state.cart.code,"TEST");strcpy(state.cart.title,"TEST");
 strcpy(state.cart.save,"Unknown (ROM miss)");state.save_profile=p;mode=m;
 reads=opens=writes=output_size=fault_read=fault_file=fault_open=fault_write=0;cancelled=identity_rc=0;approved=1;
 selected_size=selected_reads=0;selected_fault=cart_changed=change_after_original=original_second_fault=0;
 restore_calls=0;restore_fault=restore_unrecoverable=0;
}
int main(void){
 for(unsigned p=0;p<3;p++)for(unsigned m=0;m<4;m++){
  reset(p,m);int rc=backup_sram138();
  if(!p&&m==3){assert(rc&&opens==0);continue;}
  assert(!rc&&opens==1);
  unsigned expected=p==2?131072:p==1?32768:m==1?32768:m==2?65536:131072;
  assert(output_size==expected&&ui->done==expected&&writes==expected/32768);
  for(unsigned i=0;i<expected;i++)assert(output[i]==sample(i));
 }
 reset(2,4);assert(backup_sram138()!=0&&!opens); // changed second snapshot
 for(unsigned i=1;i<=8;i++){reset(2,0);fault_read=i;assert(backup_sram138()!=0&&!opens);}
 reset(2,0);fault_file=1;assert(backup_sram138()!=0&&opens);
 reset(2,0);approved=0;assert(backup_sram138()!=0&&!reads&&!opens);
 reset(2,0);cancelled=1;assert(backup_sram138()!=0&&!reads&&!opens);
 reset(2,0);identity_rc=-3;assert(backup_sram138()!=0&&!reads&&!opens);
 reset(1,0);state.save_detected_type=3;state.save_detected_bytes=selected_size=32768;
 assert(!check_save157()&&state.done==32768&&selected_reads==2&&!writes);
 reset(2,0);state.save_detected_type=8;state.save_detected_bytes=selected_size=131072;
 assert(!check_save157()&&state.done==131072&&selected_reads==8&&!writes);
 reset(1,0);state.save_detected_type=3;state.save_detected_bytes=32768;selected_size=131072;
 assert(check_save157()==OF_ERR_BAD_RANGE&&!selected_reads&&
  !strcmp(state.notice,"SAV 131072 B; cartridge needs 32768 B"));
 reset(2,0);state.save_detected_type=8;state.save_detected_bytes=131072;selected_size=-1;
 assert(check_save157()==OF_ERR_BAD_RANGE&&!selected_reads&&
  !strcmp(state.notice,"Select SAV via Pocket: Saves SAV test"));
 reset(1,0);state.save_detected_type=4;state.save_detected_bytes=selected_size=32768;
 assert(check_save157()==OF_ERR_BAD_RANGE&&!selected_reads);
 reset(2,0);state.save_detected_type=7;state.save_detected_bytes=selected_size=131072;
 assert(check_save157()==OF_ERR_BAD_RANGE&&!selected_reads);
 reset(1,0);state.save_detected_type=3;state.save_detected_bytes=selected_size=32768;selected_fault=2;
 assert(check_save157()==OF_ERR_IO&&selected_reads==2&&!writes);
 reset(1,0);state.save_detected_type=3;state.save_detected_bytes=selected_size=32768;cart_changed=1;
 assert(check_save157()==OF_ERR_IO&&selected_reads==2&&!writes);
 reset(1,0);state.save_detected_type=3;state.save_detected_bytes=selected_size=32768;
 assert(!prepare_save159()&&opens==1&&writes==1&&output_size==32768&&
  !memcmp(output,save138_snapshots[1],32768));
 reset(2,0);state.save_detected_type=8;state.save_detected_bytes=selected_size=131072;
 assert(!prepare_save159()&&opens==1&&writes==4&&output_size==131072&&
  !memcmp(output,save138_snapshots[1],131072));
 reset(2,0);state.save_detected_type=8;state.save_detected_bytes=selected_size=131072;
 original_second_fault=1;assert(prepare_save159()==OF_ERR_IO&&!opens);
 reset(2,0);state.save_detected_type=8;state.save_detected_bytes=selected_size=131072;
 fault_file=1;assert(prepare_save159()==OF_ERR_IO&&opens==1);
 reset(2,0);state.save_detected_type=8;state.save_detected_bytes=selected_size=131072;
 fault_open=1;assert(prepare_save159()==OF_ERR_IO&&opens==1&&!writes);
 reset(2,0);state.save_detected_type=8;state.save_detected_bytes=selected_size=131072;
 fault_write=2;assert(prepare_save159()==OF_ERR_IO&&opens==1&&writes==2);
 reset(2,0);state.save_detected_type=8;state.save_detected_bytes=selected_size=131072;
 cart_changed=1;assert(prepare_save159()==OF_ERR_IO&&!opens);
 reset(2,0);state.save_detected_type=8;state.save_detected_bytes=selected_size=131072;
 change_after_original=1;assert(prepare_save159()==OF_ERR_IO&&!opens);
 reset(2,0);state.save_detected_type=8;state.save_detected_bytes=selected_size=131072;
 assert(!restore_save160()&&restore_calls==1&&state.done==131072);
 reset(2,0);state.save_detected_type=8;state.save_detected_bytes=selected_size=131072;
 approved=0;assert(restore_save160()==OF_ERR_DENIED&&opens==1&&!restore_calls);
 reset(2,0);state.save_detected_type=8;state.save_detected_bytes=selected_size=131072;
 fault_file=1;assert(restore_save160()==OF_ERR_IO&&!restore_calls);
 reset(2,0);state.save_detected_type=8;state.save_detected_bytes=selected_size=131072;
 restore_fault=OF_ERR_IO;restore_unrecoverable=1;
 assert(restore_save160()==OF_ERR_IO&&restore_calls==1&&
  strstr(state.notice,"may be damaged"));
 puts("SRAM138 transfer PASS: ROM miss allowed, 32/64/128 KiB, uniform profile retention, double-read/file mismatch and cancellation faults");
}
