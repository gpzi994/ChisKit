#include "ui.h"
#include "of_input_types.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void cart(struct cb_ui *u){
 cb_reset164(u);u->cart.platform=CB_GBA;u->cart.result=CB_READY;
 u->cart.header_valid=1;u->cart.rom_source=1;u->analysis.ran=1;
 u->analysis.matched=u->analysis.writable=1;
 strcpy(u->cart.title,"BLANK SRAM");cb_save171_update(u);
}
int main(void){
 struct cb_ui u;cart(&u);
 assert(u.save_selection.type==8&&u.save_selection.source==CB_SAVE171_FLASHCART_DEFAULT);
 assert(cb_action_enabled(&u,5)); /* OS still validates selected profile */
 u.focus=7;cb_key(&u,OF_BTN_A);assert(u.options);
 cb_key(&u,OF_BTN_RIGHT);assert(u.save_choice_draft==1&&!u.save_choice);
 cb_key(&u,OF_BTN_B);assert(!u.options&&!u.save_choice);
 cb_key(&u,OF_BTN_A);cb_key(&u,OF_BTN_RIGHT);cb_key(&u,OF_BTN_RIGHT);
 cb_key(&u,OF_BTN_A);assert(u.save_choice==2&&u.save_selection.type==1);
 assert(cb_action_enabled(&u,5)&&u.save_detected_type==-1);
 cb_key(&u,OF_BTN_A);assert(u.save_choice_draft==2);
 cb_key(&u,OF_BTN_LEFT);cb_key(&u,OF_BTN_LEFT);cb_key(&u,OF_BTN_A);
 assert(!u.save_choice&&u.save_selection.type==8);
 u.save_detected_type=5;u.save_detected_bytes=131072;cb_save171_update(&u);
 assert(cb_action_enabled(&u,5)&&u.save_selection.type==5);
 u.save_choice=9;cb_save171_update(&u);
 assert(u.save_selection.conflict&&!cb_action_enabled(&u,5));
 u.save_choice=0;u.save_rc=-3;cb_save171_update(&u);assert(!cb_action_enabled(&u,5));
 cb_reset164(&u);assert(!u.save_choice&&u.save_selection.type==-1&&!cb_action_enabled(&u,5));
 static uint8_t pixels[CB_W*CB_H];cart(&u);u.options=1;
 cb_render(pixels,&u);
 FILE *f=fopen("build/save171-test/options.ppm","wb");assert(f);
 fprintf(f,"P6\n%d %d\n255\n",CB_W,CB_H);
 for(unsigned n=0;n<sizeof pixels;n++){
  unsigned c=cb_palette[pixels[n]];fputc(c>>16,f);fputc(c>>8,f);fputc(c,f);
 }fclose(f);
 puts("save171 UI PASS: draft/cancel/apply/auto/reset, selected restore enabled, conflicts/errors locked");
}
