/* Production UI orchestration with fault-injected OS/file boundaries. */
#define OF_PC 1
#define CB_NOR108 1
#include "ui.h"
#include "of_file.h"
#include "of_output.h"
#include "of_input.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
static struct of_services_table svc;
#undef OF_SVC
#define OF_SVC (&svc)
#undef OF_CART_SVC_HAS
#define OF_CART_SVC_HAS(f) (svc.f!=0)
static of_input_state_t _of_input_p0;
static uint8_t source[131072],cart[131072];
static uint32_t ticks,erased,written,readback,scans,begins,finishes,revokes;
static int cancel_test,begin_error,erase_error,program_error,change_source,bad_read,cancel_after_erase;
static unsigned buffer_limit=32,sector_bytes=8192,test_bus=1;
static int route_read_enabled,route_read_error;
static uint32_t slot_bytes=sizeof(source),legacy_expected,legacy_calls;
static uint32_t clock_ms(void){return ticks;}
static long size_file(const char*p){assert(!strcmp(p,"slot:4"));return slot_bytes;}
static int sleep_test(unsigned n){ticks+=n/1000;return 0;}
static void poll_test(void){_of_input_p0.buttons=cancel_test||(cancel_after_erase&&erased)?OF_BTN_B:ticks<200?OF_BTN_A:ticks<400?0:OF_BTN_A;}
static int safe_test(void){return 0;}
static int caps_test(of_cart_caps_t*p){(void)p;assert(0);return -1;}
static int flash_test(of_cart_flash_caps_t*p){(void)p;assert(0);return -1;}
static int old_erase(uint32_t a,uint32_t t){(void)a;(void)t;assert(0);return -1;}
static int old_program(uint32_t a,const void*p,uint32_t n,uint32_t t){(void)a;(void)p;(void)n;(void)t;assert(0);return -1;}
static int old_read(of_cart_bus_t b,uint32_t a,void*p,uint32_t n,uint32_t t){
 assert(test_bus==2&&b==2&&n==32&&t==50000&&a+n<=sizeof(cart));
 memcpy(p,cart+a,n);readback+=n;if(bad_read)((uint8_t*)p)[0]^=1;return 0;
}
static int read_space(of_cart_bus_t b,of_cart_space_t s,uint32_t a,void*p,uint32_t n,uint32_t t){
 assert(b==1&&s==4&&t==50000&&n==32&&(a&0xff000000)==0x15000000);
 a&=0xffffff;assert(a+n<=sizeof(cart));memcpy(p,cart+a,n);readback+=n;
 if(bad_read)((uint8_t*)p)[0]^=1;return 0;
}
static int output_open_test(const char*p,uint32_t n){(void)p;(void)n;assert(0);return -1;}
static int output_write_test(uint32_t a,const void*p,uint32_t n){(void)a;(void)p;(void)n;assert(0);return -1;}
static int async_idle(void){return 0;}
static int async_read(uint32_t slot,uint32_t a,void*p,uint32_t n,void(*cb)(int,int)){
 assert(slot==4&&n==65536&&a+n<=sizeof(source));memcpy(p,source+a,n);scans++;
 if(change_source&&begins)((uint8_t*)p)[500]^=1;cb(108,0);return 108;
}
static int available(void){return 1;}
static int begin_test(uint32_t g,uint32_t bytes,uint32_t*token){
 assert(g==108&&bytes==sizeof(source)&&ticks>=400&&scans==2);begins++;
 if(begin_error)return -7;*token=17;return 0;
}
static int erase_test(uint32_t token,uint32_t address,uint32_t*length){
 assert(token==17&&address==erased&&!written&&address+sector_bytes<=sizeof(cart));
 if(erase_error)return -6;memset(cart+address,255,sector_bytes);erased+=sector_bytes;*length=sector_bytes;return 0;
}
static int program_test(uint32_t token,uint32_t address,const void*p,uint32_t n){
 assert(token==17&&erased==sizeof(cart)&&address==written&&n&&n<=buffer_limit);
 assert(address/16384==(address+n-1)/16384&&address/sector_bytes==(address+n-1)/sector_bytes);
 if(program_error)return -6;memcpy(cart+address,p,n);written+=n;return 0;
}
static int finish_test(uint32_t token){
 if(!token){revokes++;return 0;}assert(token==17&&written==sizeof(cart));
 if(route_read_enabled)assert(readback==sizeof(cart));
 finishes++;return 0;
}
static int verify_available_test(void){return route_read_enabled;}
static int verify_read_test(uint32_t token,uint32_t a,void*p,uint32_t n){
 assert(route_read_enabled&&token==17&&!finishes&&written==sizeof(cart)&&a+n<=sizeof(cart)&&n==1024);
 if(route_read_error)return -5;
 memcpy(p,cart+a,n);readback+=n;if(bad_read)((uint8_t*)p)[0]^=1;return 0;
}
#define usleep sleep_test
#define of_input_poll_p0 poll_test
#define of_cart_get_caps caps_test
#define of_cart_flash_get_caps flash_test
#define of_cart_enter_safe_idle safe_test
#define of_cart_flash_disarm safe_test
#define of_cart_flash_erase_sector old_erase
#define of_cart_flash_program old_program
#define of_cart_read old_read
#define of_cart_read_space read_space
#define of_output_open output_open_test
#define of_output_write output_write_test
#define of_file_async_busy async_idle
#define of_file_async_poll async_idle
#define of_file_read_async async_read
#define of_nor_available available
#define of_nor_begin begin_test
#define of_nor_erase erase_test
#define of_nor_program program_test
#define of_nor_finish finish_test
#define of_nor_verify_available verify_available_test
#define of_nor_verify_read verify_read_test
#include "../app/ui98/transfer.c"
int cb_program32(struct cb_ui*u,int(*c)(void)){(void)u;assert(legacy_expected==32&&c());legacy_calls++;return 0;}
int cb_program8(struct cb_ui*u,int(*c)(void)){(void)u;assert(legacy_expected==8&&c());legacy_calls++;return 0;}
static void paint_test(void){assert(ui->page==CB_TRANSFER);}
static int run(void){
 struct cb_ui u={.cart={.platform=CB_GB,.result=CB_READY},.analysis={.matched=1,.writable=1}};
 u.cart.platform=test_bus==1?CB_GB:CB_GBA;
 u.analysis.legacy_programmer=legacy_expected!=0;
 u.analysis.nor=(of_nor_info_t){.version=1,.generation=108,.bus=test_bus,.mapper=5,.flags=OF_NOR_WRITE_OK,
  .flash_bytes=sizeof(cart),.accessible_bytes=sizeof(cart),.buffer_bytes=buffer_limit==1?0:buffer_limit,
  .region_count=1,.regions={{0,sizeof(cart)/8192,8192}}};
 ticks=erased=written=readback=scans=begins=finishes=revokes=0;
 cb_transfer(&u,4,paint_test);
 assert(u.finished&&!u.busy&&!u.analysis.writable&&!u.analysis.matched&&revokes);
 return u.task_rc;
}
int main(int argc,char**argv){
 for(unsigned i=0;i<sizeof(source);i++)source[i]=(uint8_t)(i*37+(i>>14));
 memcpy(source+0x104,cb_gb_logo,48);memcpy(source+0x134,"NOR108 TEST     ",15);
 source[0x143]=0x80;source[0x147]=0x1b;source[0x148]=2;source[0x149]=3;
 source[0x14d]=0;for(unsigned i=0x134;i<=0x14c;i++)source[0x14d]=(uint8_t)(source[0x14d]-source[i]-1);
 svc.timer_get_ms=clock_ms;svc.file_size=size_file;svc.output_open=output_open_test;svc.output_write=output_write_test;
 cancel_test=1;assert(run()==-3&&!begins&&!erased);cancel_test=0;
 begin_error=1;assert(run()==-7&&begins==1&&!erased);begin_error=0;
 erase_error=1;assert(run()==-6&&!written);erase_error=0;
 program_error=1;assert(run()==-6&&erased==sizeof(cart)&&!written);program_error=0;
 cancel_after_erase=1;assert(run()==-3&&erased==sector_bytes&&!written);cancel_after_erase=0;
 change_source=1;assert(run()==-7&&erased==sizeof(cart)&&!written);change_source=0;
 bad_read=1;assert(run()==-7&&finishes==1&&readback==sizeof(cart));bad_read=0;
 for(buffer_limit=1;buffer_limit<=1024;buffer_limit*=2){
  assert(!run()&&scans==4&&written==sizeof(cart)&&readback==sizeof(cart)&&finishes==1);
  assert(!memcmp(source,cart,sizeof(cart)));
 }
 assert(argc==2);FILE*f=fopen(argv[1],"rb");assert(f&&fread(source,1,192,f)==192);fclose(f);
 test_bus=2;
 for(buffer_limit=2;buffer_limit<=1024;buffer_limit*=2){
  assert(!run()&&written==sizeof(cart)&&readback==sizeof(cart)&&finishes==1);
  assert(!memcmp(source,cart,sizeof(cart)));
 }
 begin_error=1;assert(run()==-7&&!erased);begin_error=0;
 bad_read=1;assert(run()==-7&&finishes==1);bad_read=0;
 slot_bytes=0x2000000;legacy_expected=32;assert(!run()&&legacy_calls==1&&!begins&&!erased);
 slot_bytes=0x84d500;legacy_expected=8;assert(!run()&&legacy_calls==2&&!begins&&!erased);
 slot_bytes=sizeof(source);legacy_expected=0;buffer_limit=32;route_read_enabled=1;
 assert(!run()&&readback==sizeof(cart)&&finishes==1);
 bad_read=1;assert(run()==-7&&!finishes&&revokes);bad_read=0;
 route_read_error=1;assert(run()==-5&&!finishes&&revokes);route_read_error=0;
 puts("PASS UI108: production two-A write flow, fresh-ID failure, erase/program failure, changed source, readback corruption, byte/buffer/window boundaries");
}
