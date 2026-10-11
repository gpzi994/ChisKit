/* Fixed-layout smoke.56 OS extension.  The application receives one bounded
 * OF_CART_SPACE_IDENTIFY snapshot; only the FPGA owns the constant JEDEC/CFI
 * command sequence.  No generic cartridge write service is introduced. */
#include <stdint.h>
#include "of_cart.h"

#ifndef IDENT56_TEST
#define R56(i) (*(volatile uint32_t *)(uintptr_t)(0x400001a0u + 4u * (i)))
#define W56(i,v) (R56(i) = (v))
#define FEATURES56 (*(volatile uint32_t *)(uintptr_t)0x40000098u)
#endif

#define SECTION56(s) __attribute__((section(s),noinline))
#define CAPS56 0x02200f02u
#define HEADER56 384u

static inline int present56(void) {
    return (FEATURES56 & 32u) && R56(0) == 0x31424746u;
}

SECTION56(".caps56") int caps56(void *out, uint32_t n) {
    if (!out || n < sizeof(of_cart_caps_t)) return OF_ERR_INVALID_PARAM;
    uint32_t *p = out;
    for (unsigned i = 0; i < 11; ++i) p[i] = 0;
    of_cart_caps_t *c = out;
    c->struct_size = sizeof(*c); c->version = OF_CART_CAPS_VERSION;
    if (!present56() || R56(1) != CAPS56) return OF_ERR_NOT_SUPPORTED;
    c->bus_mask = OF_CART_BUS_MASK_GBA;
    c->operation_mask = OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
                        OF_CART_OP_ABORT | OF_CART_OP_QUERY;
    c->max_read_bytes = 32;
    c->writes_enabled = 0;
    c->lifecycle_status = R56(11); c->host_b1_raw = R56(12);
    c->host_diag = R56(13);
    c->gba_space_mask = OF_CART_SPACE_MASK_ROM | OF_CART_SPACE_MASK_IDENTIFY;
    return OF_OK;
}

SECTION56(".patch56.first") int output_disabled56(const char *name, uint32_t size) {
    (void)name; (void)size; return OF_ERR_NOT_SUPPORTED;
}

static int xfer56(uint32_t space, uint32_t off, void *dst) {
    if (!present56() || R56(1) != CAPS56) return OF_ERR_NOT_SUPPORTED;
    if ((R56(11) & 0x80000307u) != 0x80000303u) return OF_ERR_INVALID_STATE;
    if (R56(2) & 2u) return OF_ERR_BUSY;
    W56(16, space); W56(4, OF_CART_BUS_GBA); W56(5, off);
    W56(6, 32); W56(7, 4500000); W56(3, 2);
    unsigned wait = 1000000; uint32_t status;
    do { status = R56(2); if (!(status & 2u)) break; } while (--wait);
    if (!wait) { W56(3, 3); return OF_ERR_TIMEOUT; }
    if (status & 32u) return OF_ERR_TIMEOUT;
    if (status & 16u) return OF_ERR_INVALID_STATE;
    if ((status & 8u) || !(status & 4u) || R56(9) != 32u) return OF_ERR_IO;
    uint8_t *p = dst;
    for (unsigned i = 0; i < 8; ++i) {
        uint32_t word = R56(8);
        for (unsigned j = 0; j < 4; ++j) *p++ = (uint8_t)(word >> (8u * j));
    }
    return OF_OK;
}

SECTION56(".patch56") int read56(uint32_t bus, uint32_t off, void *dst,
                                  uint32_t n, uint32_t us) {
    if (!dst || !us) return OF_ERR_INVALID_PARAM;
    if (bus != OF_CART_BUS_GBA || n != 32u || (off & 31u) ||
        off > HEADER56 - 32u || us != 50000u) return OF_ERR_BAD_RANGE;
    return xfer56(OF_CART_SPACE_ROM, off, dst);
}

SECTION56(".patch56") int space56(uint32_t bus, uint32_t space, uint32_t off,
                                   void *dst, uint32_t n, uint32_t us) {
    if (space == OF_CART_SPACE_ROM) return read56(bus, off, dst, n, us);
    if (!dst || !us) return OF_ERR_INVALID_PARAM;
    if (bus != OF_CART_BUS_GBA || space != OF_CART_SPACE_IDENTIFY ||
        off != 0 || n != 32u || us != 50000u) return OF_ERR_BAD_RANGE;
    return xfer56(OF_CART_SPACE_IDENTIFY, 0, dst);
}
