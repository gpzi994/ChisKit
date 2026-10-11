#include <assert.h>
#include <stdio.h>

#include "mapper_decision.h"

int main(void) {
    assert(mapper_family_from_type(0x10) == MAP_MBC3);
    assert(mapper_probe_compatible(MAP_MBC3, 1)); /* flash-cart bank-0 alias */
    assert(mapper_probe_compatible(MAP_MBC3, 0)); /* canonical MBC3 behavior */

    assert(mapper_family_from_type(0x1b) == MAP_MBC5);
    assert(mapper_probe_compatible(MAP_MBC5, 1));
    assert(!mapper_probe_compatible(MAP_MBC5, 0));

    assert(mapper_family_from_type(0x06) == MAP_UNKNOWN);
    assert(!mapper_probe_compatible(MAP_UNKNOWN, 1));
    puts("gb mapper decision94: PASS");
    return 0;
}
