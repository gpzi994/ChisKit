//------------------------------------------------------------------------------
// SPDX-License-Identifier: Apache-2.0
// SPDX-FileType: SOURCE
// SPDX-FileCopyrightText: (c) 2026, ChisBurner contributors
//------------------------------------------------------------------------------

/*
 * of_cart.h -- capability-gated cartridge service API
 *
 * The application never accesses cartridge registers directly.  A target
 * advertises OF_HW_CARTRIDGE and publishes the append-only service-table
 * entries before any operation becomes available.
 */

#ifndef OF_CART_H
#define OF_CART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "of_caps.h"
#include "of_error.h"
#include "of_services.h"

#define OF_CART_CAPS_VERSION 5u

typedef enum of_cart_bus {
    OF_CART_BUS_GB_GBC = 1,
    OF_CART_BUS_GBA    = 2,
} of_cart_bus_t;

/* A cartridge bus is not a single flat address space.  In particular, GBA
 * ROM, parallel save memory, serial EEPROM, and RTC/GPIO use different
 * electrical transactions even when an address value happens to overlap. */
typedef enum of_cart_space {
    OF_CART_SPACE_ROM           = 0,
    OF_CART_SPACE_SAVE_PARALLEL = 1,
    OF_CART_SPACE_SAVE_EEPROM   = 2,
    OF_CART_SPACE_RTC_GPIO      = 3,
    /* Fixed, OS-owned hardware-identification snapshot.  This is not a
     * generic write space and accepts only address 0 / length 32. */
    OF_CART_SPACE_IDENTIFY      = 4,
} of_cart_space_t;

#define OF_CART_SPACE_MASK_ROM           (1u << OF_CART_SPACE_ROM)
#define OF_CART_SPACE_MASK_SAVE_PARALLEL (1u << OF_CART_SPACE_SAVE_PARALLEL)
#define OF_CART_SPACE_MASK_SAVE_EEPROM   (1u << OF_CART_SPACE_SAVE_EEPROM)
#define OF_CART_SPACE_MASK_RTC_GPIO      (1u << OF_CART_SPACE_RTC_GPIO)
#define OF_CART_SPACE_MASK_IDENTIFY      (1u << OF_CART_SPACE_IDENTIFY)

/* SRAM138 opt-in, SAVE_PARALLEL only. Requires raw capability 0x06713f03.
 * Unlicensed 1M SRAM: two 64 KiB banks via ROM-relative 0x01000000 latch.
 * Maximum 32768 bytes/call, cached SDRAM destination, 1 ms..10 s deadline.
 * This reads raw parallel storage, does not identify SRAM vs FLASH, and does
 * not authorize save-data writes. Plain addresses retain the 137 contract. */
#define OF_CART_SAVE_BANKED_1M 0x80000000u
/* Explicit unbanked 64 KiB array read. No command or bank writes.
 * Requires the flash165-r2 OS; older OS rejects this address flag. */
#define OF_CART_SAVE_ARRAY64 0x40000000u
/* Explicit Flash128 AA/55/B0 protocol; not the SRAM bank latch. */
#define OF_CART_SAVE_FLASH128 0x20000000u

#define OF_CART_BUS_MASK_GB_GBC (1u << 0)
#define OF_CART_BUS_MASK_GBA    (1u << 1)

#define OF_CART_OP_SAFE_IDLE    (1u << 0)
#define OF_CART_OP_READ         (1u << 1)
#define OF_CART_OP_ABORT        (1u << 2)
#define OF_CART_OP_QUERY        (1u << 3)
#define OF_CART_OP_ERASE_SECTOR (1u << 4)
#define OF_CART_OP_PROGRAM      (1u << 5)

#define OF_CART_FLASH_CAPS_VERSION 1u
#define OF_CART_FLASH_CMDSET_AMD    1u
#define OF_CART_FLASH_PROFILE_OK    (1u << 0)

