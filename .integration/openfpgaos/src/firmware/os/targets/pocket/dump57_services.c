/* Fixed-layout smoke.57 OS extension.  Normal reads cover one 32 MiB GBA ROM
 * pass; OF_CART_SPACE_IDENTIFY invokes only the FPGA-owned constant probe.
 * Output is statically bound to exact-size slot 4.  No cartridge write API. */
#include <stdint.h>
#include "of_cart.h"

#define TOTAL57 33554432u
#define MAGIC57 ((int)0x53543537u)
#define OUTPUT57 "dump57-32m.gba"
#define CAPS57 0x02200f02u
#ifndef DUMP57_TEST
#define R57(i) (*(volatile uint32_t *)(uintptr_t)(0x400001a0u + 4u * (i)))
#define W57(i,v) (R57(i) = (v))
#define FEATURES57 (*(volatile uint32_t *)(uintptr_t)0x40000098u)
#endif
#define SECTION57(s) __attribute__((section(s),noinline))

extern int of_file_async_busy(void);
extern long of_file_size(uint32_t);
extern volatile int output_is_open;
extern volatile uint32_t output_open_size, output_open_slot;

static inline int present57(void) {
    return (FEATURES57 & 32u) && R57(0) == 0x31424746u;
}

SECTION57(".caps57") int caps57(void *out, uint32_t n) {
    if (!out || n < sizeof(of_cart_caps_t)) return OF_ERR_INVALID_PARAM;
    uint32_t *p = out; for (unsigned i = 0; i < 11; ++i) p[i] = 0;
    of_cart_caps_t *c = out;
    c->struct_size = sizeof(*c); c->version = OF_CART_CAPS_VERSION;
    if (!present57() || R57(1) != CAPS57) return OF_ERR_NOT_SUPPORTED;
    c->bus_mask = OF_CART_BUS_MASK_GBA;
    c->operation_mask = OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
                        OF_CART_OP_ABORT | OF_CART_OP_QUERY;
    c->max_read_bytes = 32; c->writes_enabled = 0;
    c->lifecycle_status = R57(11); c->host_b1_raw = R57(12);
    c->host_diag = R57(13);
    c->gba_space_mask = OF_CART_SPACE_MASK_ROM | OF_CART_SPACE_MASK_IDENTIFY;
    return OF_OK;
}

static const char expected57[] __attribute__((section(".space57.rodata"))) =
    OUTPUT57;
SECTION57(".patch57.first") int bind57(const char *name, uint32_t size) {
    if (!name) return size == 0 ? MAGIC57 : OF_ERR_INVALID_PARAM;
    if (size != TOTAL57) return OF_ERR_INVALID_PARAM;
    for (unsigned i = 0; i < sizeof(expected57); ++i)
        if (name[i] != expected57[i]) return OF_ERR_INVALID_PARAM;
    int busy = of_file_async_busy();
    if (busy) return busy < 0 ? busy : OF_ERR_BUSY;
    if (of_file_size(4) != TOTAL57) return OF_ERR_BAD_RANGE;
    output_open_slot = 4; output_open_size = TOTAL57; output_is_open = 1;
    return OF_OK;
}

SECTION57(".patch57") int xfer57(uint32_t space, uint32_t off, void *dst) {
    if (!present57() || R57(1) != CAPS57) return OF_ERR_NOT_SUPPORTED;
    if ((R57(11) & 0x80000307u) != 0x80000303u)
        return OF_ERR_INVALID_STATE;
    if (R57(2) & 2u) return OF_ERR_BUSY;
    W57(16, space); W57(4, OF_CART_BUS_GBA); W57(5, off);
    W57(6, 32); W57(7, 4500000); W57(3, 2);
    unsigned wait = 1000000; uint32_t status;
    do { status = R57(2); if (!(status & 2u)) break; } while (--wait);
    if (!wait) { W57(3, 3); return OF_ERR_TIMEOUT; }
    if (status & 32u) return OF_ERR_TIMEOUT;
    if (status & 16u) return OF_ERR_INVALID_STATE;
    if ((status & 8u) || !(status & 4u) || R57(9) != 32u)
        return OF_ERR_IO;
    uint8_t *p = dst;
    for (unsigned i = 0; i < 8; ++i) {
        uint32_t word = R57(8);
        for (unsigned j = 0; j < 4; ++j) *p++ = (uint8_t)(word >> (8u * j));
    }
    return OF_OK;
}

SECTION57(".patch57") int ident57(uint32_t bus, uint32_t space, uint32_t off,
                                  void *dst, uint32_t n, uint32_t us) {
    if (!dst || !us) return OF_ERR_INVALID_PARAM;
    if (bus != OF_CART_BUS_GBA || space != OF_CART_SPACE_IDENTIFY ||
        off != 0 || n != 32u || us != 50000u) return OF_ERR_BAD_RANGE;
    return xfer57(OF_CART_SPACE_IDENTIFY, 0, dst);
}

#ifdef DUMP57_TEST
int read57_entry(uint32_t bus, uint32_t off, void *dst, uint32_t n, uint32_t us) {
    if (!dst || !us) return OF_ERR_INVALID_PARAM;
    if (bus != OF_CART_BUS_GBA || n != 32u || (off & 31u) ||
        off > TOTAL57 - 32u || us != 50000u) return OF_ERR_BAD_RANGE;
    return xfer57(OF_CART_SPACE_ROM, off, dst);
}
int space57_entry(uint32_t bus, uint32_t space, uint32_t off,
                  void *dst, uint32_t n, uint32_t us) {
    if (space == OF_CART_SPACE_ROM)
        return read57_entry(bus, off, dst, n, us);
    return ident57(bus, space, off, dst, n, us);
}
#endif
