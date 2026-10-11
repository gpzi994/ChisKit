/*
 * Source-guided fixed-layout patch for the proven smoke.31 os.bin.
 *
 * Hook location: of_file_output_open success path, file offset 0xF392.
 * Cave location: the function-alignment padding at file offset 0xF3EC.
 *
 * s2 still holds the requested output size at this point.  Slot IDs 0..4
 * are declared densely in data.json, so slot 4 occupies datatable entry 4;
 * its size word is word 9.  The two stores mirror:
 *
 *     SAVE_DT_WORD = 9;
 *     SAVE_DT_SIZE = size;
 *
 * The final LUI recreates the one instruction displaced by the hook before
 * control returns to the unmodified smoke.31 success epilogue.
 */

    .section .text
    .option rvc
    .global patch_start
patch_start:
    c.j     patch_cave
    c.nop

    /* 0xF3EC - 0xF396 = 0x56 bytes after the four-byte hook. */
    .space  86, 0

patch_cave:
    lui     a5, 0x40000
    li      a4, 9
    sw      a4, 200(a5)
    sw      s2, 76(a5)
    lui     a5, 0x1038e
    c.j     patch_start + 4
