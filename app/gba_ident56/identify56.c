#include <string.h>
#include "identify56.h"

void ident56_parse(const uint8_t snapshot[IDENT56_SNAPSHOT_BYTES],
                   struct ident56_hw *out) {
    memset(out, 0, sizeof(*out));
    memcpy(out->baseline, snapshot, 8);
    memcpy(out->jedec, snapshot + 8, 8);
    memcpy(out->cfi_qry, snapshot + 16, 8);
    memcpy(out->cfi_size, snapshot + 24, 8);
    out->id_changed = (uint8_t)(memcmp(out->baseline, out->jedec, 8) != 0);
    out->qry_ok = (uint8_t)(out->cfi_qry[0] == 'Q' && out->cfi_qry[1] == 0 &&
                            out->cfi_qry[2] == 'R' && out->cfi_qry[3] == 0 &&
                            out->cfi_qry[4] == 'Y' && out->cfi_qry[5] == 0);
    out->size_exponent = out->cfi_size[0];
    if (out->size_exponent < 32u)
        out->physical_size = 1u << out->size_exponent;
    out->supported_nor = (uint8_t)(out->id_changed && out->qry_ok &&
        out->jedec[0] == 0x01u && out->jedec[1] == 0x00u &&
        out->jedec[2] == 0x7eu && out->jedec[3] == 0x22u &&
        out->size_exponent == 25u && out->physical_size == IDENT56_PHYSICAL_32M);
}

uint32_t ident56_dump_plan(const struct db55_result *content,
                           const struct ident56_hw *hardware,
                           int probe_complete) {
    if (probe_complete && hardware && hardware->supported_nor)
        return hardware->physical_size;
    if (content && content->entry)
        return content->entry->rom_size;
    return DB55_UNKNOWN_ROM_SIZE;
}
