// SPDX-License-Identifier: AGPL-3.0-only
// OS-owned NOR108 transport and fresh-identification job gate.
#include "nor108.h"
#include "of_nor.h"
#include "../hal/regs.h"
#if CART_SAVE158
#include "../hal/file.h"
#endif
#include "probe.h"
#include "write_policy112.h"
#if CART_STREAM131
#include "submit131.h"
#endif
#if CART_NOR110
#if !CART_NOR109
#error CART_NOR110 requires CART_NOR109 transport
#endif
#include "bank_probe110.h"
static struct cb_bank110_capture captured_bank,observed_bank;
static struct cb_nor_probe bank_probe_scratch;
static bool active_bank;
static int select_normal_bank(uint32_t address);
#endif
#if CART_NOR109
#include "board_probe109.h"
static struct cb_board109_capture captured_board,observed_board;
static struct cb_board109_scratch board_scratch;
static enum cb_board109_kind active_board;
static int select_board(uint32_t address);
#endif
#include <string.h>
static struct cb_nor_probe observed,fresh;
static struct cb_nor_probe_scratch scratch;
static struct cb_nor_job job;
static of_nor_info_t info;
static uint32_t serial,bus_kind,mapper_kind,bank_cache=UINT32_MAX;
static uint32_t job_token,image_bytes,erase_end,erased_to,programmed_to;
static uint32_t capture_epoch,job_epoch;
static bool busy,private_owned;
static int select_bus(void*ctx,uint32_t address);
static uint32_t next_serial(void){if(++serial==0)++serial;return serial;}
static bool owner_signature_ok(void){
#if CART_STREAM131
 /* Wrapper-owned identity is independent of the raw adapter's read mux.
  * enter() still checks CBN8 before admitting a private operation. */
 return (CART_STREAM_LEASE&0xffff0000u)==0x53130000u;
#else
 return CART_ID==0x384e4243u;
#endif
}
static bool alive(void*ctx){
 (void)ctx;
 return private_owned&&CART_SPACE==31&&owner_signature_ok()&&
        (CART_LIFECYCLE&CART_LC_READY)!=0&&CART_QUERY_ARG==capture_epoch;
}
static uint32_t now_ms(void*ctx){(void)ctx;return (uint32_t)(read_cycles()/(CPU_FREQ_HZ/1000u));}
static void idle_hook(void*ctx){(void)ctx;}
static bool cancelled(void*ctx){(void)ctx;return false;}
static int wait_idle(uint32_t ms,bool done){
 uint32_t start=now_ms(NULL),status=0;
 for(unsigned n=0;n<1000000;n++){
  status=CART_STATUS;
  if(!(status&CART_ST_BUSY))return
   (status&(CART_ST_ERROR|CART_ST_TIMEOUT|CART_ST_ABORTED))||
   (done&&!(status&CART_ST_DONE))?CB_NOR_IO:0;
  if((uint32_t)(now_ms(NULL)-start)>=ms)return CB_NOR_TIMEOUT;
 }
 return CB_NOR_TIMEOUT;
}
static void release(void*ctx){
 (void)ctx;
 if(private_owned&&CART_SPACE==31){CART_COMMAND=6;fence();(void)wait_idle(10,false);}
}
static int leave(void){
 int mapper_rc=0;
 if(bus_kind==1&&mapper_kind&&alive(NULL))mapper_rc=select_bus(NULL,0x4000);
#if CART_NOR110
 if(bus_kind==2&&active_bank&&alive(NULL))mapper_rc=select_normal_bank(0);
 active_bank=false;
#endif
#if CART_NOR109
 if(bus_kind==2&&active_board&&alive(NULL))mapper_rc=select_board(0);
 active_board=CB_BOARD109_NONE;
#endif
 release(NULL);CART_SPACE=0;fence();
 uint32_t start=now_ms(NULL);
 while(CART_SPACE==31){if((uint32_t)(now_ms(NULL)-start)>=10){private_owned=false;return CB_NOR_TIMEOUT;}}
 private_owned=false;bank_cache=UINT32_MAX;
 CART_COMMAND=1;fence();int rc=wait_idle(50,false);return mapper_rc?mapper_rc:rc;
}
static int enter(uint32_t bus){
#if CART_NOR110
 active_bank=false;
#endif
#if CART_NOR109
 active_board=CB_BOARD109_NONE;
#endif
 if((bus!=1&&bus!=2)||CPU_FREQ_HZ<1000u)return CB_NOR_ARGUMENT;
 if(!(CART_LIFECYCLE&CART_LC_READY))return CB_NOR_CANCELLED;
 if(CART_STATUS&CART_ST_BUSY)return CB_NOR_IO;
 CART_COMMAND=1;fence();int rc=wait_idle(50,false);if(rc)return rc;
 // Status reads above acknowledge completion to the owner router.
 CART_SPACE=31;fence();
 uint32_t start=now_ms(NULL);
 while(CART_SPACE!=31||CART_ID!=0x384e4243u){
  if((uint32_t)(now_ms(NULL)-start)>=10){CART_SPACE=0;return CB_NOR_UNSUPPORTED;}
 }
 private_owned=true;bus_kind=bus;bank_cache=UINT32_MAX;
 capture_epoch=CART_QUERY_ARG; // Private logical register 14: lifecycle epoch.
 CART_BUS=bus;CART_TIMEOUT=CPU_FREQ_HZ/100u;fence();
 return alive(NULL)?0:CB_NOR_CANCELLED;
}
static int transaction(uint32_t address,uint16_t*value,bool write){
 if(!alive(NULL))return CB_NOR_CANCELLED;
 if(CART_STATUS&CART_ST_BUSY)return CB_NOR_IO;
 CART_ADDRESS=address;
 if(write){CART_TX_DATA=*value;CART_ARM=0x43424e31u;CART_ARM=0x43424e32u;}
 CART_COMMAND=write?7:2;fence();
 int rc=wait_idle(20,true);if(rc){release(NULL);return rc;}
 if(!alive(NULL))return CB_NOR_CANCELLED;
 if(!write){
  if(CART_RX_LEVEL!=(bus_kind==2?2u:1u))return CB_NOR_IO;
  *value=(uint16_t)CART_RX_DATA;
 }
 return 0;
}
static int read_bus(void*ctx,uint32_t a,uint16_t*v){(void)ctx;return transaction(a,v,false);}
static int write_bus(void*ctx,uint32_t a,uint16_t v){(void)ctx;return transaction(a,&v,true);}
static int select_bus(void*ctx,uint32_t a){
 (void)ctx;
 if(bus_kind==2){
#if CART_NOR110
  if(active_bank)return select_normal_bank(a);
#endif
#if CART_NOR109
  if(active_board)return select_board(a);
#endif
  return a<0x2000000?0:CB_NOR_UNSUPPORTED;
 }
 uint32_t bank=a/0x4000u;
 uint32_t limit=mapper_kind==5?512:mapper_kind==30?256:128;
 if(bank>=limit)return CB_NOR_UNSUPPORTED;
 if(bank==bank_cache)return 0;
 int rc=0;
 if(mapper_kind==5)rc=write_bus(NULL,0x3000,(uint16_t)(bank>>8));
 if(!rc)rc=write_bus(NULL,0x2000,(uint16_t)(bank?bank&255u:1u));
 if(!rc)bank_cache=bank;
 return rc;
}
static struct cb_nor_io io={NULL,select_bus,read_bus,write_bus,now_ms,idle_hook,alive,cancelled,release};
#if CART_NOR109
static uint32_t board_epoch(void*ctx){(void)ctx;return CART_QUERY_ARG;}
static int board_write(void*ctx,unsigned bus,uint32_t a,uint16_t value){
 (void)ctx;
 if(bus_kind!=2||(bus!=2&&bus!=3)||!alive(NULL))return CB_NOR_UNSUPPORTED;
 /* Private FPGA bus 3 permits only the board-control register whitelist.
  * The board probe calls this only after the repeated exact dual ID gate. */
 CART_BUS=bus;fence();
 int rc=transaction(a,&value,true);
 if(alive(NULL)){CART_BUS=2;fence();}
 return rc;
}
static const struct cb_board109_io board_io={NULL,board_write,read_bus,alive,
 board_epoch,now_ms,idle_hook,release};
