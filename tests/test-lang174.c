#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../app/ui98/ui.h"
#include "../app/ui98/lang174.h"
#include "../app/ui98/font174.h"
#include "of_input_types.h"
static void render(struct cb_ui *u,const char *path){
 struct {unsigned char pre[32],fb[CB_W*CB_H],post[32];} b;
 memset(&b,0xa5,sizeof b);cb_render(b.fb,u);
 for(unsigned i=0;i<32;i++)assert(b.pre[i]==0xa5&&b.post[i]==0xa5);
 FILE *f=fopen(path,"wb");assert(f);fprintf(f,"P6\n320 288\n255\n");
 for(unsigned i=0;i<sizeof b.fb;i++){unsigned c=((u->options==1?u->theme_draft:u->theme)?cb_palette_dark:cb_palette)[b.fb[i]];fputc(c>>16,f);fputc(c>>8,f);fputc(c,f);}fclose(f);
}
int main(void){
#ifdef CB_GB_RTC189
 struct cb_ui keytest={0};keytest.page=CB_FUNCTIONS;keytest.cart.result=CB_READY;
 keytest.cart.platform=CB_GB;assert(cb_key(&keytest,OF_BTN_X)==7);
 keytest.cart.platform=CB_GBA;assert(cb_key(&keytest,OF_BTN_X)==7);
 keytest.cart.result=CB_IO;assert(cb_key(&keytest,OF_BTN_X)==0);
#endif
 for(unsigned i=0;i<sizeof cb_translations174/sizeof *cb_translations174;i++){
  const unsigned char *p=(const unsigned char *)cb_translations174[i].zh;
  while(*p){unsigned c=cb_utf174(&p);if(c<128)continue;int found=0;
   for(unsigned j=0;j<sizeof cb_font174/sizeof *cb_font174;j++)found|=cb_font174[j].code==c;
   assert(found);
  }
 }
 const unsigned char *p=(const unsigned char *)"\xe4\xb8";assert(cb_utf174(&p)==0xfffd&&!*p);
 struct cb_ui u={0};u.cart.platform=CB_GBA;u.cart.result=CB_READY;u.focus=1;
 render(&u,"build/lang174-test/functions-cn.ppm");
 u.theme=1;render(&u,"build/lang174-test/functions-dark.ppm");u.theme=0;
 u.page=CB_INFO;strcpy(u.cart.title,"POKEMON EMER");strcpy(u.cart.code,"BPEE / 01");
 strcpy(u.cart.rom,"32 MiB (NOR)");strcpy(u.cart.save,"Flash 128 KiB");
 render(&u,"build/lang174-test/info-cn.ppm");
 u.page=CB_TRANSFER;render(&u,"build/lang174-test/transfer-cn.ppm");u.page=CB_FUNCTIONS;
 u.page=CB_TRANSFER;u.cart.platform=CB_GB;u.confirm=2;
 u.task="Set cartridge RTC";u.phase="GB RTC";u.notice="Changes RTC only; SAV and ROM untouched";
 u.confirm=7;u.rtc_edit[0]=511;u.rtc_edit[1]=23;u.rtc_edit[2]=59;u.rtc_edit[3]=59;
 render(&u,"build/lang174-test/gb-rtc-editor192.ppm");
 u.confirm=8;render(&u,"build/lang174-test/gb-rtc-review192.ppm");
 u.confirm=2;
 strcpy(u.cart.rtc,"2d 03:29:40");
 render(&u,"build/lang174-test/gb-rtc-confirm-cn.ppm");
 u.confirm=0;u.cart.platform=CB_GBA;u.page=CB_FUNCTIONS;
 u.language=1;render(&u,"build/lang174-test/functions-en.ppm");
 u.focus=7;assert(!cb_key(&u,OF_BTN_A)&&u.options==1&&u.language_draft==1);
 cb_key(&u,OF_BTN_RIGHT);assert(!u.language_draft&&u.language==1);
 render(&u,"build/lang174-test/settings-cn.ppm");
 assert(cb_key(&u,OF_BTN_A)==8);cb_key(&u,OF_BTN_B);assert(!u.options&&u.language==1);
 cb_key(&u,OF_BTN_A);cb_key(&u,OF_BTN_DOWN);cb_key(&u,OF_BTN_A);assert(u.options==2);
 cb_key(&u,OF_BTN_B);assert(!u.options);
 u.cart.platform=CB_GB;u.options=1;u.settings_focus=1;
 cb_key(&u,OF_BTN_DOWN);assert(u.settings_focus==2);
 cb_key(&u,OF_BTN_RIGHT);assert(u.theme_draft==1);
 render(&u,"build/lang174-test/settings-dark.ppm");
 cb_key(&u,OF_BTN_B);assert(u.theme_draft==u.theme);
 u.options=1;u.settings_focus=0;
 assert(cb_key(&u,OF_BTN_A)==8&&u.options==1);
 render(&u,"build/lang174-test/settings-gb.ppm");
 u.options=2;unsigned profile=u.save_profile;cb_key(&u,OF_BTN_RIGHT);
 assert(!u.options&&u.save_profile==profile);
 u.cart.platform=CB_UNKNOWN;u.options=1;u.settings_focus=1;
 assert(cb_key(&u,OF_BTN_A)==8&&u.settings_focus==0);
 u.options=0;
 cb_reset164(&u);assert(u.language==1&&u.cart.result==CB_LOCKED);
 puts("LANG174 UI PASS: glyph coverage, UTF-8, guarded framebuffer, toggle/cancel, old options, refresh persistence");
}
