#ifndef DUMP53_H
#define DUMP53_H
#include "reference.h"
#include "md5.h"
#include "of_cart.h"
#include "../storage_stream52/gate.h"
enum stage53 {BOOT53,CAPS53,SAMPLE53,BIND53,CART53,WRITE53,LOCAL53,VERIFY53,DONE53};
enum kind53 {UNKNOWN53,ZERO53,ROM53};
struct ops53 {
    int (*caps)(of_cart_caps_t *);int (*cart)(uint32_t,uint8_t *);
    int (*safe)(void);int (*abort)(void);int (*bind)(void);
    int (*write)(uint32_t,const void *,uint32_t);int (*read)(uint32_t,uint8_t *,uint32_t);
    uint32_t (*now)(void);void (*progress)(enum stage53,unsigned);
    /* No cartridge write, save space, erase, voltage or FLUSH operation. */
};
struct scan53 {int rc;enum kind53 kind;unsigned bytes,blocks;uint32_t ms;uint8_t md5[16];};
struct job53 {
    int rc,safe_rc;enum stage53 stage;unsigned bytes,blocks,samples,stop;
    uint32_t ms,lifecycle;uint8_t cart_md5[16];struct scan53 verify;
};
static uint8_t payload53[CHUNK53] __attribute__((aligned(64)));
static uint8_t compare53[CHUNK53] __attribute__((aligned(64)));
static int caps_valid53(const of_cart_caps_t *c){
    return c->struct_size==sizeof(*c) && c->version==4 && c->bus_mask==2 && c->operation_mask==15 &&
        c->max_read_bytes==32 && !c->writes_enabled && !c->gb_gbc_space_mask && c->gba_space_mask==1 &&
        (c->lifecycle_status&0x80000307u)==0x80000303u;
}
static struct scan53 scan53(const struct ops53 *o,enum stage53 phase){
    struct scan53 s={0};struct md5_53 md;md5_init53(&md);unsigned nonzero=0;uint32_t begin=o->now();
    for(unsigned off=0;off<TOTAL53;off+=CHUNK53){
        if(!(s.blocks&7))o->progress(phase,s.blocks);
        if((uint32_t)(o->now()-begin)>=180000u){s.rc=OF_ERR_TIMEOUT;break;}
        unsigned n=length53(off);s.rc=o->read(off,payload53,n);if(s.rc)break;
        for(unsigned i=0;i<n;++i)nonzero|=payload53[i];
        md5_add53(&md,payload53,n);s.bytes+=n;++s.blocks;
    }
    md5_end53(&md,s.md5);s.ms=o->now()-begin;
    if(!s.rc && s.bytes==TOTAL53){
        if(!nonzero)s.kind=ZERO53;
        else if(!memcmp(s.md5,expected53,16))s.kind=ROM53;
        else s.rc=OF_ERR_FAILED;
    }
    o->progress(phase,s.blocks);return s;
}
static struct job53 run53(struct guard51 *gate,const struct scan53 *boot,const struct ops53 *o){
    struct job53 r={0};r.rc=OF_ERR_DENIED;r.stage=CAPS53;r.stop=TOTAL53;
    if(!boot || boot->rc || boot->kind!=ZERO53 || boot->bytes!=TOTAL53 || boot->blocks!=BLOCKS53 || take51(gate)!=A51)return r;
    uint32_t begin=o->now();of_cart_caps_t c={0};struct md5_53 md;md5_init53(&md);
    r.rc=o->caps(&c);r.lifecycle=c.lifecycle_status;if(r.rc)goto stop;
    if(!caps_valid53(&c)){r.rc=OF_ERR_DENIED;goto stop;}
    r.rc=o->safe();if(r.rc)goto stop;
    r.stage=SAMPLE53;o->progress(r.stage,0);
    for(unsigned i=0;i<5;++i){
        r.stop=sample_off53[i];r.rc=o->cart(r.stop,payload53);if(r.rc)goto stop;
        if(crc53(payload53,32)!=sample_crc53[i]){r.rc=OF_ERR_FAILED;goto stop;}++r.samples;
    }
    r.rc=o->safe();if(r.rc)goto stop;
    r.stage=BIND53;o->progress(r.stage,0);r.rc=o->bind();if(r.rc)goto stop;
    for(unsigned off=0;off<TOTAL53;off+=CHUNK53){
        unsigned n=length53(off);r.stage=CART53;r.stop=off;
        if(!(r.blocks&7))o->progress(r.stage,r.blocks);
        if((uint32_t)(o->now()-begin)>=900000u){r.rc=OF_ERR_TIMEOUT;goto stop;}
        for(unsigned j=0;j<n;j+=32){r.stop=off+j;r.rc=o->cart(off+j,payload53+j);if(r.rc)goto stop;}
        r.rc=o->safe();if(r.rc)goto stop;
        md5_add53(&md,payload53,n);r.stop=off;
        r.stage=WRITE53;r.rc=o->write(off,payload53,n);if(r.rc)goto stop;
        r.stage=LOCAL53;r.rc=o->read(off,compare53,n);if(r.rc)goto stop;
        for(unsigned j=0;j<n;++j)if(payload53[j]!=compare53[j]){r.stop=off+j;r.rc=OF_ERR_FAILED;goto stop;}
        r.bytes+=n;++r.blocks;
    }
    md5_end53(&md,r.cart_md5);
    if(memcmp(r.cart_md5,expected53,16)){r.stage=CART53;r.rc=OF_ERR_FAILED;goto stop;}
    r.stage=VERIFY53;r.verify=scan53(o,VERIFY53);r.rc=r.verify.rc;
    if(r.rc || r.verify.kind!=ROM53){if(!r.rc)r.rc=OF_ERR_FAILED;r.stop=r.verify.bytes;goto stop;}
    r.stage=DONE53;r.stop=TOTAL53;o->progress(r.stage,r.blocks);
stop:
    if(r.rc)o->abort();
    r.safe_rc=o->safe();if(!r.rc && r.safe_rc)r.rc=r.safe_rc;
    r.ms=o->now()-begin;return r;
}
#endif
