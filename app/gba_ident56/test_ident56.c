#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "identify56.h"

static void supported(void) {
    uint8_t s[32] = {0x50,0x4f,0x4b,0x45,0x4d,0x4f,0x4e,0x20,
                    0x01,0x00,0x7e,0x22,0x22,0x22,0x01,0x22,
                    'Q',0,'R',0,'Y',0,0,0,25,0,0,0,0,0,0,0};
    struct ident56_hw hw;
    ident56_parse(s, &hw);
    assert(hw.id_changed && hw.qry_ok && hw.supported_nor);
    assert(hw.physical_size == 33554432u);
    struct db55_result miss = {0};
    assert(ident56_dump_plan(&miss, &hw, 1) == 33554432u);
}

static void generic(void) {
    uint8_t s[32];
    for (unsigned i = 0; i < sizeof(s); ++i) s[i] = (uint8_t)i;
    memcpy(s + 8, s, 8);
    struct ident56_hw hw;
    ident56_parse(s, &hw);
    assert(!hw.id_changed && !hw.supported_nor);
    const uint8_t awac_sha1[20] = {0xe3,0x49,0x7e,0xc9,0xcf,0x54,0xb0,0x86,
        0xa7,0x76,0x0c,0x34,0xc6,0x20,0x7b,0xc9,0xc1,0x37,0x07,0xfd};
    struct db55_result wario = {0};
    wario.entry = db55_lookup(awac_sha1);
    assert(wario.entry && wario.entry->rom_size == 8388608u);
    assert(wario.entry->rom_crc32 == 0xcf7585f1u);
    assert(ident56_dump_plan(&wario, &hw, 1) == 8388608u);
}

int main(void) {
    assert(db55_validate_index());
    supported(); generic();
    puts("identify56 parser: S29GL256N 32MiB + AWAC generic 8MiB policy PASS");
    return 0;
}
