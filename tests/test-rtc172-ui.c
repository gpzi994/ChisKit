#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../app/ui98/ui.h"
int main(void){
 struct cb_ui u={0};u.page=CB_TRANSFER;u.confirm=3;u.rtc_restore_choice=2;
 u.task="Write Flash SAV";u.phase="RTC data found in selected SAV";
 u.notice="RTC data stays outside save memory";
 static uint8_t pixels[CB_W*CB_H];cb_render(pixels,&u);
 FILE *f=fopen("build/rtc172-test/options.ppm","wb");assert(f);
 fprintf(f,"P6\n%d %d\n255\n",CB_W,CB_H);
 for(unsigned n=0;n<sizeof pixels;n++){
  unsigned c=cb_palette[pixels[n]];fputc(c>>16,f);fputc(c>>8,f);fputc(c,f);
 }fclose(f);
 puts("RTC172 UI rendered");
}
