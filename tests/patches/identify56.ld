OUTPUT_ARCH(riscv)
ENTRY(read56)
SECTIONS {
 .caps56 0x1032e200 : { *(.caps56) }
 ASSERT(SIZEOF(.caps56)<=0x100,"caps56 overflow")
 .entry56 0x1032e340 : { *(.entry56) }
 ASSERT(SIZEOF(.entry56)<=0x40,"read entry overflow")
 .space56 0x1032e3c0 : { *(.space56) }
 ASSERT(SIZEOF(.space56)<=0x40,"space entry overflow")
 .patch56 0x1032f180 : { *(.patch56.first) *(.patch56) *(.text*) *(.rodata*) *(.srodata*) }
 ASSERT(SIZEOF(.patch56)<=0x280,"identify service region overflow")
 /DISCARD/ : { *(.comment) *(.riscv.attributes) *(.eh_frame*) }
}
