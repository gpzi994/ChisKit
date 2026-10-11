#ifndef SEQUENCE51_H
#define SEQUENCE51_H
#include "guard.h"
#include "of_error.h"
enum step51 { S51_BIND,S51_WRITE,S51_READ,S51_FLUSH,S51_DONE };
struct result51 {
    unsigned mode,attempted,mismatches,first_bad;
    int bind_rc,write_rc,read_rc,flush_rc,rc;
};
struct ops51 {
    int (*bind)(void);
    int (*write)(uint32_t,const void *,uint32_t);
    int (*read)(uint8_t *);
    int (*flush)(void);
    void (*progress)(enum step51);
};
static uint8_t write51[BYTES51] __attribute__((aligned(64)));
static uint8_t read51[BYTES51] __attribute__((aligned(64)));
static struct result51 run51(struct guard51 *g,const struct ops51 *o) {
    struct result51 r={0};r.first_bad=BYTES51;
    r.bind_rc=r.write_rc=r.read_rc=r.flush_rc=OF_ERR_INVALID_STATE;
    r.mode=take51(g);r.rc=OF_ERR_DENIED;
    if(!r.mode)return r; /* No service call before a consumed confirmation. */
    o->progress(S51_BIND);r.attempted|=1;
    r.rc=r.bind_rc=o->bind();if(r.rc)return r;
    fill51(write51,r.mode);
    o->progress(S51_WRITE);r.attempted|=2;
    r.rc=r.write_rc=o->write(0,write51,BYTES51);if(r.rc)return r;
    o->progress(S51_READ);r.attempted|=4;
    r.rc=r.read_rc=o->read(read51);if(r.rc)return r;
    r.mismatches=diff51(read51,r.mode,&r.first_bad);
    if(r.mismatches){r.rc=OF_ERR_FAILED;return r;}
    if(r.mode==B51) {
        o->progress(S51_FLUSH);r.attempted|=8;
        r.rc=r.flush_rc=o->flush();if(r.rc)return r;
    }
    o->progress(S51_DONE);r.rc=0;return r;
}
#endif
