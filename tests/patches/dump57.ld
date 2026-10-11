OUTPUT_ARCH(riscv)
ENTRY(read57_entry)
SECTIONS {
 .caps57 0x1032e200 : { *(.caps57) }
 ASSERT(SIZEOF(.caps57)<=0x100,"caps57 overflow")
 .entry57 0x1032e340 : { *(.entry57) }
 ASSERT(SIZEOF(.entry57)<=0x40,"read57 entry overflow")
 .space57 0x1032e3c0 : { *(.space57) *(.space57.rodata) }
 ASSERT(SIZEOF(.space57)<=0x40,"space57 entry overflow")
 .patch57 0x1032f180 : { *(.patch57.first) *(.patch57) *(.text*) *(.rodata*) *(.srodata*) }
 ASSERT(SIZEOF(.patch57)<=0x280,"dump57 service region overflow")
 PROVIDE(of_file_async_busy = 0x10328100);
 PROVIDE(of_file_size = 0x103297c0);
 PROVIDE(output_is_open = 0x1038e530);
 PROVIDE(output_open_size = 0x1038e534);
 PROVIDE(output_open_slot = 0x1038e538);
 /DISCARD/ : { *(.comment) *(.riscv.attributes) *(.eh_frame*) }
}