static int board_sequence(const struct cb_board109_write*w,size_t n){
 if(!n)return CB_NOR_UNSUPPORTED;
 for(size_t k=0;k<n;k++){
  int rc=board_write(NULL,w[k].bus,w[k].address,w[k].value);
  if(rc){active_board=CB_BOARD109_NONE;bank_cache=UINT32_MAX;job.authorized=false;return rc;}
 }
 /* Match the bounded latch-settle interval used during identification. */
 uint32_t start=now_ms(NULL);
 for(unsigned k=0;k<100000;k++){
  if(!alive(NULL)){active_board=CB_BOARD109_NONE;bank_cache=UINT32_MAX;job.authorized=false;return CB_NOR_CANCELLED;}
  if((uint32_t)(now_ms(NULL)-start)>=1)return 0;
  idle_hook(NULL);
 }
 active_board=CB_BOARD109_NONE;bank_cache=UINT32_MAX;job.authorized=false;
 return CB_NOR_TIMEOUT;
}
static int select_board(uint32_t address){
 unsigned bank;uint32_t physical;struct cb_board109_write w[8];
 enum cb_board109_kind kind=active_board;
 if(!kind||cb_board109_map(kind,address,&bank,&physical))return CB_NOR_UNSUPPORTED;
 (void)physical;
 if(bank_cache==bank)return 0;
 int rc=0;
 if(bank_cache==UINT32_MAX)rc=board_sequence(w,cb_board109_unlock(kind,w,8));
 if(!rc&&kind==CB_BOARD109_F2AULTRA)rc=board_sequence(w,cb_board109_select(kind,bank,w,8));
 if(!rc)bank_cache=bank;
 return rc;
}
static int capture_board_ids(struct cb_nor_probe*p){
 memset(&captured_board,0,sizeof(captured_board));
 if(bus_kind!=2||p->method!=CB_NOR_INTEL_DUAL||p->swapped||!p->repeated||!p->restored)return 0;
 for(unsigned kind=CB_BOARD109_F2A128;kind<=CB_BOARD109_F2AULTRA;kind++){
  uint8_t expected[8];
  if(!cb_board109_bank_id(kind,0,expected)&&!memcmp(expected,p->id,8)){
   int rc=cb_board109_capture_full(&board_io,kind,p->id,&board_scratch,&captured_board);
   if(!rc){
    p->plan_rc=CB_NOR_UNSUPPORTED;
    if(captured_board.geometry_valid&&captured_board.ids_valid&&captured_board.restored){
     struct cb_nor_plan plan={0};plan.entry=p->entry;
     if(cb_nor_recipe_make(CB_NOR_INTEL_DUAL,2,false,&plan.recipe))return CB_NOR_UNSUPPORTED;
     plan.geometry=captured_board.profile.geometry;
     plan.flags=chisburner_nor_entry_flags(p->entry);
     plan.window_bytes=captured_board.profile.window_bytes;
     plan.accessible_bytes=plan.geometry.bytes;
     plan.reset_stride=captured_board.profile.reset_stride;
     plan.skip_block_unlock=kind!=CB_BOARD109_F2AULTRA;
     p->plan=plan;p->plan_rc=0;p->cfi_valid=true;
    }
   }
   return rc;
  }
 }
 return 0;
}
#endif
#if CART_NOR110
static int normal_control(void*ctx,uint32_t address,uint8_t value){
 return board_write(ctx,3,address,value);
}
static int fixed_window(void*ctx,uint32_t address){
 (void)ctx;return alive(NULL)&&address<CB_BANK110_WINDOW?0:CB_NOR_UNSUPPORTED;
}
static int normal_probe(void*ctx,struct cb_nor_probe*out){
 (void)ctx;struct cb_nor_io fixed=io;fixed.select=fixed_window;
 return cb_nor_probe_bus(&fixed,2,&scratch,out);
}
static const struct cb_bank110_io normal_io={NULL,normal_control,normal_probe,
 alive,board_epoch,now_ms,idle_hook,release};
