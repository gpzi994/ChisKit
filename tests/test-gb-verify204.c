#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#define CB_NOR_ARGUMENT -1
#define CB_NOR_CANCELLED -2
#define CB_NOR_IO -3
#define CB_NOR_TIMEOUT -4
#define CART_ST_BUSY 1u
#define CART_ST_DONE 2u
#define CART_ST_ERROR 4u
#define CART_ST_TIMEOUT 8u
#define CART_ST_ABORTED 16u
static unsigned bus_kind,mapper_kind,reads,selects,waits,releases,bank;
static unsigned cancel_at,fail_at,delay,command_delay,level,status,initial_busy;
static uint32_t addr,command;
static int select_error,wait_error;
#define CART_ADDRESS addr
#define CART_COMMAND command
#define CART_RX_LEVEL level
static uint32_t read_status(void){
 if(initial_busy)return CART_ST_BUSY;
 if(delay){delay--;return CART_ST_BUSY;}
 return status;
}
#define CART_STATUS read_status()
static uint32_t read_data(void){
 uint32_t absolute=addr<0x4000?addr:bank*0x4000+(addr&0x3fff);
 return (absolute^(absolute>>8)^(absolute>>16))&255;
}
#define CART_RX_DATA read_data()
static bool alive(void *p){(void)p;return !cancel_at||reads<cancel_at;}
static int select_bus(void *p,uint32_t a){
 (void)p;selects++;bank=a/0x4000;return select_error;
}
static void fence(void){
 assert(command==2);assert(addr<0x8000);reads++;
 delay=command_delay;
 if(reads==fail_at)status=CART_ST_ERROR;
}
static int wait_idle(unsigned ms,bool done){assert(ms==20&&done);waits++;return wait_error;}
static void release(void *p){(void)p;releases++;}
#include "../.integration/openfpgaos/src/firmware/os/kernel/gb_verify204.h"
static void reset(void){
 bus_kind=1;mapper_kind=5;reads=selects=waits=releases=bank=0;
 cancel_at=fail_at=delay=command_delay=initial_busy=0;level=1;status=CART_ST_DONE;
 select_error=wait_error=0;
}
int main(void){
 uint8_t out[1024];
 const uint32_t offsets[]={0,0x3ff0,0x7ff0,0x3ffff0,0x7ffc00};
 for(unsigned i=0;i<sizeof(offsets)/sizeof(offsets[0]);i++){
  reset();assert(!gb_verify204_read(offsets[i],out,sizeof(out)));
  for(unsigned n=0;n<sizeof(out);n++){
   uint32_t a=offsets[i]+n;
   assert(out[n]==((a^(a>>8)^(a>>16))&255));
  }
  assert(reads==1024&&selects<=2&&waits==0);
 }
 reset();bus_kind=2;assert(gb_verify204_read(0,out,1)==CB_NOR_ARGUMENT&&reads==0);
 reset();initial_busy=1;assert(gb_verify204_read(0,out,1)==CB_NOR_IO&&reads==0);
 reset();select_error=-9;assert(gb_verify204_read(0,out,1)==-9&&mapper_kind==0);
 reset();cancel_at=7;assert(gb_verify204_read(0,out,1024)==CB_NOR_CANCELLED&&reads==7);
 reset();cancel_at=1;assert(gb_verify204_read(0,out,1)==CB_NOR_CANCELLED);
 reset();fail_at=3;assert(gb_verify204_read(0,out,1024)==CB_NOR_IO&&reads==3&&releases==1);
 reset();level=2;assert(gb_verify204_read(0,out,1)==CB_NOR_IO);
 reset();status=0;assert(gb_verify204_read(0,out,1)==CB_NOR_IO);
 reset();command_delay=4;assert(!gb_verify204_read(0,out,1)&&waits==0);
 reset();command_delay=32;assert(!gb_verify204_read(0,out,1)&&waits==1);
 reset();command_delay=40;wait_error=CB_NOR_TIMEOUT;
 assert(gb_verify204_read(0,out,1)==CB_NOR_TIMEOUT&&waits==1&&releases==1);
 reset();status=CART_ST_TIMEOUT;assert(gb_verify204_read(0,out,1)==CB_NOR_IO);
 reset();status=CART_ST_ABORTED;assert(gb_verify204_read(0,out,1)==CB_NOR_IO);
 puts("PASS GB204 mapping/boundaries/faults/lifecycle/GBA rejection");
 return 0;
}
