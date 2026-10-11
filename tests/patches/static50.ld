/* Exact smoke31 symbol addresses, preserved by the smoke47/49 patches. */
OUTPUT_ARCH(riscv)
ENTRY(static50_service)
of_file_async_busy = 0x10328100;
of_file_size = 0x103297c0;
output_is_open = 0x1038e530;
output_open_size = 0x1038e534;
output_open_slot = 0x1038e538;
SECTIONS {
  . = 0x1032f180;
  .patch : { *(.text.entry) *(.text*) *(.rodata*) }
  ASSERT(SIZEOF(.patch) <= 0x280, "static bind exceeds original service")
  /DISCARD/ : { *(.comment) *(.riscv.attributes) *(.eh_frame*) }
}
