#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include "of_error.h"
#define CART_UI98 1
#define CART_SAVE158 1
#define CHISBURNER_PSRAM127 1
#define DMA_CHUNK_SIZE 32768u
#define DS_STATUS_ERR_MASK 0x1cu
#define DS_STATUS_ERR_SHIFT 2
#define DS_CMD_OPENFILE 0x192
static uint8_t dma_memory[65536];
#define scratch dma_memory
#define OF_TARGET_SDRAM_FILE_SCRATCH_ADDR ((uintptr_t)::dma_memory)
static unsigned DS_STATUS,SAVE_DT_WORD,SAVE_DT_SIZE;
static unsigned commands,probe=3,creates=1,writes;
static long existing_size=32768;
static int transport_error;
static const char *expected_path;
static int async_gate(){return 0;}
static void fence(){}
static long of_file_size(unsigned s){assert(s==5);return existing_size;}
static int of_file_datatable_entry_for_slot(unsigned s,unsigned *e){assert(s==5);*e=6;return 0;}
static int sdram127_outbound_command(unsigned cmd,unsigned slot,uintptr_t addr,unsigned len,unsigned mask){
 assert(cmd==DS_CMD_OPENFILE&&slot==5&&addr==(uintptr_t)scratch&&len==264);
 assert(!strcmp((char*)scratch,expected_path));
 unsigned result=commands++?creates:probe;
 if(commands==1){assert(mask==9);for(unsigned i=256;i<264;i++)assert(!scratch[i]);}
 else assert(mask==2&&scratch[259]==3);
 DS_STATUS=result<<DS_STATUS_ERR_SHIFT;return transport_error;
}
static int of_file_slot_write_at(unsigned s,unsigned off,uintptr_t addr,unsigned len){
 assert(s==5&&off==0&&addr==(uintptr_t)scratch&&len==32768);writes++;return 0;
}
#include "../build/save160-output-body.inc"
static void reset(const char *p,unsigned result){
 expected_path=p;probe=result;commands=writes=0;output_is_open=0;
 existing_size=32768;transport_error=0;SAVE_DT_SIZE=0;
}
int main(){
 for(unsigned n : {512u,8192u,32768u,131072u}){
  const char *e="/Saves/chisburner/common/GBA-EEPROM-TEST-backup.sav";
  reset(e,3);existing_size=n;
  assert(of_file_output_open(5,e,n)==0&&commands==2&&output_is_open);
  reset(e,0);existing_size=n;
  assert(of_file_output_open(5,e,n)==1&&commands==1&&output_is_open&&!SAVE_DT_SIZE);
  reset(e,0);existing_size=n+1;
  assert(of_file_output_open(5,e,n)==OF_ERR_BAD_RANGE&&!output_is_open);
  const char *ordinary="/Saves/chisburner/common/GBA-EEPROM-TEST.sav";
  reset(ordinary,0);existing_size=n;
  assert(of_file_output_open(5,ordinary,n)==OF_ERR_DENIED&&!output_is_open);
 }
 const char *p="/Saves/chisburner/common/GBA-TEST-CODE-32KiB-backup.sav";
 reset(p,3);assert(of_file_output_open(5,p,32768)==0&&commands==2&&output_is_open);
 reset(p,0);assert(of_file_output_open(5,p,32768)==1&&commands==1&&output_is_open&&!SAVE_DT_SIZE);
 uint8_t data[32768]={0};assert(!of_file_output_write(5,0,data,sizeof data)&&writes==1);
 assert(of_file_output_write(5,32768,data,1)==OF_ERR_BAD_RANGE&&writes==1);
 reset(p,0);existing_size=131072;assert(of_file_output_open(5,p,32768)<0&&!output_is_open);
 reset(p,0);transport_error=-12;assert(of_file_output_open(5,p,32768)==-12&&!output_is_open);
 const char *q="/Saves/chisburner/common/GBA-TEST-123.sav";
 reset(q,0);assert(of_file_output_open(5,q,32768)==OF_ERR_DENIED&&!output_is_open);
 q="/Saves/chisburner/common/GB-TEST-backup.sav";
#if CART_GB_RESTORE180
 reset(q,0);assert(of_file_output_open(5,q,32768)==1&&output_is_open&&commands==1);
 for(unsigned n : {
#if CART_GB_RESTORE185
 256u,
#endif
 512u,8192u,32768u,65536u,131072u}){
  reset(q,3);existing_size=n;assert(!of_file_output_open(5,q,n)&&commands==2);
  reset(q,0);existing_size=n;assert(of_file_output_open(5,q,n)==1&&commands==1&&!SAVE_DT_SIZE);
  reset(q,0);existing_size=n+1;assert(of_file_output_open(5,q,n)==OF_ERR_BAD_RANGE&&!output_is_open);
 }
 for(unsigned n : {0u,
#if !CART_GB_RESTORE185
 256u,
#endif
 511u,513u,2048u,16384u,262144u}){
  reset(q,0);existing_size=n;assert(of_file_output_open(5,q,n)==OF_ERR_DENIED&&!output_is_open);
 }
 for(const char *bad : {"/Saves/chisburner/common/GB-ZELDA.sav",
  "/Saves/chisburner/common/GB-ZELDA-backup.sav.extra",
  "/Saves/chisburner/common/GB--backup.sav",
  "/Saves/chisburner/common/GB-../ZELDA-backup.sav",
  "/Saves/chisburner/common/GB-X/ZELDA-backup.sav",
  "/Saves/chisburner/common/GB-X\\ZELDA-backup.sav",
  "/Assets/chisburner/common/GB-ZELDA-backup.sav"}){
  reset(bad,0);assert(of_file_output_open(5,bad,32768)==OF_ERR_DENIED&&!output_is_open);
 }
 puts("PASS GB180 actual output-open body: existing GB backup returns +1, exact sizes, invalid names denied");
#else
 reset(q,0);assert(of_file_output_open(5,q,32768)==OF_ERR_DENIED&&!output_is_open);
#endif
 reset(p,0);assert(of_file_output_open(5,p,65536)==OF_ERR_DENIED&&!output_is_open);
 puts("PASS rotating backup: create, reopen without truncate, size and ordinary-file protection, I/O failure");
}
