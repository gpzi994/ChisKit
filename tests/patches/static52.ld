/* Pinned original smoke31 ABI, preserved in the smoke51 base image. */
OUTPUT_ARCH(riscv)
ENTRY(static52_service)
of_file_async_busy = 0x10328100;
of_file_size = 0x103297c0;
output_is_open = 0x1038e530;
output_open_size = 0x1038e534;
output_open_slot = 0x1038e538;
SECTIONS {
  . = 0x1032f180;
  .patch : { *(.text.entry) *(.text*) *(.rodata*) }
  ASSERT(SIZEOF(.patch) <= 0x280, "static52 bind exceeds original service")
  /DISCARD/ : { *(.comment) *(.riscv.attributes) *(.eh_frame*) }
}
