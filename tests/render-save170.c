#include "ui.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 static uint8_t pixels[CB_W*CB_H];
 const unsigned sizes[]={32768,131072,512};
 const char *tasks[]={"Write SRAM SAV","Write Flash SAV","Write EEPROM SAV"};
 for(unsigned k=0;k<3;k++){
  struct cb_ui u={0};u.page=CB_TRANSFER;u.confirm=1;u.busy=1;
  u.transfer_action=5;u.task=tasks[k];u.total=sizes[k];
  u.phase="Press A again to confirm";u.notice="Backup file retained in Saves";
  cb_render(pixels,&u);char name[128];snprintf(name,sizeof name,"build/ui98-preview/save170-r3-%u.ppm",k);
  FILE*f=fopen(name,"wb");assert(f);fprintf(f,"P6\n320 288\n255\n");
  for(unsigned i=0;i<sizeof pixels;i++){unsigned c=cb_palette[pixels[i]];fputc(c>>16,f);fputc(c>>8,f);fputc(c,f);}fclose(f);
 }
 return 0;
}
