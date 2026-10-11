#define CART_SRAM137 1
#define main old_nor_test_main
#include "test-nor108-os.c"
#undef main
#include <sys/mman.h>
int main(void){
 uint8_t *dst=mmap((void*)0x11000000u,4096,PROT_READ|PROT_WRITE,
  MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);
 assert(dst!=MAP_FAILED);
 CART_LIFECYCLE=CART_LC_READY;CART_STATUS=5;
 for(unsigned off=0;off<32768;off+=32){
  assert(!sram137_read(off,dst,32,1000000));
  for(unsigned i=0;i<32;i++)assert(dst[i]==(uint8_t)((off+i)*31u+(off+i)/257u));
  assert(!private_owned&&!busy&&CART_SPACE==0&&!job.authorized&&!job_token);
 }
 assert(!raw_writes&&sram_reads==32768);
 assert(sram137_read(32767,dst,2,50000)==CB_NOR_ARGUMENT);
 assert(sram137_read(0,dst,33,50000)==CB_NOR_ARGUMENT);
 assert(sram137_read(0,(void*)0x20000000u,32,50000)==CB_NOR_ARGUMENT);
 assert(sram137_read(0,(void*)0x30000000u,32,50000)==CB_NOR_ARGUMENT);
 assert(sram137_read(0,(void*)0x13ffffffu,32,50000)==CB_NOR_ARGUMENT);
 sram_cap=0x10007;assert(sram137_read(0,dst,32,1000000)==CB_NOR_UNSUPPORTED);sram_cap=0x1000f;
 for(unsigned i=1;i<=32;i++){
  sram_fault=sram_reads+i;assert(sram137_read(0,dst,32,1000000)==CB_NOR_IO);
  assert(!private_owned&&!busy&&CART_SPACE==0);
 }
 sram_fault=0;epoch_read_fault=1;assert(sram137_read(0,dst,32,1000000)==CB_NOR_IO);
 CART_LIFECYCLE=0;assert(sram137_read(0,dst,32,1000000)==CB_NOR_CANCELLED);
 CART_LIFECYCLE=CART_LC_READY;
 assert(sram137_read(0,dst,32,1000)==CB_NOR_TIMEOUT);
 assert(!private_owned&&!busy&&CART_SPACE==0&&!raw_writes);
 munmap(dst,4096);
 puts("SRAM137 OS PASS: 32 KiB bytes, PSRAM rejection, old FPGA rejection, 32 IO failures, lifecycle and timeout cleanup; no writes");
}
