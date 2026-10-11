#ifndef STREAM52_H
#define STREAM52_H
#include <string.h>
#include "of_error.h"
#include "gate.h"
#include "pattern.h"
enum stage52 { ST52_BOOT,ST52_BIND,ST52_WRITE,ST52_VERIFY,ST52_DONE };
struct ops52 {
    int (*bind)(void);
    int (*write)(uint32_t,const void *,uint32_t);
    int (*read)(uint32_t,uint8_t *);
    uint32_t (*now)(void);
    void (*progress)(enum stage52,unsigned);
    /* Intentionally NO flush method. This test cannot invoke it. */
};
struct scan52 {
    int rc;
    unsigned kind,blocks,bytes,bad_initial,bad_data,first_initial,first_data;
    uint32_t crc,ms;
    uint8_t head[8];
};
struct job52 {
    int rc,bind_rc,write_rc;
    enum stage52 stage;
    unsigned write_done,failed_offset;
    uint32_t write_ms,total_ms;
    struct scan52 verify;
};
static uint8_t payload52[CHUNK52] __attribute__((aligned(64)));
static int valid_boot52(const struct scan52 *s) {
    return s && !s->rc && s->blocks==BLOCKS52 && s->bytes==TOTAL52 &&
        ((s->kind==INITIAL52 && !s->bad_initial && s->crc==CRC_INITIAL52) ||
         (s->kind==DATA52 && !s->bad_data && s->crc==CRC_DATA52));
}
static struct scan52 scan_file52(const struct ops52 *o,unsigned expect,enum stage52 phase) {
    struct scan52 s={0};s.first_initial=s.first_data=TOTAL52;
    uint32_t crc=0xffffffffu,begin=o->now();
    for(unsigned b=0;b<BLOCKS52;++b) {
        if((b&7u)==0)o->progress(phase,b);
        if((uint32_t)(o->now()-begin)>=120000u){s.rc=OF_ERR_TIMEOUT;break;}
        unsigned off=b*CHUNK52;
        s.rc=o->read(off,payload52);if(s.rc)break;
        if(!b)memcpy(s.head,payload52,sizeof(s.head));
        for(unsigned j=0;j<CHUNK52;++j) {
            if(payload52[j]!=byte52(off+j,INITIAL52)) {
                if(!s.bad_initial)s.first_initial=off+j;
                ++s.bad_initial;
            }
            if(payload52[j]!=byte52(off+j,DATA52)) {
                if(!s.bad_data)s.first_data=off+j;
                ++s.bad_data;
            }
        }
        crc=crc52(crc,payload52,CHUNK52);++s.blocks;s.bytes+=CHUNK52;
        /* No more reads after a verification mismatch. Boot classification
         * scans all blocks so a mixed/partial file can never enable writes. */
        if(expect==DATA52 && s.bad_data){s.rc=OF_ERR_FAILED;break;}
    }
    s.crc=~crc;s.ms=o->now()-begin;
    if(!s.rc && s.bytes==TOTAL52) {
        if(!s.bad_initial && s.crc==CRC_INITIAL52)s.kind=INITIAL52;
        else if(!s.bad_data && s.crc==CRC_DATA52)s.kind=DATA52;
        else s.rc=OF_ERR_FAILED;
    }
    if(!s.rc && expect && s.kind!=expect)s.rc=OF_ERR_FAILED;
    o->progress(phase,s.blocks);return s;
}
static struct job52 run_stream52(struct guard51 *g,const struct scan52 *boot,const struct ops52 *o) {
    struct job52 r={0};r.rc=OF_ERR_DENIED;r.stage=ST52_BIND;
    r.bind_rc=r.write_rc=r.verify.rc=OF_ERR_INVALID_STATE;r.failed_offset=TOTAL52;
    if(!valid_boot52(boot) || boot->kind!=INITIAL52 || take51(g)!=A51)return r;
    uint32_t begin=o->now();o->progress(ST52_BIND,0);
    r.rc=r.bind_rc=o->bind();if(r.rc)goto done;
    r.stage=ST52_WRITE;uint32_t write_begin=o->now();
    for(unsigned b=0;b<BLOCKS52;++b) {
        if((b&7u)==0)o->progress(ST52_WRITE,b);
        r.failed_offset=b*CHUNK52;
        if((uint32_t)(o->now()-write_begin)>=120000u){r.rc=OF_ERR_TIMEOUT;break;}
        fill52(payload52,b*CHUNK52,CHUNK52,DATA52);
        r.rc=r.write_rc=o->write(b*CHUNK52,payload52,CHUNK52);
        if(r.rc)break;
        ++r.write_done;
    }
    r.write_ms=o->now()-write_begin;
    if(r.rc)goto done;
    r.stage=ST52_VERIFY;r.verify=scan_file52(o,DATA52,ST52_VERIFY);r.rc=r.verify.rc;
    if(r.rc){r.failed_offset=r.verify.bad_data?r.verify.first_data:r.verify.bytes;goto done;}
    r.stage=ST52_DONE;r.failed_offset=TOTAL52;o->progress(ST52_DONE,BLOCKS52);
done:
    r.total_ms=o->now()-begin;return r;
}
#endif
