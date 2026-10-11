#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint32_t regs56[17], features56, wr_index[16], wr_value[16];
static unsigned write_count, rx_count;
static uint32_t reply56 = 4;
static uint32_t readreg56(unsigned i) {
    assert(i < 17);
    if (i == 8) { ++rx_count; return 0x03020100u + 0x04040404u * (rx_count - 1); }
    return regs56[i];
}
static void writereg56(unsigned i, uint32_t value) {
    assert(i < 17 && write_count < 16);
    wr_index[write_count] = i; wr_value[write_count++] = value;
    if (i == 3 && value == 2) regs56[2] = reply56;
}
#define IDENT56_TEST
#define R56(i) readreg56(i)
#define W56(i,v) writereg56((i),(v))
#define FEATURES56 features56
#include "../.integration/openfpgaos/src/firmware/os/targets/pocket/identify56_services.c"

static void reset56(void) {
    memset(regs56, 0, sizeof(regs56));
    features56 = 32; regs56[0] = 0x31424746; regs56[1] = CAPS56;
    regs56[11] = 0x80000303; regs56[9] = 32;
    write_count = rx_count = 0; reply56 = 4;
}

static void good_xfer(uint32_t space, int (*call)(uint32_t,uint32_t,uint32_t,void*,uint32_t,uint32_t)) {
    uint8_t out[34]; memset(out, 0xcc, sizeof(out)); reset56();
    assert(call(2, space, 0, out + 1, 32, 50000) == 0);
    assert(write_count == 6 && rx_count == 8);
    const unsigned wi[6] = {16,4,5,6,7,3};
    const uint32_t wv[6] = {space,2,0,32,4500000,2};
    assert(!memcmp(wi, wr_index, sizeof(wi)) && !memcmp(wv, wr_value, sizeof(wv)));
    for (unsigned i = 0; i < 32; ++i) assert(out[i + 1] == i);
    assert(out[0] == 0xcc && out[33] == 0xcc);
}

static int space_call(uint32_t bus, uint32_t space, uint32_t off,
                      void *dst, uint32_t n, uint32_t us) {
    return space56(bus, space, off, dst, n, us);
}

int main(void) {
    reset56(); of_cart_caps_t caps;
    assert(caps56(&caps, sizeof(caps)) == 0 && caps.bus_mask == 2 &&
           caps.operation_mask == 15 && caps.max_read_bytes == 32 &&
           caps.writes_enabled == 0 && caps.gba_space_mask == 0x11);
    assert(output_disabled56("x", 1) == OF_ERR_NOT_SUPPORTED);
    good_xfer(OF_CART_SPACE_ROM, space_call);
    good_xfer(OF_CART_SPACE_IDENTIFY, space_call);
    uint8_t out[32];
    reset56(); assert(space56(2,4,2,out,32,50000) == OF_ERR_BAD_RANGE && !write_count);
    reset56(); assert(space56(2,4,0,out,16,50000) == OF_ERR_BAD_RANGE && !write_count);
    reset56(); assert(space56(2,1,0,out,32,50000) == OF_ERR_BAD_RANGE && !write_count);
    reset56(); assert(read56(2,352,out,32,50000) == 0);
    reset56(); assert(read56(2,384,out,32,50000) == OF_ERR_BAD_RANGE && !write_count);
    reset56(); regs56[11] = 0;
    assert(space56(2,4,0,out,32,50000) == OF_ERR_INVALID_STATE && !write_count);
    reset56(); reply56 = 32;
    assert(space56(2,4,0,out,32,50000) == OF_ERR_TIMEOUT);
    puts("identify56 native OS caps/ROM/identify shape + reject-without-MMIO PASS");
    return 0;
}