typedef struct of_cart_flash_caps {
    uint32_t struct_size;
    uint32_t version;
    uint32_t command_set;
    uint32_t flash_size_bytes;
    uint32_t sector_size_bytes;
    uint32_t program_buffer_bytes;
    uint32_t profile_status;
    uint32_t id_prefix;
    uint32_t id_tail;
    uint32_t cfi_signature;
} of_cart_flash_caps_t;

#define OF_CART_PROTOCOL_MAGIC    0x50424746u /* legacy wire value; keep binary compatibility */
#define OF_CART_PROTOCOL_VERSION  0x00010000u /* 1.0 */
#define OF_CART_PROTOCOL_QUERY    (1u << 0)
#define OF_CART_PROTOCOL_PING     (1u << 1)

#define OF_CART_LIFECYCLE_STATE_MASK  0x00000007u
#define OF_CART_LIFECYCLE_LOCKED      0u
#define OF_CART_LIFECYCLE_WAIT_RESET  1u
#define OF_CART_LIFECYCLE_SETTLING    2u
#define OF_CART_LIFECYCLE_READY       3u
#define OF_CART_LIFECYCLE_PLAY        (1u << 8)
#define OF_CART_LIFECYCLE_POWER       (1u << 9)
#define OF_CART_LIFECYCLE_ADAPTER(v)  (((v) >> 16) & 0xFFu)
#define OF_CART_LIFECYCLE_READ_READY  (1u << 31)

typedef struct of_cart_caps {
    uint32_t struct_size;
    uint32_t version;
    uint32_t bus_mask;
    uint32_t operation_mask;
    uint32_t max_read_bytes;
    uint32_t writes_enabled;
    uint32_t lifecycle_status;
    uint32_t host_b1_raw;
    uint32_t host_diag;
    uint32_t gb_gbc_space_mask;
    uint32_t gba_space_mask;
    /* v5 diagnostic suffix.  These are read-only snapshots of the hardware
     * contract and command state.  They are populated even when capability
     * validation fails, so a field diagnostic can distinguish a malformed
     * capability word from a command that ended ABORTED/ERROR. */
    uint32_t hw_features_raw;
    uint32_t hw_contract_rev;
    uint32_t cart_id_raw;
    uint32_t cart_caps_raw;
    uint32_t cart_status_raw;
    uint32_t cart_error_raw;
    uint32_t cart_profile_raw;
    uint32_t flash_bytes_raw;
    uint32_t sector_bytes_raw;
    uint32_t buffer_bytes_raw;
} of_cart_caps_t;

/* v3 ended immediately before the per-bus space masks.  cart_get_caps uses
 * out_size, so a v4 OS accepts this prefix size from an older application and
 * a v4 application zero-initializes the suffix before calling an older OS. */
#define OF_CART_CAPS_V3_SIZE \
    ((uint32_t)offsetof(of_cart_caps_t, gb_gbc_space_mask))
#define OF_CART_CAPS_V4_SIZE \
    ((uint32_t)offsetof(of_cart_caps_t, hw_features_raw))
#define OF_CART_CAPS_V5_SIZE ((uint32_t)sizeof(of_cart_caps_t))

typedef struct of_cart_protocol_info {
    uint32_t struct_size;
    uint32_t magic;
    uint32_t version;
    uint32_t features;
} of_cart_protocol_info_t;

static inline void of_cart_clear_bytes(void *ptr, uint32_t size) {
    uint8_t *p = (uint8_t *)ptr;
    while (size--)
        *p++ = 0;
}

#ifndef OF_PC

#define OF_CART_SVC_INDEX(field) \
    ((uint32_t)((offsetof(struct of_services_table, field) - \
                 offsetof(struct of_services_table, video_init)) / sizeof(void *)))

#define OF_CART_SVC_HAS(field) \
    (OF_SVC && OF_SVC->magic == OF_SVC_MAGIC && \
     OF_SVC->count > OF_CART_SVC_INDEX(field) && OF_SVC->field)

static inline int of_cart_available(void) {
    const struct of_capabilities *caps = of_get_caps();
    return caps && caps->magic == OF_CAPS_MAGIC &&
           (caps->hw_features & OF_HW_CARTRIDGE) != 0 &&
           OF_CART_SVC_HAS(cart_get_caps);
}

