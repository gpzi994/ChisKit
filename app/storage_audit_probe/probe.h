#ifndef STORAGE_AUDIT_PROBE_H
#define STORAGE_AUDIT_PROBE_H
#include "of_bridge_audit.h"
struct audit_event { uint32_t word[4]; };
struct audit_snapshot {
    int rc;
    uint32_t count, flags, legacy, header[OF_BRIDGE_AUDIT_HEADER_WORDS];
    struct audit_event event[OF_BRIDGE_TRACE_MAX_EVENTS];
};
static inline int audit_snapshot_read(struct audit_snapshot *s,of_bridge_audit_reader read_word) {
    uint32_t ready=0,flags=0;
    s->count=0;
    for(unsigned i=0;i<10000 && !(ready&1);++i) {
        s->rc=read_word(OF_BRIDGE_TRACE_FREEZE,&ready);
        if(s->rc) return s->rc;
    }
    if(!(ready&1)) return s->rc=OF_ERR_TIMEOUT;
    s->rc=read_word(OF_BRIDGE_TRACE_COUNT,&flags);
    if(s->rc) return s->rc;
    unsigned count=flags&65535;
    if(count>OF_BRIDGE_TRACE_MAX_EVENTS || (flags&0x80000000u)) return s->rc=OF_ERR_FAILED;
    s->flags=flags;
    s->rc=read_word(OF_BRIDGE_TRACE_LEGACY,&s->legacy);
    if(s->rc) return s->rc;
    for(unsigned i=0;i<OF_BRIDGE_AUDIT_HEADER_WORDS;++i) {
        s->rc=read_word(OF_BRIDGE_TRACE_DATA,&s->header[i]);
        if(s->rc) return s->rc;
    }
    if(s->header[0]!=OF_BRIDGE_AUDIT_STREAM_MAGIC || s->header[1]!=1 ||
       s->header[2]!=OF_BRIDGE_AUDIT_HEADER_WORDS || s->header[3]!=count ||
       s->header[5]!=(flags&0xffff0000u) || s->header[6]>2) return s->rc=OF_ERR_FAILED;
    for(unsigned i=0;i<count;++i) for(unsigned j=0;j<4;++j) {
        s->rc=read_word(OF_BRIDGE_TRACE_DATA,&s->event[i].word[j]);
        if(s->rc) return s->rc;
    }
    s->count=count;
    return s->rc=0;
}
#endif
