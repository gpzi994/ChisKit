#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
static uint32_t regs57[17], features57, wridx57[16], wrval57[16];
static unsigned nw57, rx57; static int busy57; static long size57;
static uint32_t readreg57(unsigned i) {
    assert(i < 17); if (i == 8) { ++rx57; return 0x03020100u + 0x04040404u * (rx57 - 1); }
    return regs57[i];
}
static unsigned response57 = 4;
static void writereg57(unsigned i, uint32_t v) {
    assert(i < 17 && nw57 < 16); wridx57[nw57] = i; wrval57[nw57++] = v;
    if (i == 3 && v == 2) regs57[2] = response57;
}
#define DUMP57_TEST
#define R57(i) readreg57(i)
#define W57(i,v) writereg57(i,v)
#define FEATURES57 features57
#include "../.integration/openfpgaos/src/firmware/os/targets/pocket/dump57_services.c"
volatile int output_is_open;
volatile uint32_t output_open_size, output_open_slot;
int of_file_async_busy(void) { return busy57; }
long of_file_size(uint32_t slot) { assert(slot == 4); return size57; }
static void reset57(void) {
    memset(regs57, 0, sizeof(regs57)); features57 = 32;
    regs57[0] = 0x31424746; regs57[1] = CAPS57; regs57[11] = 0x80000303;
    regs57[9] = 32; nw57 = rx57 = 0; response57 = 4; busy57 = 0;
    size57 = TOTAL57; output_is_open = 17; output_open_slot = 18; output_open_size = 19;
}
int main(void) {
    reset57(); of_cart_caps_t c;
    assert(!caps57(&c, sizeof(c)) && c.bus_mask == OF_CART_BUS_MASK_GBA &&
           c.operation_mask == 15 && !c.writes_enabled && c.max_read_bytes == 32 &&
           c.gba_space_mask == (OF_CART_SPACE_MASK_ROM | OF_CART_SPACE_MASK_IDENTIFY));
    uint8_t dst[34]; memset(dst, 0xcc, sizeof(dst));
    assert(!read57_entry(2, TOTAL57 - 32, dst + 1, 32, 50000));
    assert(dst[0] == 0xcc && dst[33] == 0xcc && nw57 == 6 && rx57 == 8);
    unsigned idx[] = {16,4,5,6,7,3};
    uint32_t val[] = {0,2,TOTAL57-32,32,4500000,2};
    assert(!memcmp(idx, wridx57, sizeof(idx)) && !memcmp(val, wrval57, sizeof(val)));
    reset57(); assert(!space57_entry(2, OF_CART_SPACE_IDENTIFY, 0, dst, 32, 50000));
    assert(wrval57[0] == OF_CART_SPACE_IDENTIFY && rx57 == 8);
    for (unsigned space = 1; space <= 3; ++space) {
        reset57(); assert(space57_entry(2, space, 0, dst, 32, 50000) == OF_ERR_BAD_RANGE && !nw57);
    }
    reset57(); assert(read57_entry(2, TOTAL57, dst, 32, 50000) == OF_ERR_BAD_RANGE && !nw57);
    assert(read57_entry(1, 0, dst, 32, 50000) == OF_ERR_BAD_RANGE && !nw57);
    assert(read57_entry(2, 0, dst, 16, 50000) == OF_ERR_BAD_RANGE && !nw57);
    regs57[2] = 2; assert(read57_entry(2, 0, dst, 32, 50000) == OF_ERR_BUSY && !nw57);
    reset57(); regs57[11] = 0;
    assert(read57_entry(2, 0, dst, 32, 50000) == OF_ERR_INVALID_STATE && !nw57);
    reset57(); regs57[1] = 0;
    assert(read57_entry(2, 0, dst, 32, 50000) == OF_ERR_NOT_SUPPORTED && !nw57);
    for (int busy = -1; busy <= 1; ++busy)
        for (unsigned sz = TOTAL57 - 1; sz <= TOTAL57 + 1; ++sz) {
            reset57(); busy57 = busy; size57 = sz;
            int rc = bind57(OUTPUT57, TOTAL57);
            assert((rc == 0) == (!busy && sz == TOTAL57));
            if (!rc) assert(output_is_open == 1 && output_open_slot == 4 && output_open_size == TOTAL57);
            else assert(output_is_open == 17);
        }
    for (unsigned i = 0; i < sizeof(OUTPUT57); ++i) {
        reset57(); char s[] = OUTPUT57; s[i] ^= 1;
        assert(bind57(s, TOTAL57) == OF_ERR_INVALID_PARAM && output_is_open == 17);
    }
    reset57(); assert(bind57(NULL, 0) == MAGIC57);
    assert(bind57(NULL, 1) == OF_ERR_INVALID_PARAM);
    puts("dump57 OS: 32MiB ROM + IDENTIFY + static exact output + rejects PASS");
    return 0;
}
