/* SPDX-License-Identifier: Apache-2.0 */
/* Included by file.c only for CHISBURNER_SDRAM122. Not a production API.
 * Call at boot, before apps/idle hooks/async jobs. Failure requires cold boot.
 * NOLOAD buffers are never touched through their cached aliases, including
 * BSS initialization. Do not move them into ordinary BSS or use memcpy. */
#include "file122.h"
#include "hal/timer.h"
#if CHISBURNER_SDRAM123_FIXTURE
#include "fixture123.h"
#endif
/* 64-byte prefix keeps the appended OABI/OSE2/CRC metadata and its cache
 * line outside the two DMA buffers even when initialized data ends nearby. */
static uint8_t sd122_buffers[65536+64]
 __attribute__((section(".sd122_nocache"),aligned(64),used));
static int sd122_failed,sd122_busy;
static uint32_t sd122_read(void *ctx,uint32_t off) {
 (void)ctx; return REG32(0x40000200u+off);
}
static void sd122_write(void *ctx,uint32_t off,uint32_t value) {
 (void)ctx; __asm__ volatile("fence" ::: "memory");
 REG32(0x40000200u+off)=value;
 __asm__ volatile("fence" ::: "memory");
}
static int sd122_transfer(void *ctx,int write,uint32_t slot,uint32_t off,
 uint32_t address,uint32_t bytes) {
 (void)ctx;
 return write ? of_file_slot_write_at(slot,off,address,bytes)
              : of_file_read_raw(slot,off,address,bytes);
}
__attribute__((used,externally_visible))
int of_file_sdram122_roundtrip(uint32_t input,uint32_t output,
 const char *new_path,uint32_t bytes,uint32_t *verified) {
 if(!verified) return OF_ERR_BAD_RANGE;
 *verified=0;
 if(sd122_failed||sd122_busy||async_state.active||async_state.copy_pending||
    idle_hook||output_is_open) return OF_ERR_BUSY;
 if(!bytes||bytes>0x02000000u||input==output||!new_path||
    of_file_size(input)<(long)bytes) return OF_ERR_BAD_RANGE;
 const struct cb_sd122_io io={{0,sd122_read,sd122_write},sd122_transfer};
 /* No output creation on an old bitstream. */
 if(sd122_read(0,0)!=CB_SD116_SIGNATURE) return OF_ERR_INVALID_STATE;
#if CHISBURNER_SDRAM124
 sd122_write(0,28,3);
 if(sd122_read(0,28)!=0x53443124u) return OF_ERR_INVALID_STATE;
#endif
 sd122_busy=1;
 const char *stage="Create output";
 uint32_t position=0,phase=0,input_checked=0;
 of_term_puts("Create new output...\n");
 int rc=of_file_output_open(output,new_path,bytes);
 if(rc) goto fail;
 for(uint32_t off=0;off<bytes;) {
  uint32_t n=bytes-off; if(n>32768u) n=32768u;
  uint32_t a=(uint32_t)(uintptr_t)&sd122_buffers[64];
  position=off; stage="TF -> SDRAM";
  of_term_printf("Read/write %u + %u\n",(unsigned)off,(unsigned)n);
  rc=cb_sd122_transfer_trace(&io,0,input,off,a,n,2000000u,&phase); if(rc) goto fail;
#if CHISBURNER_SDRAM123_FIXTURE
  stage="Input pattern";
  volatile uint8_t *source=(volatile uint8_t *)(uintptr_t)(a+0x40000000u);
  for(uint32_t i=0;i<n;i++) if(source[i]!=cb_sd123_byte(off+i)) {
   position=off+i; rc=OF_ERR_IO; goto fail;
  }
  input_checked+=n;
#endif
  stage="SDRAM -> TF";
  rc=cb_sd122_transfer_trace(&io,1,output,off,a,n,2000000u,&phase); if(rc) goto fail;
  off+=n;
 }
 /* Pocket hardware does not acknowledge target command 0x0188 reliably.
  * Use the field-proven dump contract instead: every 0x0184 write completes,
  * then the host serves a full independent reread of the open output slot.
  * This is the same commit/verification path used by production backups. */
 stage="Host reread output"; of_term_puts("Host reread output...\n");
 for(uint32_t off=0;off<bytes;) {
  uint32_t n=bytes-off; if(n>32768u) n=32768u;
  uint32_t a=(uint32_t)(uintptr_t)&sd122_buffers[64];
  uint32_t b=(uint32_t)(uintptr_t)&sd122_buffers[64+32768];
  volatile uint8_t *p=(volatile uint8_t *)(uintptr_t)(a+0x40000000u);
  volatile uint8_t *q=(volatile uint8_t *)(uintptr_t)(b+0x40000000u);
  position=off; stage="Verify source read";
  of_term_printf("Verify %u + %u\n",(unsigned)off,(unsigned)n);
  rc=cb_sd122_transfer_trace(&io,0,input,off,a,n,2000000u,&phase); if(rc) goto fail;
  stage="Verify output reread";
  rc=cb_sd122_transfer_trace(&io,0,output,off,b,n,2000000u,&phase); if(rc) goto fail;
  __asm__ volatile("fence" ::: "memory");
  stage="Compare bytes";
  for(uint32_t i=0;i<n;i++) if(p[i]!=q[i]
#if CHISBURNER_SDRAM123_FIXTURE
     ||q[i]!=cb_sd123_byte(off+i)
#endif
  ) {position=off+i;rc=OF_ERR_IO;goto fail;}
  off+=n; *verified=off;
 }
 sd122_busy=0; return 0;
fail:
 of_term_printf("STOP %s @ %u\n",stage,(unsigned)position);
 {
  static const char *const names[]={"Validate","Acquire","Prepare snapshot",
   "Unlock for APF","APF transfer","Reacquire","Release snapshot",
   "Final unlock","Transfer done"};
  of_term_printf("Phase %u: %s\n",(unsigned)phase,phase<=8?names[phase]:"Unknown");
 }
 of_term_printf("Input checked %u bytes\n",(unsigned)input_checked);
 of_term_printf("Source %08x size %u\n",(unsigned)sd122_read(0,8),
   (unsigned)sd122_read(0,12));
 of_term_printf("ctl %08x lock %08x DS %08x\n",
   (unsigned)sd122_read(0,4),(unsigned)sd122_read(0,24),(unsigned)DS_STATUS);
 /* Do not reset an outstanding APF/AXI operation or overwrite on retry. */
#if CHISBURNER_SDRAM124
 /* Only select read-only diagnostic pages; never reset/unlock the engine. */
 of_timer_delay_ms(1);
 sd122_write(0,28,0);uint32_t cause=sd122_read(0,28);
 sd122_write(0,28,1);uint32_t bad_addr=sd122_read(0,28);
 sd122_write(0,28,2);uint32_t reads=sd122_read(0,28);
 of_term_printf("First %08x addr %08x\n",(unsigned)cause,(unsigned)bad_addr);
 of_term_printf("Bridge reads %u\n",(unsigned)reads);
#endif
 sd122_failed=1; sd122_busy=0; return rc;
}
