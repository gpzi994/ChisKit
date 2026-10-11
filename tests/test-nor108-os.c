/* Compile the production OS job gate; replace only MMIO and flash callbacks. */
#define OF_PC 1
#define OFOS_REGS_H
#include "probe.h"
#include "mori/chisburner_nor_db_data.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint32_t regs[32],clock_tick,raw_writes,probe_count,erase_count,program_count;
static int probe_failure,execute_failure,changed_id;
static int epoch_read_fault;
static bool gb_header_model;
static uint8_t gb_header_bytes[0x50];
#if CART_NOR110
static bool normal_model;
static struct cb_nor_probe normal_seed;
#endif
#if CART_NOR109
static int board_good,route_model;
static unsigned board_high,board_low,board_selected,board_seen;
static unsigned route_fault_at;
#endif
#define CART_ID regs[0]
#if CART_SRAM137
#define CART_CAPS regs[1]
static unsigned sram_cap=0x1000f,sram_reads,sram_fault;
#if CART_SRAM138
static unsigned save_bank,bank_writes,bank_fault;
static int save_epoch_fault;
#if CART_SAVE139
static uint8_t save139_data[65536];
static unsigned save139_writes,save139_fail_write;
#endif
#endif
#endif
#define CART_STATUS regs[2]
#define CART_COMMAND regs[3]
#define CART_BUS regs[4]
#define CART_ADDRESS regs[5]
#define CART_TIMEOUT regs[7]
#define CART_RX_DATA regs[8]
#define CART_RX_LEVEL regs[9]
#define CART_LIFECYCLE regs[11]
#define CART_QUERY_ARG regs[14]
#define CART_SPACE regs[16]
#define CART_TX_DATA regs[17]
#define CART_ARM regs[19]
#if CART_STREAM131
#define CART_STREAM_INDEX regs[20]
#define CART_STREAM_DATA regs[21]
#define CART_STREAM_CONTROL regs[22]
#define CART_STREAM_STATUS regs[23]
#define CART_STREAM_KEY regs[24]
#define CART_STREAM_LEASE regs[25]
#define CART_STREAM_AUTH regs[26]
static bool stream_model_hold;
static bool gb_compare_capability=true;
static unsigned auth_delay;
#endif
#define CART_LC_READY (1u<<31)
#define CART_ST_BUSY 2u
#define CART_ST_DONE 4u
#define CART_ST_ERROR 8u
#define CART_ST_ABORTED 16u
#define CART_ST_TIMEOUT 32u
#define CPU_FREQ_HZ 1000000u
static uint64_t read_cycles(void){
#if CART_SRAM138
 return (uint64_t)++clock_tick*100;
#else
 return (uint64_t)++clock_tick*1000;
#endif
}
static void fence(void){
#if CART_STREAM131
 if(CART_STREAM_INDEX==15)CART_STREAM_DATA=gb_compare_capability?0x47423230u:0;
 if((CART_STREAM_STATUS&3u)==2u){
  if(CART_STREAM_INDEX==12)CART_STREAM_DATA=16;
  if(CART_STREAM_INDEX==13)CART_STREAM_DATA=16;
  if(CART_STREAM_INDEX==14)CART_STREAM_DATA=0x1234ffffu;
 }
 if(CART_STREAM_AUTH==0x53413132u){if(auth_delay)auth_delay--;else CART_STREAM_AUTH=1;}
 if(CART_STREAM_CONTROL==1){CART_STREAM_KEY=0;CART_STREAM_STATUS=1;CART_STREAM_CONTROL=0;}
 if(CART_STREAM_CONTROL==2){CART_STREAM_STATUS=0;CART_STREAM_CONTROL=0;}
 if(CART_STREAM_LEASE==1)CART_STREAM_LEASE=0x53130007;
 else if(CART_STREAM_LEASE==0 || (CART_STREAM_LEASE&7)==2)
  CART_STREAM_LEASE=0x53130000|(stream_model_hold?2:0);
#endif
 int route_error=0;
 CART_ID=CART_SPACE==31?0x384e4243:0x31424746;
 if(CART_COMMAND==7)raw_writes++;
#if CART_SRAM137
 CART_CAPS=sram_cap;
#if CART_SRAM138
 if(CART_BUS==5&&CART_COMMAND==7){
  assert(CART_ADDRESS==0x01000000&&CART_TX_DATA<=1);
  save_bank=CART_TX_DATA;bank_writes++;
  if(bank_writes==bank_fault)route_error=1;
 }
#endif
 if(CART_BUS==4&&CART_COMMAND==2){
#if CART_SRAM138
  assert(CART_ADDRESS<65536);++sram_reads;
#if CART_SAVE139
  CART_RX_DATA=save139_data[CART_ADDRESS];CART_RX_LEVEL=1;
#else
  CART_RX_DATA=(CART_ADDRESS*31u+CART_ADDRESS/257u+save_bank*97u)&255u;CART_RX_LEVEL=1;
#endif
  if(save_epoch_fault){CART_QUERY_ARG++;save_epoch_fault=0;}
#else
  assert(CART_ADDRESS<32768);++sram_reads;
  CART_RX_DATA=(CART_ADDRESS*31u+CART_ADDRESS/257u)&255u;CART_RX_LEVEL=1;
#endif
 if(sram_reads==sram_fault)route_error=1;
 }
#if CART_SAVE139
 if(CART_BUS==6&&CART_COMMAND==7){
  assert(CART_ADDRESS==0||CART_ADDRESS==4||CART_ADDRESS==0x5555||CART_ADDRESS==0x2aaa);
  assert(CART_TX_DATA<=255&&CART_ARM==0x43424e32u);
  save139_data[CART_ADDRESS]=CART_TX_DATA;save139_writes++;
  if(save139_writes==save139_fail_write)route_error=1;
 }
#endif
#endif
 if(gb_header_model&&CART_COMMAND==2){
  assert(CART_BUS==1&&CART_ADDRESS>=0x100&&CART_ADDRESS<0x150);
  CART_RX_DATA=gb_header_bytes[CART_ADDRESS-0x100];CART_RX_LEVEL=1;
 }
#if CART_NOR109
 if(route_model&&CART_COMMAND==7){
  if(route_fault_at&&raw_writes==route_fault_at)route_error=1;
#if CART_NOR110
  if(normal_model&&CART_BUS==3&&CART_ADDRESS==2)board_selected=CART_TX_DATA>>4;
#endif
  if(CART_BUS==3&&CART_ADDRESS==6)board_high=CART_TX_DATA==0x10?4:0;
  if(CART_BUS==2&&CART_ADDRESS==0x16b592e)board_low=CART_TX_DATA/0x400;
  if(CART_BUS==2&&CART_ADDRESS==0x1e2468a){board_selected=board_high+board_low;assert(board_selected<8);}
 }
 if(route_model&&CART_COMMAND==2){assert(CART_ADDRESS<0x2000000);CART_RX_DATA=0xa000|board_selected;CART_RX_LEVEL=2;}
#endif
 if(CART_COMMAND==2&&epoch_read_fault){CART_QUERY_ARG++;epoch_read_fault=0;}
 CART_STATUS=route_error?CART_ST_ERROR:5;CART_COMMAND=0;
}
static struct cb_nor_plan fixture_plan;
#if CART_NOR109
#include "board_probe109.h"
static int probe_dual,board_failure;
static unsigned board_calls;
static int board_capture_mock(const struct cb_board109_io*i,enum cb_board109_kind kind,
 const uint8_t seed[8],struct cb_board109_scratch*scratch,struct cb_board109_capture*out){
 assert(scratch);
 assert(kind==CB_BOARD109_F2AULTRA&&seed[0]==0x89&&i->alive(i->ctx));board_calls++;
 if(board_failure)return CB_NOR_IO;
 assert(!i->write(i->ctx,3,6,0x10)&&CART_BUS==2);
 memset(out,0,sizeof(*out));assert(!cb_board109_profile(kind,&out->profile));
 out->ids_valid=out->restored=true;out->geometry_valid=board_good;return 0;
}
#define cb_board109_capture_full board_capture_mock
#endif
static int probe_mock(const struct cb_nor_io*i,unsigned unit,struct cb_nor_probe_scratch*s,struct cb_nor_probe*out){
 (void)s;assert(unit==2&&i->alive(i->ctx));probe_count++;
 if(probe_failure)return probe_failure;
#if CART_NOR110
 if(normal_model){
  assert(!i->select(i->ctx,0));*out=normal_seed;
  out->baseline[0]=board_selected;return 0;
 }
#endif
 memset(out,0,sizeof(*out));out->entry=g_chisburner_nor_db_gba+3;
 out->plan=fixture_plan;out->method=CB_NOR_AMD_AAA;out->cfi_valid=true;
 memcpy(out->id,out->entry->id,8);out->id[0]^=changed_id;out->baseline[0]=123;
#if CART_NOR109
 if(probe_dual){
  assert(!cb_board109_bank_id(CB_BOARD109_F2AULTRA,0,out->id));
  out->entry=chisburner_nor_db_lookup_gba(out->id);assert(out->entry);
  out->method=CB_NOR_INTEL_DUAL;out->repeated=out->restored=true;out->plan_rc=CB_NOR_UNSUPPORTED;
 }
#endif
 return 0;
}
static int erase_mock(struct cb_nor_job*j,uint32_t a,uint32_t t){
#if CART_NOR109
 if(route_model){assert(j->authorized&&t==120000);int rc=j->io.select(j->io.ctx,a);if(rc)return rc;
  assert(board_selected==a/0x2000000);board_seen|=1u<<board_selected;erase_count++;return 0;}
#endif
 assert(j->authorized&&j->io.alive(j->io.ctx)&&a<0x20000&&t==120000);erase_count++;return execute_failure;
}
static int program_mock(struct cb_nor_job*j,uint32_t a,const uint8_t*p,size_t n,uint32_t t){
#if CART_NOR109
 if(route_model){assert(j->authorized&&p&&n&&t==1000);int rc=j->io.select(j->io.ctx,a);if(rc)return rc;
  assert(board_selected==a/0x2000000);board_seen|=1u<<board_selected;program_count++;return 0;}
#endif
 assert(j->authorized&&j->io.alive(j->io.ctx)&&a+n<=512&&p&&t==1000);program_count++;return execute_failure;
}
static int finish_mock(struct cb_nor_job*j){assert(j->authorized);return execute_failure;}
#define cb_nor_probe_bus probe_mock
#define cb_nor_erase erase_mock
#define cb_nor_program program_mock
#define cb_nor_erase_poll erase_mock
#define cb_nor_program_poll program_mock
#define cb_nor_finish finish_mock
#include "../.integration/openfpgaos/src/firmware/os/kernel/nor108.c"
static uint32_t prepare(void){
 of_nor_info_t report;uint32_t token;
 memset(regs,0,sizeof(regs));CART_LIFECYCLE=CART_LC_READY;CART_STATUS=5;
 probe_failure=execute_failure=changed_id=epoch_read_fault=0;raw_writes=erase_count=program_count=0;
 assert(!nor108_probe(2,&report,sizeof(report))&&(report.flags&OF_NOR_WRITE_OK));
 assert(!nor108_begin(report.generation,512,&token)&&token&&!private_owned&&!busy);
 return token;
}
int main(void){
 memset(regs,0,sizeof(regs));CART_LIFECYCLE=CART_LC_READY;CART_STATUS=5;
 assert(!enter(1));gb_header_model=true;
 for(unsigned t=0;t<256;t++)for(unsigned rom=0;rom<9;rom++)for(unsigned ram=0;ram<7;ram++){
  memset(gb_header_bytes,0,sizeof(gb_header_bytes));
  gb_header_bytes[0x47]=t;gb_header_bytes[0x48]=rom;gb_header_bytes[0x49]=ram;
  uint8_t sum=0;for(unsigned n=0x34;n<=0x4c;n++)sum=(uint8_t)(sum-gb_header_bytes[n]-1);
  gb_header_bytes[0x4d]=sum;
  unsigned expected=t>=0x19&&t<=0x1e?5:t>=0x0f&&t<=0x13?(t==0x10&&ram==5?30:3):0;
  int rc=gb_header();assert(expected?(!rc&&mapper_kind==expected):(rc==CB_NOR_UNSUPPORTED&&!mapper_kind));
 }
 gb_header_bytes[0x4d]^=1;mapper_kind=30;
 assert(gb_header()==CB_NOR_VERIFY&&!mapper_kind&&!raw_writes);
 gb_header_model=false;
 const unsigned mappers[]={3,30,5},banks[]={128,256,512};
 for(unsigned n=0;n<3;n++){
  mapper_kind=mappers[n];bank_cache=UINT32_MAX;unsigned before=raw_writes;
  assert(!select_bus(NULL,(banks[n]-1)*0x4000));
  assert(CART_ADDRESS==0x2000&&CART_TX_DATA==((banks[n]-1)&255));
  assert(raw_writes==before+(mappers[n]==5?2:1));before=raw_writes;
  assert(select_bus(NULL,banks[n]*0x4000)==CB_NOR_UNSUPPORTED&&raw_writes==before);
 }
 assert(!leave());
 puts("PASS OS GB mapper: 16128 header combinations; invalid checksum; final bank and no-write bounds");
 struct cb_nor_cfi geometry={.bytes=0x2000000,.buffer_bytes=1024,.cmdset=CHISBURNER_NOR_CMDSET_AMD,
 .region_count=1,.region={{0,256,0x20000}}};
 assert(!cb_nor_plan_make(g_chisburner_nor_db_gba+3,CB_NOR_AMD_AAA,false,&geometry,&fixture_plan));
 uint8_t data[512]={0};uint32_t length,token=prepare(),other;
 assert(nor108_program(token,0,data,sizeof(data))==CB_NOR_ARGUMENT&&!program_count&&!job_token);
 token=prepare();assert(nor108_erase(token,1,&length)==CB_NOR_ARGUMENT&&!erase_count);
 token=prepare();assert(nor108_erase(token+1,0,&length)==CB_NOR_UNARMED&&!erase_count);
 assert(!nor108_erase(token,0,&length)&&length==0x20000&&erase_count==1);
 assert(!nor108_program(token,0,data,512)&&program_count==1);
 CART_RX_LEVEL=2;CART_RX_DATA=0xabcd;
 assert(!nor108_verify_read(token,0,data,512)&&data[0]==0xcd&&data[511]==0xab);
 assert(job_token==token&&job.authorized&&!private_owned&&!busy);
 assert(!nor108_finish(token)&&!job_token&&!private_owned);
 assert(nor108_verify_read(token,0,data,512)==CB_NOR_UNARMED&&data[0]==0);
 assert(nor108_program(token,0,data,512)==CB_NOR_UNARMED);
 token=prepare();assert(!nor108_legacy_touch());assert(nor108_erase(token,0,&length)==CB_NOR_UNARMED);
 token=prepare();busy=true;assert(nor108_legacy_touch()==CB_NOR_IO&&job_token==token);busy=false;
 assert(!nor108_finish(0)&&!job_token);
 token=prepare();CART_QUERY_ARG++;info.bus=1;mapper_kind=5;
 assert(nor108_erase(token,0,&length)==CB_NOR_CANCELLED&&!raw_writes&&!erase_count&&!job_token);
 token=prepare();CART_LIFECYCLE=0;
 assert(nor108_erase(token,0,&length)==CB_NOR_CANCELLED&&!raw_writes&&!erase_count&&!job_token);
 token=prepare();execute_failure=CB_NOR_STATUS;
 assert(nor108_erase(token,0,&length)==CB_NOR_STATUS&&!job_token&&!private_owned);
 token=prepare();changed_id=1;
 assert(nor108_begin(info.generation,512,&other)==CB_NOR_VERIFY&&!other&&!job_token);
 token=prepare();assert(nor108_begin(info.generation-1,512,&other)==CB_NOR_UNARMED&&!other);
 token=prepare();assert(nor108_begin(info.generation,0x2000002,&other)==CB_NOR_UNARMED&&!other);
 assert(probe_count>20);
 token=prepare();
 assert(nor108_verify_read(token,0,data,2)==CB_NOR_ARGUMENT&&!job_token);
 token=prepare();assert(!nor108_erase(token,0,&length));assert(!nor108_program(token,0,data,512));
 assert(nor108_verify_read(token,510,data,4)==CB_NOR_ARGUMENT&&!job_token);
 token=prepare();assert(!nor108_erase(token,0,&length));assert(!nor108_program(token,0,data,512));
 CART_RX_LEVEL=0;memset(data,0xff,512);
 assert(nor108_verify_read(token,0,data,512)==CB_NOR_IO&&!job_token&&data[0]==0&&data[511]==0);
 token=prepare();assert(!nor108_erase(token,0,&length));assert(!nor108_program(token,0,data,512));
 CART_RX_LEVEL=2;epoch_read_fault=1;memset(data,0xff,512);
 assert(nor108_verify_read(token,0,data,2)==CB_NOR_CANCELLED&&!job_token&&data[0]==0&&!private_owned);
 /* GB204 dispatch must preserve the GBA path, token gate, full-NOR bounds
  * and zero-on-error contract of the production exported function. */
 token=prepare();programmed_to=image_bytes;erase_end=0x20000;
 CART_RX_LEVEL=2;CART_RX_DATA=0xabcd;
 assert(!nor130_verify_full_read(token,512,data,512)&&data[0]==0xcd&&data[511]==0xab);
 assert(!nor108_finish(token));
 for(unsigned fault=0;fault<4;fault++){
  token=prepare();programmed_to=image_bytes;erase_end=0x20000;
  struct cb_nor_plan gb204=*job.plan;gb204.recipe.unit=1;job.plan=&gb204;
  info.bus=1;mapper_kind=5;CART_RX_LEVEL=fault==1?2:1;CART_RX_DATA=0x5a;
  epoch_read_fault=fault==2;
  memset(data,0xff,sizeof(data));
  int result=nor130_verify_full_read(token,fault==3?erase_end-1:0x3ff0,data,512);
  if(!fault){
   assert(!result&&data[0]==0x5a&&data[511]==0x5a&&job_token==token);
   assert(!private_owned&&!busy);assert(!nor108_finish(token));
  }else{
   assert(result<0&&!job_token&&!private_owned&&!busy);
   for(unsigned j=0;j<sizeof(data);j++)assert(!data[j]);
  }
 }
 puts("PASS GB204 full verify: GBA unchanged, GB dispatch, blank-tail range, cleanup, partial rejection");
#if CART_NOR109
 token=prepare();(void)token;probe_dual=1;of_nor_info_t board_report;
 assert(!nor108_probe(2,&board_report,sizeof(board_report))&&board_calls==1);
 assert((board_report.flags&OF_NOR_BOARD_IDS_OK)&&!(board_report.flags&OF_NOR_WRITE_OK));
 assert(nor108_begin(board_report.generation,512,&other)==CB_NOR_UNARMED);
 board_failure=1;
 assert(nor108_probe(2,&board_report,sizeof(board_report))==CB_NOR_IO&&!board_report.flags&&!private_owned);
 board_failure=0;board_good=1;
 assert(!nor108_probe(2,&board_report,sizeof(board_report))&&!(board_report.flags&OF_NOR_WRITE_OK));
 assert(board_report.accessible_bytes==0x10000000);
 unsigned excluded_writes=raw_writes;
 assert(nor108_begin(board_report.generation,0x10000000,&token)==CB_NOR_UNARMED&&!token);
 assert(nor108_begin(board_report.generation,512,&token)==CB_NOR_UNARMED&&!token);
 /* Even a forged eligibility flag cannot bypass the OS capacity check. */
 info.flags|=OF_NOR_WRITE_OK;
 assert(nor108_begin(board_report.generation,512,&token)==CB_NOR_UNARMED&&!token);
 assert(raw_writes==excluded_writes&&!job_token&&!private_owned);
 route_fault_at=0;route_model=board_good=0;
 probe_dual=board_failure=0;
#endif
 puts("PASS OS108: fresh-ID binding, sequential extent, no program before erase, stale token, legacy revoke, reentry, lifecycle change, failed operation cleanup");
#if CART_STREAM131
 token=prepare();
 assert(nor131_program_start(token,0,0,32)==CB_NOR_ARGUMENT&&!job_token);
 token=prepare();assert(!nor108_erase(token,0,&length));
 assert(nor131_program_start(token+1,0,0,32)==CB_NOR_UNARMED);
 assert(nor131_program_start(token,0,0,32)==1 && busy && private_owned);
 /* Every stream predicate has a distinct first-failure diagnostic. */
 uint32_t saved_space=CART_SPACE,saved_id=CART_ID,saved_lc=CART_LIFECYCLE,saved_epoch=CART_QUERY_ARG;
 stream_fault_rc=0;CART_SPACE=0;assert(stream_check(NULL)&&stream_fault_rc==-27);
 CART_ID=0;assert(stream_check(NULL)&&stream_fault_rc==-27); // First failure remains.
 CART_SPACE=saved_space;stream_fault_rc=0;
 assert(!stream_check(NULL)&&alive(NULL)); // Raw adapter data is NOT an identity register.
 uint32_t saved_lease=CART_STREAM_LEASE;CART_STREAM_LEASE=0;
 assert(stream_check(NULL)&&stream_fault_rc==-28&&!alive(NULL));
 CART_STREAM_LEASE=saved_lease;
 CART_ID=saved_id;CART_LIFECYCLE=0;stream_fault_rc=0;assert(stream_check(NULL)&&stream_fault_rc==-29);
 CART_LIFECYCLE=saved_lc;CART_QUERY_ARG++;stream_fault_rc=0;assert(stream_check(NULL)&&stream_fault_rc==-30);
 CART_QUERY_ARG=saved_epoch;stream_fault_rc=0;assert(!stream_check(NULL));
 auth_delay=4;
 assert(nor131_program_step(token,0)==1); // Delayed authorization -> submit.
 assert(!stream_mmio_fault&&!auth_delay);
 assert(nor108_legacy_touch()==CB_NOR_IO);
 CART_STREAM_STATUS=2;stream_model_hold=true;
 assert(nor131_program_step(token,0)==1 && programmed_to==0);
 assert(nor131_program_step(token,0)==1 && busy && private_owned);
 stream_model_hold=false;
 assert(nor131_program_step(token,0)==0 && programmed_to==32 && !busy);
 assert(job_token==token && job.authorized);
 assert(nor131_program_start(token,32,32,32)==1);
 CART_QUERY_ARG++;
 assert(nor131_program_step(token,0)==1);
 assert(nor131_program_step(token,0)==-30 && !job_token && !busy);
 token=prepare();assert(!nor108_erase(token,0,&length));
 assert(nor131_program_start(token,0,0,32)==1);
 auth_delay=2000;
 assert(nor131_program_step(token,0)==1);
 assert(stream_mmio_fault&&stream_fault_rc==-20);
 assert(nor131_program_step(token,0)==1); // Cancel and drain, never program.
 assert(nor131_program_step(token,0)==-20&&!job_token&&!busy&&programmed_to==0);
 auth_delay=0;
 token=prepare();assert(!nor108_erase(token,0,&length));
 assert(nor131_program_start(token,0,0,32)==1);
 assert(nor131_program_step(token+1,0)==1);
 assert(nor131_program_step(token+1,0)==-37 && !job_token);
 puts("PASS kernel131: erase/token gate, MMIO submit ack, retained lock, successful accounting, epoch and caller-token revocation");
 token=prepare();assert(!nor108_erase(token,0,&length));
 assert(nor131_compare_start(token,0,32)==CB_NOR_ARGUMENT); // Not programmed yet.
 token=prepare();assert(!nor108_erase(token,0,&length));
 programmed_to=image_bytes; // Model a completed programming operation.
 assert(nor131_compare_start(token+1,0,32)==CB_NOR_UNARMED);
 assert(nor131_compare_start(token,0,32)==1);
 assert(stream_job.descriptor.words[0]==0x81 && stream_job.descriptor.words[2]==32);
 assert(nor131_program_step(token,0)==1);
 CART_STREAM_STATUS=2;
 assert(nor131_program_step(token,0)==1);
 assert(nor131_program_step(token,0)==0);
 assert(programmed_to==image_bytes && job_token==token && !busy);
 puts("PASS kernel131 compare: completion gate, token, descriptor, drain and unchanged program count");
 uint32_t diag[4];
 assert(!nor131_compare_result(token,diag) && diag[0]==2 && diag[1]==16);
 assert(nor131_compare_result(token+1,diag)==CB_NOR_ARGUMENT && diag[0]==0);
 assert(nor131_compare_start(token,0,32)==1);
 assert(nor131_program_step(token,0)==1);
 CART_STREAM_STATUS=0x62;
 assert(nor131_program_step(token,0)==1);
 assert(nor131_program_step(token,0)==-5 && !job_token);
 assert(!nor131_compare_result(token,diag) && diag[0]==0x62 && diag[2]==16 && diag[3]==0x1234ffffu);
 puts("PASS compare snapshot survives cleanup; wrong-token result access denied");
 token=prepare();assert(!nor108_erase(token,0,&length));
 /* Model the already-authorized GB byte geometry. Exercise the production
  * dispatch, descriptor length and accounting, not a replacement executor. */
 struct cb_nor_plan gb_plan=*job.plan;
 gb_plan.recipe.unit=1;gb_plan.recipe.unlock1=0x555;
 gb_plan.geometry.buffer_bytes=0;gb_plan.window_bytes=0x4000;gb_plan.flags=0;
 job.plan=&gb_plan;
 assert(nor131_program_next(token,0,512,&length)==1 && length==256);
 assert(stream_job.descriptor.words[CB_S131_LENGTH]==256);
 assert(stream_job.descriptor.words[CB_S131_BUFFER]==0);
 assert(nor131_program_step(token,0)==1);
 CART_STREAM_STATUS=2;assert(nor131_program_step(token,0)==1);
 assert(!nor131_program_step(token,0) && programmed_to==256 && job_token==token);
 assert(nor131_program_next(token,256,256,&length)==1 && length==256);
 assert(nor131_program_step(token,1)==1);
 assert(nor131_program_step(token,1)==-36 && !job_token);
 puts("PASS GB batch dispatch: 256-byte descriptor, retained authority, accounting and cancel");
 token=prepare();assert(!nor108_erase(token,0,&length));
 programmed_to=image_bytes;info.bus=bus_kind=1;info.mapper=mapper_kind=5;
 gb_compare_capability=false;
 assert(nor131_compare_start(token,0,32)==CB_NOR_UNSUPPORTED);
 assert(job_token==token && job.authorized && !busy); // Legacy fallback retains authority.
 gb_compare_capability=true;
 assert(nor131_compare_start(token,1,32)==CB_NOR_ARGUMENT);
 assert(nor131_compare_start(token,0,16385)==CB_NOR_ARGUMENT);
 assert(nor131_compare_start(token,0,32)==1);
 assert(stream_job.descriptor.words[0]==0x80 && stream_job.descriptor.words[2]==32);
 assert(nor131_program_step(token,0)==1);
 CART_STREAM_STATUS=2;assert(nor131_program_step(token,0)==1);
 assert(!nor131_program_step(token,0) && programmed_to==image_bytes && job_token==token);
 assert(nor131_compare_start(token,0,32)==1);
 assert(nor131_program_step(token,1)==1);
 assert(nor131_program_step(token,1)==-36 && !job_token);
 puts("PASS GB208 compare: capability fallback, bounds, byte descriptor, completion, cancel and token isolation");
 for(unsigned kind=0;kind<3;kind++){
  token=prepare();assert(!nor108_erase(token,0,&length));
  unsigned mapper=kind==0?3:kind==1?30:5;
  unsigned banks=kind==0?128:kind==1?256:512;
  struct cb_nor_plan bank_plan=*job.plan;bank_plan.accessible_bytes=banks*16384u;
  job.plan=&bank_plan;info.bus=bus_kind=1;info.mapper=mapper_kind=mapper;
  image_bytes=programmed_to=erased_to=erase_end=bank_plan.accessible_bytes;
  uint32_t base=(banks-1)*16384u;
  assert(nor131_compare_start(token,base,16384)==1 && bank_cache==banks-1);
  assert(stream_job.descriptor.words[1]==base && stream_job.descriptor.words[2]==16384);
  assert(nor131_program_step(token,0)==1);
  CART_STREAM_STATUS=2;assert(nor131_program_step(token,0)==1);
  assert(!nor131_program_step(token,0) && bank_cache==UINT32_MAX && job_token==token);
  assert(CART_ADDRESS==0x2000 && CART_TX_DATA==1); // Restore bank1 then invalidate cache.
  unsigned writes_before=raw_writes;
  assert(nor131_compare_start(token,erase_end,1)==CB_NOR_ARGUMENT && raw_writes==writes_before);
 }
 puts("PASS GB208 mapper limits: MBC3/30/5 final banks, window selection, cleanup bank1, no out-of-range bus access");
#endif
#if CART_NOR110
 for(unsigned absent=0;absent<2;absent++)for(unsigned profile=0;profile<5;profile++){
  bool intel=profile==1;
  if(absent&&intel)continue;
  memset(&normal_seed,0,sizeof(normal_seed));
  uint8_t id[8]={2,1,0x7d,0x7e};
  if(intel)memcpy(id,(uint8_t[]){0x89,0,0xb0,0x88,4,0,0x89,0},8);
  if(profile>=2)memcpy(id,(uint8_t[]){1,0,0x7e,0x22,profile==2?0x23:profile==3?0x28:0x48,0x22,1,0x22},8);
  normal_seed.entry=chisburner_nor_db_lookup_gba(id);assert(normal_seed.entry);
  memcpy(normal_seed.id,id,8);normal_seed.method=intel?CB_NOR_INTEL:CB_NOR_MSP55_AAA;
  if(profile>=2)normal_seed.method=profile==2?CB_NOR_AMD_AAA:CB_NOR_AMD_555;
  normal_seed.repeated=normal_seed.restored=normal_seed.cfi_valid=true;
  uint32_t total=intel?0x8000000:0x4000000,sector=intel?0x40000:0x20000;
  if(profile>=2)total=UINT32_C(0x4000000)<<(profile-2);
  struct cb_nor_cfi c={.bytes=total,.buffer_bytes=32,.region_count=1,
   .cmdset=intel?CHISBURNER_NOR_CMDSET_INTEL:CHISBURNER_NOR_CMDSET_AMD};
  c.region[0]=(struct cb_nor_region){0,total/sector,sector};
  assert(!cb_nor_plan_make(normal_seed.entry,normal_seed.method,false,&c,&normal_seed.plan));
  if(absent){
   normal_seed.cfi_valid=false;
   assert(!cb_nor_plan_make(normal_seed.entry,normal_seed.method,false,NULL,&normal_seed.plan));
  }
  normal_model=true;route_model=1;board_selected=0;board_seen=0;
  assert(!nor108_probe(2,&board_report,sizeof(board_report)));
  assert(board_report.accessible_bytes==total&&!(board_report.flags&OF_NOR_WRITE_OK));
  assert(!!(board_report.flags&OF_NOR_CFI_OK)==!absent);
  assert(observed_bank.qualified&&!active_bank&&board_selected==0);
  unsigned before=raw_writes;
  assert(nor108_begin(board_report.generation,total,&token)==CB_NOR_UNARMED&&!token);
  assert(nor108_begin(board_report.generation,512,&token)==CB_NOR_UNARMED&&!token);
  info.flags|=OF_NOR_WRITE_OK;
  assert(nor108_begin(board_report.generation,512,&token)==CB_NOR_UNARMED&&!token);
  assert(raw_writes==before&&!job_token&&!active_bank);
  normal_model=false;route_model=0;
 }
 puts("PASS scope112: five CFI/four no-CFI large GBA profiles remain identifiable but cannot start any write, including small images and forged WRITE_OK");
#endif
 return 0;
}
