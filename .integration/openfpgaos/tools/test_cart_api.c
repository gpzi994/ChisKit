//------------------------------------------------------------------------------
// SPDX-License-Identifier: Apache-2.0
// SPDX-FileType: SOURCE
// SPDX-FileCopyrightText: (c) 2026, ChisBurner contributors
//------------------------------------------------------------------------------

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "of_cart.h"

_Static_assert(OF_CART_CAPS_V3_SIZE == 36u, "cart caps v3 ABI size");
_Static_assert(OF_CART_CAPS_V4_SIZE == 44u, "cart caps v4 ABI size");
_Static_assert(OF_CART_CAPS_V5_SIZE == 84u, "cart caps v5 ABI size");

const struct of_capabilities *_of_caps_ptr;
const struct of_services_table *_of_svc_ptr;

static int safe_idle_calls;
static int abort_calls;
static int read_calls;
static int read_space_calls;
static uint32_t read_space_bus;
static uint32_t read_space_kind;
static int query_calls;
static int ping_calls;

static int mock_get_caps(void *out, uint32_t out_size) {
    of_cart_caps_t *caps = (of_cart_caps_t *)out;
    if (!caps || out_size < sizeof(*caps))
        return OF_ERR_INVALID_PARAM;
    of_cart_clear_bytes(caps, sizeof(*caps));
    caps->struct_size = sizeof(*caps);
    caps->version = OF_CART_CAPS_VERSION;
    caps->operation_mask = OF_CART_OP_SAFE_IDLE | OF_CART_OP_ABORT |
                           OF_CART_OP_QUERY;
    return OF_OK;
}

static int mock_get_caps_readable(void *out, uint32_t out_size) {
    of_cart_caps_t *caps = (of_cart_caps_t *)out;
    if (!caps || out_size < OF_CART_CAPS_V4_SIZE)
        return OF_ERR_INVALID_PARAM;
    of_cart_clear_bytes(caps, OF_CART_CAPS_V4_SIZE);
    caps->struct_size = OF_CART_CAPS_V4_SIZE;
    caps->version = 4;
    caps->bus_mask = OF_CART_BUS_MASK_GB_GBC | OF_CART_BUS_MASK_GBA;
    caps->operation_mask = OF_CART_OP_READ;
    caps->max_read_bytes = 32;
    caps->gb_gbc_space_mask = OF_CART_SPACE_MASK_ROM |
                              OF_CART_SPACE_MASK_SAVE_PARALLEL;
    caps->gba_space_mask = OF_CART_SPACE_MASK_ROM;
    return OF_OK;
}

/* Simulate a v3 OS: it knows only the 36-byte prefix and deliberately leaves
 * the v4 suffix untouched.  The v4 SDK wrapper must have cleared that suffix
 * before entering this service. */
static int mock_get_caps_v3(void *out, uint32_t out_size) {
    of_cart_caps_t *caps = (of_cart_caps_t *)out;
    if (!caps || out_size < OF_CART_CAPS_V3_SIZE)
        return OF_ERR_INVALID_PARAM;
    of_cart_clear_bytes(caps, OF_CART_CAPS_V3_SIZE);
    caps->struct_size = OF_CART_CAPS_V3_SIZE;
    caps->version = 3;
    caps->operation_mask = OF_CART_OP_SAFE_IDLE;
    return OF_OK;
}

static int mock_safe_idle(void) {
    safe_idle_calls++;
    return OF_OK;
}

static int mock_read(uint32_t bus, uint32_t address, void *dst,
                     uint32_t length, uint32_t timeout_us) {
    (void)bus;
    (void)address;
    (void)dst;
    (void)length;
    (void)timeout_us;
    read_calls++;
    return OF_ERR_NOT_SUPPORTED;
}

static int mock_read_space(uint32_t bus, uint32_t space,
                           uint32_t address, void *dst,
                           uint32_t length, uint32_t timeout_us) {
    (void)address;
    (void)dst;
    (void)length;
    (void)timeout_us;
    read_space_calls++;
    read_space_bus = bus;
    read_space_kind = space;
    return OF_ERR_NOT_SUPPORTED;
}

static int mock_abort(void) {
    abort_calls++;
    return OF_OK;
}

