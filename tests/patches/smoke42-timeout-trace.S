/* Source-guided, fixed-layout replacement for file_wait_complete_mask's
 * ACK/DONE timeout print arguments ONLY. s3 is DS_STATUS (0x4000003c).
 * Read the dedicated DS_TRACE register at 0x400001f0, and do not mask its
 * high bits. Poll loops, return code, stack and all code addresses unchanged.
 * Apply word 0 to OS offsets 0x933c/0x9356 and word 1 to 0x934c/0x9366.
 * The existing st=%02x format prints all eight digits (2 is a minimum width).
 */
.option norvc
.text
    lw a2, 436(s3)
    nop
