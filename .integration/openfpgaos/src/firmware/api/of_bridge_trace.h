#ifndef OF_BRIDGE_TRACE_H
#define OF_BRIDGE_TRACE_H
#include <stdint.h>
#include "of_error.h"
#include "of_syscall.h"
#include "of_syscall_numbers.h"
#define OF_BRIDGE_TRACE_MAGIC 0x46474236u
#define OF_BRIDGE_TRACE_MAX_EVENTS 256u
enum of_bridge_trace_word {
    OF_BRIDGE_TRACE_ID = 0,
    OF_BRIDGE_TRACE_FREEZE = 1,
    OF_BRIDGE_TRACE_COUNT = 2,
    OF_BRIDGE_TRACE_DATA = 3,
    OF_BRIDGE_TRACE_LEGACY = 4
};
/* Kernel-limited diagnostic interface. FREEZE locks only the recorder;
 * DATA advances its local cursor. Neither issues any APF file command.
 * Snapshot once, then paginate the application's ordinary RAM copy. */
static inline int of_bridge_trace_read(unsigned word, uint32_t *out) {
    if (!out || word > OF_BRIDGE_TRACE_LEGACY) return OF_ERR_INVALID_PARAM;
#ifdef OF_PC
    *out = 0;
    return OF_ERR_NOT_SUPPORTED;
#else
    struct of_sbiret r = of_ecall1(OF_EID_BRIDGE_TRACE,
                                  OF_BRIDGE_TRACE_FID_READ, word);
    if (r.error) return (int)r.error;
    *out = (uint32_t)r.value;
    return OF_OK;
#endif
}
#endif
