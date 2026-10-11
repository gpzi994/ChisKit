#include "of_rtc167.h"
#include "rtc167_ui.h"
#include "rtc168_policy.h"
#include "rtc169_footer.h"
#include <time.h>
#include "rtc_backup188_choice.h"
#ifndef CB_RTC169_UTC_OFFSET_MINUTES
#define CB_RTC169_UTC_OFFSET_MINUTES 480
#endif
static of_rtc167_report rtc168_capture;
static int rtc168_read_current(void){
 uint8_t header[192];struct cb_cart current;
 int rc=of_cart_enter_safe_idle();if(rc)return rc;
 rc=cart_read(0,header,32,50000);if(rc)return rc;
 for(unsigned i=0;i<sizeof header;i+=32){rc=cart_read(i,header+i,32,50000);if(rc)return rc;}
 if(!cb_parse_gba(&current,header)||current.result!=CB_READY||
    strcmp(current.code,ui->cart.code)||strcmp(current.title,ui->cart.title))return OF_ERR_IO;
 memset(&rtc168_capture,0,sizeof rtc168_capture);
 rc=of_rtc167_read(&rtc168_capture);
 cb_rtc167_format(ui->cart.rtc,sizeof ui->cart.rtc,rc,&rtc168_capture);
 return rc?rc:rtc168_capture.cleanup_rc;
}
static int rtc168_choose(int *include){
 *include=0;ui->task="Backup save";ui->phase="Checking current cartridge RTC";paint();
 int rc=rtc168_read_current();if(rc)return rc;
 if(!cb_rtc168_available(rc,&rtc168_capture))return 0;
 ui->confirm=2;ui->phase="Include RTC with this backup?";paint();
 rc=rtc_backup188_choose(include);if(rc)return rc;
 ui->phase="Backing up save";paint();return 0;
}
static int rtc168_backup(void){
 uint8_t footer[16],check[16];struct timespec timestamp;
 ui->phase="Read RTC for backup";paint();
 int rc=rtc168_read_current();if(rc)return rc;
 if(!cb_rtc168_available(rc,&rtc168_capture))return OF_ERR_IO;
 ui->phase="Read Pocket backup timestamp";paint();
 if(clock_gettime(CLOCK_REALTIME,&timestamp)||
    cb_rtc169_footer(footer,&rtc168_capture,timestamp.tv_sec,CB_RTC169_UTC_OFFSET_MINUTES))return OF_ERR_IO;
 uint32_t bytes=ui->total;
 if(OF_SVC->file_size("slot:5")!=(long)(bytes+16u))return OF_ERR_BAD_RANGE;
 ui->phase="Write and verify SAV RTC footer";paint();
 rc=of_output_write(bytes,footer,sizeof footer);if(rc)return rc;
 rc=backup_read(bytes,check,sizeof check);if(rc)return rc;
 if(memcmp(footer,check,sizeof footer))return OF_ERR_IO;
 struct md5_53 md;uint8_t digest[16];md5_init53(&md);
 for(uint32_t off=0;off<bytes+16u;){
  uint32_t part=bytes+16u-off;if(part>sizeof buffer)part=sizeof buffer;
  rc=backup_read(off,buffer,part);if(rc)return rc;
  md5_add53(&md,buffer,part);off+=part;
 }
 md5_end53(&md,digest);md5_hex53(digest,ui->hash);
 ui->done=ui->total=bytes+16u;
 ui->notice="SAV + 16-byte RTC footer verified";return 0;
}
