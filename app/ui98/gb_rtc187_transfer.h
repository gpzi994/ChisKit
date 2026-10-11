#include "gb_rtc186_ui.h"
#include "gb_rtc187_footer.h"
#include <time.h>
#include "rtc_backup188_choice.h"
static of_gb186_report gb187_rtc;
static uint8_t gb187_footer[48];
static int gb187_current(void){
 memset(&gb187_rtc,0,sizeof gb187_rtc);
 int rc=of_gb186_read(ui->gb_save175_header,&gb187_rtc);
 cb_gbrtc186_format(ui->cart.rtc,sizeof ui->cart.rtc,rc,&gb187_rtc);
 if(gb187_rtc.cleanup_rc)return OF_ERR_IO;
 return rc;
}
static int gb187_choose(int *include){
 *include=0;
 if(ui->cart.platform!=CB_GB)return OF_ERR_DENIED;
 if(ui->gb_save175_header[0x47]!=0x0f&&ui->gb_save175_header[0x47]!=0x10)return 0;
 ui->phase="Checking GB RTC";paint();
 int rc=gb187_current();
 if(rc==OF_ERR_NOT_SUPPORTED)return 0;
 if(rc)return rc;
 if(!gb187_rtc.supported)return 0;
 ui->confirm=2;ui->phase="Include GB RTC with backup?";paint();
 return rtc_backup188_choose(include);
}
static int gb187_prepare(void){
 ui->phase="Read GB RTC for backup";paint();
 int rc=gb187_current();if(rc)return rc;
 if(!gb187_rtc.supported)return OF_ERR_IO;
 struct timespec ts;
 if(clock_gettime(CLOCK_REALTIME,&ts)||cb_gbrtc187_footer(gb187_footer,gb187_rtc.raw,ts.tv_sec))return OF_ERR_IO;
 return 0;
}
