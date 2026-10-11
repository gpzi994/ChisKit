#ifndef CHISBURNER_DUMP57_H
#define CHISBURNER_DUMP57_H
#include <string.h>
#include "reference.h"
#include "of_cart.h"
#include "../gba_dump53/md5.h"
#include "../gba_ident56/identify56.h"
#include "../storage_stream52/gate.h"

#ifndef DUMP57_BLOCK_READBACK
#define DUMP57_BLOCK_READBACK 1
#endif

enum stage57 {
    IDENT57, BOOT57, CAPS57, SAMPLE57, BIND57, CART57, WRITE57,
    LOCAL57, VERIFY57, DONE57
};
enum kind57 { UNKNOWN57, ZERO57, ROM57 };

struct ops57 {
    int (*caps)(of_cart_caps_t *);
    int (*cart)(uint32_t,uint32_t,uint8_t *);
    int (*safe)(void);
    int (*abort)(void);
    int (*bind)(void);
    int (*write)(uint32_t,const void *,uint32_t);
    int (*read)(uint32_t,uint8_t *,uint32_t);
    uint32_t (*now)(void);
    void (*progress)(enum stage57,unsigned);
    /* No cartridge write/save/erase/voltage control or output FLUSH. */
};

struct scan57 {
    int rc;
    enum kind57 kind;
    unsigned bytes, blocks;
    uint32_t ms, crc32, db_crc32;
    uint8_t md5[16];
};
struct times57 {
    uint32_t cart, write, readback, hash, safe, verify;
};
struct job57 {
    int rc, safe_rc;
    enum stage57 stage;
    unsigned bytes, blocks, samples, stop;
    uint32_t ms, lifecycle, cart_crc32, cart_db_crc32;
    uint8_t cart_md5[16];
    struct scan57 verify;
    struct times57 time;
};

static uint8_t payload57[CHUNK57] __attribute__((aligned(64)));
#if DUMP57_BLOCK_READBACK
static uint8_t compare57[CHUNK57] __attribute__((aligned(64)));
#endif

static uint32_t accounted57(const struct times57 *t) {
    return t->cart + t->write + t->readback + t->hash + t->safe + t->verify;
}
static int caps_valid57(const of_cart_caps_t *c) {
    return c->struct_size == sizeof(*c) && c->version == OF_CART_CAPS_VERSION &&
        c->bus_mask == OF_CART_BUS_MASK_GBA &&
        c->operation_mask == (OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
                              OF_CART_OP_ABORT | OF_CART_OP_QUERY) &&
        c->max_read_bytes == 32 && !c->writes_enabled &&
        !c->gb_gbc_space_mask &&
        c->gba_space_mask == (OF_CART_SPACE_MASK_ROM | OF_CART_SPACE_MASK_IDENTIFY) &&
        (c->lifecycle_status & 0x80000307u) == 0x80000303u;
}

static struct scan57 scan57(const struct ops57 *o, enum stage57 phase) {
    struct scan57 s = {0};
    struct md5_53 md; md5_init53(&md);
    unsigned nonzero = 0;
    uint32_t crc = ~0u, dbcrc = ~0u, begin = o->now();
    for (unsigned off = 0; off < TOTAL57; off += CHUNK57) {
        if (!(s.blocks & 7u)) o->progress(phase, s.blocks);
        if ((uint32_t)(o->now() - begin) >= 420000u) {
            s.rc = OF_ERR_TIMEOUT; break;
        }
        unsigned n = length57(off);
        s.rc = o->read(off, payload57, n);
        if (s.rc) break;
        for (unsigned i = 0; i < n; ++i) nonzero |= payload57[i];
        md5_add53(&md, payload57, n);
        crc = crc32_update57(crc, payload57, n);
        if (off < DB_PREFIX57) dbcrc = crc32_update57(dbcrc, payload57, n);
        s.bytes += n; ++s.blocks;
    }
    md5_end53(&md, s.md5);
    s.crc32 = ~crc; s.db_crc32 = ~dbcrc; s.ms = o->now() - begin;
    if (!s.rc && s.bytes == TOTAL57) {
        if (!nonzero) s.kind = ZERO57;
        else if (!memcmp(s.md5, expected57, 16) &&
                 s.crc32 == SOURCE_FULL_CRC57 &&
                 s.db_crc32 == SOURCE_PREFIX_CRC57)
            s.kind = ROM57;
        else s.rc = OF_ERR_FAILED;
    }
    o->progress(phase, s.blocks);
    return s;
}

