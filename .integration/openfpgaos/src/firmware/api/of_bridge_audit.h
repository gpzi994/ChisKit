#ifndef OF_BRIDGE_AUDIT_H
#define OF_BRIDGE_AUDIT_H
#include "of_bridge_trace.h"
#define OF_BRIDGE_AUDIT_MAGIC 0x46474237u
#define OF_BRIDGE_AUDIT_STREAM_MAGIC 0x41554437u
#define OF_BRIDGE_AUDIT_HEADER_WORDS 72u
/* Uses the unchanged smoke46 bounded diagnostic syscall. On audit hardware,
 * ID also arms the recorder once per cold boot; COUNT bit31 acknowledges it.
 * OPEN/FLUSH execute normally; freeze only AFTER the file test has returned.
 * All counters refer to FPGA observations, never proven host receipt. */
typedef int (*of_bridge_audit_reader)(unsigned,uint32_t *);
static inline int of_bridge_audit_begin_with(of_bridge_audit_reader read_word) {
    uint32_t v=0;
    int rc=read_word(OF_BRIDGE_TRACE_ID,&v);
    if(rc) return rc;
    if(v!=OF_BRIDGE_AUDIT_MAGIC) return OF_ERR_INVALID_STATE;
    for(unsigned i=0;i<10000;++i) {
        rc=read_word(OF_BRIDGE_TRACE_COUNT,&v);
        if(rc) return rc;
        if(v&0x80000000u) return 0;
    }
    return OF_ERR_TIMEOUT;
}
#endif
