#ifndef CHISBURNER_GBA_IDENTIFY56_H
#define CHISBURNER_GBA_IDENTIFY56_H

#include <stdint.h>
#include "../gba_db55/db55.h"

#define IDENT56_SNAPSHOT_BYTES 32u
#define IDENT56_PHYSICAL_32M 0x02000000u

struct ident56_hw {
    uint8_t baseline[8];
    uint8_t jedec[8];
    uint8_t cfi_qry[8];
    uint8_t cfi_size[8];
    uint8_t id_changed;
    uint8_t qry_ok;
    uint8_t size_exponent;
    uint8_t supported_nor;
    uint32_t physical_size;
};

void ident56_parse(const uint8_t snapshot[IDENT56_SNAPSHOT_BYTES],
                   struct ident56_hw *out);
uint32_t ident56_dump_plan(const struct db55_result *content,
                           const struct ident56_hw *hardware,
                           int probe_complete);

#endif
