#include "transfer.h"
#include "of_file.h"
#include "of_output.h"
#include "of_input.h"
#include "of_psram_cache.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "version.h"
#include "rom_input.h"
#include "backup_name.h"
#include "caps137.h"
#include "save_backup161.h"
#define PROGRAM65_BUFFER_BYTES 512u
#include "../gba_program65/program65.h"
#define DUMP57_CRC32_TABLE 1
#include "../gba_dump57/reference.h"
static struct cb_ui *ui;
static void (*paint)(void);
static uint32_t began,phase_began,image_bytes;
static int last_stage=-1,poisoned;
static volatile unsigned callbacks;
static volatile int callback_token,callback_result;
static uint8_t buffer[65536] __attribute__((aligned(64)));
static unsigned rtc169_extra;
#ifdef CB_FLASH170
static int flash170_original_backup;
#endif
static uint32_t save169_file_size(uint32_t bytes){return bytes+rtc169_extra;}
static uint32_t now(void){return OF_CART_SVC_HAS(timer_get_ms)?OF_SVC->timer_get_ms():0;}
static void progress(enum program65_stage s,uint32_t done,uint32_t total){
 static const char *names[]={"Scan ROM file","Identify NOR","Erase sectors","Program ROM","Verify cartridge","Complete"};
 if(last_stage!=(int)s){last_stage=(int)s;phase_began=now();}
 if(s==P65_ERASE){done*=0x20000u;total*=0x20000u;}
 else if(s==P65_SOURCE_SCAN||s==P65_PROGRAM||s==P65_VERIFY){
  done*=65536u;if(done>image_bytes)done=image_bytes;total=image_bytes;
 }
 ui->phase=names[s];ui->done=done;ui->total=total;ui->elapsed_ms=now()-began;ui->phase_ms=now()-phase_began;paint();
}
static void callback(int token,int rc){
 if(!callbacks){callback_token=token;callback_result=rc;}
 __asm__ volatile("" ::: "memory");callbacks++;
}
static int file_read_chunk(unsigned slot,uint32_t off,void *dst,uint32_t n){
 if(poisoned)return OF_ERR_INVALID_STATE;
 int busy=of_file_async_busy();if(busy)return busy<0?busy:OF_ERR_BUSY;
 callbacks=0;callback_token=callback_result=OF_ERR_INVALID_STATE;
 uint32_t start=now();int token=of_file_read_async(slot,off,dst,n,callback);
 if(token<0){poisoned=1;return token;}
 while(!callbacks){
  int rc=of_file_async_poll();if(rc<0){poisoned=1;return rc;}
  if(now()-start>=10000u){poisoned=1;return OF_ERR_TIMEOUT;}
  usleep(1000);
 }
 if(callbacks!=1||callback_token!=token||callback_result||of_file_async_busy()){
  poisoned=1;return callback_result?callback_result:OF_ERR_IO;
 }
 return 0;
}
/* SDRAM async bounce window is 32 KiB. Callers may retain their 64 KiB
 * hashing/output buffers, but each OS request must fit the bounce window. */
static int file_read(unsigned slot,uint32_t off,void *dst,uint32_t n){
 if(n>UINT32_MAX-off)return OF_ERR_BAD_RANGE;
 while(n){
  uint32_t part=n>32768u?32768u:n;
  int rc=file_read_chunk(slot,off,dst,part);if(rc)return rc;
  off+=part;dst=(uint8_t *)dst+part;n-=part;
 }
 return 0;
}
static int source_read(uint32_t off,void *dst,uint32_t n){return file_read(CB_ROM_INPUT_SLOT,off,dst,n);}
static int backup_read(uint32_t off,void *dst,uint32_t n){return file_read(5,off,dst,n);}
/* Dynamic configuration uses the same uncached SDRAM read as backup checks. */
int cb_settings_read198(void *dst){return file_read(5,0,dst,16);}
/* Dynamic output slot 5 is rebound for each file. Bypass POSIX slot-keyed
 * read cache so verification always reaches the current file on TF. */
