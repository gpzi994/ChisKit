#include "ui.h"
#include "transfer.h"
#include "version.h"
#include "of_cart.h"
#include "caps137.h"
#ifdef CB_GB_RTC186
#include "gb_rtc186_ui.h"
static of_gb186_report gb186_report;
#endif
#ifdef CB_GB_SAVE175
#include "of_gb175.h"
#include "../../core/gb_save175/gb_save175.h"
#endif
#ifdef CB_ROM164
#include "capacity164.h"
static int capacity_nor164(void *ctx,of_nor_info_t *n){(void)ctx;return of_nor_probe(2,n);}
#endif
#if defined(CB_SAVE160) && (!defined(CB_EEPROM139) || !defined(CB_SRAM138) || !defined(CB_SRAM137))
#error "SAVE160 requires automatic save detection and SRAM support"
#endif
#ifdef CB_EEPROM139
#include "of_save139.h"
#endif
#include "of_video.h"
#include "of_input.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#ifdef CB_GB_SAVE176
#include "gb_selection176.h"
static uint8_t gb176_probe_snapshot[131072] __attribute__((aligned(64)));
static of_gb175_report gb176_probe_report;
#endif
#ifdef CB_GB_EMPTY181
#include "gb_fallback181.h"
#include "gb_database183.h"
static int gb183_read(void *ctx,uint32_t addr,void *out,unsigned n){
 uint32_t mapped;
 if(gb183_address((const struct cb_cart *)ctx,addr,n,&mapped))return OF_ERR_BAD_RANGE;
 return of_cart_read_space(OF_CART_BUS_GB_GBC,OF_CART_SPACE_IDENTIFY,mapped,out,n,50000);
}
static int gb181_read(void *ctx,uint32_t addr,void *out,unsigned n){
 (void)ctx;return of_cart_read(OF_CART_BUS_GB_GBC,addr,out,n,50000);
}
static int gb181_safe(void *ctx){(void)ctx;return of_cart_enter_safe_idle();}
#endif
#ifdef CB_GB_DIAG182
#include "gb_preflight182.h"
static struct gb182_trace gb182_trace;
static char gb182_notice[64];
#endif
#ifdef CB_LANG174
#include "settings174.h"
#endif
#ifdef CB_RTC167
#include "of_rtc167.h"
#include "rtc167_ui.h"
static of_rtc167_report rtc167_report;
#endif
#ifdef CB_SAVE148
#include "of_output.h"
#include "save_log148.h"
#include "header_diag.h"
static char save148_notice[64];
static int header150_log(const struct cb_cart *c){
 static char log[8192];static unsigned sequence;
 char path[112];
 int n=snprintf(log,sizeof log,"HEADER150 result=%d platform=%d rc=%d safe=%d reads=%u\n",c->result,c->platform,c->rc,c->safe_rc,cb_header150_trace.count);
 for(unsigned i=0;i<cb_header150_trace.count;i++){
  const struct cb_header150_read *r=&cb_header150_trace.reads[i];
  n+=snprintf(log+n,sizeof log-(unsigned)n,"%u bus=%u addr=%08X n=%u rc=%d data=",i,r->bus,r->address,r->length,r->rc);
  if(!r->rc)for(unsigned j=0;j<r->length&&j<32;j++)n+=snprintf(log+n,sizeof log-(unsigned)n,"%02X",r->bytes[j]);
  n+=snprintf(log+n,sizeof log-(unsigned)n,"\n");
 }
 n+=snprintf(log+n,sizeof log-(unsigned)n,"SIZE151 header_valid=%d reads=%u rc=%d safe=%d\n",c->header_valid,cb_size151_trace.count,c->rc,c->safe_rc);
 for(unsigned i=0;i<2;i++){
  const struct cb_header150_read *r=&cb_size151_trace.last[i];
  n+=snprintf(log+n,sizeof log-(unsigned)n,"tail%u addr=%08X n=%u rc=%d data=",i,r->address,r->length,r->rc);
  if(!r->rc)for(unsigned j=0;j<r->length&&j<32;j++)n+=snprintf(log+n,sizeof log-(unsigned)n,"%02X",r->bytes[j]);
  n+=snprintf(log+n,sizeof log-(unsigned)n,"\n");
 }
#ifdef CB_ROM164
 n+=snprintf(log+n,sizeof log-(unsigned)n,"ROM164 source=%u bytes=%lu header_sha1=",c->rom_source,(unsigned long)c->rom_bytes);
 for(unsigned i=0;i<20;i++)n+=snprintf(log+n,sizeof log-(unsigned)n,"%02X",c->rom_header_sha1[i]);
 n+=snprintf(log+n,sizeof log-(unsigned)n,"\n");
#endif
 n+=snprintf(log+n,sizeof log-(unsigned)n,"END HEADER152\n");
 unsigned long tick=OF_CART_SVC_HAS(timer_get_ms)?OF_SVC->timer_get_ms():0;
 int rc=-4;
 for(unsigned i=0;i<16&&rc==-4;i++){
  snprintf(path,sizeof path,"/Saves/chiskit/common/hd152-%08lx-%04x.txt",tick,++sequence);
  rc=of_output_open(path,(uint32_t)n);
 }
 if(!rc)rc=of_output_write(0,log,(uint32_t)n);
 if(!rc)rc=cb_output_verify161(log,(uint32_t)n);
 return rc;
}
#ifdef CB_GB_DIAG182
static int gb182_log(const struct cb_ui *u){
 static char log[2048];static unsigned sequence;char path[112];
 const struct gb182_trace *t=&gb182_trace;
 int n=snprintf(log,sizeof log,
  "GB182 version=%s title=%.16s type=%02X header_ram=%lu checksum=%02X\n"
  "stage=%d rc=%d read=%d cleanup=%d blocks=%u mismatch=%u db=%d\n"
  "analysis_rc=%d nor_rc=%d matched=%d writable=%d\n"
  "probe_rc=%d capture_rc=%ld detail=%ld cleanup=%ld compared=%lu\n"
  "detected=%lu selected=%lu choice=%u source=%u ready=%d restore_enabled=%d\nheader=",
  CB_RELEASE_NAME,u->cart.title,u->cart.type,(unsigned long)u->cart.ram_bytes,u->cart.checksum,
  t->stage,t->rc,t->read_rc,t->cleanup_rc,t->blocks,t->mismatch,t->db_rc,
  u->analysis.rc,u->analysis.nor_rc,u->analysis.matched,u->analysis.writable,
  u->gb_probe_rc,(long)gb176_probe_report.rc,(long)gb176_probe_report.detail,
  (long)gb176_probe_report.cleanup_rc,(unsigned long)gb176_probe_report.compared,
  (unsigned long)u->gb_detected_bytes,(unsigned long)u->gb_save175_bytes,u->gb_choice,
  u->gb_capacity_source,u->gb_save175_ready,cb_action_enabled(u,5));
 for(unsigned i=0;i<96;i++)n+=snprintf(log+n,sizeof log-(unsigned)n,"%02X",t->header[i]);
 n+=snprintf(log+n,sizeof log-(unsigned)n,"\nEND GB182\n");
 unsigned long tick=OF_CART_SVC_HAS(timer_get_ms)?OF_SVC->timer_get_ms():0;int rc=-4;
 for(unsigned i=0;i<16&&rc==-4;i++){
  snprintf(path,sizeof path,"/Saves/chiskit/common/gb182-%08lx-%04x.txt",tick,++sequence);
  rc=of_output_open(path,(uint32_t)n);
 }
 if(!rc)rc=of_output_write(0,log,(uint32_t)n);
 if(!rc)rc=cb_output_verify161(log,(uint32_t)n);
 return rc;
}
#endif
#ifdef CB_RTC167
static int rtc167_log(const struct cb_cart *c,int result,const of_rtc167_report *r){
 static char log[768];static unsigned sequence;char path[112];
 int n=snprintf(log,sizeof log,
  "chiskit " CB_RELEASE_NAME "\nROM=%.16s code=%.15s\nrc=%d version=%lu flags=%lu stage=%lu cleanup=%ld\n"
  "status=%02X date=%02X %02X %02X %02X %02X %02X %02X\n"
  "rom=%04X %04X %04X mapped=%04X %04X %04X\nUI=%s\n"
  "Only RTC read commands 63/65. No clock reset or date/status writes.\nEND RTC167\n",
  c->title,c->code,result,(unsigned long)r->version,(unsigned long)r->flags,(unsigned long)r->stage,(long)r->cleanup_rc,
  r->status,r->date[0],r->date[1],r->date[2],r->date[3],r->date[4],r->date[5],r->date[6],
  r->rom[0],r->rom[1],r->rom[2],r->mapped[0],r->mapped[1],r->mapped[2],c->rtc);
 if(n<0||(unsigned)n>=sizeof log)return -1;
 unsigned long tick=OF_CART_SVC_HAS(timer_get_ms)?OF_SVC->timer_get_ms():0;int rc=-4;
 for(unsigned i=0;i<16&&rc==-4;i++){
  snprintf(path,sizeof path,"/Saves/chiskit/common/rtc167-%08lx-%04x.txt",tick,++sequence);
  rc=of_output_open(path,(uint32_t)n);
 }
 if(!rc)rc=of_output_write(0,log,(uint32_t)n);
 if(!rc)rc=cb_output_verify161(log,(uint32_t)n);
 return rc;
}
#endif
static int save148_log(const char *title,int result,const of_save141_diagnostic *d){
 static char log[1024];static unsigned sequence;
 char path[112];int n=cb_save148_format(log,sizeof log,title,result,d);
 if(n<0)return n;
 unsigned long tick=OF_CART_SVC_HAS(timer_get_ms)?OF_SVC->timer_get_ms():0;
 int rc=-4;
 for(unsigned i=0;i<16&&rc==-4;i++){
  snprintf(path,sizeof path,"/Saves/chiskit/common/sv149-%08lx-%04x.txt",tick,++sequence);
  rc=of_output_open(path,(uint32_t)n);
 }
 if(!rc)rc=of_output_write(0,log,(uint32_t)n);
 if(!rc)rc=cb_output_verify161(log,(uint32_t)n);
 return rc;
}
#endif
#ifdef CB_CAMERA136
#include "camera136.h"
#endif
static struct cb_ui state;
#ifdef CB_GBA_HEADER140
#include "gba_header140.h"
struct cb_gba_header140 cb_gba_header140;
#ifdef CB_STAGES141
#include "stages141.h"
#include "save_read142.h"
#include "stages141_log.h"
#include "of_output.h"
unsigned cb_stage141_selected=8,cb_stage141_ran;
static of_save145_banks cb_banks145;
static of_save144_readonly cb_scan144;
static of_save143_readonly cb_session143;
struct cb_read142 cb_save_read142;
static int save_read142(unsigned a,void *dst,unsigned n){
 return of_cart_read_space(OF_CART_BUS_GBA,OF_CART_SPACE_SAVE_PARALLEL,a,dst,n,50000);
}
int cb_stage141_rc,cb_stage141_safe,cb_stage141_executed;
int cb_stage141_log_rc=-100;
const char *cb_stage141_log_phase="pending";
struct cb_gba_header140 cb_stage141_before;
of_save141_diagnostic cb_stage141_save;
static void stage141_log(void){
 static char log[16384];static unsigned sequence;
 char path[112],check[256];
 int n=cb_log141_format(log,sizeof log,cb_stage141_ran,cb_stage141_executed,
  cb_stage141_rc,cb_stage141_safe,&cb_stage141_before,&cb_gba_header140,&cb_stage141_save);
 cb_stage141_log_phase="format";
 cb_stage141_log_rc=-100;if(n<0){cb_stage141_log_rc=-1;return;}
 size_t used=(size_t)n;
 if(cb_log141_add(log,sizeof log,&used,"READ142 completed=%u/64 rc=%d differences=%u first_round=%u first_address=%u\n",
 cb_save_read142.completed,cb_save_read142.rc,cb_save_read142.differences,cb_save_read142.first_round,cb_save_read142.first_address)){
  cb_stage141_log_rc=-1;return;
 }
 for(unsigned p=0;p<cb_save_read142.completed;p++){
  if(cb_log141_add(log,sizeof log,&used,"round=%u single4=%02X block=",p,cb_save_read142.single[p])){cb_stage141_log_rc=-1;return;}
  for(unsigned i=0;i<32;i++)if(cb_log141_add(log,sizeof log,&used,"%02X",cb_save_read142.block[p][i])){cb_stage141_log_rc=-1;return;}
  if(cb_log141_add(log,sizeof log,&used,"\n")){cb_stage141_log_rc=-1;return;}
 }
 if(cb_log141_add(log,sizeof log,&used,"END diag.142-read\n")){cb_stage141_log_rc=-1;return;}
 n=(int)used;
 if(cb_log141_add(log,sizeof log,&used,"SESSION143 rc=%ld cleanup=%ld\n",(long)cb_session143.rc,(long)cb_session143.cleanup_rc)){
  cb_stage141_log_rc=-1;return;
 }
 if(cb_log141_add(log,sizeof log,&used,"SCAN144 bytes=%lu sum=%08lX head_diff=%lu\nEND diag.144-read\n",(unsigned long)cb_scan144.scanned,(unsigned long)cb_scan144.sum,(unsigned long)cb_scan144.head_diff)){cb_stage141_log_rc=-1;return;}
 if(cb_log141_add(log,sizeof log,&used,"BANK145 rc0=%ld rc1=%ld restore=%ld bytes0=%lu bytes1=%lu sum0=%08lX sum1=%08lX head_diff=%lu\nEND diag.146-wait\n",(long)cb_banks145.bank_rc[0],(long)cb_banks145.bank_rc[1],(long)cb_banks145.restore_rc,(unsigned long)cb_banks145.scanned[0],(unsigned long)cb_banks145.scanned[1],(unsigned long)cb_banks145.sum[0],(unsigned long)cb_banks145.sum[1],(unsigned long)cb_banks145.head_diff)){cb_stage141_log_rc=-1;return;}
 n=(int)used;
 unsigned long tick=OF_CART_SVC_HAS(timer_get_ms)?OF_SVC->timer_get_ms():0;
 cb_stage141_log_phase="create";
 int rc=cb_log141_open(of_output_open,path,sizeof path,tick,&sequence,(uint32_t)n);
 if(!rc){
  cb_stage141_log_phase="write";
  /* Same contract as the proven ROM/output backups: host-completed write
   * followed by full readback. APF target FLUSH is unsupported here. */
  rc=of_output_write(0,log,(uint32_t)n);
 }
 if(!rc){
  cb_stage141_log_phase="readback";
  FILE *f=fopen("slot:5","rb");
  if(!f)rc=-2;
  else{
   for(unsigned off=0;off<(unsigned)n&&!rc;){
    unsigned part=(unsigned)n-off;if(part>sizeof check)part=sizeof check;
    if(fread(check,1,part,f)!=part||memcmp(check,log+off,part))rc=-3;
    off+=part;
   }
   if(fclose(f)&&!rc)rc=-5;
  }
 }
 if(!rc)cb_stage141_log_phase="verified";
 cb_stage141_log_rc=rc;
}
#endif
#endif
static void present(void){
 static unsigned applied_theme=~0u;
 unsigned theme=state.options==1?state.theme_draft:state.theme;
 if(theme!=applied_theme){of_video_palette_bulk(theme?cb_palette_dark:cb_palette,16);applied_theme=theme;}
 cb_render(of_video_surface(),&state);of_video_flush();of_video_flip();of_video_wait_flip();
}
static int read_cart(void *ctx,unsigned bus,uint32_t addr,void *dst,unsigned n){
 (void)ctx;return of_cart_read_space((of_cart_bus_t)bus,OF_CART_SPACE_ROM,addr,dst,n,50000);
}
static int safe(void *ctx){(void)ctx;return of_cart_enter_safe_idle();}
static void progress(void *ctx,const char *message){
 (void)ctx;state.notice=message;present();
}
static int analyze_read(void *ctx,unsigned bus,unsigned space,uint32_t addr,void *dst,unsigned n){
 (void)ctx;return of_cart_read_space((of_cart_bus_t)bus,(of_cart_space_t)space,addr,dst,n,50000);
}
static int analyze_flash(void *ctx,of_cart_flash_caps_t *f){(void)ctx;return of_cart_flash_get_caps(f);}
static int analyze_caps(void *ctx,of_cart_caps_t *c){(void)ctx;return of_cart_get_caps(c);}
#ifdef CB_NOR108
#include "nor108_analysis.h"
static int confirm_headerless_gba(void){
 int released=0;
 state.notice="No header: A=GBA NOR, B=Cancel";present();
 for(;;){
  of_input_poll_p0();unsigned keys=_of_input_p0.buttons;
  if(keys&OF_BTN_B)return 0;
  if(!keys)released=1;
  else if(released&&(keys&OF_BTN_A))return 1;
  usleep(16000);
 }
}
#endif
static void scan(int analyze){
#ifdef CB_GB_DIAG182
 memset(&gb182_trace,0,sizeof gb182_trace);
 memset(&gb176_probe_report,0,sizeof gb176_probe_report);
#endif
#ifdef CB_GB_SAVE175
 state.gb_save175_ready=0;state.gb_save175_bytes=0;
#ifdef CB_GB_SAVE176
 state.gb_choice=state.gb_choice_draft=0;state.gb_detected_bytes=0;state.gb_probe_rc=GB175_UNSUPPORTED;
#ifdef CB_GB_EMPTY181
 state.gb_capacity_source=0;
#endif
 memset(state.gb_save175_header,0,sizeof state.gb_save175_header);
#endif
#endif
#ifdef CB_ROM164
 cb_reset164(&state);
 memset(&cb_header_diag,0,sizeof cb_header_diag);
 memset(&cb_header150_trace,0,sizeof cb_header150_trace);
 memset(&cb_size151_trace,0,sizeof cb_size151_trace);
 if(!analyze){present();return;}
#endif
 state.save_serial_candidate=0;state.save_rom_hint[0]=0;
 state.save_detected_type=-1;state.save_detected_bytes=0;
#ifdef CB_SRAM138
 state.save_profile=0;state.save_observed=0;state.save_repeat=0;state.save_rc=0;
#endif
 of_cart_caps_t caps;
#ifdef CB_CAMERA136
 memset(&cb_camera136_report,0,sizeof(cb_camera136_report));
 cb_camera136_report.rc[0]=-100; /* lifecycle/caps not ready: no reads */
#endif
 state.probing=1;state.notice="Waiting for cartridge";state.page=CB_FUNCTIONS;
 state.cart=(struct cb_cart){.result=CB_LOCKED};state.analysis=(struct cb_analysis){0};state.detail=0;present();
 int ready=0;
 for(unsigned i=0;i<100;i++){
  int rc=of_cart_get_caps(&caps);
  if(rc){state.cart.rc=rc;state.cart.result=CB_IO;break;}
  if(!cb_caps137_bundle(&caps)||caps.bus_mask!=3||caps.operation_mask!=63||
     caps.gb_gbc_space_mask!=17||caps.max_read_bytes!=32){
   state.cart.result=CB_IO;state.notice="Wrong core / install full package";break;
  }
  if((caps.lifecycle_status&OF_CART_LIFECYCLE_READ_READY)&&
     (caps.lifecycle_status&OF_CART_LIFECYCLE_STATE_MASK)==OF_CART_LIFECYCLE_READY){ready=1;break;}
  usleep(20000);
 }
 if(ready){
  /* Preserve the field-proven power settle and discard first transaction. */
  state.notice="Settling cartridge...";present();usleep(1000000);
  const struct cb_probe_ops ops={0,read_cart,safe,progress};
#ifdef CB_GBA_HEADER140
  cb_gba_header140_read(&cb_gba_header140,&ops);
#ifdef CB_STAGES141
  cb_stage141_before=cb_gba_header140;cb_stage141_ran=cb_stage141_selected;
  cb_stage141_rc=0;cb_stage141_safe=0;cb_stage141_executed=0;
  memset(&cb_stage141_save,0,sizeof cb_stage141_save);
  memset(&cb_save_read142,0,sizeof cb_save_read142);
  memset(&cb_session143,0,sizeof cb_session143);
  memset(&cb_scan144,0,sizeof cb_scan144);
  memset(&cb_banks145,0,sizeof cb_banks145);
  int good=!cb_gba_header140.before&&!cb_gba_header140.after;
  struct cb_cart parsed={0};
  for(unsigned p=0;p<4;p++){
   if(cb_gba_header140.rc[p]||cb_gba_header140.blocks[p]!=6||
      !cb_parse_gba(&parsed,cb_gba_header140.data[p])||
      memcmp(cb_gba_header140.data[0],cb_gba_header140.data[p],192))good=0;
  }
  if(good){
   cb_stage141_executed=1;
   if(cb_stage141_ran==8){
    cb_stage141_rc=of_save141_detect(&cb_stage141_save);
   }else if(cb_stage141_ran==4){
    cb_stage141_rc=cb_read142_run(&cb_save_read142,save_read142);
   }else if(cb_stage141_ran==5||cb_stage141_ran==6||cb_stage141_ran==7){
    if(cb_stage141_ran==7){cb_stage141_rc=of_save145_read(&cb_banks145);cb_session143=cb_banks145.sample;}
    else if(cb_stage141_ran==6){cb_stage141_rc=of_save144_read(&cb_scan144);cb_session143=cb_scan144.sample;}
    else cb_stage141_rc=of_save143_read(&cb_session143);
    cb_save_read142.completed=cb_session143.completed;
    cb_save_read142.differences=cb_session143.differences;
    cb_save_read142.first_round=cb_session143.first_round;
    cb_save_read142.first_address=cb_session143.first_address;
    cb_save_read142.rc=cb_stage141_rc;
    memcpy(cb_save_read142.block,cb_session143.block,sizeof cb_save_read142.block);
    memcpy(cb_save_read142.single,cb_session143.single,sizeof cb_save_read142.single);
   }
   /* Snapshot after operation, before any additional safe-idle recovery.
    * Public read service may itself perform its normal bus setup. */
   memset(&cb_gba_header140,0,sizeof cb_gba_header140);
   for(unsigned p=0;p<4;p++)cb_gba_header140.rc[p]=-100;
   for(unsigned p=0;p<4;p++){
    cb_gba_header140.rc[p]=0;
    for(unsigned i=0;i<192&&!cb_gba_header140.rc[p];i+=32){
     cb_gba_header140.rc[p]=read_cart(0,2,i,cb_gba_header140.data[p]+i,32);
     if(!cb_gba_header140.rc[p])cb_gba_header140.blocks[p]++;
    }
    if(cb_gba_header140.rc[p])break;
   }
   cb_gba_header140.after=safe(0);
  }
  /* Capture first, write the diagnostic only after cartridge activity ends. */
  stage141_log();
#endif
  state.notice=0;state.page=CB_INFO;state.probing=0;present();return;
#endif
#ifdef CB_CAMERA136
  cb_camera136_read(&cb_camera136_report,&ops);
  state.cart=(struct cb_cart){.result=CB_LOCKED};
  state.notice=0;
#else
  cb_probe(&state.cart,&ops);
  state.notice=0;
#if defined(CB_SAVE148) && !defined(CB_ROM164)
  /* Persist failures too, before manual GBA selection can change platform.
   * Logging adds no retry, delay, or cartridge access to the captured probe. */
  int header_log_rc=header150_log(&state.cart);
  if(header_log_rc){snprintf(save148_notice,sizeof save148_notice,"Header log failed %d",header_log_rc);state.notice=save148_notice;}
#endif
  if(analyze&&state.cart.result==CB_READY){
   state.notice="Analyzing mapper / NOR...";present();
#ifdef CB_ROM164
   if(state.cart.platform==CB_GBA){
    const struct cb_capacity164_ops capacity_ops={{0,read_cart,safe,progress},capacity_nor164};
    cb_gba_analyze164(&state.cart,&state.analysis,&capacity_ops);
   }else{
#endif
   const struct cb_analysis_ops aops={0,analyze_read,safe,analyze_flash,analyze_caps};
   cb_analyze(&state.analysis,&state.cart,&aops);
#ifdef CB_NOR108
   state.notice="Detecting NOR configuration...";present();
   analyze_nor108(&state.analysis,&state.cart);
#endif
#ifdef CB_ROM164
   }
#endif
   state.notice=state.analysis.status;state.detail=0;
  }
#ifdef CB_NOR108
  else if(analyze&&(state.cart.result==CB_EMPTY||state.cart.result==CB_BAD_HEADER)&&
          !state.cart.rc&&!state.cart.safe_rc&&confirm_headerless_gba()){
   /* Explicit bus selection, not a fabricated valid ROM header. A fresh NOR
    * probe must succeed before any write action becomes available. */
   state.cart.platform=CB_GBA;state.analysis.ran=1;
   state.notice="Probing GBA NOR without ROM header";present();
   analyze_nor108(&state.analysis,&state.cart);
   state.notice=state.analysis.status;state.detail=0;
  }
#endif
#endif /* !CB_CAMERA136 */
#if defined(CB_ROM164) && defined(CB_SAVE148)
  int header_log_rc=header150_log(&state.cart);
  if(header_log_rc){snprintf(save148_notice,sizeof save148_notice,"Header log failed %d",header_log_rc);state.notice=save148_notice;}
#endif
 }
#ifdef CB_SRAM137
 if(state.cart.result==CB_READY&&state.cart.platform==CB_GBA){
  const char *prior=state.notice;
  /* ROM hint is display-only. Hardware discovery does not require an SDK
   * signature; a genuine ROM read failure still aborts this scan. */
  int save_rc=cb_save_quick137(&state,present);
  snprintf(state.save_rom_hint,sizeof state.save_rom_hint,"%s",state.cart.save);
#ifdef CB_EEPROM139
  if(!save_rc){
   state.notice="Detecting save hardware...";present();
   of_save139_result result;
#ifdef CB_SAVE148
   of_save141_diagnostic diag;int rc=of_save141_detect(&diag);result=diag.result;
   int log_rc=save148_log(state.cart.title,rc,&diag);
   snprintf(save148_notice,sizeof save148_notice,"Save stage %lu rc %d; log %s %d",(unsigned long)diag.stage,rc,log_rc?"failed":"verified",log_rc);
   prior=save148_notice;
#else
   int rc=of_eeprom139_detect(&result);
#endif
   static const char *const names[]={"None (probe)","EEPROM","EEPROM",
    "SRAM/FRAM","Flash","Flash","DACS","Unlicensed SRAM","Unlicensed SRAM","Batteryless SRAM"};
   /* Only SRAM results remain eligible for the existing backup path. */
   state.save_serial_candidate=rc||(result.type!=3&&result.type!=7&&result.type!=8);
   state.save_rc=rc;
   if(!rc){state.save_detected_type=result.type;state.save_detected_bytes=result.bytes;}
   if(rc)
    snprintf(state.cart.save,sizeof state.cart.save,"Save probe failed rc %d",rc);
   else if(result.type<0||result.type>9)
    snprintf(state.cart.save,sizeof state.cart.save,"Hardware unconfirmed");
   else if(!result.bytes)
    snprintf(state.cart.save,sizeof state.cart.save,"%s",result.type?"Save size unconfirmed":names[0]);
   else if(result.bytes==512)
    snprintf(state.cart.save,sizeof state.cart.save,"EEPROM 512 B (estimate)");
   else snprintf(state.cart.save,sizeof state.cart.save,"%s %lu KiB",names[result.type],(unsigned long)(result.bytes/1024));
  }
#endif
  if(!save_rc)state.notice=prior;
 }
#endif
#ifdef CB_RTC167
 if(analyze&&state.cart.platform==CB_GBA&&state.cart.result==CB_READY){
  const char *prior=state.notice;state.notice="Reading cartridge RTC...";present();
  memset(&rtc167_report,0,sizeof rtc167_report);
  int rtc_rc=of_rtc167_read(&rtc167_report);
  cb_rtc167_format(state.cart.rtc,sizeof state.cart.rtc,rtc_rc,&rtc167_report);
  state.notice=prior;
#ifdef CB_SAVE148
  int log_rc=rtc167_log(&state.cart,rtc_rc,&rtc167_report);
  if(log_rc){snprintf(save148_notice,sizeof save148_notice,"RTC log failed rc %d",log_rc);state.notice=save148_notice;}
#endif
  if(rtc167_report.cleanup_rc){
   state.cart.result=CB_IO;state.cart.safe_rc=rtc167_report.cleanup_rc;
   state.analysis.writable=0;state.save_rc=rtc167_report.cleanup_rc;
   state.notice="RTC cleanup failed; analyze again";
  }
 }
#endif
#ifdef CB_SAVE171
 cb_save171_update(&state);
#endif
#ifdef CB_GB_SAVE175
 if(analyze&&state.cart.platform==CB_GB&&state.cart.result==CB_READY&&
    OF_CART_SVC_HAS(gb175_capture)){
#ifdef CB_GB_DIAG182
  struct gb175_plan plan;const struct gb181_ops check_ops={0,gb181_read,gb181_safe};
  int rc=gb182_preflight(&state.cart,&check_ops,&plan,&gb182_trace);
  const uint8_t *h=gb182_trace.header;
  if(!rc){
#else
  uint8_t h[96];struct gb175_plan plan;struct cb_cart current={0};
  int rc=of_cart_enter_safe_idle();
  for(unsigned off=0;off<sizeof h&&!rc;off+=32)
   rc=of_cart_read(OF_CART_BUS_GB_GBC,0x100+off,h+off,32,50000);
  int clean=of_cart_enter_safe_idle();if(!rc)rc=clean;
  if(!rc&&cb_parse_gb(&current,h)&&current.result==CB_READY&&
     !strcmp(current.title,state.cart.title)&&current.type==state.cart.type&&
     current.checksum==state.cart.checksum&&current.ram_bytes==state.cart.ram_bytes&&
#ifdef CB_GB_SAVE179
     !gb179_plan(1,h,0,&plan)){
#elif defined(CB_GB_SAVE178)
     !gb178_plan(1,h,0,&plan)){
#elif defined(CB_GB_SAVE177)
     !gb177_plan(1,h,0,&plan)){
#else
     !gb175_plan_header(1,h,&plan)){
#endif
#endif
   memcpy(state.gb_save175_header,h,80);state.gb_save175_bytes=plan.bytes;
   state.gb_save175_ready=1;
#ifdef CB_GB_SAVE176
   state.gb_save175_ready=0;state.gb_save175_bytes=0;
   rc=gb176_plan(1,h,0,&plan);
#ifdef CB_GB_DIAG182
   gb182_trace.stage=7;
#endif
   if(!rc)rc=of_gb176_capture(h,0,gb176_probe_snapshot,plan.bytes,&gb176_probe_report);
   if(!rc&&(gb176_probe_report.rc||gb176_probe_report.cleanup_rc||gb176_probe_report.compared!=plan.bytes))rc=GB175_IO;
   state.gb_probe_rc=rc;
#ifdef CB_GB_DIAG182
   gb182_trace.rc=rc;
#endif
   if(!rc){
    state.gb_detected_bytes=plan.mapper==GB179_MBC7?256:plan.mapper==GB178_MBC2?512:gb176_content_bytes(gb176_probe_snapshot,plan.bytes);
#ifdef CB_GB_EMPTY181
    int db_match=0;
    if(!state.gb_detected_bytes&&!state.analysis.rc&&
       !(state.analysis.matched&&state.analysis.writable)){
     struct cb_cart db_cart={0};
     cb_parse_gb(&db_cart,h);
     const struct gb181_ops db_ops={&db_cart,gb183_read,gb181_safe};
     db_match=gb181_database_header(h,&db_ops);
#ifdef CB_GB_DIAG182
     gb182_trace.stage=8;gb182_trace.db_rc=db_match;
     if(db_match<0)gb182_trace.rc=db_match;
#endif
     if(db_match<0){state.gb_probe_rc=db_match;state.notice="GB database header read failed";}
    }
    state.gb_detected_bytes=gb181_capacity(h,state.gb_detected_bytes,state.gb_probe_rc,
      state.analysis.rc,state.analysis.matched&&state.analysis.writable,db_match,&state.gb_capacity_source);
    if(!state.gb_detected_bytes&&!state.gb_probe_rc){
     snprintf(state.cart.save,sizeof state.cart.save,"RAM unknown / manual");
     state.notice="GB capacity unknown; select RAM size";
    }
    gb176_selection(&state);
#ifdef CB_GB_DIAG182
    if(!gb182_trace.rc)gb182_trace.stage=9;
#endif
#else
    /* An inconclusive content pattern is NOT an I/O failure. Board fallback
     * is labelled separately and only allowed for a writable matched NOR. */
    if(!state.gb_detected_bytes&&state.analysis.matched&&state.analysis.writable&&plan.bytes==131072){
     state.gb_choice=state.gb_choice_draft=7;
     state.notice="GB default SRAM128; confirm board";
    }
    gb176_selection(&state);
    if(plan.mapper!=GB178_MBC2&&plan.mapper!=GB179_MBC7)snprintf(state.cart.save,sizeof state.cart.save,state.gb_save175_bytes?"RAM %lu KiB (%s)":"RAM size unknown",(unsigned long)(state.gb_save175_bytes/1024),state.gb_choice?"default":"probe");
#endif
   }else state.notice="GB RAM probe failed; analyze again";
#endif
  }
#ifdef CB_GB_DIAG182
  if(rc||state.gb_probe_rc){
   if(rc)state.gb_probe_rc=rc;
   state.gb_save175_ready=0;state.gb_save175_bytes=0;
   snprintf(state.cart.save,sizeof state.cart.save,"GB S%d rc %d",gb182_trace.stage,state.gb_probe_rc);
   snprintf(gb182_notice,sizeof gb182_notice,"GB save stage %d rc %d; see log",gb182_trace.stage,state.gb_probe_rc);
   state.notice=gb182_notice;
  }
#endif
 }
#endif
#ifdef CB_GB_SAVE179
 if(analyze&&state.cart.platform==CB_GB&&!state.gb_save175_ready&&state.cart.result==CB_READY){
  if(state.cart.type==0x22)snprintf(state.cart.save,sizeof state.cart.save,
    state.gb_probe_rc==GB175_UNSUPPORTED?"MBC7 size unsupported":"MBC7 probe failed rc %d",state.gb_probe_rc);
  else if(state.cart.type==0x20||state.cart.ram_bytes==2048)snprintf(state.cart.save,sizeof state.cart.save,"Save type unsupported");
 }
#endif
#ifdef CB_GB_DIAG182
 if(analyze&&state.cart.platform==CB_GB){
  if(!gb182_trace.stage&&state.cart.result==CB_READY){
   gb182_trace.rc=state.gb_probe_rc=GB175_UNSUPPORTED;
   snprintf(state.cart.save,sizeof state.cart.save,"GB service unavailable");
   state.notice="GB capture service missing";
  }
  int log_rc=gb182_log(&state);
  if(log_rc){snprintf(gb182_notice,sizeof gb182_notice,"GB stage %d rc %d; log failed %d",gb182_trace.stage,state.gb_probe_rc,log_rc);state.notice=gb182_notice;}
 }
#endif
#ifdef CB_GB_RTC186
 if(analyze&&state.cart.platform==CB_GB&&state.cart.result==CB_READY){
  memset(&gb186_report,0,sizeof gb186_report);
  if(state.cart.type!=0x0f&&state.cart.type!=0x10){
   snprintf(state.cart.rtc,sizeof state.cart.rtc,"Not supported");
  }else{
   static uint8_t rtc_header[96];struct cb_cart current={0};
   int rc=of_cart_enter_safe_idle();
   if(!rc)rc=cb_gbrtc186_header(rtc_header,cb_gbrtc186_read_block,NULL);
   int clean=of_cart_enter_safe_idle();if(!rc)rc=clean;
   if(!rc&&(!cb_parse_gb(&current,rtc_header)||current.result!=CB_READY||
      current.type!=state.cart.type||current.checksum!=state.cart.checksum||strcmp(current.title,state.cart.title)))rc=GB175_IDENTITY;
   if(!rc)rc=of_gb186_read(rtc_header,&gb186_report);
   cb_gbrtc186_format(state.cart.rtc,sizeof state.cart.rtc,rc,&gb186_report);
   if(clean||gb186_report.cleanup_rc){state.cart.result=CB_IO;state.gb_save175_ready=0;state.analysis.writable=0;state.notice="GB RTC cleanup failed";}
#ifdef CB_SAVE148
   static char log[512];static unsigned sequence;char path[112];
   int n=snprintf(log,sizeof log,"GBRTC186 version=" CB_RELEASE_NAME " title=%.16s type=%02x\nrc=%d detail=%ld cleanup=%ld supported=%lu\nraw=%02x %02x %02x %02x %02x\ndays=%lu h=%lu m=%lu s=%lu halt=%lu carry=%lu\nRead-only RTC registers; no save or clock data writes\nEND GBRTC186\n",state.cart.title,state.cart.type,rc,(long)gb186_report.detail,(long)gb186_report.cleanup_rc,(unsigned long)gb186_report.supported,gb186_report.raw[0],gb186_report.raw[1],gb186_report.raw[2],gb186_report.raw[3],gb186_report.raw[4],(unsigned long)gb186_report.days,(unsigned long)gb186_report.hours,(unsigned long)gb186_report.minutes,(unsigned long)gb186_report.seconds,(unsigned long)gb186_report.halt,(unsigned long)gb186_report.carry);
   if(n>0&&(unsigned)n<sizeof log){
    snprintf(path,sizeof path,"/Saves/chiskit/common/gbrtc186-%08lx-%04x.txt",(unsigned long)OF_SVC->timer_get_ms(),++sequence);
    int lr=of_output_open(path,(uint32_t)n);if(!lr)lr=of_output_write(0,log,(uint32_t)n);
    if(!lr)lr=cb_output_verify161(log,(uint32_t)n);
    if(lr)state.notice="GB RTC log failed";
   }
#endif
  }
 }
#endif
 state.probing=0;if(analyze)state.page=CB_INFO;
#ifdef CB_CAMERA136
 state.page=CB_INFO;
#endif
 present();
}
int main(void){
 if(!OF_CART_SVC_HAS(video_init)||!OF_CART_SVC_HAS(video_get_surface)||
    !OF_CART_SVC_HAS(video_flip)||!OF_CART_SVC_HAS(video_wait_flip)||
    !OF_CART_SVC_HAS(video_flush_cache)||!OF_CART_SVC_HAS(video_set_palette_bulk)||
    !OF_CART_SVC_HAS(video_set_mode)||!OF_CART_SVC_HAS(video_set_display_mode)||
    !OF_CART_SVC_HAS(input_poll_p0)){
  printf("chiskit " CB_RELEASE_NAME ": UI service missing\n");return 1;
 }
 of_video_init();
 const of_video_mode_t mode={CB_W,CB_H,CB_W,OF_VIDEO_MODE_8BIT,0};
 if(of_video_set_mode(&mode)<0){printf("chiskit: 320x288 display mode unavailable\n");return 1;}
 of_video_set_display_mode(OF_DISPLAY_FRAMEBUFFER);of_video_palette_bulk(cb_palette,16);
 state.cart.result=CB_LOCKED;present();
#ifdef CB_LANG174
 unsigned preferences=cb_settings174_load();
 state.language=state.language_draft=preferences&1u;
 state.theme=state.theme_draft=(preferences>>1)&1u;
#endif
 state.cart.result=CB_LOCKED;present();scan(0);
 unsigned previous=~0u;
 for(;;){
  of_input_poll_p0();unsigned held=_of_input_p0.buttons,edge=held&~previous;previous=held;
#if defined(CB_GBA_HEADER140)
  if(edge&OF_BTN_A){scan(0);previous=~0u;}
#ifdef CB_STAGES141
  else if(edge&(OF_BTN_UP|OF_BTN_DOWN)){
   cb_stage141_selected=cb_stage141_selected==8?0:8;present();
  }
#endif
  else if(edge&(OF_BTN_UP|OF_BTN_DOWN|OF_BTN_LEFT|OF_BTN_RIGHT)){
   state.detail=(state.detail+1)%4;present();
  }
#elif defined(CB_CAMERA136)
  if(edge&OF_BTN_A){scan(0);previous=~0u;}
  else if(edge&(OF_BTN_UP|OF_BTN_DOWN|OF_BTN_LEFT|OF_BTN_RIGHT)){
   state.detail=(state.detail+1)%3;present();
  }
#else
  if(edge){int action=cb_key(&state,edge);
#ifdef CB_LANG174
   if(action==8){
    int rc=cb_settings174_save(state.language_draft|(state.theme_draft<<1));
    if(!rc){state.language=state.language_draft;state.theme=state.theme_draft;state.options=0;}
    static char language_error[40];
    snprintf(language_error,sizeof language_error,"Settings save failed rc %d",rc);
    state.notice=rc?language_error:"Settings saved";
    present();previous=~0u;
   }else
#endif
   if(action==1||action==2){scan(action==2);previous=~0u;}else if(action>=3&&action<=7){cb_transfer(&state,(unsigned)action-1,present);previous=~0u;}else present();}
#endif
  usleep(16000);
 }
}
