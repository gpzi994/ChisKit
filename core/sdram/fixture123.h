/* SPDX-License-Identifier: Apache-2.0 */
#ifndef CHISBURNER_FIXTURE123_H
#define CHISBURNER_FIXTURE123_H
#include <stdint.h>
#define CB_SD123_BYTES 65539u
/* Offset-dependent, non-palindromic words and different 32 KiB blocks. */
static inline uint8_t cb_sd123_byte(uint32_t offset) {
 uint32_t x=offset+0x6d2b79f5u;
 x^=x>>15; x*=0x2c1b3c6du;
 x^=x>>12; x*=0x297a2d39u;
 x^=x>>15;
 return (uint8_t)x;
}
#endif
