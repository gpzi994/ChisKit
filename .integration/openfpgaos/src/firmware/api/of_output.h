//------------------------------------------------------------------------------
// SPDX-License-Identifier: Apache-2.0
// SPDX-FileType: SOURCE
// SPDX-FileCopyrightText: (c) 2026, ChisBurner contributors
//------------------------------------------------------------------------------

/*
 * of_output.h -- policy-gated dynamic output file API
 *
 * Applications do not issue Pocket target commands or use bridge/MMIO
 * addresses.  The OS stages each block, serializes the data-slot engine and
 * explicitly flushes the output file.
 */

#ifndef OF_OUTPUT_H
#define OF_OUTPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "of_error.h"
#include "of_services.h"

#define OF_OUTPUT_MAX_SIZE (32u * 1024u * 1024u)
/* output_open normally returns 0 for a new file. SAVE158-enabled Pocket OS
 * returns +1 when reopening a same-size GBA *-backup.sav for rotating backup.
 * Such callers must compare/update and readback-verify before cartridge write.
 * All other existing filenames remain protected; negative values are errors. */

#ifndef OF_PC

#define OF_OUTPUT_SVC_INDEX(field) \
    ((uint32_t)((offsetof(struct of_services_table, field) - \
                 offsetof(struct of_services_table, video_init)) / sizeof(void *)))

#define OF_OUTPUT_SVC_HAS(field) \
    (OF_SVC && OF_SVC->magic == OF_SVC_MAGIC && \
     OF_SVC->count > OF_OUTPUT_SVC_INDEX(field) && OF_SVC->field)

static inline int of_output_available(void) {
    return OF_OUTPUT_SVC_HAS(output_open) &&
           OF_OUTPUT_SVC_HAS(output_write) &&
           OF_OUTPUT_SVC_HAS(output_flush);
}

static inline int of_output_open(const char *path, uint32_t size) {
    if (!OF_OUTPUT_SVC_HAS(output_open))
        return OF_ERR_NOT_SUPPORTED;
    return OF_SVC->output_open(path, size);
}

static inline int of_output_write(uint32_t offset, const void *src,
                                  uint32_t length) {
    if (!OF_OUTPUT_SVC_HAS(output_write))
        return OF_ERR_NOT_SUPPORTED;
    return OF_SVC->output_write(offset, src, length);
}

static inline int of_output_flush(void) {
    if (!OF_OUTPUT_SVC_HAS(output_flush))
        return OF_ERR_NOT_SUPPORTED;
    return OF_SVC->output_flush();
}

#else

static inline int of_output_available(void) { return 0; }
static inline int of_output_open(const char *path, uint32_t size) {
    (void)path;
    (void)size;
    return OF_ERR_NOT_SUPPORTED;
}
static inline int of_output_write(uint32_t offset, const void *src,
                                  uint32_t length) {
    (void)offset;
    (void)src;
    (void)length;
    return OF_ERR_NOT_SUPPORTED;
}
static inline int of_output_flush(void) { return OF_ERR_NOT_SUPPORTED; }

#endif

#ifdef __cplusplus
}
#endif

#endif /* OF_OUTPUT_H */