static inline int of_cart_get_caps(of_cart_caps_t *caps) {
    if (!caps)
        return OF_ERR_INVALID_PARAM;
    of_cart_clear_bytes(caps, sizeof(*caps));
    if (!of_cart_available())
        return OF_ERR_NOT_SUPPORTED;
    return OF_SVC->cart_get_caps(caps, sizeof(*caps));
}

static inline int of_cart_enter_safe_idle(void) {
    if (!of_cart_available() || !OF_CART_SVC_HAS(cart_enter_safe_idle))
        return OF_ERR_NOT_SUPPORTED;
    return OF_SVC->cart_enter_safe_idle();
}

static inline int of_cart_read(of_cart_bus_t bus, uint32_t address,
                               void *dst, uint32_t length,
                               uint32_t timeout_us) {
    if (!of_cart_available() || !OF_CART_SVC_HAS(cart_read))
        return OF_ERR_NOT_SUPPORTED;
    if ((bus != OF_CART_BUS_GB_GBC && bus != OF_CART_BUS_GBA) ||
        !dst || length == 0 || timeout_us == 0)
        return OF_ERR_INVALID_PARAM;
    return OF_SVC->cart_read((uint32_t)bus, address, dst, length,
                             timeout_us);
}

/* Explicit-space read API.  of_cart_read() above remains the append-only ABI
 * compatibility entry point and always means cartridge ROM. */
static inline int of_cart_read_space(of_cart_bus_t bus,
                                     of_cart_space_t space,
                                     uint32_t address, void *dst,
                                     uint32_t length,
                                     uint32_t timeout_us) {
    if (!of_cart_available() || !OF_CART_SVC_HAS(cart_read_space))
        return OF_ERR_NOT_SUPPORTED;
    if ((bus != OF_CART_BUS_GB_GBC && bus != OF_CART_BUS_GBA) ||
        (space != OF_CART_SPACE_ROM &&
         space != OF_CART_SPACE_SAVE_PARALLEL &&
         space != OF_CART_SPACE_SAVE_EEPROM &&
         space != OF_CART_SPACE_RTC_GPIO &&
         space != OF_CART_SPACE_IDENTIFY) ||
        (bus == OF_CART_BUS_GB_GBC &&
         space != OF_CART_SPACE_ROM &&
         space != OF_CART_SPACE_SAVE_PARALLEL &&
         space != OF_CART_SPACE_IDENTIFY) ||
        !dst || length == 0 || timeout_us == 0)
        return OF_ERR_INVALID_PARAM;
    return OF_SVC->cart_read_space((uint32_t)bus, (uint32_t)space,
                                   address, dst, length, timeout_us);
}

static inline int of_cart_abort(void) {
    if (!of_cart_available() || !OF_CART_SVC_HAS(cart_abort))
        return OF_ERR_NOT_SUPPORTED;
    return OF_SVC->cart_abort();
}

static inline int of_cart_query_info(of_cart_protocol_info_t *info) {
    if (!info)
        return OF_ERR_INVALID_PARAM;
    of_cart_clear_bytes(info, sizeof(*info));
    if (!of_cart_available() || !OF_CART_SVC_HAS(cart_query_info))
        return OF_ERR_NOT_SUPPORTED;
    return OF_SVC->cart_query_info(info, sizeof(*info));
}

static inline int of_cart_ping(uint32_t value, uint32_t *response) {
    if (!response)
        return OF_ERR_INVALID_PARAM;
    if (!of_cart_available() || !OF_CART_SVC_HAS(cart_ping))
        return OF_ERR_NOT_SUPPORTED;
    return OF_SVC->cart_ping(value, response);
}

static inline int of_cart_flash_get_caps(of_cart_flash_caps_t *caps) {
    if (!caps)
        return OF_ERR_INVALID_PARAM;
    of_cart_clear_bytes(caps, sizeof(*caps));
    if (!of_cart_available() || !OF_CART_SVC_HAS(cart_flash_get_caps))
        return OF_ERR_NOT_SUPPORTED;
    return OF_SVC->cart_flash_get_caps(caps, sizeof(*caps));
}

