#define CART_SRAM137 1
#define CART_SRAM138 1
#define main old_nor_test_main
#include "test-nor108-os.c"
#undef main
#include <sys/mman.h>
int main(void){
 uint8_t *dst=mmap((void*)0x11000000u,32768,PROT_READ|PROT_WRITE,
  MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);
 assert(dst!=MAP_FAILED);sram_cap=0x1001f;
 CART_LIFECYCLE=CART_LC_READY;CART_STATUS=5;
 /* Actual OS adapter: all 64 KiB including 0x8000, never any bank writes. */
 unsigned initial_writes=raw_writes;
 for(unsigned off=0;off<65536;off+=32){
  assert(!flash165_read(0x40000000u|off,dst,32,50000));
  for(unsigned i=0;i<32;i++)assert(dst[i]==(uint8_t)((off+i)*31u+(off+i)/257u));
 }
 assert(raw_writes==initial_writes&&!save_bank&&!busy&&!private_owned);
 assert(sram137_read(32768,dst,32,50000)==CB_NOR_ARGUMENT);
 assert(flash165_read(0x4000ffffu,dst,2,50000)==CB_NOR_ARGUMENT);
 assert(flash165_read(0x40010000u,dst,1,50000)==CB_NOR_ARGUMENT);
 assert(flash165_read(0xc0000000u,dst,32,50000)==CB_NOR_ARGUMENT);
 assert(flash165_read(0x40000000u,(void*)0x20000000u,32,50000)==CB_NOR_ARGUMENT);
 assert(flash165_read(0x40000000u,(void*)0x30000000u,32,50000)==CB_NOR_ARGUMENT);
 assert(raw_writes==initial_writes);
 sram_reads=0;
 /* Exercise the actual maximum request size, not just short reads. */
 for(unsigned off=0;off<131072;off+=32768){
  assert(!sram138_read(SRAM138_BANKED|off,dst,32768,10000000));
  for(unsigned i=0;i<32768;i++){
   unsigned a=(off+i)&65535u;
   assert(dst[i]==(uint8_t)(a*31u+a/257u+((off+i)>>16)*97u));
  }
  assert(!save_bank&&!private_owned&&!busy&&CART_SPACE==0&&!job.authorized&&!job_token);
 }
 assert(raw_writes==bank_writes&&sram_reads==131072);
 unsigned before=bank_writes;
 assert(!sram138_read(0,dst,32,1000000)&&bank_writes==before);
 assert(!sram138_read(SRAM138_BANKED|65520,dst,32,1000000));
 for(unsigned i=0;i<32;i++){
  unsigned a=(65520+i)&65535u;
  assert(dst[i]==(uint8_t)(a*31u+a/257u+((65520+i)>>16)*97u));
 }
 assert(sram138_read(SRAM138_BANKED|131071,dst,2,1000000)==CB_NOR_ARGUMENT);
 assert(sram138_read(65536,dst,1,1000000)==CB_NOR_ARGUMENT);
 assert(sram138_read(0,dst,32769,1000000)==CB_NOR_ARGUMENT);
 assert(sram138_read(0,(void*)0x20000000u,32,1000000)==CB_NOR_ARGUMENT);
 assert(sram138_read(0,(void*)0x30000000u,32,1000000)==CB_NOR_ARGUMENT);
 before=bank_writes;sram_cap=0x1000f;
 assert(sram138_read(SRAM138_BANKED,dst,32,1000000)==CB_NOR_UNSUPPORTED);
 assert(bank_writes==before);sram_cap=0x1001f;
 for(unsigned i=1;i<=32;i++){
  sram_fault=sram_reads+i;
  assert(sram138_read(SRAM138_BANKED|65536,dst,32,1000000)==CB_NOR_IO);
  assert(!save_bank&&!private_owned&&!busy&&CART_SPACE==0);
 }
 sram_fault=0;bank_fault=bank_writes+1;
 assert(sram138_read(SRAM138_BANKED|65536,dst,32,1000000)==CB_NOR_IO);
 assert(!save_bank);bank_fault=bank_writes+2;
 assert(sram138_read(SRAM138_BANKED|65536,dst,32,1000000)==CB_NOR_IO); // cleanup failure is not PASS
 bank_fault=0;before=bank_writes;save_epoch_fault=1;
 assert(sram138_read(SRAM138_BANKED|65536,dst,32,1000000)!=0);
 assert(bank_writes==before+1); // no recovery write to changed lifecycle
 assert(!private_owned&&!busy&&CART_SPACE==0);
 before=bank_writes;CART_LIFECYCLE=0;
 assert(sram138_read(SRAM138_BANKED,dst,32,1000000)==CB_NOR_CANCELLED);
 assert(bank_writes==before);CART_LIFECYCLE=CART_LC_READY;
 assert(!sram138_read(SRAM138_BANKED,dst,32,1000000));
 assert(sram138_read(SRAM138_BANKED|65536,dst,32,1000)==CB_NOR_TIMEOUT);
 assert(!save_bank&&!private_owned&&!busy&&raw_writes==bank_writes);
 munmap(dst,32768);
 puts("SRAM138 OS PASS: 128 KiB, bank boundary, explicit opt-in, SDRAM-only, legacy rejection, read/select/restore/lifecycle/timeout faults; bank latch writes only");
}