static int select_normal_bank(uint32_t address){
 unsigned bank;uint32_t physical;struct cb_bank110_write w[3];
 if(!active_bank||!observed_bank.qualified||
    cb_bank110_map(observed_bank.kind,observed_bank.bytes,address,&bank,&physical))
  return CB_NOR_UNSUPPORTED;
 (void)physical;if(bank_cache==bank)return 0;
 size_t n=cb_bank110_select(observed_bank.kind,observed_bank.bytes,bank,w);
 int rc=n?0:CB_NOR_UNSUPPORTED;
 for(size_t k=0;k<n&&!rc;k++)rc=normal_control(NULL,w[k].address,w[k].value);
 if(!rc){
  uint32_t start=now_ms(NULL);rc=CB_NOR_TIMEOUT;
  for(unsigned k=0;k<100000;k++){
   if(!alive(NULL)){rc=CB_NOR_CANCELLED;break;}
   if((uint32_t)(now_ms(NULL)-start)>=1){rc=0;break;}
   idle_hook(NULL);
  }
 }
 if(rc){active_bank=false;bank_cache=UINT32_MAX;job.authorized=false;}
 else bank_cache=bank;
 return rc;
}
#endif
static int gb_header(void){
 uint8_t header[0x50];
 mapper_kind=0;
 for(unsigned n=0;n<sizeof(header);n++){
  uint16_t v;int rc=read_bus(NULL,0x100+n,&v);if(rc)return rc;header[n]=(uint8_t)v;
 }
 uint8_t sum=0;
 for(unsigned n=0x34;n<=0x4c;n++)sum=(uint8_t)(sum-header[n]-1);
 if(sum!=header[0x4d])return CB_NOR_VERIFY;
 unsigned t=header[0x47];
 if(t>=0x19&&t<=0x1e)mapper_kind=5;
 else if(t>=0x0f&&t<=0x13)
  /* Same header heuristic as UI model.c / FlashGBX RomFileDMG.
   * ROM capacity alone does not establish an extended mapper route. */
  mapper_kind=(t==0x10&&header[0x49]==0x05)?30:3;
 else return CB_NOR_UNSUPPORTED;
 return 0;
}
static int capture(uint32_t bus,struct cb_nor_probe*out){
#if CART_NOR110
 memset(&captured_bank,0,sizeof(captured_bank));
#endif
#if CART_NOR109
 memset(&captured_board,0,sizeof(captured_board));
#endif
 int rc=enter(bus);
 if(!rc&&bus==1)rc=gb_header();
 if(!rc)rc=cb_nor_probe_bus(&io,bus==2?2:1,&scratch,out);
#if CART_NOR109
 if(!rc)rc=capture_board_ids(out);
#endif
#if CART_NOR110
 if(!rc&&bus==2&&!captured_board.profile.kind){
  int bank_rc=cb_bank110_capture(&normal_io,out,&bank_probe_scratch,&captured_bank);
  /* Unsupported before candidate selection is ordinary single-window mode.
   * The same rc AFTER a latch/probe attempt is a fatal capture failure. */
  if(bank_rc!=CB_NOR_UNSUPPORTED||captured_bank.kind!=CB_BANK110_NONE)rc=bank_rc;
  if(!rc&&captured_bank.qualified)out->plan.reset_stride=captured_bank.reset_stride;
 }
#endif
 int cleanup=private_owned?leave():0;
 return rc?rc:cleanup;
}
int nor108_legacy_touch(void){
 if(busy)return CB_NOR_IO;
 job.authorized=false;job_token=0;return 0;
}
#if CART_GB_SAVE175
#include "gb_save175.h"
#if CART_GB_RTC186
#include "gb_rtc186.h"
#if CART_GB_RTC189
#include "gb_rtc189.h"
#endif
#endif
#if CART_GB_RESTORE180
#include "gb_restore180.h"
#endif
#endif
#if CART_SRAM137
/* No ARM or write commands in this path. The private FPGA bus independently
 * rejects every write, even if an older caller left an arm sequence behind. */
