#ifndef CHISBURNER_MAPPER_DECISION_H
#define CHISBURNER_MAPPER_DECISION_H

#include <stdint.h>

enum mapper_family { MAP_UNKNOWN, MAP_MBC3, MAP_MBC5 };

enum mapper_family mapper_family_from_type(uint8_t type);
unsigned mapper_probe_compatible(enum mapper_family header_family,
                                 unsigned bank0_equal);

#endif
