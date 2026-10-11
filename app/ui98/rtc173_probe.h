/* Legacy GBA ROM backend requires aligned 32-byte reads. */
static struct {
 unsigned stage,offset,blocks;int rc,parsed,identity;
 uint8_t header[192],id[32];struct cb_cart current;
} rtc173_diag;
static int rtc173_probe(uint8_t id[16]){
 memset(&rtc173_diag,0,sizeof rtc173_diag);
 memset(&rtc168_capture,0,sizeof rtc168_capture);
 rtc173_diag.stage=1;ui->phase="RTC entry: safe idle";
 int rc=of_cart_enter_safe_idle();if(rc)goto done;
 rtc173_diag.stage=2;ui->phase="RTC entry: dummy ROM read";
 rc=cart_read(0,rtc173_diag.header,32,50000);if(rc)goto done;
 rtc173_diag.stage=3;ui->phase="RTC entry: ROM header read";
 for(unsigned i=0;i<192;i+=32){
  rtc173_diag.offset=i;rc=cart_read(i,rtc173_diag.header+i,32,50000);
  if(rc)goto done;
  rtc173_diag.blocks++;
 }
 rtc173_diag.stage=4;ui->phase="RTC entry: header validation";
 rtc173_diag.parsed=cb_parse_gba(&rtc173_diag.current,rtc173_diag.header);
 if(!rtc173_diag.parsed||rtc173_diag.current.result!=CB_READY){rc=OF_ERR_IO;goto done;}
 rtc173_diag.stage=5;ui->phase="RTC entry: identity comparison";
 rtc173_diag.identity=!strcmp(rtc173_diag.current.code,ui->cart.code)&&!strcmp(rtc173_diag.current.title,ui->cart.title);
 if(!rtc173_diag.identity){rc=OF_ERR_IO;goto done;}
 rtc173_diag.stage=6;ui->phase="RTC entry: clock read";
 rc=of_rtc167_read(&rtc168_capture);
 cb_rtc167_format(ui->cart.rtc,sizeof ui->cart.rtc,rc,&rtc168_capture);
 if(rc)goto done;
 rtc173_diag.stage=7;ui->phase="RTC entry: cleanup check";
 rc=rtc168_capture.cleanup_rc;if(rc)goto done;
 rtc173_diag.stage=8;ui->phase="RTC entry: support check";
 if(!cb_rtc168_available(0,&rtc168_capture)||rtc168_capture.status!=0x40||!OF_CART_SVC_HAS(rtc172_write)){
  rc=OF_ERR_NOT_SUPPORTED;goto done;
 }
 rtc173_diag.stage=9;rtc173_diag.offset=0xa0;ui->phase="RTC entry: post-RTC ROM read";
 rc=cart_read(0xa0,rtc173_diag.id,32,50000);if(rc)goto done;
 if(memcmp(rtc173_diag.id,rtc173_diag.header+0xa0,16)){
  ui->phase="RTC entry: post-RTC identity";rc=OF_ERR_IO;goto done;
 }
 memcpy(id,rtc173_diag.id,16);rtc173_diag.stage=10;
done:
 rtc173_diag.rc=rc;return rc;
}
