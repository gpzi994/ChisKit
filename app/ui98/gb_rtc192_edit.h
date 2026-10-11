/* GB-only standalone editor; no SAV input, save writes or Pocket calendar. */
static int gb192_edit(void){
 static uint8_t header[80],target[5];
 static of_gb186_report before,after;
 static struct gb189_report result;
 if(ui->cart.platform!=CB_GB||ui->cart.result!=CB_READY)return OF_ERR_DENIED;
 if(!OF_CART_SVC_HAS(gb189_write))return OF_ERR_NOT_SUPPORTED;
 memcpy(header,ui->gb_save175_header,80);
 if(header[0x47]!=0x0f&&header[0x47]!=0x10)return OF_ERR_NOT_SUPPORTED;
 ui->task="Set cartridge RTC";ui->phase="Checking GB RTC";paint();
 memset(&before,0,sizeof before);
 int rc=of_gb186_read(header,&before);
 if(rc||before.cleanup_rc||!before.supported)return rc?rc:OF_ERR_NOT_SUPPORTED;
 unsigned *v=ui->rtc_edit;
 v[0]=before.days;v[1]=before.hours;v[2]=before.minutes;v[3]=before.seconds;
 v[4]=before.raw[4]&0xc0; /* Preserve halt/carry; not calendar fields. */
 ui->rtc_field=0;ui->confirm=7;ui->phase="GB RTC";
 ui->notice="Changes RTC only; SAV and ROM untouched";paint();
 int released=0;
 for(;;){
  of_input_poll_p0();unsigned k=_of_input_p0.buttons;
  if(!k)released=1;
  else if(released){
   released=0;
   if(k&OF_BTN_B){if(ui->confirm==8){ui->confirm=7;paint();continue;}ui->confirm=0;return OF_ERR_DENIED;}
   if(ui->confirm==8){if(k&OF_BTN_A)break;}
   else if(k&OF_BTN_A)ui->confirm=8;
   else if(k&OF_BTN_LEFT)ui->rtc_field=(ui->rtc_field+3)%4;
   else if(k&OF_BTN_RIGHT)ui->rtc_field=(ui->rtc_field+1)%4;
   else if(k&(OF_BTN_UP|OF_BTN_DOWN)){
    unsigned f=ui->rtc_field,hi=f==0?511:f==1?23:59;
    if(k&OF_BTN_UP)v[f]=v[f]==hi?0:v[f]+1;
    else v[f]=v[f]==0?hi:v[f]-1;
   }
   paint();
  }usleep(16000);
 }
 ui->confirm=0;ui->phase="Write RTC and verify";paint();
 target[0]=v[3];target[1]=v[2];target[2]=v[1];target[3]=v[0];target[4]=(v[0]>>8)|v[4];
 memset(&result,0,sizeof result);
 rc=OF_SVC->gb189_write(header,target,&result,sizeof result);
 if(!rc&&(result.rc||result.cleanup_rc||!result.verified))rc=OF_ERR_IO;
 snprintf(ui->io_diag,sizeof ui->io_diag,"RTC %d S%u W%lu C%ld",rc,result.reserved[0],(unsigned long)result.written,(long)result.cleanup_rc);
 snprintf(ui->io_regs,sizeof ui->io_regs,"T%02X%02X%02X%02X%02X R%02X%02X%02X%02X%02X",target[0],target[1],target[2],target[3],target[4],result.raw[0],result.raw[1],result.raw[2],result.raw[3],result.raw[4]);
 if(!rc){
  memset(&after,0,sizeof after);rc=of_gb186_read(header,&after);
  if(!rc&&(!after.supported||after.cleanup_rc))rc=OF_ERR_IO;
  cb_gbrtc186_format(ui->cart.rtc,sizeof ui->cart.rtc,rc,&after);
 }
 ui->notice=rc?"RTC failed; clock may have changed":"RTC updated and readback verified";
 if(rc)ui->gb_save175_ready=0;
 return rc;
}