int sram137_read(uint32_t address,void *dst,uint32_t length,uint32_t timeout_us){
 uintptr_t p=(uintptr_t)dst;
 if(!dst||!length||length>32||address>=32768u||length>32768u-address||
    timeout_us<1000u||timeout_us>1000000u)return CB_NOR_ARGUMENT;
 /* Application cached SDRAM only. Explicitly exclude both PSRAM windows. */
 if(p<0x10000000u||p>=0x14000000u||length>0x14000000u-p)return CB_NOR_ARGUMENT;
 if(busy)return CB_NOR_IO;
 busy=true;job.authorized=false;job_token=0;mapper_kind=0;
 int rc=enter(2);
 if(!rc && (CART_CAPS&0x00010008u)!=0x00010008u)rc=CB_NOR_UNSUPPORTED;
 if(!rc){
  CART_BUS=4;fence();
  uint32_t start=now_ms(NULL);
  for(uint32_t i=0;i<length&&!rc;i++){
   if((uint32_t)(now_ms(NULL)-start)>=(timeout_us+999u)/1000u){rc=CB_NOR_TIMEOUT;break;}
   /* transaction()'s GBA ROM RX contract is 16 bit; SRAM is byte-wide. */
   if(!alive(NULL)){rc=CB_NOR_CANCELLED;break;}
   CART_ADDRESS=address+i;CART_COMMAND=2;fence();
   rc=wait_idle(20,true);
   if(!rc && (!alive(NULL)||CART_RX_LEVEL!=1))rc=CB_NOR_IO;
   if(!rc)((uint8_t*)dst)[i]=(uint8_t)CART_RX_DATA;
  }
 }
 int cleanup=private_owned?leave():0;busy=false;
 return rc?rc:cleanup;
}
#endif
#if CART_SRAM138
#include "sram138.h"
#endif
#if CART_SAVE139 || CART_EEPROM139
#include "save139.h"
#include "flash166.h"
#if CART_RTC167
#include "rtc167.h"
#if CART_RTC172
#include "rtc172.h"
#endif
#endif
#endif
#if CART_EEPROM139
#include "eeprom139.h"
#endif
#if CART_SAVE158
#if !CART_EEPROM139 || !CART_SRAM138
#error CART_SAVE158 requires CART_EEPROM139 and CART_SRAM138
#endif
#include "save158.h"
#endif
#if CART_EEPROM162
#if !CART_SAVE158 || !CART_EEPROM139
#error CART_EEPROM162 requires CART_SAVE158 and CART_EEPROM139
#endif
#include "eeprom162.h"
#ifdef CB_SAVE171
#include "save171.h"
#endif
#if CART_FLASH170
#include "flash170.h"
#endif
#endif
static void text_copy(char*dst,size_t size,const char*src){
 size_t n=0;if(src)while(n+1<size&&src[n]){dst[n]=src[n];n++;}dst[n]=0;
}
int nor108_probe(uint32_t bus,void*out,uint32_t out_size){
 if(!out||out_size<sizeof(info)||busy)return CB_NOR_ARGUMENT;
 busy=true;job.authorized=false;job_token=0;memset(&info,0,sizeof(info));
 memset(&observed,0,sizeof(observed));mapper_kind=0;
 int rc=capture(bus,&observed);
#if CART_NOR110
 observed_bank=captured_bank;
#endif
#if CART_NOR109
 observed_board=captured_board;
#endif
 info.struct_size=sizeof(info);info.version=OF_NOR_VERSION;
 info.generation=next_serial();info.bus=bus;info.mapper=mapper_kind;info.probe_rc=rc;
 if(!rc){
  info.flags=OF_NOR_ID_OK|(observed.cfi_valid?OF_NOR_CFI_OK:0);
#if CART_NOR109
  if(observed_board.ids_valid&&observed_board.restored)info.flags|=OF_NOR_BOARD_IDS_OK;
#endif
  memcpy(info.id,observed.id,sizeof(info.id));info.plan_rc=observed.plan_rc;
  info.method=observed.method;info.swapped=observed.swapped;
  text_copy(info.model,sizeof(info.model),chisburner_nor_entry_name(observed.entry));
  if(!observed.plan_rc){
   const struct cb_nor_plan*p=&observed.plan;
   info.flash_bytes=p->geometry.bytes;info.buffer_bytes=p->geometry.buffer_bytes;
   info.command_set=p->geometry.cmdset;info.region_count=p->geometry.region_count;
   for(unsigned n=0;n<info.region_count;n++){
    info.regions[n].start=p->geometry.region[n].start;
    info.regions[n].count=p->geometry.region[n].count;
    info.regions[n].sector_bytes=p->geometry.region[n].sector_bytes;
   }
   uint32_t limit=bus==2?0x2000000u:mapper_kind==5?0x800000u:mapper_kind==30?0x400000u:0x200000u;
#if CART_NOR110
   if(bus==2&&observed_bank.qualified&&observed_bank.restored)limit=observed_bank.bytes;
#endif
#if CART_NOR109
   if(bus==2&&observed_board.geometry_valid&&observed_board.restored)
    limit=observed_board.profile.bytes;
#endif
   info.accessible_bytes=info.flash_bytes<limit?info.flash_bytes:limit;
   observed.plan.accessible_bytes=info.accessible_bytes;
   if(info.accessible_bytes<info.flash_bytes)info.flags|=OF_NOR_WINDOW_LIMITED;
   if(cb_nor_write_capacity112(bus,info.flash_bytes))info.flags|=OF_NOR_WRITE_OK;
  }
 }
 memcpy(out,&info,sizeof(info));busy=false;return rc;
}
static int begin_impl(uint32_t generation,uint32_t bytes,uint32_t erase_bytes_requested,uint32_t*token){
 if(!token||busy)return CB_NOR_ARGUMENT;
 *token=0;busy=true;job.authorized=false;job_token=0;
 int rc=CB_NOR_UNARMED;
 if(generation!=info.generation||!(info.flags&OF_NOR_WRITE_OK)||!bytes||!erase_bytes_requested||
    !cb_nor_write_capacity112(info.bus,observed.plan.geometry.bytes)||
    bytes>erase_bytes_requested||erase_bytes_requested>info.accessible_bytes||
    bytes%observed.plan.recipe.unit||erase_bytes_requested%observed.plan.recipe.unit)goto done;
 rc=capture(info.bus,&fresh);if(rc)goto done;
#if CART_NOR110
 if(captured_bank.kind!=observed_bank.kind||captured_bank.bytes!=observed_bank.bytes||
    captured_bank.qualified!=observed_bank.qualified||captured_bank.restored!=observed_bank.restored||
    captured_bank.reset_stride!=observed_bank.reset_stride||
    memcmp(captured_bank.id,observed_bank.id,sizeof(captured_bank.id))||
    memcmp(captured_bank.normal,observed_bank.normal,sizeof(captured_bank.normal))){rc=CB_NOR_VERIFY;goto done;}
#endif
#if CART_NOR109
 if(captured_board.profile.kind!=observed_board.profile.kind||
    captured_board.geometry_valid!=observed_board.geometry_valid||
    memcmp(captured_board.cfi,observed_board.cfi,sizeof(captured_board.cfi))||
    memcmp(captured_board.id,observed_board.id,sizeof(captured_board.id))||
    memcmp(captured_board.normal,observed_board.normal,sizeof(captured_board.normal))){rc=CB_NOR_VERIFY;goto done;}
#endif
 if(fresh.plan_rc||fresh.entry!=observed.entry||fresh.method!=observed.method||
    fresh.swapped!=observed.swapped||memcmp(fresh.id,observed.id,sizeof(fresh.id))||
    memcmp(&fresh.plan.geometry,&observed.plan.geometry,sizeof(fresh.plan.geometry))||
    memcmp(fresh.baseline,observed.baseline,sizeof(fresh.baseline))||mapper_kind!=info.mapper){rc=CB_NOR_VERIFY;goto done;}
 uint32_t start,len;
 if(cb_nor_sector_at(&observed.plan,erase_bytes_requested-1,&start,&len)||
    start+len>info.accessible_bytes){rc=CB_NOR_ARGUMENT;goto done;}
 image_bytes=bytes;erase_end=start+len;erased_to=programmed_to=0;job_epoch=capture_epoch;
 job=(struct cb_nor_job){&observed.plan,io,true,0,0};job_token=next_serial();*token=job_token;rc=0;
done:busy=false;return rc;
}
int nor108_begin(uint32_t generation,uint32_t bytes,uint32_t*token){return begin_impl(generation,bytes,bytes,token);}
int nor130_begin_full(uint32_t generation,uint32_t program_bytes,uint32_t erase_bytes_requested,uint32_t*token){
 return begin_impl(generation,program_bytes,erase_bytes_requested,token);
}
static int operation_begin(uint32_t token){
 if(busy)return CB_NOR_IO;
 if(!token||token!=job_token||!job.authorized)return CB_NOR_UNARMED;
 busy=true;int rc=enter(info.bus);
 if(!rc&&capture_epoch!=job_epoch){
  /* A different lifecycle may mean a different cartridge. Never restore
   * mapper registers using the previous cartridge's assumptions. */
  mapper_kind=0;rc=CB_NOR_CANCELLED;
 }
#if CART_NOR109
 if(!rc&&info.bus==2&&observed_board.geometry_valid&&observed_board.restored)
  active_board=observed_board.profile.kind;
#endif
#if CART_NOR110
 if(!rc&&info.bus==2&&observed_bank.qualified&&observed_bank.restored)
  active_bank=true;
#endif
 if(rc){if(private_owned)(void)leave();job.authorized=false;job_token=0;busy=false;}
 return rc;
}
static int operation_end(int rc){
 int cleanup=leave();if(!rc)rc=cleanup;
 if(rc){job.authorized=false;job_token=0;}
 busy=false;return rc;
}
#if CART_STREAM131
/* Kernel-only, single descriptor operation. No userspace dispatch yet.
 * begin_impl owns fresh identification; operation_begin validates its token
 * and epoch. Keep private ownership across polling until both adapters drain. */