static int mock_query_info(void *out, uint32_t out_size) {
    of_cart_protocol_info_t *info = (of_cart_protocol_info_t *)out;
    if (!info || out_size < sizeof(*info))
        return OF_ERR_INVALID_PARAM;
    of_cart_clear_bytes(info, sizeof(*info));
    info->struct_size = sizeof(*info);
    info->magic = OF_CART_PROTOCOL_MAGIC;
    info->version = OF_CART_PROTOCOL_VERSION;
    info->features = OF_CART_PROTOCOL_QUERY | OF_CART_PROTOCOL_PING;
    query_calls++;
    return OF_OK;
}

static int mock_ping(uint32_t value, uint32_t *response) {
    if (!response)
        return OF_ERR_INVALID_PARAM;
    *response = ~value;
    ping_calls++;
    return OF_OK;
}

int main(void) {
    struct of_capabilities platform_caps = {0};
    struct of_services_table services = {0};
    of_cart_caps_t cart_caps;
    of_cart_protocol_info_t protocol_info;
    uint8_t byte = 0;
    uint32_t ping_response = 0;

    platform_caps.magic = OF_CAPS_MAGIC;
    services.magic = OF_SVC_MAGIC;
    services.count = OF_CART_SVC_INDEX(cart_read_space) + 1u;
    services.cart_get_caps = mock_get_caps;
    services.cart_enter_safe_idle = mock_safe_idle;
    services.cart_read = mock_read;
    services.cart_abort = mock_abort;
    services.cart_query_info = mock_query_info;
    services.cart_ping = mock_ping;
    services.cart_read_space = mock_read_space;
    _of_caps_ptr = &platform_caps;
    _of_svc_ptr = &services;

    assert(of_cart_available() == 0);
    assert(of_cart_enter_safe_idle() == OF_ERR_NOT_SUPPORTED);

    platform_caps.hw_features = OF_HW_CARTRIDGE;
    assert(of_cart_available() == 1);
    assert(of_cart_get_caps(&cart_caps) == OF_OK);
    assert(cart_caps.struct_size == sizeof(cart_caps));
    assert(cart_caps.version == OF_CART_CAPS_VERSION);
    assert(cart_caps.bus_mask == 0);
    assert(cart_caps.operation_mask ==
           (OF_CART_OP_SAFE_IDLE | OF_CART_OP_ABORT | OF_CART_OP_QUERY));
    assert(cart_caps.max_read_bytes == 0);
    assert(cart_caps.writes_enabled == 0);
    assert(cart_caps.gb_gbc_space_mask == 0);
    assert(cart_caps.gba_space_mask == 0);
    assert(cart_caps.cart_caps_raw == 0);

    services.cart_get_caps = mock_get_caps_readable;
    assert(of_cart_get_caps(&cart_caps) == OF_OK);
    assert(cart_caps.struct_size == OF_CART_CAPS_V4_SIZE);
    assert(cart_caps.version == 4);
    assert(cart_caps.bus_mask ==
           (OF_CART_BUS_MASK_GB_GBC | OF_CART_BUS_MASK_GBA));
    assert(cart_caps.gb_gbc_space_mask ==
           (OF_CART_SPACE_MASK_ROM |
            OF_CART_SPACE_MASK_SAVE_PARALLEL));
    assert(cart_caps.gba_space_mask == OF_CART_SPACE_MASK_ROM);

    cart_caps.gb_gbc_space_mask = 0xFFFFFFFFu;
    cart_caps.gba_space_mask = 0xFFFFFFFFu;
    services.cart_get_caps = mock_get_caps_v3;
    assert(of_cart_get_caps(&cart_caps) == OF_OK);
    assert(cart_caps.struct_size == OF_CART_CAPS_V3_SIZE);
    assert(cart_caps.version == 3);
    assert(cart_caps.gb_gbc_space_mask == 0);
    assert(cart_caps.gba_space_mask == 0);
    services.cart_get_caps = mock_get_caps;

    assert(of_cart_enter_safe_idle() == OF_OK);
    assert(safe_idle_calls == 1);
    assert(of_cart_abort() == OF_OK);
    assert(abort_calls == 1);
    assert(of_cart_query_info(&protocol_info) == OF_OK);
    assert(query_calls == 1);
    assert(protocol_info.struct_size == sizeof(protocol_info));
    assert(protocol_info.magic == OF_CART_PROTOCOL_MAGIC);
    assert(protocol_info.version == OF_CART_PROTOCOL_VERSION);
    assert(protocol_info.features ==
           (OF_CART_PROTOCOL_QUERY | OF_CART_PROTOCOL_PING));
    assert(of_cart_ping(0x13579BDFu, &ping_response) == OF_OK);
    assert(ping_calls == 1);
    assert(ping_response == 0xECA86420u);
    assert(of_cart_ping(0, NULL) == OF_ERR_INVALID_PARAM);

    assert(of_cart_read((of_cart_bus_t)0, 0, &byte, 1, 1) ==
           OF_ERR_INVALID_PARAM);
    assert(of_cart_read(OF_CART_BUS_GBA, 0, NULL, 1, 1) ==
           OF_ERR_INVALID_PARAM);
    assert(of_cart_read(OF_CART_BUS_GB_GBC, 0, &byte, 0, 1) ==
           OF_ERR_INVALID_PARAM);
    assert(of_cart_read(OF_CART_BUS_GBA, 0, &byte, 1, 0) ==
           OF_ERR_INVALID_PARAM);
    assert(read_calls == 0);
    assert(of_cart_read(OF_CART_BUS_GBA, 0, &byte, 1, 1) ==
           OF_ERR_NOT_SUPPORTED);
    assert(read_calls == 1);

    assert(of_cart_read_space((of_cart_bus_t)0, OF_CART_SPACE_ROM,
                              0, &byte, 1, 1) == OF_ERR_INVALID_PARAM);
    assert(of_cart_read_space(OF_CART_BUS_GBA, (of_cart_space_t)5,
                              0, &byte, 1, 1) == OF_ERR_INVALID_PARAM);
    assert(of_cart_read_space(OF_CART_BUS_GB_GBC,
                              OF_CART_SPACE_SAVE_EEPROM,
                              0, &byte, 1, 1) == OF_ERR_INVALID_PARAM);
    assert(of_cart_read_space(OF_CART_BUS_GBA, OF_CART_SPACE_ROM,
                              0, NULL, 1, 1) == OF_ERR_INVALID_PARAM);
    assert(of_cart_read_space(OF_CART_BUS_GBA, OF_CART_SPACE_ROM,
                              0, &byte, 0, 1) == OF_ERR_INVALID_PARAM);
    assert(of_cart_read_space(OF_CART_BUS_GBA, OF_CART_SPACE_ROM,
                              0, &byte, 1, 0) == OF_ERR_INVALID_PARAM);
    assert(read_space_calls == 0);

    assert(of_cart_read_space(OF_CART_BUS_GB_GBC, OF_CART_SPACE_ROM,
                              0, &byte, 1, 1) == OF_ERR_NOT_SUPPORTED);
    assert(read_space_bus == OF_CART_BUS_GB_GBC);
    assert(read_space_kind == OF_CART_SPACE_ROM);
    assert(of_cart_read_space(OF_CART_BUS_GB_GBC,
                              OF_CART_SPACE_SAVE_PARALLEL,
                              0, &byte, 1, 1) == OF_ERR_NOT_SUPPORTED);
    assert(of_cart_read_space(OF_CART_BUS_GBA, OF_CART_SPACE_ROM,
                              0, &byte, 1, 1) == OF_ERR_NOT_SUPPORTED);
    assert(of_cart_read_space(OF_CART_BUS_GBA,
                              OF_CART_SPACE_SAVE_PARALLEL,
                              0, &byte, 1, 1) == OF_ERR_NOT_SUPPORTED);
    assert(of_cart_read_space(OF_CART_BUS_GBA,
                              OF_CART_SPACE_SAVE_EEPROM,
                              0, &byte, 1, 1) == OF_ERR_NOT_SUPPORTED);
    assert(of_cart_read_space(OF_CART_BUS_GBA, OF_CART_SPACE_RTC_GPIO,
                              0, &byte, 1, 1) == OF_ERR_NOT_SUPPORTED);
    assert(read_space_calls == 6);

    services.count = OF_CART_SVC_INDEX(cart_ping) + 1u;
    assert(of_cart_read_space(OF_CART_BUS_GBA, OF_CART_SPACE_ROM,
                              0, &byte, 1, 1) == OF_ERR_NOT_SUPPORTED);
    assert(read_space_calls == 6);

    services.count = OF_CART_SVC_INDEX(cart_get_caps);
    assert(of_cart_available() == 0);
    assert(of_cart_get_caps(&cart_caps) == OF_ERR_NOT_SUPPORTED);
    assert(cart_caps.struct_size == 0);
    assert(cart_caps.writes_enabled == 0);
    assert(of_cart_query_info(&protocol_info) == OF_ERR_NOT_SUPPORTED);
    assert(protocol_info.struct_size == 0);

    return 0;
}
