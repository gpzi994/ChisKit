#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "dump57.h"

static uint8_t *rom, *disk;
static unsigned nw, nr, nc, nb, ns, na, maxend;
static int failwrite = -1, failread = -1, failcart = -1;
static int corrupt = -1, capbad, bindbad, safebad, wrongcart, verifybad;
static uint32_t ticks;
static enum stage57 phase;

static int cap57(of_cart_caps_t *c) {
    *c = (of_cart_caps_t){
        sizeof(*c), OF_CART_CAPS_VERSION, OF_CART_BUS_MASK_GBA,
        OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ | OF_CART_OP_ABORT | OF_CART_OP_QUERY,
        32, 0, 0x80000303, 0, 0, 0,
        OF_CART_SPACE_MASK_ROM | OF_CART_SPACE_MASK_IDENTIFY
    };
    if (capbad) c->writes_enabled = 1;
    return 0;
}
static int cart57(uint32_t space, uint32_t off, uint8_t *p) {
    assert(space == OF_CART_SPACE_ROM && !(off & 31u) && off <= TOTAL57 - 32);
    ++nc; ++ticks;
    if ((int)off == failcart) return OF_ERR_NOT_SUPPORTED;
    memcpy(p, rom + off, 32); if (wrongcart) p[0] ^= 1;
    return 0;
}
static int safe57(void) { ++ns; ticks += 3; return safebad ? OF_ERR_NOT_SUPPORTED : 0; }
static int abort57(void) { ++na; return 0; }
static int bind57_test(void) { ++nb; return bindbad ? OF_ERR_NOT_SUPPORTED : 0; }
static int write57_test(uint32_t off, const void *p, uint32_t n) {
    assert(!(off % CHUNK57) && off < TOTAL57 && n == length57(off));
    ++nw; ticks += 10;
    if ((int)(off / CHUNK57) == failwrite) return OF_ERR_IO;
    memcpy(disk + off, p, n); if (off + n > maxend) maxend = off + n;
    if ((int)(off / CHUNK57) == corrupt) disk[off + n - 1] ^= 1;
    return 0;
}
static int read57_test(uint32_t off, uint8_t *p, uint32_t n) {
    assert(!(off % CHUNK57) && off < TOTAL57 && n == length57(off));
    ++nr; ticks += 20;
    if ((int)(off / CHUNK57) == failread) return OF_ERR_IO;
    memcpy(p, disk + off, n);
    if (verifybad && phase == VERIFY57 && off + n == TOTAL57) p[n - 1] ^= 1;
    return 0;
}
static uint32_t now57_test(void) { return ticks; }
static void progress57_test(enum stage57 p, unsigned n) {
    assert(p <= DONE57 && n <= BLOCKS57); phase = p; ticks += 2;
}
static const struct ops57 ops57 = {
    cap57, cart57, safe57, abort57, bind57_test, write57_test,
    read57_test, now57_test, progress57_test
};
static struct guard51 armed57(void) {
    return (struct guard51){.state=G_DONE,.mode=A51,.confirmations=1};
}
static struct scan57 seed57(void) {
    return (struct scan57){.kind=ZERO57,.bytes=TOTAL57,.blocks=BLOCKS57};
}
static void reset57_test(void) {
    nw = nr = nc = nb = ns = na = maxend = ticks = 0;
    failwrite = failread = failcart = corrupt = -1;
    capbad = bindbad = safebad = wrongcart = verifybad = 0;
    phase = BOOT57; memset(disk, 0, TOTAL57);
}
static void file_hash57(uint8_t md5[16], uint32_t *crc, uint32_t *dbcrc) {
    struct md5_53 m; md5_init53(&m);
    uint32_t c = ~0u, d = ~0u;
    for (unsigned off = 0; off < TOTAL57; off += CHUNK57) {
        md5_add53(&m, rom + off, CHUNK57);
        c = crc32_update57(c, rom + off, CHUNK57);
        if (off < DB_PREFIX57) d = crc32_update57(d, rom + off, CHUNK57);
    }
    md5_end53(&m, md5); *crc = ~c; *dbcrc = ~d;
}