int cb_output_verify161(const void *expected,uint32_t bytes){
 uint8_t check[256];const uint8_t *p=expected;
 if(!p||!bytes)return OF_ERR_INVALID_PARAM;
 for(uint32_t off=0;off<bytes;){
  uint32_t part=bytes-off;if(part>sizeof check)part=sizeof check;
  int rc=backup_read(off,check,part);if(rc)return rc;
  if(memcmp(check,p+off,part))return OF_ERR_IO;
  off+=part;
 }
 return 0;
}
static int cart_read(uint32_t off,void *dst,uint32_t n,uint32_t us){
 return of_cart_read(OF_CART_BUS_GBA,off,dst,n,us);
}
static int program_caps(of_cart_caps_t *c){
 int rc=of_cart_get_caps(c);if(rc)return rc;
 /* Narrow the verified combined contract for the unchanged GBA-only runner.
  * No values are fabricated: require exact aggregate capabilities first. */
 if(!cb_caps137_bundle(c)||c->bus_mask!=3||c->operation_mask!=63||
    c->gb_gbc_space_mask!=17||c->writes_enabled!=1)
  return OF_ERR_DENIED;
 c->bus_mask=OF_CART_BUS_MASK_GBA;c->gb_gbc_space_mask=0;
 c->gba_space_mask=17; /* Narrow the read-space superset for legacy ROM runner. */
 return 0;
}
const struct program65_ops cb_program_ops={
 source_read,program_caps,of_cart_flash_get_caps,of_cart_enter_safe_idle,
 of_cart_flash_erase_sector,of_cart_flash_program,cart_read,of_cart_flash_disarm,now,progress
};
static int confirm(void){
 int released=0;
 ui->confirm=1;ui->phase="Release A, then press A again";paint();
 for(;;){
  of_input_poll_p0();unsigned keys=_of_input_p0.buttons;
  if(keys&OF_BTN_B){ui->confirm=0;return 0;}
  if(!keys){
   if(!released){released=1;ui->phase="Press A again to confirm";paint();}
  }else if(released&&(keys&OF_BTN_A)){
   ui->confirm=0;ui->phase="Authorized";paint();return 1;
  }
  usleep(16000);
 }
}
#ifdef CB_ROM164
#include "capacity164.h"
static int capacity_safe164(void *ctx){(void)ctx;return of_cart_enter_safe_idle();}
static int capacity_nor164(void *ctx,of_nor_info_t *n){(void)ctx;return of_nor_probe(2,n);}
#else
#include "size107.h"
#endif
static int gba_size_read(void *ctx,unsigned bus,uint32_t addr,void *p,unsigned n){
 (void)ctx;return of_cart_read((of_cart_bus_t)bus,addr,p,n,50000);
}
static int backup(void){
 uint32_t total=0;
 ui->task="Backup GBA ROM";ui->phase="Estimating GBA ROM capacity";paint();
 int rc=of_cart_enter_safe_idle();if(rc)return rc;
#ifdef CB_ROM164
 struct cb_cart current=ui->cart;struct cb_analysis detected;
 const struct cb_capacity164_ops ops={{0,gba_size_read,capacity_safe164,0},capacity_nor164};
 ui->phase="Rechecking cartridge and capacity";paint();
 rc=cb_gba_analyze164(&current,&detected,&ops);if(rc)return rc;
 if(current.rom_bytes!=ui->cart.rom_bytes||current.rom_source!=ui->cart.rom_source||
    memcmp(current.rom_header_sha1,ui->cart.rom_header_sha1,20)){
  ui->notice="Cartridge changed; Analyze Cart again";return OF_ERR_IO;
 }
 total=current.rom_bytes;
#else
 const struct cb_probe_ops ops={0,gba_size_read,0,0};
 rc=cb_gba_size107(&ops,&total);if(rc)return rc;
#endif
 ui->cart.rom_bytes=total;ui->total=total;
 struct md5_53 md;uint8_t original[16],actual[16];uint32_t crc=~0u,check=~0u;
#ifdef CB_ROM164
 ui->phase=current.rom_source==1?"NOR capacity backup":current.rom_source==2?"Database ROM capacity backup":"32 MiB window; database miss";
 snprintf(ui->source_title,sizeof ui->source_title,"%.16s",ui->cart.title);
#else
 ui->phase=total==0x2000000?"32 MiB window; no mirror found":"Mirror-based capacity estimate";ui->source_title[0]=0;
#endif
 cb_backup_name(ui->path,sizeof(ui->path),&ui->cart,"gba",now());
 if(!confirm())return OF_ERR_DENIED;
 ui->total=total;
 /* Backup capacity does not grant programming authority. */
 ui->phase="Entering safe idle";paint();
 rc=of_cart_enter_safe_idle();if(rc)return rc;
 ui->phase="Creating backup in output slot 5";paint();
 rc=of_output_open(ui->path,total);if(rc)return rc;
 if(OF_SVC->file_size("slot:5")!=(long)total)return OF_ERR_BAD_RANGE;
 /* file_size reads a locally published datatable value. Independently ask
  * the host to read the last 32 bytes; do not infer host allocation from it. */
 ui->phase="Checking output file end";paint();
 rc=backup_read(total-32u,buffer,32u);if(rc)return rc;
 md5_init53(&md);phase_began=now();
 for(uint32_t off=0;off<total;off+=sizeof(buffer)){
  ui->phase="Reading cartridge";
  for(uint32_t b=0;b<sizeof(buffer);b+=32){
   rc=cart_read(off+b,buffer+b,32,50000);if(rc)return rc;
  }
  md5_add53(&md,buffer,sizeof(buffer));
  for(unsigned i=0;i<sizeof(buffer);i++)crc=crc32_table57[(crc^buffer[i])&255]^(crc>>8);
  ui->phase="Writing backup output";
  rc=of_output_write(off,buffer,sizeof(buffer));
  if(rc){
   /* Snapshot immediately: final SAFE or a later operation must not hide
    * the command which failed. Read-only diagnostic registers, no retry. */
   snprintf(ui->io_diag,sizeof(ui->io_diag),"Request off %08lX n %lu",(unsigned long)off,(unsigned long)sizeof(buffer));
#ifndef OF_PC
   volatile const uint32_t *ds=(volatile const uint32_t *)(uintptr_t)0x40000000u;
   snprintf(ui->io_regs,sizeof(ui->io_regs),"S%04X O%08X N%08X T%02X",
    (unsigned)(ds[8]&65535),(unsigned)ds[9],(unsigned)ds[11],(unsigned)(ds[15]&255));
#endif
   return rc;
  }
  ui->phase="Backup GBA ROM -> new file";ui->done=off+sizeof(buffer);ui->total=total;
  ui->elapsed_ms=now()-began;ui->phase_ms=now()-phase_began;paint();
 }
 md5_end53(&md,original);md5_init53(&md);phase_began=now();
 ui->phase="Verify file MD5 + CRC32";ui->done=0;paint();
 for(uint32_t off=0;off<total;off+=sizeof(buffer)){
  rc=backup_read(off,buffer,sizeof(buffer));if(rc)return rc;
  md5_add53(&md,buffer,sizeof(buffer));
  for(unsigned i=0;i<sizeof(buffer);i++)check=crc32_table57[(check^buffer[i])&255]^(check>>8);
  ui->phase="Verify file MD5 + CRC32";ui->done=off+sizeof(buffer);
  ui->elapsed_ms=now()-began;ui->phase_ms=now()-phase_began;paint();
 }
 md5_end53(&md,actual);md5_hex53(actual,ui->hash);
 /* smoke.64 contract: no APF 0188 FLUSH; complete host writes plus full reread. */
 return memcmp(original,actual,16)||crc!=check?OF_ERR_IO:0;
}
#include "gbdump107.h"
#ifdef CB_RTC167
#include "rtc168_transfer.h"
#endif
#ifdef CB_RTC172
#include "rtc172_transfer.h"
#ifdef CB_RTC173
#include "rtc173_transfer.h"
#endif
#endif
#ifdef CB_SRAM137
#include "sram137_transfer.h"
#ifdef CB_SRAM138
#include "sram138_transfer.h"
#ifdef CB_FLASH165
#include "flash165_transfer.h"
#ifdef CB_FLASH170
#include "flash170_transfer.h"
#endif
#endif
#include "save_restore157.h"
#ifdef CB_SAVE159
#include "save_backup159.h"
#ifdef CB_SAVE160
#include "of_save160.h"
#include "save_restore160.h"
#endif
#endif
#define backup_sram137 backup_sram138
#endif
#endif
#ifdef CB_NOR108
#include "nor108_transfer.h"
#endif
#ifdef CB_NOR130
#include "nor130_transfer.h"
#endif
#if defined(CB_EEPROM139) && defined(CB_SRAM137)
#include "of_save139.h"
#include "eeprom161_transfer.h"
#ifdef CB_EEPROM162
#include "of_save162.h"
#include "eeprom162_transfer.h"
#ifdef CB_SAVE171
#include "save_restore171.h"
#endif
#endif
#endif
#ifdef CB_EEPROM139_DIAG
#include "of_save139.h"
#include "eeprom139_transfer.h"
#endif
#ifdef CB_GB_SAVE175
#include "gb_save175_transfer.h"
#ifdef CB_GB_RESTORE180
#include "gb_restore180_transfer.h"
#endif
#endif
#ifdef CB_GB_RTC189
#include "gb_rtc192_edit.h"
#endif
void cb_transfer(struct cb_ui *u,unsigned action,void (*present)(void)){
 if(poisoned){u->notice="Transfer unavailable / restart core";return;}
#ifdef CB_RTC173
 if(action==6){if(u->cart.result!=CB_READY)return;
#ifdef CB_GB_RTC189
  if(u->cart.platform!=CB_GBA&&u->cart.platform!=CB_GB)return;
#else
  if(u->cart.platform!=CB_GBA)return;
#endif
 }
 else
#endif
 if(!cb_action_enabled(u,action)){u->notice="Transfer unavailable / restart core";return;}
 ui=u;paint=present;began=phase_began=now();last_stage=-1;
 u->page=CB_TRANSFER;u->busy=1;u->finished=0;u->confirm=0;u->transfer_action=action;
 u->notice=0;
 u->rtc_restore_pending=u->rtc_restore_choice=0;
 u->done=u->total=u->elapsed_ms=u->phase_ms=0;
 u->hash[0]=u->path[0]=u->source_title[0]=u->failure_phase[0]=0;
 u->sha256[0]=0;
 u->io_diag[0]=u->io_regs[0]=0;
 u->rtc_backup_cancelled=u->rtc_backup_choice=0;
 u->task=action==2?"Backup ROM":"Write ROM (erases entire NOR)";
 u->phase="Checking services";paint();
 int rc=OF_ERR_NOT_SUPPORTED;
#ifdef CB_RTC173
 if(action==6){
#ifdef CB_GB_RTC189
  if(u->cart.platform==CB_GB)rc=gb192_edit();else
#endif
  rc=rtc173_edit();goto end;}
#endif
#ifdef CB_RTC167
 int include_rtc=0;
 rtc169_extra=0;
#endif
 if(!OF_CART_SVC_HAS(timer_get_ms)||!OF_CART_SVC_HAS(file_size)||
    !OF_CART_SVC_HAS(output_open)||
    !OF_CART_SVC_HAS(output_write))goto end;
#ifdef CB_RTC167
 if(action==3&&u->cart.platform==CB_GBA){rc=rtc168_choose(&include_rtc);if(rc)goto end;rtc169_extra=include_rtc?16u:0u;}
#endif
 if(action==2)rc=u->cart.platform==CB_GB?backup_gb107():backup();
#ifdef CB_GB_SAVE175
 else if(action==3&&u->cart.platform==CB_GB){rc=backup_gb175();}
#endif
#ifdef CB_EEPROM139_DIAG
 else if(action==3)rc=capture_eeprom139();
#else
#ifdef CB_SRAM137
#ifdef CB_EEPROM139
 else if(action==3&&cb_eeprom161_supported(ui->save_detected_type,ui->save_detected_bytes))rc=backup_eeprom161();
#endif
#ifdef CB_FLASH165
 else if(action==3&&(ui->save_detected_type==4||ui->save_detected_type==5))rc=backup_flash165();
#endif
 else if(action==3)rc=ui->save_serial_candidate?OF_ERR_NOT_SUPPORTED:backup_sram137();
#endif
#endif
#ifdef CB_GB_RESTORE180
 else if(action==5&&u->cart.platform==CB_GB){rc=restore_gb180();}
#endif
#ifdef CB_SRAM138
 else if(action==5){
#ifdef CB_SAVE171
  if(ui->save_selection.type!=ui->save_detected_type||
     ui->save_selection.bytes!=ui->save_detected_bytes)rc=restore_save171();
  else
#endif
#ifdef CB_FLASH170
  if(ui->save_detected_type==4||ui->save_detected_type==5)rc=restore_flash170();
  else
#endif
#ifdef CB_EEPROM162
  if(cb_eeprom161_supported(ui->save_detected_type,ui->save_detected_bytes))
   rc=restore_eeprom162();
  else
#endif
#ifdef CB_SAVE159
#ifdef CB_SAVE160
  rc=restore_save160();
#else
  rc=prepare_save159();
#endif
#else
  rc=check_save157();
#endif
 }
#endif
 else{
  long size=OF_SVC->file_size(CB_ROM_INPUT_PATH);
  if(size<=0){rc=OF_ERR_BAD_RANGE;u->phase="Check Saves ROM selection";
   u->notice="Select Saves ROM in Pocket menu";goto end;}
  image_bytes=size>0?(uint32_t)size:0;
#ifdef CB_NOR108
  if(of_nor_pipeline_available()&&u->analysis.nor.version==OF_NOR_VERSION)rc=program_nor130();
  else
  /* Retain the hardware-validated S29GL256S loops for their two accepted
   * image lengths. Their own post-confirmation hardware ID gate remains. */
  if(u->analysis.legacy_programmer&&size==0x2000000)rc=cb_program32(u,confirm);
  else if(u->analysis.legacy_programmer&&size==0x84d500)rc=cb_program8(u,confirm);
  else if(u->analysis.nor.version==OF_NOR_VERSION)rc=program_nor108();
  else
#endif
  if(size==0x2000000)rc=cb_program32(u,confirm);
  else if(size==0x84d500)rc=cb_program8(u,confirm);
  else {rc=OF_ERR_BAD_RANGE;u->notice="Load 32 MiB / 8705280 B ROM in menu";}
 }
#ifdef CB_RTC167
 if(!rc&&action==3&&include_rtc){
  rc=rtc168_backup();
  if(rc)u->notice="Incomplete RTC footer; backup failed";
 }
#endif
end:
 {int dr=0;
#ifdef CB_NOR108
 if(action==4&&u->analysis.nor.version==OF_NOR_VERSION)dr=of_nor_finish(0);
 else
#endif
 if(action==4)dr=of_cart_flash_disarm();
 int sr=of_cart_enter_safe_idle();if(!rc&&dr)rc=dr;
 if(u->rtc_backup_cancelled&&sr){u->rtc_backup_cancelled=0;rc=sr;}
 else if(!rc&&sr)rc=sr;}
#ifdef CB_GB_SAVE175
 /* Successful save jobs retain the analyzed GB configuration. Each next job
  * still checks the live cartridge header before accessing save hardware. */
 if(u->cart.platform==CB_GB&&(action==3||action==5)&&rc&&!u->rtc_backup_cancelled)u->gb_save175_ready=0;
#endif
 if(action==3&&u->rtc_backup_cancelled&&rc==OF_ERR_DENIED){
  u->task_rc=0;u->busy=u->finished=u->confirm=0;
  u->page=CB_FUNCTIONS;u->notice="Backup cancelled";paint();return;
 }
 u->task_rc=rc;u->busy=0;u->finished=1;u->confirm=0;u->elapsed_ms=now()-began;
 if(rc){
  const char *failed=u->phase?u->phase:"Unknown stage";
  snprintf(u->failure_phase,sizeof(u->failure_phase),"STOP at %.34s",failed);
  u->phase=u->failure_phase;
 }else u->phase="Complete / verified";
 /* Result page is terminal. A fresh scan is mandatory for every next job. */
 u->analysis.writable=0;u->analysis.matched=0;paint();
}
