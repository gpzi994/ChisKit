#ifndef STORAGE_SNAPSHOT_FORENSICS_H
#define STORAGE_SNAPSHOT_FORENSICS_H
#include <string.h>
#include "of_bridge_audit.h"

#define FORENSIC_MAX_WORDS (OF_BRIDGE_AUDIT_HEADER_WORDS + 4u*OF_BRIDGE_TRACE_MAX_EVENTS)
#define FORENSIC_NO_WORD 0xffffffffu
enum forensic_stage {
    FS_NONE, FS_FREEZE, FS_COUNT, FS_COUNT_CHECK, FS_LEGACY,
    FS_HEADER, FS_PAYLOAD, FS_HEADER_CHECK, FS_DONE
};
struct forensic_snapshot {
    int rc, api_rc;
    enum forensic_stage stage;
    unsigned selector, calls, freeze_polls;
    unsigned flags_read, legacy_read, count_valid, header_checked;
    unsigned words_read, expected_words, bad_mask;
    uint32_t failure_word, first_bad, ready, flags, legacy;
    uint32_t raw[FORENSIC_MAX_WORDS];
};
static inline const char *forensic_stage_name(enum forensic_stage stage) {
    switch(stage) {
    case FS_FREEZE:return "FREEZE";
    case FS_COUNT:return "COUNT";
    case FS_COUNT_CHECK:return "COUNT-CHECK";
    case FS_LEGACY:return "LEGACY";
    case FS_HEADER:return "HEADER-READ";
    case FS_PAYLOAD:return "PAYLOAD-READ";
    case FS_HEADER_CHECK:return "HEADER-CHECK";
    case FS_DONE:return "DONE";
    default:return "NONE";
    }
}
static inline int forensic_get(struct forensic_snapshot *s,
        of_bridge_audit_reader reader,unsigned selector,uint32_t *out) {
    uint32_t value=0;
    s->selector=selector;
    ++s->calls;
    s->api_rc=reader(selector,&value);
    if(s->api_rc) { s->rc=s->api_rc; return s->rc; }
    *out=value; /* An unsuccessful pop may still advance hardware: never retry. */
    return OF_OK;
}
static inline unsigned forensic_header_bad(const struct forensic_snapshot *s) {
    unsigned bad=0;
    if(s->raw[0]!=OF_BRIDGE_AUDIT_STREAM_MAGIC) bad|=1u<<0;
    if(s->raw[1]!=1) bad|=1u<<1;
    if(s->raw[2]!=OF_BRIDGE_AUDIT_HEADER_WORDS) bad|=1u<<2;
    if(s->raw[3]!=(s->flags&0xffffu)) bad|=1u<<3;
    if(s->raw[5]!=(s->flags&0xffff0000u)) bad|=1u<<5;
    if(s->raw[6]>2) bad|=1u<<6;
    /* H7 is reserved and shown raw; use exactly smoke48's six checks. */
    return bad;
}
static inline unsigned forensic_raw_groups(const struct forensic_snapshot *s) {
    return s->words_read>OF_BRIDGE_AUDIT_HEADER_WORDS ?
           (s->words_read-OF_BRIDGE_AUDIT_HEADER_WORDS)/4u : 0;
}
static inline int forensic_header_valid(const struct forensic_snapshot *s) {
    return s->header_checked && !s->bad_mask;
}
static inline int forensic_snapshot_read(struct forensic_snapshot *s,
                                         of_bridge_audit_reader reader) {
    if(!s) return OF_ERR_INVALID_PARAM;
    memset(s,0,sizeof(*s));
    s->failure_word=s->first_bad=FORENSIC_NO_WORD;
    if(!reader) return s->rc=OF_ERR_INVALID_PARAM;
    s->stage=FS_FREEZE;
    do {
        ++s->freeze_polls;
        if(forensic_get(s,reader,OF_BRIDGE_TRACE_FREEZE,&s->ready)) return s->rc;
    } while(!(s->ready&1u) && s->freeze_polls<10000u);
    if(!(s->ready&1u)) return s->rc=OF_ERR_TIMEOUT;
    s->stage=FS_COUNT;
    if(forensic_get(s,reader,OF_BRIDGE_TRACE_COUNT,&s->flags)) return s->rc;
    s->flags_read=1;
    s->stage=FS_COUNT_CHECK;
    unsigned count=s->flags&0xffffu;
    if(count>OF_BRIDGE_TRACE_MAX_EVENTS || (s->flags&0x80000000u))
        return s->rc=OF_ERR_FAILED;
    s->count_valid=1;
    s->expected_words=OF_BRIDGE_AUDIT_HEADER_WORDS+4u*count;
    s->stage=FS_LEGACY;
    if(forensic_get(s,reader,OF_BRIDGE_TRACE_LEGACY,&s->legacy)) return s->rc;
    s->legacy_read=1;
    /* COUNT, not a possibly corrupt header, bounds the read. Keep raw payload
     * on a header mismatch, but never interpret it as verified events/stats.
     * There is exactly one DATA call per word, no rewind or resynchronization. */
    for(unsigned i=0;i<s->expected_words;++i) {
        s->stage=i<OF_BRIDGE_AUDIT_HEADER_WORDS ? FS_HEADER : FS_PAYLOAD;
        if(forensic_get(s,reader,OF_BRIDGE_TRACE_DATA,&s->raw[i])) {
            s->failure_word=i;
            return s->rc;
        }
        s->words_read=i+1;
        if(s->words_read==OF_BRIDGE_AUDIT_HEADER_WORDS) {
            s->header_checked=1;
            s->bad_mask=forensic_header_bad(s);
            for(unsigned j=0;j<8;++j) if(s->bad_mask&(1u<<j)) {
                s->first_bad=j; break;
            }
        }
    }
    s->stage=s->bad_mask ? FS_HEADER_CHECK : FS_DONE;
    return s->rc=s->bad_mask ? OF_ERR_FAILED : OF_OK;
}
#endif
