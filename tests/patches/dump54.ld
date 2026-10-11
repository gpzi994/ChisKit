OUTPUT_ARCH(riscv)
ENTRY(bind54)
of_file_async_busy = 0x10328100;
of_file_size = 0x103297c0;
output_is_open = 0x1038e530;
output_open_size = 0x1038e534;
output_open_slot = 0x1038e538;
SECTIONS {
 .caps54 0x1032e200 : { *(.caps54) }
 ASSERT(SIZEOF(.caps54)<=0x100,"caps54 overflow")
 .entry54 0x1032e340 : { *(.entry54) *(.rodata*) *(.srodata*) }
 ASSERT(SIZEOF(.entry54)<=0x40,"read entry overflow")
 .space54 0x1032e3c0 : { *(.space54) }
 ASSERT(SIZEOF(.space54)<=0x40,"space entry overflow")
 .patch54 0x1032f180 : { *(.bind54) *(.read54) *(.text*) }
 ASSERT(SIZEOF(.patch54)<=0x280,"bind/read region overflow")
 /DISCARD/ : { *(.comment) *(.riscv.attributes) *(.eh_frame*) }
}
