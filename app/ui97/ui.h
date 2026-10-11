#ifndef CB_UI97_H
#define CB_UI97_H
#include "model.h"
#define CB_W 320
#define CB_H 288
enum cb_page {CB_FUNCTIONS,CB_INFO,CB_TRANSFER};
struct cb_ui {
 struct cb_cart cart;
 enum cb_page page;
 unsigned focus;
 int options, probing;
 const char *notice;
};
extern const uint32_t cb_palette[16];
void cb_render(uint8_t *fb,const struct cb_ui *ui);
/* Returns 1=refresh, 2=analyze. Transfer entries remain unavailable. */
int cb_key(struct cb_ui *ui,uint32_t pressed);
#endif
