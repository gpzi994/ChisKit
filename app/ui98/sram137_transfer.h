/* SRAM137 read-only diagnostic. Included after transfer file helpers.
 * Reuses the existing 64 KiB SDRAM buffer as two disjoint 32 KiB regions.
 * No PSRAM, erase, flash ID, EEPROM serial command, RTC or save writes. */
#include "../../core/save/save.c"
#include "../../core/save/mori_head137.h"
static struct cb_save_hint sram137_hint;
int cb_save_quick137(struct cb_ui *u,void (*show)(void)){
 sram137_hint=(struct cb_save_hint){CB_SAVE_UNKNOWN,0};
 if(u->cart.platform!=CB_GBA||u->cart.result!=CB_READY)return OF_ERR_NOT_SUPPORTED;
 strcpy(u->cart.save,"Unknown (head not scanned)");
 uint32_t total=u->cart.rom_bytes;
 if(!total||total>0x2000000u)return OF_ERR_BAD_RANGE;
 if(total>CB_MORI_HEAD_BYTES)total=CB_MORI_HEAD_BYTES;
 u->notice="Save type: scan first 64 KiB";show();
 int rc=of_cart_enter_safe_idle();if(rc)goto fail;
 /* Preserve the proven dummy read following bus/mode changes. */
 rc=cart_read(0,buffer,32,50000);if(rc)goto fail;
 for(uint32_t off=0;off<total;off+=32){
  uint32_t n=total-off<32?total-off:32;
  rc=cart_read(off,buffer+off,n,50000);if(rc)goto fail;
 }
 {int by_code=0;sram137_hint=cb_mori_head137(buffer,total,&by_code);
 const char *label="Unknown (64 KiB/code miss)";
 switch(sram137_hint.kind){
 case CB_SAVE_SRAM:label=by_code?"SRAM/FRAM 32 KiB (code)":"SRAM/FRAM 32 KiB (ROM)";break;
 case CB_SAVE_FLASH64:label="Flash 64 KiB (ROM hint)";break;
 case CB_SAVE_FLASH128:label="Flash 128 KiB (ROM hint)";break;
 case CB_SAVE_EEPROM:label="EEPROM (size unknown)";break;
 default:break;
 }
 snprintf(u->cart.save,sizeof(u->cart.save),"%s",label);
 }
 u->notice=0;return 0;
fail:
 strcpy(u->cart.save,"Save hint read failed");u->notice="Save hint read failed";return rc;
}
static uint8_t sram137_header[192];
static unsigned sram137_pass;
static int sram137_cancel(void *ctx){
 (void)ctx;of_input_poll_p0();return (_of_input_p0.buttons&OF_BTN_B)!=0;
}
static int sram137_identity(void){
 uint8_t h[192];struct cb_cart c;
 int first=cart_read(0,h,32,50000);if(first)return first;
 for(unsigned i=0;i<sizeof(h);i+=32){int rc=cart_read(i,h+i,32,50000);if(rc)return rc;}
 if(!cb_parse_gba(&c,h)||c.result!=CB_READY)return OF_ERR_IO;
 return memcmp(sram137_header,h,sizeof(h))?OF_ERR_IO:0;
}
static int sram137_prepare(void *ctx,enum cb_save_kind k,uint32_t n){
 (void)ctx;of_cart_caps_t c;
 if(k!=CB_SAVE_SRAM||n!=32768)return OF_ERR_BAD_RANGE;
 int rc=of_cart_get_caps(&c);if(rc)return rc;
 if(c.cart_caps_raw!=0x02713f03u||c.gba_space_mask!=19||c.max_read_bytes!=32)
  return OF_ERR_NOT_SUPPORTED;
 rc=of_cart_enter_safe_idle();if(rc)return rc;
 return sram137_identity();
}
static int sram137_read(void *ctx,uint32_t off,void *dst,uint32_t n){
 (void)ctx;
 if(off==0){++sram137_pass;int rc=sram137_identity();if(rc)return rc;}
 ui->phase=sram137_pass==1?"Read SRAM/FRAM pass 1":"Read SRAM/FRAM pass 2";
 for(uint32_t i=0;i<n;i+=32){
  if((i&1023u)==0){
   if(sram137_cancel(0))return OF_ERR_DENIED;
   ui->done=off+i;ui->total=32768;ui->elapsed_ms=now()-began;paint();
  }
  uint32_t part=n-i<32?n-i:32;
  int rc=of_cart_read_space(OF_CART_BUS_GBA,OF_CART_SPACE_SAVE_PARALLEL,
   off+i,(uint8_t*)dst+i,part,50000);if(rc)return rc;
 }
 return 0;
}
static int sram137_restore(void *ctx){
 (void)ctx;int rc=of_cart_enter_safe_idle();if(rc)return rc;
 return sram137_identity();
}
static int sram137_create(void *ctx,uint32_t n){
 (void)ctx;ui->phase="Create new 32 KiB SAV";paint();
 int rc=of_output_open(ui->path,n);if(rc)return rc;
 return OF_SVC->file_size("slot:5")==32768?0:OF_ERR_BAD_RANGE;
}
static int sram137_write_file(void *ctx,uint32_t off,const void *src,uint32_t n){
 (void)ctx;ui->phase="SRAM backup -> TF file";paint();
 return of_output_write(off,src,n);
}
static int sram137_finish_file(void *ctx){
 (void)ctx;
 /* Proven Pocket output contract: completed writes + full host reread.
  * Do not issue unsupported APF 0188 FLUSH (smoke.125 ACK timeout). */
 return 0;
}
static int sram137_read_file(void *ctx,uint32_t off,void *dst,uint32_t n){
 (void)ctx;ui->phase="Verify TF SAV against SRAM";paint();
 return backup_read(off,dst,n);
}
static void sram137_abort_file(void *ctx){
 (void)ctx;ui->notice="Incomplete backup; do not restore";
}
static int backup_sram137(void){
 ui->task="Backup SRAM/FRAM 32 KiB";
 ui->phase="Read only; B cancels";ui->total=32768;paint();
 of_cart_caps_t caps;int rc=of_cart_get_caps(&caps);if(rc)return rc;
 if(caps.cart_caps_raw!=0x02713f03u||caps.gba_space_mask!=19)return OF_ERR_NOT_SUPPORTED;
 rc=of_cart_enter_safe_idle();if(rc)return rc;
 rc=cart_read(0,sram137_header,32,50000);if(rc)return rc;
 for(unsigned i=0;i<sizeof(sram137_header);i+=32){
  rc=cart_read(i,sram137_header+i,32,50000);if(rc)return rc;
 }
 struct cb_cart current;
 if(!cb_parse_gba(&current,sram137_header)||current.result!=CB_READY)return OF_ERR_IO;
 if(strcmp(current.code,ui->cart.code)||strcmp(current.title,ui->cart.title))return OF_ERR_IO;
 ui->phase="Check save hint / first 64 KiB";
 rc=cb_save_quick137(ui,paint);if(rc)return rc;
 rc=sram137_identity();if(rc)return rc;
 if(sram137_hint.kind!=CB_SAVE_SRAM){
  ui->notice=ui->cart.save;return OF_ERR_NOT_SUPPORTED;
 }
 snprintf(ui->path,sizeof(ui->path),"/Saves/chiskit/common/sram137-%08lx.sav",(unsigned long)now());
 /* Candidate type, not physical chip identification. Confirm the known SRAM
  * test cartridge explicitly; no flash/EEPROM fallback is attempted. */
 ui->notice="Known SRAM/FRAM cart only; read-only";
 if(!confirm())return OF_ERR_DENIED;
 const struct cb_save_ops ops={0,sram137_prepare,sram137_read,sram137_restore,
  sram137_create,sram137_write_file,sram137_finish_file,sram137_read_file,
  sram137_abort_file,sram137_cancel};
 struct cb_save_report report;sram137_pass=0;phase_began=now();
 rc=cb_save_backup(&ops,CB_SAVE_SRAM,32768,buffer,32768,buffer+32768,32768,&report);
 snprintf(ui->io_diag,sizeof(ui->io_diag),"Save %d / IO %d / restore %d",rc,report.detail,report.restore_rc);
 if(!rc){
  struct md5_53 md;uint8_t digest[16];md5_init53(&md);md5_add53(&md,buffer,32768);
  md5_end53(&md,digest);md5_hex53(digest,ui->hash);
  ui->done=ui->total=32768;ui->notice="Double-read + file compare PASS";
  unsigned uniform=1;for(unsigned i=1;i<32768;i++)if(buffer[i]!=buffer[0]){uniform=0;break;}
  if(uniform)ui->notice="Uniform data: compare external backup";
 }
 return rc;
}
