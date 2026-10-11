#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "of_error.h"
#define CART_UI98 1
#define DMA_TIMEOUT 1000
#define DMA_CHUNK_SIZE 32768u
#define OF_TARGET_CRAM0_DMA_CHUNK_SIZE 65536u
#define DS_STATUS_ACK 1u
#define DS_STATUS_DONE 2u
#define DS_STATUS_ERR_MASK 0x1cu
#define DS_STATUS_ERR_SHIFT 2
#define DS_STATUS_READY 32u
#define DS_STATUS_WR_IDLE 64u
#define DS_STATUS_IRQ_PENDING 128u
#define DS_BWC_CRAM0_OVERRUN (1u<<31)
#define DS_CMD_OPENFILE 0x192
struct Status {uint32_t value=99;operator uint32_t()const{return value;}void operator=(uint32_t v){value&=~(v&128u);}};
static Status DS_STATUS;
static unsigned file_op_count,DS_BRIDGE_WCNT,CRAM0_MODE,DS_SLOT_ID,DS_PARAM_ADDR,SAVE_DT_WORD,SAVE_DT_SIZE;
static uint8_t scratch[65536];
#define CRAM0_SCRATCH ((uintptr_t)scratch)
#define CRAM0_SCRATCH_BRIDGE 0x1234u
#define CRAM0_MODE_CPU 1
#define CRAM0_MODE_BRIDGE 2
static void call_idle_hook(){}
#define of_term_printf(...) ((void)0)
static void fence(){}
static int async_gate(){return 0;}
static int of_file_datatable_entry_for_slot(unsigned slot,unsigned*e){assert(slot==5);*e=6;return 0;}
static unsigned commands,probe_result=3,create_result=1,expected_size=0x2000000;
static int ds_issue_command(unsigned c){
 assert(c==0x192&&DS_SLOT_ID==5&&DS_PARAM_ADDR==0x1234);
 assert(!strcmp((char*)scratch,"/Saves/chisburner/common/cb101-test.gba"));
 if(commands++==0){for(int i=256;i<264;i++)assert(!scratch[i]);DS_STATUS.value=99|(probe_result<<2)|128;}
 else{assert(commands==2&&scratch[256]==0&&scratch[259]==3);for(unsigned i=0;i<4;i++)assert(scratch[260+i]==((expected_size>>(24-8*i))&255));DS_STATUS.value=99|(create_result<<2)|128;}
 return 0;
}
#define of_file_slot_write_at mock_slot_write_at
static unsigned write_commands;
static int mock_slot_write_at(unsigned slot,uint32_t offset,
                              uintptr_t bridge,uint32_t length){
 assert(slot==5&&bridge==0x1234u&&length==32768u);
 assert(offset==write_commands*32768u);
 ++write_commands;return 0;
}
#include "../build/ui98-output-body.inc"
static void reset(unsigned probe,unsigned create){commands=write_commands=0;expected_size=0x2000000;probe_result=probe;create_result=create;output_is_open=1;DS_STATUS.value=99;SAVE_DT_SIZE=0;}
int main(){
 const char*p="/Saves/chisburner/common/cb101-test.gba";
 reset(0,1);assert(of_file_output_open(5,p,0x2000000)==OF_ERR_DENIED&&commands==1&&!output_is_open&&!SAVE_DT_SIZE);
 reset(3,1);assert(!of_file_output_open(5,p,0x2000000)&&commands==2&&output_is_open&&SAVE_DT_WORD==13&&SAVE_DT_SIZE==0x2000000);
 uint8_t source[65536];memset(source,0x5a,sizeof(source));
 assert(!of_file_output_write(5,0,source,sizeof(source))&&write_commands==2);
 reset(3,1);expected_size=32768;
 assert(!of_file_output_open(5,p,32768)&&SAVE_DT_SIZE==32768);
 assert(!of_file_output_write(5,0,source,32768)&&write_commands==1);
 assert(of_file_output_write(5,32768,source,1)==OF_ERR_BAD_RANGE&&write_commands==1);
 reset(3,0);assert(of_file_output_open(5,p,0x2000000)==OF_ERR_DENIED&&!output_is_open&&!SAVE_DT_SIZE);
 reset(5,1);assert(of_file_output_open(5,p,0x2000000)!=0&&commands==1&&!output_is_open);
 reset(3,5);assert(of_file_output_open(5,p,0x2000000)!=0&&commands==2&&!output_is_open);
 std::puts("PASS: output open, 64 KiB split writes, 32 KiB ROM-only file and end-boundary guard");
}
