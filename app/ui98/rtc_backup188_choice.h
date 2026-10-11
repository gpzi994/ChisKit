#ifndef CB_RTC_BACKUP188_CHOICE_H
#define CB_RTC_BACKUP188_CHOICE_H
/* UI only: GB and GBA retain their independent RTC detection and formats. */
static int rtc_backup188_choose(int *include){
 *include=0;ui->rtc_backup_choice=0;ui->confirm=2;paint();
 int released=0;
 for(;;){
  of_input_poll_p0();unsigned keys=_of_input_p0.buttons;
  if(!keys)released=1;
  else if(released){
   released=0;
   if(keys&OF_BTN_B){ui->rtc_backup_cancelled=1;break;}
   if(keys&(OF_BTN_UP|OF_BTN_DOWN)){ui->rtc_backup_choice^=1;paint();}
   else if(keys&OF_BTN_A){*include=ui->rtc_backup_choice!=0;break;}
  }
  usleep(16000);
 }
 do{of_input_poll_p0();if(!_of_input_p0.buttons)break;usleep(16000);}while(1);
 ui->confirm=0;
 return ui->rtc_backup_cancelled?OF_ERR_DENIED:0;
}
#endif
