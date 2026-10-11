#include "mapper_decision.h"

enum mapper_family mapper_family_from_type(uint8_t type) {
    if (type >= 0x0fu && type <= 0x13u) return MAP_MBC3;
    if (type >= 0x19u && type <= 0x1eu) return MAP_MBC5;
    return MAP_UNKNOWN;
}

unsigned mapper_probe_compatible(enum mapper_family header_family,
                                 unsigned bank0_equal) {
    if (header_family == MAP_MBC3) return 1u;
    if (header_family == MAP_MBC5) return bank0_equal != 0u;
    return 0u;
}
