#ifndef DUMP54_H
#define DUMP54_H
#include "reference.h"
#include "../gba_dump53/md5.h"
#include "of_cart.h"
#include "../storage_stream52/gate.h"
enum stage54 {BOOT54,CAPS54,SAMPLE54,BIND54,CART54,WRITE54,LOCAL54,VERIFY54,DONE54};
enum kind54 {UNKNOWN54,ZERO54,ROM54};
struct ops54 {
    int (*caps)(of_cart_caps_t *);int (*cart)(uint32_t,uint8_t *);
    int (*safe)(void);int (*abort)(void);int (*bind)(void);
    int (*write)(uint32_t,const void *,uint32_t);int (*read)(uint32_t,uint8_t *,uint32_t);
    uint32_t (*now)(void);void (*progress)(enum stage54,unsigned);
    /* No cartridge write, save space, erase, voltage or FLUSH operation. */
};
struct scan54 {int rc;enum kind54 kind;unsigned bytes,blocks;uint32_t ms;uint8_t md5[16];};
/* Wall time in disjoint scopes, milliseconds. No per-32-byte timer calls.
 * Verify includes TF reads + its MD5/UI. Other is the total remainder.
 * Boot scan is outside the dump total, like smoke53's reported 431 seconds. */
struct times54 {uint32_t cart,write,readback,hash,safe,verify;};
struct job54 {
    int rc,safe_rc;enum stage54 stage;unsigned bytes,blocks,samples,stop;
    uint32_t ms,lifecycle;uint8_t cart_md5[16];struct scan54 verify;
    struct times54 time;
};
static uint32_t accounted54(const struct times54 *t){return t->cart+t->write+t->readback+t->hash+t->safe+t->verify;}
static uint8_t payload54[CHUNK54] __attribute__((aligned(64)));
static uint8_t compare54[CHUNK54] __attribute__((aligned(64)));
static int caps_valid54(const of_cart_caps_t *c){
    return c->struct_size==sizeof(*c) && c->version==4 && c->bus_mask==2 && c->operation_mask==15 &&
        c->max_read_bytes==32 && !c->writes_enabled && !c->gb_gbc_space_mask && c->gba_space_mask==1 &&
        (c->lifecycle_status&0x80000307u)==0x80000303u;
}
static struct scan54 scan54(const struct ops54 *o,enum stage54 phase){
    struct scan54 s={0};struct md5_53 md;md5_init53(&md);unsigned nonzero=0;uint32_t begin=o->now();
    for(unsigned off=0;off<TOTAL54;off+=CHUNK54){
        if(!(s.blocks&7))o->progress(phase,s.blocks);
        if((uint32_t)(o->now()-begin)>=180000u){s.rc=OF_ERR_TIMEOUT;break;}
        unsigned n=length54(off);s.rc=o->read(off,payload54,n);if(s.rc)break;
        for(unsigned i=0;i<n;++i)nonzero|=payload54[i];
        md5_add53(&md,payload54,n);s.bytes+=n;++s.blocks;
    }
    md5_end53(&md,s.md5);s.ms=o->now()-begin;
    if(!s.rc && s.bytes==TOTAL54){
        if(!nonzero)s.kind=ZERO54;
        else if(!memcmp(s.md5,expected54,16))s.kind=ROM54;
        else s.rc=OF_ERR_FAILED;
    }
    o->progress(phase,s.blocks);return s;
}
static struct job54 run54(struct guard51 *gate,const struct scan54 *boot,const struct ops54 *o){
    struct job54 r={0};r.rc=OF_ERR_DENIED;r.stage=CAPS54;r.stop=TOTAL54;
    if(!boot || boot->rc || boot->kind!=ZERO54 || boot->bytes!=TOTAL54 || boot->blocks!=BLOCKS54 || take51(gate)!=A51)return r;
    uint32_t begin=o->now(),t;of_cart_caps_t c={0};struct md5_53 md;md5_init53(&md);
    r.rc=o->caps(&c);r.lifecycle=c.lifecycle_status;if(r.rc)goto stop;
    if(!caps_valid54(&c)){r.rc=OF_ERR_DENIED;goto stop;}
    t=o->now();r.rc=o->safe();r.time.safe+=o->now()-t;if(r.rc)goto stop;
    r.stage=SAMPLE54;o->progress(r.stage,0);
    for(unsigned i=0;i<5;++i){
        r.stop=sample_off54[i];t=o->now();r.rc=o->cart(r.stop,payload54);r.time.cart+=o->now()-t;if(r.rc)goto stop;
        if(crc54(payload54,32)!=sample_crc54[i]){r.rc=OF_ERR_FAILED;goto stop;}++r.samples;
    }
    t=o->now();r.rc=o->safe();r.time.safe+=o->now()-t;if(r.rc)goto stop;
    r.stage=BIND54;o->progress(r.stage,0);r.rc=o->bind();if(r.rc)goto stop;
    for(unsigned off=0;off<TOTAL54;off+=CHUNK54){
        unsigned n=length54(off);r.stage=CART54;r.stop=off;
        if(!(r.blocks&7))o->progress(r.stage,r.blocks);
        if((uint32_t)(o->now()-begin)>=900000u){r.rc=OF_ERR_TIMEOUT;goto stop;}
        t=o->now();
        for(unsigned j=0;j<n;j+=32){r.stop=off+j;r.rc=o->cart(off+j,payload54+j);if(r.rc)break;}
        r.time.cart+=o->now()-t;if(r.rc)goto stop;
        t=o->now();r.rc=o->safe();r.time.safe+=o->now()-t;if(r.rc)goto stop;
        t=o->now();md5_add53(&md,payload54,n);r.time.hash+=o->now()-t;r.stop=off;
        r.stage=WRITE54;t=o->now();r.rc=o->write(off,payload54,n);r.time.write+=o->now()-t;if(r.rc)goto stop;
        r.stage=LOCAL54;t=o->now();r.rc=o->read(off,compare54,n);r.time.readback+=o->now()-t;if(r.rc)goto stop;
        t=o->now();
        for(unsigned j=0;j<n;++j)if(payload54[j]!=compare54[j]){r.stop=off+j;r.rc=OF_ERR_FAILED;break;}
        r.time.hash+=o->now()-t;if(r.rc)goto stop;
        r.bytes+=n;++r.blocks;
    }
    t=o->now();md5_end53(&md,r.cart_md5);r.time.hash+=o->now()-t;
    if(memcmp(r.cart_md5,expected54,16)){r.stage=CART54;r.rc=OF_ERR_FAILED;goto stop;}
    r.stage=VERIFY54;t=o->now();r.verify=scan54(o,VERIFY54);r.time.verify+=o->now()-t;r.rc=r.verify.rc;
    if(r.rc || r.verify.kind!=ROM54){if(!r.rc)r.rc=OF_ERR_FAILED;r.stop=r.verify.bytes;goto stop;}
    r.stage=DONE54;r.stop=TOTAL54;o->progress(r.stage,r.blocks);
stop:
    if(r.rc)o->abort();
    t=o->now();r.safe_rc=o->safe();r.time.safe+=o->now()-t;if(!r.rc && r.safe_rc)r.rc=r.safe_rc;
    r.ms=o->now()-begin;return r;
}
#endif
