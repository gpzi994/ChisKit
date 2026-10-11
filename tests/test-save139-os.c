#define CART_SRAM137 1
#define CART_SRAM138 1
#define CART_SAVE139 1
#define main old_nor_test_main
#include "test-nor108-os.c"
#undef main
int main(void){
 struct cb_save139_flash_report r;
 CART_LIFECYCLE=CART_LC_READY;CART_STATUS=5;sram_cap=0x1003f;
 save139_data[4]=0x72;
 assert(!save139_probe_kernel(&r));
 assert(r.direct.state==CB_SAVE139_PROBE_SRAM_LIKE&&r.direct.restore_verified);
 assert(save139_data[4]==0x72&&save139_writes==2&&!r.id_read);
 assert(!busy&&!private_owned&&CART_SPACE==0&&!job.authorized&&!job_token);
 sram_cap=0x1007f;
 assert(!save139_probe_kernel(&r));
 assert(r.direct.restore_verified&&save139_data[4]==0x72);
 sram_cap=0x2007f;
 assert(save139_probe_kernel(&r)==CB_NOR_UNSUPPORTED);
 unsigned before=save139_writes;sram_cap=0x1001f;
 assert(save139_probe_kernel(&r)==CB_NOR_UNSUPPORTED&&save139_writes==before);
 sram_cap=0x1003f;
 for(unsigned i=1;i<=4;i++){
  sram_fault=sram_reads+i;assert(save139_probe_kernel(&r));
  assert(save139_data[4]==0x72&&!busy&&!private_owned&&CART_SPACE==0);
 }
 sram_fault=0;
 for(unsigned i=1;i<=2;i++){
  save139_fail_write=save139_writes+i;assert(save139_probe_kernel(&r));
  assert(save139_data[4]==0x72&&!busy&&!private_owned);
 }
 save139_fail_write=0;save_epoch_fault=1;before=save139_writes;
 assert(save139_probe_kernel(&r)&&save139_writes==before);
 assert(!busy&&!private_owned);
 busy=true;assert(save139_probe_kernel(&r)==CB_NOR_IO);busy=false;
 assert(!save139_probe_kernel(&r));
 puts("SAVE139 OS PASS: production adapter, SRAM restoration, old core rejection, read/write faults, lifecycle and lock cleanup");
}
