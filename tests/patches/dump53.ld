OUTPUT_ARCH(riscv)
ENTRY(bind53)
of_file_async_busy = 0x10328100;
of_file_size = 0x103297c0;
output_is_open = 0x1038e530;
output_open_size = 0x1038e534;
output_open_slot = 0x1038e538;
SECTIONS {
 .caps53 0x1032e200 : { *(.caps53) }
 ASSERT(SIZEOF(.caps53)<=0x100,"caps53 overflow")
 .entry53 0x1032e340 : { *(.entry53) *(.rodata*) *(.srodata*) }
 ASSERT(SIZEOF(.entry53)<=0x40,"read entry overflow")
 .space53 0x1032e3c0 : { *(.space53) }
 ASSERT(SIZEOF(.space53)<=0x40,"space entry overflow")
 .patch53 0x1032f180 : { *(.bind53) *(.read53) *(.text*) }
 ASSERT(SIZEOF(.patch53)<=0x280,"bind/read region overflow")
 /DISCARD/ : { *(.comment) *(.riscv.attributes) *(.eh_frame*) }
}