static struct job57 run57(struct guard51 *gate, const struct scan57 *boot,
                          const struct ops57 *o) {
    struct job57 r = {0};
    r.rc = OF_ERR_DENIED; r.stage = CAPS57; r.stop = TOTAL57;
    if (!boot || boot->rc || boot->kind != ZERO57 ||
        boot->bytes != TOTAL57 || boot->blocks != BLOCKS57 ||
        take51(gate) != A51) return r;
    uint32_t begin = o->now(), t, crc = ~0u, dbcrc = ~0u;
    of_cart_caps_t c = {0};
    struct md5_53 md; md5_init53(&md);
    r.rc = o->caps(&c); r.lifecycle = c.lifecycle_status;
    if (r.rc || !caps_valid57(&c)) { if (!r.rc) r.rc = OF_ERR_DENIED; goto stop; }
    t = o->now(); r.rc = o->safe(); r.time.safe += o->now() - t;
    if (r.rc) goto stop;
    r.stage = SAMPLE57; o->progress(r.stage, 0);
    for (unsigned i = 0; i < 5; ++i) {
        r.stop = sample_off57[i]; t = o->now();
        r.rc = o->cart(OF_CART_SPACE_ROM, r.stop, payload57);
        r.time.cart += o->now() - t;
        if (r.rc) goto stop;
        if (crc57(payload57, 32) != sample_crc57[i]) {
            r.rc = OF_ERR_FAILED; goto stop;
        }
        ++r.samples;
    }
    t = o->now(); r.rc = o->safe(); r.time.safe += o->now() - t;
    if (r.rc) goto stop;
    r.stage = BIND57; o->progress(r.stage, 0); r.rc = o->bind();
    if (r.rc) goto stop;
    for (unsigned off = 0; off < TOTAL57; off += CHUNK57) {
        unsigned n = length57(off); r.stage = CART57; r.stop = off;
        if (!(r.blocks & 7u)) o->progress(r.stage, r.blocks);
        if ((uint32_t)(o->now() - begin) >= 1800000u) {
            r.rc = OF_ERR_TIMEOUT; goto stop;
        }
        t = o->now();
        for (unsigned j = 0; j < n; j += 32) {
            r.stop = off + j;
            r.rc = o->cart(OF_CART_SPACE_ROM, off + j, payload57 + j);
            if (r.rc) break;
        }
        r.time.cart += o->now() - t;
        if (r.rc) goto stop;
        t = o->now(); r.rc = o->safe(); r.time.safe += o->now() - t;
        if (r.rc) goto stop;
        t = o->now();
        md5_add53(&md, payload57, n);
        crc = crc32_update57(crc, payload57, n);
        if (off < DB_PREFIX57) dbcrc = crc32_update57(dbcrc, payload57, n);
        r.time.hash += o->now() - t;
        r.stop = off; r.stage = WRITE57; t = o->now();
        r.rc = o->write(off, payload57, n); r.time.write += o->now() - t;
        if (r.rc) goto stop;
#if DUMP57_BLOCK_READBACK
        r.stage = LOCAL57; t = o->now();
        r.rc = o->read(off, compare57, n); r.time.readback += o->now() - t;
        if (r.rc) goto stop;
        t = o->now();
        for (unsigned j = 0; j < n; ++j)
            if (payload57[j] != compare57[j]) {
                r.stop = off + j; r.rc = OF_ERR_FAILED; break;
            }
        r.time.hash += o->now() - t;
        if (r.rc) goto stop;
#endif
        r.bytes += n; ++r.blocks;
    }
    t = o->now(); md5_end53(&md, r.cart_md5);
    r.cart_crc32 = ~crc; r.cart_db_crc32 = ~dbcrc;
    r.time.hash += o->now() - t;
    if (memcmp(r.cart_md5, expected57, 16) ||
        r.cart_crc32 != SOURCE_FULL_CRC57 ||
        r.cart_db_crc32 != SOURCE_PREFIX_CRC57) {
        r.stage = CART57; r.rc = OF_ERR_FAILED; goto stop;
    }
    r.stage = VERIFY57; t = o->now();
    r.verify = scan57(o, VERIFY57); r.time.verify += o->now() - t;
    r.rc = r.verify.rc;
    if (r.rc || r.verify.kind != ROM57 ||
        r.verify.crc32 != r.cart_crc32 ||
        r.verify.db_crc32 != r.cart_db_crc32 ||
        memcmp(r.verify.md5, r.cart_md5, 16)) {
        if (!r.rc) r.rc = OF_ERR_FAILED;
        r.stop = r.verify.bytes; goto stop;
    }
    r.stage = DONE57; r.stop = TOTAL57; o->progress(r.stage, r.blocks);
stop:
    if (r.rc) o->abort();
    t = o->now(); r.safe_rc = o->safe(); r.time.safe += o->now() - t;
    if (!r.rc && r.safe_rc) r.rc = r.safe_rc;
    r.ms = o->now() - begin;
    return r;
}
#endif
