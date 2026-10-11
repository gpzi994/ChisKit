#ifndef STATIC50_SEQUENCE_H
#define STATIC50_SEQUENCE_H
#include "pattern.h"
#include "of_error.h"
enum step50 { S_BIND, S_WRITE, S_READ, S_FLUSH, S_DONE };
struct result50 {
    unsigned mode, attempted, mismatches, first_bad;
    int bind_rc,write_rc,read_rc,flush_rc,rc;
};
struct ops50 {
    int (*bind)(void);
    int (*write)(uint32_t,const void *,uint32_t);
    int (*read)(uint8_t *);
    int (*flush)(void);
    void (*progress)(enum step50);
};
/* Persistent buffers also make late completion after a timeout safe. */
static uint8_t write50[STATIC50_BYTES] __attribute__((aligned(64)));
static uint8_t read50[STATIC50_BYTES] __attribute__((aligned(64)));
static struct result50 sequence50(const struct ops50 *o,unsigned mode) {
    struct result50 r={0}; r.mode=mode; r.first_bad=STATIC50_BYTES;
    r.bind_rc=r.write_rc=r.read_rc=r.flush_rc=OF_ERR_INVALID_STATE;
    r.rc=OF_ERR_INVALID_PARAM;
    if(mode!=P_A && mode!=P_B) return r;
    o->progress(S_BIND); r.attempted|=1;
    r.rc=r.bind_rc=o->bind(); if(r.rc) return r;
    pattern_fill(write50,mode);
    o->progress(S_WRITE); r.attempted|=2;
    r.rc=r.write_rc=o->write(0,write50,STATIC50_BYTES); if(r.rc) return r;
    o->progress(S_READ); r.attempted|=4;
    r.rc=r.read_rc=o->read(read50); if(r.rc) return r;
    r.mismatches=pattern_diff(read50,mode,&r.first_bad);
    if(r.mismatches) {r.rc=OF_ERR_FAILED;return r;}
    if(mode==P_B) {
        o->progress(S_FLUSH); r.attempted|=8;
        r.rc=r.flush_rc=o->flush(); if(r.rc) return r;
    }
    o->progress(S_DONE); r.rc=0;return r;
}
#endif
