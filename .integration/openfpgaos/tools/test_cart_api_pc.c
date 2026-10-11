//------------------------------------------------------------------------------
// SPDX-License-Identifier: Apache-2.0
// SPDX-FileType: SOURCE
// SPDX-FileCopyrightText: (c) 2026, ChisBurner contributors
//------------------------------------------------------------------------------

#include <assert.h>
#include <stdint.h>

#include "of_cart.h"

int main(void) {
    of_cart_caps_t caps;
    of_cart_protocol_info_t info;
    uint8_t byte = 0;
    uint32_t response = 0;

    caps.struct_size = 0xFFFFFFFFu;
    caps.writes_enabled = 0xFFFFFFFFu;

    assert(of_cart_available() == 0);
    assert(of_cart_get_caps(NULL) == OF_ERR_INVALID_PARAM);
    assert(of_cart_get_caps(&caps) == OF_ERR_NOT_SUPPORTED);
    assert(caps.struct_size == 0);
    assert(caps.writes_enabled == 0);
    assert(caps.gb_gbc_space_mask == 0);
    assert(caps.gba_space_mask == 0);
    assert(of_cart_enter_safe_idle() == OF_ERR_NOT_SUPPORTED);
    assert(of_cart_read(OF_CART_BUS_GBA, 0, &byte, 1, 1) ==
           OF_ERR_NOT_SUPPORTED);
    assert(of_cart_read_space(OF_CART_BUS_GBA, OF_CART_SPACE_ROM,
                              0, &byte, 1, 1) == OF_ERR_NOT_SUPPORTED);
    assert(of_cart_read_space(OF_CART_BUS_GBA,
                              OF_CART_SPACE_SAVE_PARALLEL,
                              0, &byte, 1, 1) == OF_ERR_NOT_SUPPORTED);
    assert(of_cart_abort() == OF_ERR_NOT_SUPPORTED);
    assert(of_cart_query_info(NULL) == OF_ERR_INVALID_PARAM);
    assert(of_cart_query_info(&info) == OF_ERR_NOT_SUPPORTED);
    assert(info.struct_size == 0);
    assert(of_cart_ping(0x13579BDFu, &response) == OF_ERR_NOT_SUPPORTED);
    assert(of_cart_ping(0, NULL) == OF_ERR_INVALID_PARAM);

    return 0;
}
