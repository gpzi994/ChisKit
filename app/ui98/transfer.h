#ifndef CB_TRANSFER98_H
#define CB_TRANSFER98_H
#include "ui.h"
void cb_transfer(struct cb_ui *,unsigned action,void (*present)(void));
int cb_output_verify161(const void *,uint32_t bytes);
int cb_program32(struct cb_ui *,int (*confirm)(void));
int cb_program8(struct cb_ui *,int (*confirm)(void));
#ifdef CB_SRAM137
int cb_save_quick137(struct cb_ui *,void (*present)(void));
#endif
#endif