static struct cb_submit131 stream_job;
static uint32_t stream_token,stream_address,stream_length;
static bool stream_active,stream_mmio_fault;
static int stream_fault_rc;
static bool stream_compare;
static uint32_t compare_result[4],compare_result_token;
static bool compare_result_valid;
static uint32_t compare_word(unsigned index){
 CART_STREAM_INDEX=index;fence();
 for(unsigned n=0;CART_STREAM_INDEX!=index;n++){
  if(n==1000 || !alive(NULL)){stream_mmio_fault=true;return 0;}
 }
 fence();return CART_STREAM_DATA;
}
static int stream_check(void *ctx);
static uint32_t stream_read(void *ctx,unsigned reg){
 (void)ctx;fence();
 switch(reg){
 case CB_S131_REG_STATUS:{
  uint32_t status=CART_STREAM_STATUS;
  if(stream_active && stream_compare && !compare_result_valid &&
     (status&3u)==2u && alive(NULL) && capture_epoch==job_epoch){
   compare_result[0]=status;
   compare_result[1]=compare_word(12);compare_result[2]=compare_word(13);
   compare_result[3]=compare_word(14);
   if(!stream_mmio_fault){compare_result_token=stream_token;compare_result_valid=true;}
  }
  return status;
 }
 case CB_S131_REG_KEY:return CART_STREAM_KEY;
 case CB_S131_REG_LEASE:return CART_STREAM_LEASE;
 default:stream_mmio_fault=true;return 0;
 }
}
static void stream_write(void *ctx,unsigned reg,uint32_t value){
 (void)ctx;
 if(reg==CB_S131_REG_KEY && value==CB_S131_KEY1){
  if(stream_check(NULL)){stream_mmio_fault=true;return;}
  CART_STREAM_AUTH=0x53413131u;fence();CART_STREAM_AUTH=0x53413132u;fence();
  /* MMIO fences order accesses; they do not acknowledge the registered
   * permission/readback pipeline. Wait boundedly, without resending keys. */
  unsigned ack=0;
  while(!(CART_STREAM_AUTH&1u)){
   if(++ack==1000 || !alive(NULL)){
    stream_fault_rc=-20;stream_mmio_fault=true;return;
   }
   fence();
  }
 }
 if(stream_mmio_fault && (reg==CB_S131_REG_KEY ||
    (reg==CB_S131_REG_CONTROL && value==CB_S131_SUBMIT)))return;
 switch(reg){
 case CB_S131_REG_INDEX:CART_STREAM_INDEX=value;break;
 case CB_S131_REG_DATA:CART_STREAM_DATA=value;break;
 case CB_S131_REG_KEY:CART_STREAM_KEY=value;break;
 case CB_S131_REG_CONTROL:CART_STREAM_CONTROL=value;break;
 case CB_S131_REG_LEASE:CART_STREAM_LEASE=value;break;
 default:stream_mmio_fault=true;return;
 }
 fence();
 if(reg==CB_S131_REG_CONTROL && value==CB_S131_SUBMIT){
  /* Submission consumes the written-word mask even when rejected. Read back
   * that acknowledgment before allowing the transport to inspect sticky DONE. */
  unsigned n=0;
  while(CART_STREAM_KEY&0xffffu){
   if(++n==1000 || !alive(NULL)){stream_fault_rc=-21;stream_mmio_fault=true;break;}
  }
  fence();
 }
}
static int stream_check(void *ctx){
 (void)ctx;
 /* Preserve the FIRST failed invariant before cleanup changes registers.
  * Diagnostic codes do not weaken any of the original checks. */
 int fault=0;
 if(stream_mmio_fault)fault=stream_fault_rc?stream_fault_rc:-22;
 else if(!busy)fault=-23;
 else if(!job.authorized)fault=-24;
 else if(!stream_token || stream_token!=job_token)fault=-25;
 else if(!private_owned)fault=-26;
 else if(CART_SPACE!=31)fault=-27;
 else if(!owner_signature_ok())fault=-28;
 else if(!(CART_LIFECYCLE&CART_LC_READY))fault=-29;
 else if(CART_QUERY_ARG!=capture_epoch)fault=-30;
 else if(capture_epoch!=job_epoch)fault=-31;
 else if(erased_to!=erase_end)fault=-32;
 else if(stream_compare?
   (programmed_to!=image_bytes || stream_address>erase_end || stream_length>erase_end-stream_address):
   (programmed_to!=stream_address || stream_address>image_bytes || stream_length>image_bytes-stream_address))fault=-33;
 if(fault&&!stream_fault_rc)stream_fault_rc=fault;
 return fault!=0;
}
int nor131_program_start(uint32_t token,uint32_t source,uint32_t address,uint32_t length){
 stream_fault_rc=0;
 int rc=operation_begin(token);if(rc)return rc==CB_NOR_CANCELLED?-34:rc;
 stream_compare=false;
 if(!length || erased_to!=erase_end || address!=programmed_to ||
    address>image_bytes || length>image_bytes-address)
  return operation_end(CB_NOR_ARGUMENT);
 /* Distinguish the new MMIO window from old cores returning zero. */
 if((CART_STREAM_LEASE&0xffff0000u)!=0x53130000u)return operation_end(CB_NOR_UNSUPPORTED);
 struct cb_stream131_descriptor descriptor;
 rc=cb_stream131_build(job.plan,source,address,length,CPU_FREQ_HZ,&descriptor);
 if(rc)return operation_end(rc== -2?CB_NOR_UNSUPPORTED:CB_NOR_ARGUMENT);
 rc=select_bus(NULL,address);if(rc)return operation_end(rc==CB_NOR_CANCELLED?-35:rc);
 stream_token=token;stream_address=address;stream_length=length;stream_mmio_fault=false;stream_fault_rc=0;
 const struct cb_submit131_io transport={NULL,stream_read,stream_write,stream_check};
 rc=cb_submit131_begin(&stream_job,&transport,&descriptor,now_ms(NULL),2000);
 if(rc!=1)return operation_end(stream_fault_rc?stream_fault_rc:rc);
 stream_active=true;return 1;
}
int nor131_program_step(uint32_t token,int cancel){
 if(!stream_active || !busy)return CB_NOR_ARGUMENT;
 /* Wrong-token callers cannot advance the job; stop it without releasing early. */
 if(!stream_fault_rc && (cancel || token!=stream_token))stream_fault_rc=cancel?-36:-37;
 int rc=cb_submit131_step(&stream_job,now_ms(NULL),cancel || token!=stream_token);
 if(rc==1)return 1;
 stream_active=false;
 if(stream_fault_rc)rc=stream_fault_rc;
 rc=operation_end(rc);
 if(!rc && !stream_compare)programmed_to+=stream_length;
 stream_token=0;return rc;
}
int nor131_program_next(uint32_t token,uint32_t address,uint32_t maximum,uint32_t *length){
 if(!length)return CB_NOR_ARGUMENT;
 *length=0;
 if(busy)return CB_NOR_IO;
 if(!token || token!=job_token || !job.authorized)return CB_NOR_UNARMED;
 if(address>image_bytes || !maximum || maximum>image_bytes-address)return CB_NOR_ARGUMENT;
 uint32_t amount=cb_stream131_chunk(job.plan,address,maximum);
 if(job.plan->flags&CHISBURNER_NOR_FLAG_DISABLE_BUFFER_PROGRAM)amount=job.plan->recipe.unit;
 if(!amount || amount>maximum)return CB_NOR_ARGUMENT;
 int rc=nor131_program_start(token,address,address,amount);
 if(rc==1)*length=amount;
 return rc;
}
int nor131_program_pending(void){return stream_active;}
int nor131_compare_result(uint32_t token,uint32_t *out){
 if(!out)return CB_NOR_ARGUMENT;
 memset(out,0,4*sizeof(*out));
 if(busy)return CB_NOR_IO;
 if(!token || !compare_result_valid || token!=compare_result_token)return CB_NOR_ARGUMENT;
 memcpy(out,compare_result,sizeof(compare_result));return 0;
}
static int nor208_gb_compare_start(uint32_t token,uint32_t address,uint32_t length){
 /* GB-only branch. Keep legacy GBA admission/descriptor unchanged. */
 if(info.bus!=1 || (info.mapper!=3 && info.mapper!=30 && info.mapper!=5))return CB_NOR_UNSUPPORTED;
 if(!length || length>16384u || (address&0x3fffu) || programmed_to!=image_bytes ||
    erased_to!=erase_end || address>erase_end || length>erase_end-address ||
    erase_end>job.plan->accessible_bytes)return CB_NOR_ARGUMENT;
 int rc=operation_begin(token);if(rc)return rc;
 compare_result_valid=false;compare_result_token=0;
 if((CART_STREAM_LEASE&0xffff0000u)!=0x53130000u)return operation_end(CB_NOR_UNSUPPORTED);
 stream_mmio_fault=false;stream_fault_rc=0;
 uint32_t capability=compare_word(15);
 if(stream_mmio_fault)return operation_end(CB_NOR_IO);
 if(capability!=0x47423230u){
  int cleanup=operation_end(0); // Clean unsupported capability permits legacy fallback.
  return cleanup?cleanup:CB_NOR_UNSUPPORTED;
 }
 rc=select_bus(NULL,address);if(rc)return operation_end(rc);
 struct cb_stream131_descriptor descriptor={0};
 descriptor.words[0]=0x80;descriptor.words[1]=address;descriptor.words[2]=length;
 descriptor.words[3]=image_bytes;descriptor.words[4]=erase_end;
 stream_compare=true;stream_token=token;stream_address=address;stream_length=length;
 stream_mmio_fault=false;stream_fault_rc=0;
 const struct cb_submit131_io transport={NULL,stream_read,stream_write,stream_check};
 rc=cb_submit131_begin(&stream_job,&transport,&descriptor,now_ms(NULL),10000);
 if(rc!=1)return operation_end(rc);
 stream_active=true;return 1;
}
int nor131_compare_start(uint32_t token,uint32_t address,uint32_t length){
 if(busy)return CB_NOR_IO;
 if(!token || token!=job_token || !job.authorized)return CB_NOR_UNARMED;
 if(info.bus==1)return nor208_gb_compare_start(token,address,length);
 if(info.bus!=2)return CB_NOR_UNSUPPORTED;
#if CART_NOR109
 if(observed_board.geometry_valid)return CB_NOR_UNSUPPORTED;
#endif
#if CART_NOR110
 if(observed_bank.qualified)return CB_NOR_UNSUPPORTED;
#endif
 int rc=operation_begin(token);if(rc)return rc;
 compare_result_valid=false;compare_result_token=0;
 if(!length || length>65536u || (address|length)&1u || programmed_to!=image_bytes ||
    erased_to!=erase_end || address>erase_end || length>erase_end-address || erase_end>0x2000000u)
  return operation_end(CB_NOR_ARGUMENT);
 if((CART_STREAM_LEASE&0xffff0000u)!=0x53130000u)return operation_end(CB_NOR_UNSUPPORTED);
 rc=select_bus(NULL,address);if(rc)return operation_end(rc);
 struct cb_stream131_descriptor descriptor={0};
 descriptor.words[0]=0x81;descriptor.words[1]=address;descriptor.words[2]=length;
 descriptor.words[3]=image_bytes;descriptor.words[4]=erase_end;
 stream_compare=true;stream_token=token;stream_address=address;stream_length=length;stream_mmio_fault=false;stream_fault_rc=0;
 const struct cb_submit131_io transport={NULL,stream_read,stream_write,stream_check};
 rc=cb_submit131_begin(&stream_job,&transport,&descriptor,now_ms(NULL),10000);
 if(rc!=1)return operation_end(rc);
 stream_active=true;return 1;
}
#else
int nor131_compare_result(uint32_t token,uint32_t *out){
 (void)token;if(out)memset(out,0,4*sizeof(*out));return CB_NOR_UNSUPPORTED;
}
int nor131_compare_start(uint32_t token,uint32_t address,uint32_t length){
 (void)token;(void)address;(void)length;return CB_NOR_UNSUPPORTED;
}
int nor131_program_next(uint32_t token,uint32_t address,uint32_t maximum,uint32_t *length){
 (void)token;(void)address;(void)maximum;if(length)*length=0;return CB_NOR_UNSUPPORTED;
}
int nor131_program_pending(void){return 0;}
int nor131_program_start(uint32_t token,uint32_t source,uint32_t address,uint32_t length){
 (void)token;(void)source;(void)address;(void)length;return CB_NOR_UNSUPPORTED;
}
int nor131_program_step(uint32_t token,int cancel){
 (void)token;(void)cancel;return CB_NOR_UNSUPPORTED;
}
#endif
int nor108_erase(uint32_t token,uint32_t address,uint32_t*length){
 if(!length)return CB_NOR_ARGUMENT;
 *length=0;
 int rc=operation_begin(token);if(rc)return rc;
 uint32_t start,len;
 if(address!=erased_to||address>=erase_end||programmed_to||
    cb_nor_sector_at(job.plan,address,&start,&len)||start!=address||len>erase_end-address)
  return operation_end(CB_NOR_ARGUMENT);
 rc=cb_nor_erase(&job,address,120000);
 if(!rc){erased_to+=len;*length=len;}
 return operation_end(rc);
}
int nor130_erase_poll(uint32_t token,uint32_t address,uint32_t*length){
 if(!length)return CB_NOR_ARGUMENT;
 *length=0;
 int rc=operation_begin(token);if(rc)return rc;uint32_t start,len;
 if(address!=erased_to||address>=erase_end||programmed_to||
    cb_nor_sector_at(job.plan,address,&start,&len)||start!=address||len>erase_end-address)
  return operation_end(CB_NOR_ARGUMENT);
 rc=cb_nor_erase_poll(&job,address,120000);
 if(!rc){erased_to+=len;*length=len;}return operation_end(rc);
}
int nor108_program(uint32_t token,uint32_t address,const void*data,uint32_t length){
 int rc=operation_begin(token);if(rc)return rc;
 if(!data||!length||erased_to!=erase_end||address!=programmed_to||
    address>image_bytes||length>image_bytes-address)return operation_end(CB_NOR_ARGUMENT);
 rc=cb_nor_program(&job,address,data,length,1000);
 if(!rc)programmed_to+=length;
 return operation_end(rc);
}
int nor130_program_poll(uint32_t token,uint32_t address,const void*data,uint32_t length){
 int rc=operation_begin(token);if(rc)return rc;
 if(!data||!length||erased_to!=erase_end||address!=programmed_to||
    address>image_bytes||length>image_bytes-address)return operation_end(CB_NOR_ARGUMENT);
 rc=cb_nor_program_poll(&job,address,data,length,1000);
 if(!rc)programmed_to+=length;
 return operation_end(rc);
}
int nor108_finish(uint32_t token){
 if(!token){if(busy)return CB_NOR_IO;job.authorized=false;job_token=0;return 0;}
 int rc=operation_begin(token);if(rc)return rc;
 rc=programmed_to==image_bytes?cb_nor_finish(&job):CB_NOR_VERIFY;
 job.authorized=false;job_token=0;return operation_end(rc);
}
int nor108_verify_read(uint32_t token,uint32_t address,void*dst,uint32_t length){
 /* Keep the write job alive until full-image verification ends. The legacy
  * dump engine cannot address additional board windows, so it must not be
  * used to verify a banked write by silently truncating the address. */
 if(!dst||!length||length>1024)return CB_NOR_ARGUMENT;
 memset(dst,0,length);
 int rc=operation_begin(token);if(rc)return rc;
 unsigned unit=job.plan->recipe.unit;
 if(programmed_to!=image_bytes||address>image_bytes||length>image_bytes-address||
    address%unit||length%unit)return operation_end(CB_NOR_ARGUMENT);
 uint8_t*out=dst;
 for(unsigned n=0;n<length;n+=unit){
  uint32_t a=address+n;uint16_t value;
  if(!alive(NULL)){rc=CB_NOR_CANCELLED;break;}
  rc=select_bus(NULL,a);
  if(rc){mapper_kind=0;break;} /* No stale mapper restoration after failure. */
  uint32_t pa=unit==2?(a&0x1ffffffu):(a<0x4000?a:0x4000+(a&0x3fff));
  rc=read_bus(NULL,pa,&value);if(rc)break;
  out[n]=(uint8_t)value;if(unit==2)out[n+1]=(uint8_t)(value>>8);
 }
 rc=operation_end(rc);
 if(rc)memset(dst,0,length); /* Never expose a partial block as valid output. */
 return rc;
}
#include "gb_verify204.h"
int nor130_verify_full_read(uint32_t token,uint32_t address,void*dst,uint32_t length){
 if(!dst||!length||length>1024)return CB_NOR_ARGUMENT;
 memset(dst,0,length);
 int rc=operation_begin(token);if(rc)return rc;unsigned unit=job.plan->recipe.unit;
 if(programmed_to!=image_bytes||address>erase_end||length>erase_end-address||
    address%unit||length%unit)return operation_end(CB_NOR_ARGUMENT);
 if(bus_kind==1&&unit==1){
  rc=operation_end(gb_verify204_read(address,dst,length));
  if(rc)memset(dst,0,length);
  return rc;
 }
 uint8_t*out=dst;
 for(unsigned n=0;n<length;n+=unit){
  uint32_t a=address+n;uint16_t value;if(!alive(NULL)){rc=CB_NOR_CANCELLED;break;}
  rc=select_bus(NULL,a);if(rc){mapper_kind=0;break;}
  uint32_t pa=unit==2?(a&0x1ffffffu):(a<0x4000?a:0x4000+(a&0x3fff));
  rc=read_bus(NULL,pa,&value);if(rc)break;
  out[n]=(uint8_t)value;if(unit==2)out[n+1]=(uint8_t)(value>>8);
 }
 rc=operation_end(rc);
 if(rc)memset(dst,0,length);
 return rc;
}
