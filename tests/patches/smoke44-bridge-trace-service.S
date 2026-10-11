/* Fixed-layout implementation of OF_EID_BRIDGE_TRACE, FID_READ.
 * Base: exact smoke.44 os.bin. No service table/BSS/entrypoint relocation.
 * At 0x16B3C a7 is already EID-0xC0DE0000, a4=14, a6=FID, a0=index.
 * The original >14 branch is replaced by a jump to 0xF084, unused padding
 * after the unconditional tail jump at 0xF080. Normal EIDs resume untouched.
 * Valid debug calls return raw a1 with a0=0, including values with bit31 set.
 */
    .option norvc
    .section .text
    .global patch_start
patch_start:
    .org 0xF084
trace_service:
    bgeu a4, a7, legacy
    li a4, 15
    bne a7, a4, unsupported
    bnez a6, unsupported
    li a4, 4
    bltu a4, a0, unsupported
    lui a4, 0x40000
    slli a0, a0, 2
    add a4, a4, a0
    lw a1, 480(a4)
    li a0, 0
    j return_ok
legacy:
    j legacy_dispatch
unsupported:
    j return_unsupported
    .org 0x16B2A
return_ok:
    .org 0x16B3C
    j trace_service
legacy_dispatch:
    .org 0x16B52
return_unsupported:
    nop
