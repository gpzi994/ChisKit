#include "ui.h"
#include "of_cart.h"
#include "of_video.h"
#include "of_input.h"
#include <unistd.h>
#include <stdio.h>
static struct cb_ui state;
static void present(void){
 cb_render(of_video_surface(),&state);of_video_flush();of_video_flip();of_video_wait_flip();
}
static int read_cart(void *ctx,unsigned bus,uint32_t addr,void *dst,unsigned n){
 (void)ctx;return of_cart_read_space((of_cart_bus_t)bus,OF_CART_SPACE_ROM,addr,dst,n,50000);
}
static int safe(void *ctx){(void)ctx;return of_cart_enter_safe_idle();}
static void progress(void *ctx,const char *message){
 (void)ctx;state.notice=message;present();
}
static void scan(int analyze){
 of_cart_caps_t caps;
 state.probing=1;state.notice="Waiting for cartridge";state.page=CB_FUNCTIONS;
 state.cart=(struct cb_cart){.result=CB_LOCKED};present();
 int ready=0;
 for(unsigned i=0;i<100;i++){
  int rc=of_cart_get_caps(&caps);
  if(rc){state.cart.rc=rc;state.cart.result=CB_IO;break;}
  if(caps.bus_mask!=3||caps.writes_enabled||caps.operation_mask!=15||
     caps.gb_gbc_space_mask!=OF_CART_SPACE_MASK_ROM||
     caps.gba_space_mask!=OF_CART_SPACE_MASK_ROM||caps.max_read_bytes!=32){
   state.cart.result=CB_IO;state.notice="Wrong core / install full package";break;
  }
  if((caps.lifecycle_status&OF_CART_LIFECYCLE_READ_READY)&&
     (caps.lifecycle_status&OF_CART_LIFECYCLE_STATE_MASK)==OF_CART_LIFECYCLE_READY){ready=1;break;}
  usleep(20000);
 }
 if(ready){
  /* Preserve the field-proven power settle and discard first transaction. */
  state.notice="Settling cartridge...";present();usleep(1000000);
  const struct cb_probe_ops ops={0,read_cart,safe,progress};cb_probe(&state.cart,&ops);
  state.notice=0;
 }
 state.probing=0;if(analyze)state.page=CB_INFO;present();
}
int main(void){
 if(!OF_CART_SVC_HAS(video_init)||!OF_CART_SVC_HAS(video_get_surface)||
    !OF_CART_SVC_HAS(video_flip)||!OF_CART_SVC_HAS(video_wait_flip)||
    !OF_CART_SVC_HAS(video_flush_cache)||!OF_CART_SVC_HAS(video_set_palette_bulk)||
    !OF_CART_SVC_HAS(video_set_mode)||!OF_CART_SVC_HAS(video_set_display_mode)||
    !OF_CART_SVC_HAS(input_poll_p0)){
  printf("ChisBurner smoke.97: UI service missing\n");return 1;
 }
 of_video_init();
 const of_video_mode_t mode={CB_W,CB_H,CB_W,OF_VIDEO_MODE_8BIT,0};
 if(of_video_set_mode(&mode)<0){printf("ChisBurner: 320x288 display mode unavailable\n");return 1;}
 of_video_set_display_mode(OF_DISPLAY_FRAMEBUFFER);of_video_palette_bulk(cb_palette,16);
 state.cart.result=CB_LOCKED;present();scan(0);
 unsigned previous=~0u;
 for(;;){
  of_input_poll_p0();unsigned held=_of_input_p0.buttons,edge=held&~previous;previous=held;
  if(edge){int action=cb_key(&state,edge);if(action){scan(action==2);previous=~0u;}else present();}
  usleep(16000);
 }
}