int main(int argc, char **argv) {
    (void)fill51; (void)identify51; (void)crc51; (void)gate_tick52;
    (void)accounted57; (void)md5_hex53;
    assert(argc == 2);
    rom = malloc(TOTAL57); disk = malloc(TOTAL57); assert(rom && disk);
    FILE *f = fopen(argv[1], "rb"); assert(f);
    assert(fread(rom, 1, TOTAL57, f) == TOTAL57 && fgetc(f) == EOF); fclose(f);
    uint8_t md5[16]; uint32_t crc, dbcrc; file_hash57(md5, &crc, &dbcrc);
    char md5_text[33]; md5_hex53(md5, md5_text);
    fprintf(stderr, "source md5=%s crc=%08x dbcrc=%08x\n", md5_text, crc, dbcrc);
    assert(!memcmp(md5, expected57, 16) && crc == SOURCE_FULL_CRC57 &&
           dbcrc == SOURCE_PREFIX_CRC57);
    for (unsigned i = 0; i < 5; ++i)
        assert(crc57(rom + sample_off57[i], 32) == sample_crc57[i]);

    reset57_test(); struct scan57 boot = scan57(&ops57, BOOT57);
    assert(!boot.rc && boot.kind == ZERO57 && nr == BLOCKS57 && !nw && !nc);
    struct guard51 gate = armed57(); struct job57 job = run57(&gate, &boot, &ops57);
    assert(!job.rc && job.stage == DONE57 && job.bytes == TOTAL57 &&
           job.blocks == BLOCKS57 && job.samples == 5);
    assert(nc == TOTAL57 / 32 + 5 && nw == BLOCKS57 && nr == BLOCKS57 * 3 &&
           nb == 1 && maxend == TOTAL57 && !memcmp(rom, disk, TOTAL57));
    assert(!memcmp(job.cart_md5, expected57, 16) &&
           !memcmp(job.verify.md5, expected57, 16));
    assert(job.cart_crc32 == crc && job.verify.crc32 == crc &&
           job.cart_db_crc32 == SOURCE_PREFIX_CRC57 &&
           job.verify.db_crc32 == SOURCE_PREFIX_CRC57);
    unsigned old = nw; assert(run57(&gate, &boot, &ops57).rc == OF_ERR_DENIED && nw == old);

    boot = scan57(&ops57, BOOT57);
    assert(!boot.rc && boot.kind == ROM57 && !memcmp(boot.md5, expected57, 16));
    gate = armed57(); old = nw;
    assert(run57(&gate, &boot, &ops57).rc == OF_ERR_DENIED && nw == old);
    disk[TOTAL57 - 1] ^= 1; boot = scan57(&ops57, BOOT57);
    assert(boot.rc && boot.kind == UNKNOWN57);

    reset57_test(); boot = seed57(); gate = armed57(); capbad = 1;
    job = run57(&gate, &boot, &ops57); assert(job.rc && !nc && !nw && !nb);
    reset57_test(); gate = armed57(); wrongcart = 1;
    job = run57(&gate, &boot, &ops57); assert(job.rc && nc == 1 && !nw && !nb);
    reset57_test(); gate = armed57(); bindbad = 1;
    job = run57(&gate, &boot, &ops57); assert(job.rc && nc == 5 && !nw);
    reset57_test(); gate = armed57(); safebad = 1;
    job = run57(&gate, &boot, &ops57); assert(job.rc && !nc && !nw);
    reset57_test(); gate = armed57(); failcart = 2 * CHUNK57;
    job = run57(&gate, &boot, &ops57); assert(job.rc && nw == 2 && na == 1);
    const int fault_blocks[] = {0, 511, 1023};
    for (unsigned fi = 0; fi < sizeof(fault_blocks) / sizeof(fault_blocks[0]); ++fi) {
        int block = fault_blocks[fi];
        reset57_test(); gate = armed57(); failwrite = block;
        job = run57(&gate, &boot, &ops57); assert(job.rc && nw == (unsigned)block + 1 && na == 1);
        reset57_test(); gate = armed57(); corrupt = block;
        job = run57(&gate, &boot, &ops57); assert(job.rc && nw == (unsigned)block + 1 && na == 1);
    }
    reset57_test(); gate = armed57(); verifybad = 1;
    job = run57(&gate, &boot, &ops57);
    assert(job.rc && job.stage == VERIFY57 && nw == BLOCKS57 && na == 1);
    reset57_test(); gate = (struct guard51){0};
    job = run57(&gate, &boot, &ops57); assert(job.rc == OF_ERR_DENIED && !nc && !nw);
    free(rom); free(disk);
    puts("dump57 32MiB exact source + DB prefix + TF compare + fault gates PASS");
    return 0;
}
