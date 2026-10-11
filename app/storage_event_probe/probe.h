#ifndef STORAGE_EVENT_PROBE_H
#define STORAGE_EVENT_PROBE_H
#include "of_bridge_trace.h"
struct bridge_event { uint32_t meta, address, data, current; };
struct bridge_snapshot {
    int rc;
    uint32_t count_flags, legacy, count;
    struct bridge_event events[OF_BRIDGE_TRACE_MAX_EVENTS];
};
static inline int read_snapshot(struct bridge_snapshot *s,
        int (*read_word)(unsigned,uint32_t *)) {
    uint32_t ready=0;
    s->count=0;
    for(unsigned i=0;i<10000 && !(ready&1);++i) {
        s->rc=read_word(OF_BRIDGE_TRACE_FREEZE,&ready);
        if(s->rc) return s->rc;
    }
    if(!(ready&1)) return s->rc=OF_ERR_TIMEOUT;
    s->rc=read_word(OF_BRIDGE_TRACE_COUNT,&s->count_flags);
    if(s->rc) return s->rc;
    s->count=s->count_flags&0xffffu;
    if(s->count>OF_BRIDGE_TRACE_MAX_EVENTS) return s->rc=OF_ERR_FAILED;
    s->rc=read_word(OF_BRIDGE_TRACE_LEGACY,&s->legacy);
    if(s->rc) return s->rc;
    for(uint32_t i=0;i<s->count;++i) {
        uint32_t words[4];
        for(unsigned j=0;j<4;++j) {
            s->rc=read_word(OF_BRIDGE_TRACE_DATA,&words[j]);
            if(s->rc) return s->rc;
        }
        s->events[i].meta=words[0];
        s->events[i].address=words[1];
        s->events[i].data=words[2];
        s->events[i].current=words[3];
    }
    return s->rc=0;
}
#endif