static inline int of_cart_flash_erase_sector(uint32_t address,
                                              uint32_t timeout_us) {
    if (!timeout_us)
        return OF_ERR_INVALID_PARAM;
    if (!of_cart_available() || !OF_CART_SVC_HAS(cart_flash_erase_sector))
        return OF_ERR_NOT_SUPPORTED;
    return OF_SVC->cart_flash_erase_sector(address, timeout_us);
}

static inline int of_cart_flash_program(uint32_t address, const void *src,
                                        uint32_t length,
                                        uint32_t timeout_us) {
    if (!src || !length || !timeout_us)
        return OF_ERR_INVALID_PARAM;
    if (!of_cart_available() || !OF_CART_SVC_HAS(cart_flash_program))
        return OF_ERR_NOT_SUPPORTED;
    return OF_SVC->cart_flash_program(address, src, length, timeout_us);
}

static inline int of_cart_flash_disarm(void) {
    if (!of_cart_available() || !OF_CART_SVC_HAS(cart_flash_disarm))
        return OF_ERR_NOT_SUPPORTED;
    return OF_SVC->cart_flash_disarm();
}

#else /* OF_PC */

static inline int of_cart_available(void) {
    return 0;
}

static inline int of_cart_get_caps(of_cart_caps_t *caps) {
    if (!caps)
        return OF_ERR_INVALID_PARAM;
    of_cart_clear_bytes(caps, sizeof(*caps));
    return OF_ERR_NOT_SUPPORTED;
}

static inline int of_cart_enter_safe_idle(void) {
    return OF_ERR_NOT_SUPPORTED;
}

static inline int of_cart_read(of_cart_bus_t bus, uint32_t address,
                               void *dst, uint32_t length,
                               uint32_t timeout_us) {
    (void)bus;
    (void)address;
    (void)dst;
    (void)length;
    (void)timeout_us;
    return OF_ERR_NOT_SUPPORTED;
}

static inline int of_cart_read_space(of_cart_bus_t bus,
                                     of_cart_space_t space,
                                     uint32_t address, void *dst,
                                     uint32_t length,
                                     uint32_t timeout_us) {
    (void)bus;
    (void)space;
    (void)address;
    (void)dst;
    (void)length;
    (void)timeout_us;
    return OF_ERR_NOT_SUPPORTED;
}

static inline int of_cart_abort(void) {
    return OF_ERR_NOT_SUPPORTED;
}

static inline int of_cart_query_info(of_cart_protocol_info_t *info) {
    if (!info)
        return OF_ERR_INVALID_PARAM;
    of_cart_clear_bytes(info, sizeof(*info));
    return OF_ERR_NOT_SUPPORTED;
}

static inline int of_cart_ping(uint32_t value, uint32_t *response) {
    (void)value;
    if (!response)
        return OF_ERR_INVALID_PARAM;
    return OF_ERR_NOT_SUPPORTED;
}

static inline int of_cart_flash_get_caps(of_cart_flash_caps_t *caps) {
    if (!caps)
        return OF_ERR_INVALID_PARAM;
    of_cart_clear_bytes(caps, sizeof(*caps));
    return OF_ERR_NOT_SUPPORTED;
}

static inline int of_cart_flash_erase_sector(uint32_t address,
                                              uint32_t timeout_us) {
    (void)address; (void)timeout_us; return OF_ERR_NOT_SUPPORTED;
}

static inline int of_cart_flash_program(uint32_t address, const void *src,
                                        uint32_t length,
                                        uint32_t timeout_us) {
    (void)address; (void)src; (void)length; (void)timeout_us;
    return OF_ERR_NOT_SUPPORTED;
}

static inline int of_cart_flash_disarm(void) {
    return OF_ERR_NOT_SUPPORTED;
}

#endif /* OF_PC */

#ifdef __cplusplus
}
#endif

#endif /* OF_CART_H */
