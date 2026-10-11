//------------------------------------------------------------------------------
// SPDX-License-Identifier: Apache-2.0
// SPDX-FileType: SOURCE
// SPDX-FileCopyrightText: (c) 2026, ThinkElastic <Think@Elastic.com>
//------------------------------------------------------------------------------

/*
 * openfpgaOS OS Services Table
 *
 * Populates a static table of function pointers in kernel BSS. Apps
 * call OS services through this table with zero syscall overhead
 * (~2 cycles indirect call vs ~50 cycles ecall trap entry/exit).
 *
 * The table address is handed to apps through the AT_OF_SVC auxv tag
 * in elf_exec(), so an app never has to know where the table lives.
 *
 * Where the SDK API signature differs from the HAL, thin wrappers
 * adapt the calling convention (e.g., video_set_palette packs RGB).
 */

#include "services_table.h"
#include "irq.h"
#include "bank_preload.h"
#include "config.h"
#include "of_services.h"
#include "of_cart.h"
#include "of_output.h"
#if CART_NOR108
#include "nor108.h"
#define NOR108_LEGACY_GUARD() do { if(nor108_legacy_touch()) return OF_ERR_BUSY; } while(0)
#else
#define NOR108_LEGACY_GUARD() do {} while(0)
#endif
#include "../hal/hal.h"
#include "../hal/regs.h"
#include "../hal/file.h"

#if CHISBURNER_PSRAM_CACHE129
static int svc_psram_cache_load(uint32_t slot,uint32_t slot_offset,
                                uint32_t cache_offset,uint32_t length){
#if CART_STREAM131
    NOR108_LEGACY_GUARD(); /* A cache rewrite invalidates a prior ROM job token. */
#endif
    if(!length||length>4096u||(cache_offset&3u)||(length&3u)||
       cache_offset>0x02000000u-length)return OF_ERR_BAD_RANGE;
    uint32_t bridge=cache_offset<0x01000000u?0x20000000u+cache_offset:
                    0x22000000u+(cache_offset-0x01000000u);
    return of_file_psram129_read_chunk(slot,slot_offset,bridge,length);
}
static int svc_psram_cache_read(uint32_t cache_offset,void*dst,uint32_t length){
#if CART_STREAM131
    if(nor131_program_pending())return OF_ERR_BUSY;
#endif
    if(!dst||!length||length>65536u||(cache_offset&3u)||(length&3u)||
       cache_offset>0x02000000u-length)return OF_ERR_BAD_RANGE;
    uint8_t*out=(uint8_t*)dst;
    for(uint32_t n=0;n<length;n+=4u){
        uint32_t request=(PSRAM129_READ_CMD^1u)&1u;
        PSRAM129_READ_ADDR=(cache_offset+n)>>2;
        PSRAM129_READ_CMD=1u;fence();
        uint32_t status=0;
        for(uint32_t spins=0;spins<1000000u;spins++){
            status=PSRAM129_READ_STATUS;
            if((status&PSRAM129_READ_ACK)==request&&!(status&PSRAM129_READ_BUSY))break;
            if(spins==999999u)return OF_ERR_TIMEOUT;
        }
        if(status&PSRAM129_READ_FAULT)return OF_ERR_IO;
        uint32_t word=PSRAM129_READ_DATA;
        out[n]=(uint8_t)word;out[n+1]=(uint8_t)(word>>8);
        out[n+2]=(uint8_t)(word>>16);out[n+3]=(uint8_t)(word>>24);
    }
    return OF_OK;
}
#endif

/* ======================================================================
 * Wrappers for SDK/HAL signature mismatches
 * ====================================================================== */

/* SDK: (index, 0x00RRGGBB)  →  HAL: (index, r, g, b) */
static void svc_video_set_palette(uint8_t index, uint32_t rgb) {
    of_video_set_palette(index, (uint8_t)(rgb >> 16),
                         (uint8_t)(rgb >> 8), (uint8_t)rgb);
}

static void svc_video_set_color_mode(int mode) {
    of_video_set_color_mode(mode);
}

/* SDK: (player, out_ptr) copies state  →  HAL: returns pointer */
static void svc_input_get_state(int player, void *out) {
    const of_input_state_t *state = of_input_get_state(player);
    if (out)
        __builtin_memcpy(out, state, sizeof(of_input_state_t));
}

static void svc_input_get_keyboard_state(void *out) {
    const of_keyboard_state_t *state = of_input_get_keyboard_state();
    if (out)
        __builtin_memcpy(out, state, sizeof(of_keyboard_state_t));
}

static void svc_input_read_mouse_state(void *out) {
    of_input_read_mouse_state((of_mouse_state_t *)out);
}

/* Timer policy is owned by syscall.c (shared 1 kHz tick + SW-mixer pump). */
extern void of_os_timer_set_callback(void (*cb)(void), uint32_t app_hz, int self_divide);

static inline uint32_t svc_irq_save_local(void)
{
    uint32_t prev;
    __asm__ volatile("csrrci %0, mstatus, 0x8"
                     : "=r"(prev) :: "memory");
    return prev & 0x8u;
}

static inline void svc_irq_restore_local(uint32_t prev)
{
    if (prev)
        __asm__ volatile("csrrsi zero, mstatus, 0x8" ::: "memory");
}

static void svc_timer_set_callback(void (*cb)(void), uint32_t hz) {
    /* Pin the hardware timer at 1 kHz regardless of the app's requested rate.
     * of_smp_tables bakes MIDI envelopes at 1 kHz and of_midi_pump accumulates
     * elapsed time to fire smp_voice_tick the right number of times per wake,
     * so this consumer always wants the raw 1 kHz tick (self_divide=1).  On a
     * SW-mixer build of_os_timer_set_callback also keeps the timer pinned ON
     * so PCM/SFX audio survives of_midi_stop(). */
    (void)hz;
    of_os_timer_set_callback(cb, 1000u, 1 /* consumer self-divides */);
}

static void svc_timer_stop(void) {
    of_os_timer_set_callback(NULL, 0u, 0);
}

/* Mixer end callback: wraps irq registration */
static void svc_mixer_set_end_callback(void (*cb)(uint32_t ended_mask)) {
    of_irq_register_mixer_end(cb);
}

/* Vsync callback: wraps irq registration */
static void svc_video_set_vsync_callback(void (*cb)(void)) {
    of_irq_register_vsync(cb);
}

/* File size by path: open, query, close */
extern long sys_openat_svc(const char *path);
extern void sys_close_svc(int fd);
extern long sys_file_size_fd(int fd);
extern void file_slot_register(uint32_t slot_id, const char *filename);

static long svc_file_size(const char *path) {
    long fd = sys_openat_svc(path);
    if (fd < 0) return -1;
    long sz = sys_file_size_fd((int)fd);
    sys_close_svc((int)fd);
    return sz;
}

static long svc_file_size_fd(int fd) {
    return sys_file_size_fd(fd);
}

/* Dynamic output is a build-time capability, independent from cartridge
 * access.  Production/default OS images leave the service pointers NULL.
 * The first field profile uses the SDK's writable deferred slot 4 only. */
#ifndef CHISBURNER_OUTPUT_ENABLED
#if CART_UI98
#define CHISBURNER_OUTPUT_ENABLED 1
#else
#define CHISBURNER_OUTPUT_ENABLED 0
#endif
#endif

#if CHISBURNER_OUTPUT_ENABLED
/* Slot 4 is the user-selected ROM input.  Generated files use a separate
 * non-readonly deferred slot; Pocket rejects create through the input slot. */
#define CHISBURNER_OUTPUT_SLOT 5u

static int svc_output_path_allowed(const char *path) {
    static const char prefix[] = "/Saves/chiskit/common/";
    if (!path)
        return 0;

    uint32_t i = 0;
    while (prefix[i]) {
        if (path[i] != prefix[i])
            return 0;
        i++;
    }

    /* Require a filename below the supported platform's common Saves root.
     * Reject path traversal, empty segments and host-specific separators. */
    if (!path[i] || path[i] == '/')
        return 0;
    uint32_t total = i;
    char previous = '/';
    while (path[total]) {
        char c = path[total];
        if (total >= 255u || c == '\\' || c == ':' ||
            (c == '/' && previous == '/') ||
            (c == '.' && path[total + 1] == '.' &&
             (previous == '/' || path[total + 2] == '/' ||
              path[total + 2] == '\0')))
            return 0;
        previous = c;
        total++;
    }
    return previous != '/';
}

static int svc_output_open(const char *path, uint32_t size) {
    if (!svc_output_path_allowed(path) || size == 0 ||
        size > OF_OUTPUT_MAX_SIZE)
        return OF_ERR_INVALID_PARAM;
    return of_file_output_open(CHISBURNER_OUTPUT_SLOT, path, size);
}

static int svc_output_write(uint32_t offset, const void *src,
                            uint32_t length) {
    if ((!src && length) || offset > OF_OUTPUT_MAX_SIZE ||
        length > OF_OUTPUT_MAX_SIZE - offset)
        return OF_ERR_BAD_RANGE;
    return of_file_output_write(CHISBURNER_OUTPUT_SLOT, offset, src, length);
}

static int svc_output_flush(void) {
    return of_file_output_flush(CHISBURNER_OUTPUT_SLOT);
}
#endif

/* Cartridge ABI v1.  The feature bit only says that the dedicated profile
 * owns the pins; CART_ID additionally proves that the matching register
 * block exists.  CART_CAPS is the final authority for individual buses and
 * operations, so an older or locked bitstream always fails closed. */
#ifndef CART_MMIO_ENABLED
#define CART_MMIO_ENABLED 0
#endif

/* Destructive GBA NOR operations require both the dedicated FPGA profile and
 * an OS build that explicitly opts into its exact register contract. */
#ifndef CART_S29GL256_PROGRAMMER
#define CART_S29GL256_PROGRAMMER 0
#endif

/* The first field-read OS is deliberately narrower than the public read ABI:
 * it permits exactly one GBA header window.  Production builds leave this at
 * zero, and later read profiles must opt in with their own explicit policy. */
#ifndef CART_GBA_HEADER_PROBE
#define CART_GBA_HEADER_PROBE 0
#endif

/* Scaling diagnostic: aligned 32-byte reads within the first 4 KiB of GBA
 * ROM.  This remains off in production and is mutually exclusive with the
 * fixed-header diagnostic. */
#ifndef CART_GBA_ROM4K_PROBE
#define CART_GBA_ROM4K_PROBE 0
#endif

/* High-address diagnostic: only the fixed ChisBurner mirror-probe sequence and
 * the final 32-byte window of the 32 MiB GBA address space are reachable. */
#ifndef CART_GBA_ADDR_PROBE
#define CART_GBA_ADDR_PROBE 0
#endif

/* First GB/GBC field profile: ROM Bank 0 header only, split into the exact
 * three transactions used by the matching app.  It contains no mapper writes
 * and therefore cannot select an MBC5 bank or access cartridge RAM. */
#ifndef CART_GB_HEADER_PROBE
#define CART_GB_HEADER_PROBE 0
#endif

/* Constrained MBC5 paging diagnostic.  The OS exposes only the exact eight
 * 32-byte logical ROM windows used by the matching app.  The hardware turns
 * each accepted read into a private 0x2000 low-bank select, ROM read, and
 * bank-1 restore; no write service or destructive capability is advertised. */
#ifndef CART_GB_MBC5_BANK_PROBE
#define CART_GB_MBC5_BANK_PROBE 0
#endif

/* Exact 2 MiB MBC5 full-ROM verification policy.  The OS permits only aligned
 * 32-byte ROM reads in that logical range; the matching RTL additionally
 * requires every address exactly once in ascending order.  No public write,
 * erase, program, or save-space operation is exposed. */
#ifndef CART_GB_MBC5_FULLROM_VERIFY
#define CART_GB_MBC5_FULLROM_VERIFY 0
#endif

/* Fixed-command MBC5 NOR identification diagnostic.  It exposes exactly one
 * 32-byte IDENTIFY snapshot.  The matching RTL owns every command address and
 * byte (reset, JEDEC autoselect and CFI query); no public write, erase, program,
 * ROM or save-space transaction is admitted by this policy. */
#ifndef CART_GB_MBC5_IDENTIFY
#define CART_GB_MBC5_IDENTIFY 0
#endif

#if (CART_GB_MBC5_IDENTIFY + CART_GB_MBC5_FULLROM_VERIFY + \
     CART_GB_MBC5_BANK_PROBE + \
     CART_GB_HEADER_PROBE + CART_GBA_HEADER_PROBE + \
     CART_GBA_ROM4K_PROBE + CART_GBA_ADDR_PROBE) > 1
#error "Only one cartridge field-read policy may be enabled"
#endif

#if CART_GB_MBC5_BANK_PROBE
static int svc_cart_gb_mbc5_bank_probe_allowed(uint32_t address) {
    switch (address) {
    case 0x00004000u:
    case 0x00008000u:
    case 0x0007C000u:
    case 0x00080000u:
    case 0x000FC000u:
    case 0x00100000u:
    case 0x001FC000u:
    case 0x001FFFE0u:
        return 1;
    default:
        return 0;
    }
}
#endif

#if CART_GBA_ADDR_PROBE
static int svc_cart_gba_addr_probe_allowed(uint32_t address) {
    switch (address) {
    case 0x000000A0u:
    case 0x000100A0u:
    case 0x000200A0u:
    case 0x000400A0u:
    case 0x000800A0u:
    case 0x001000A0u:
    case 0x002000A0u:
    case 0x004000A0u:
    case 0x008000A0u:
    case 0x010000A0u:
    case 0x01FFFFE0u:
        return 1;
    default:
        return 0;
    }
}
#endif

/* Negotiate the largest complete structure understood by both sides.  This
 * preserves old-app/new-OS operation instead of forcing an older v3 buffer to
 * satisfy sizeof(the current v4 structure). */
static int svc_cart_caps_init(void *out, uint32_t out_size) {
    of_cart_caps_t *caps = (of_cart_caps_t *)out;
    if (!caps || out_size < OF_CART_CAPS_V3_SIZE)
        return OF_ERR_INVALID_PARAM;

    uint32_t result_size = out_size >= OF_CART_CAPS_V5_SIZE ?
                           OF_CART_CAPS_V5_SIZE :
                           out_size >= OF_CART_CAPS_V4_SIZE ?
                           OF_CART_CAPS_V4_SIZE : OF_CART_CAPS_V3_SIZE;
    __builtin_memset(caps, 0, result_size);
    caps->struct_size = result_size;
    caps->version = result_size == OF_CART_CAPS_V5_SIZE ?
                    OF_CART_CAPS_VERSION :
                    result_size == OF_CART_CAPS_V4_SIZE ? 4u : 3u;
    return OF_OK;
}

static void svc_cart_caps_set_space_masks(of_cart_caps_t *caps,
                                          uint32_t out_size,
                                          uint32_t gb_gbc_mask,
                                          uint32_t gba_mask) {
    if (out_size < OF_CART_CAPS_V4_SIZE)
        return;
    caps->gb_gbc_space_mask = gb_gbc_mask;
    caps->gba_space_mask = gba_mask;
}

#if CART_MMIO_ENABLED
static void svc_cart_caps_set_diag(of_cart_caps_t *caps, uint32_t out_size) {
    if (out_size < OF_CART_CAPS_V5_SIZE)
        return;
    caps->hw_features_raw = HW_FEATURES;
    caps->hw_contract_rev = HW_CONTRACT_REV;
    caps->cart_id_raw = CART_ID;
    caps->cart_caps_raw = CART_CAPS;
    caps->cart_status_raw = CART_STATUS;
    caps->cart_error_raw = CART_ERROR;
    caps->cart_profile_raw = CART_PROFILE;
    caps->flash_bytes_raw = CART_FLASH_BYTES;
    caps->sector_bytes_raw = CART_SECTOR_BYTES;
    caps->buffer_bytes_raw = CART_BUFFER_BYTES;
}

static int svc_cart_present(void) {
    return (HW_FEATURES & HW_FEAT_CARTRIDGE) != 0 &&
           CART_ID == CART_ID_CBR1;
}

static uint32_t svc_cart_timeout_cycles(uint32_t timeout_us) {
    uint64_t cycles = ((uint64_t)timeout_us *
                       (uint64_t)CPU_FREQ_HZ) / 1000000u;
    if (cycles == 0)
        cycles = 1;
    if (cycles > 0xFFFFFFFFull)
        cycles = 0xFFFFFFFFull;
    return (uint32_t)cycles;
}

/* UI98 inserts two registered write stages between AXI and a cartridge
 * engine.  Polling the router status/readback is therefore the acknowledgement
 * for a control or selector write; a single immediate load can still observe
 * the previous engine's result. */
static int svc_cart_wait_control(uint32_t timeout_us, int allow_aborted,
                                 int require_done) {
    uint32_t timeout_cycles = svc_cart_timeout_cycles(timeout_us);
    uint64_t deadline = read_cycles() + timeout_cycles;
    uint32_t status;
    do {
        status = CART_STATUS;
        if (!(status & CART_ST_BUSY))
            break;
        if (read_cycles() >= deadline)
            return OF_ERR_TIMEOUT;
    } while (1);

    if (status & CART_ST_TIMEOUT)
        return OF_ERR_TIMEOUT;
    if ((status & CART_ST_ABORTED) && !allow_aborted)
        return OF_ERR_INVALID_STATE;
    if ((status & CART_ST_ERROR) || !(status & CART_ST_IDLE) ||
        (require_done && !(status & CART_ST_DONE)))
        return OF_ERR_IO;
    return OF_OK;
}

static int svc_cart_select(uint32_t bus, uint32_t space) {
    if (CART_STATUS & CART_ST_BUSY)
        return OF_ERR_BUSY;

    CART_BUS = bus;
    CART_SPACE = space;
#if CART_UI98
    uint64_t deadline = read_cycles() + svc_cart_timeout_cycles(1000u);
    while (CART_BUS != bus || CART_SPACE != space) {
        if (read_cycles() >= deadline)
            return OF_ERR_TIMEOUT;
    }
#endif
    return OF_OK;
}

static int svc_cart_get_caps(void *out, uint32_t out_size) {
    of_cart_caps_t *caps = (of_cart_caps_t *)out;
    int rc = svc_cart_caps_init(out, out_size);
    if (rc != OF_OK)
        return rc;

    /* Capture first: diagnostics must survive every fail-closed return below. */
    svc_cart_caps_set_diag(caps, out_size);

    if (!svc_cart_present())
        return OF_ERR_NOT_SUPPORTED;

    uint32_t hw = CART_CAPS;
    uint32_t known = CART_HW_BUS_GB_GBC | CART_HW_BUS_GBA |
                     CART_HW_OP_READ | CART_HW_OP_SAFE |
                     CART_HW_OP_ABORT | CART_HW_OP_QUERY |
                     CART_HW_GB_SPACE_MASK | CART_HW_GBA_SPACE_MASK;
#if CART_S29GL256_PROGRAMMER
    known |= CART_HW_OP_ERASE | CART_HW_OP_PROGRAM;
#endif
#if CART_SRAM138
    known |= (1u << 26); /* Restricted banked parallel-save read extension. */
#endif
    if (hw & ~known)
        return OF_ERR_INVALID_STATE;

    uint32_t gb_gbc_spaces =
        (hw & CART_HW_GB_SPACE_MASK) >> CART_HW_GB_SPACE_SHIFT;
    uint32_t gba_spaces =
        (hw & CART_HW_GBA_SPACE_MASK) >> CART_HW_GBA_SPACE_SHIFT;
    if ((gb_gbc_spaces != 0 &&
         (!(hw & CART_HW_OP_READ) || !(hw & CART_HW_BUS_GB_GBC))) ||
        (gba_spaces != 0 &&
         (!(hw & CART_HW_OP_READ) || !(hw & CART_HW_BUS_GBA))) ||
        ((hw & CART_HW_OP_READ) && (hw & CART_HW_BUS_GB_GBC) &&
         gb_gbc_spaces == 0) ||
        ((hw & CART_HW_OP_READ) && (hw & CART_HW_BUS_GBA) &&
         gba_spaces == 0))
        return OF_ERR_INVALID_STATE;

    caps->bus_mask = hw & (CART_HW_BUS_GB_GBC | CART_HW_BUS_GBA);
    if (hw & CART_HW_OP_SAFE)
        caps->operation_mask |= OF_CART_OP_SAFE_IDLE;
    if (hw & CART_HW_OP_READ)
        caps->operation_mask |= OF_CART_OP_READ;
    if (hw & CART_HW_OP_ABORT)
        caps->operation_mask |= OF_CART_OP_ABORT;
    if (hw & CART_HW_OP_QUERY)
        caps->operation_mask |= OF_CART_OP_QUERY;
#if CART_S29GL256_PROGRAMMER
    if (hw & CART_HW_OP_ERASE)
        caps->operation_mask |= OF_CART_OP_ERASE_SECTOR;
    if (hw & CART_HW_OP_PROGRAM)
        caps->operation_mask |= OF_CART_OP_PROGRAM;
#endif
    caps->max_read_bytes = (hw & CART_HW_OP_READ) ? 32u : 0u;
    caps->writes_enabled =
#if CART_S29GL256_PROGRAMMER
        (hw & (CART_HW_OP_ERASE | CART_HW_OP_PROGRAM)) ==
        (CART_HW_OP_ERASE | CART_HW_OP_PROGRAM);
#else
        0;
#endif
    caps->lifecycle_status = CART_LIFECYCLE;
    caps->host_b1_raw = CART_HOST_B1_RAW;
    caps->host_diag = CART_HOST_DIAG;
    svc_cart_caps_set_space_masks(caps, out_size, gb_gbc_spaces,
                                  gba_spaces);
    return OF_OK;
}

static int svc_cart_enter_safe_idle(void) {
    NOR108_LEGACY_GUARD();
    if (!svc_cart_present())
        return OF_ERR_NOT_SUPPORTED;

    CART_COMMAND = CART_CMD_SAFE_IDLE;
    return svc_cart_wait_control(50000u, 0, 0);
}

static int svc_cart_read(uint32_t bus, uint32_t address, void *dst,
                         uint32_t length, uint32_t timeout_us) {
    NOR108_LEGACY_GUARD();
    if (!dst || length == 0 || timeout_us == 0)
        return OF_ERR_INVALID_PARAM;
    if (!svc_cart_present())
        return OF_ERR_NOT_SUPPORTED;

    uint32_t hw = CART_CAPS;
    uint32_t bus_bit;
    if (bus == OF_CART_BUS_GB_GBC)
        bus_bit = CART_HW_BUS_GB_GBC;
    else if (bus == OF_CART_BUS_GBA)
        bus_bit = CART_HW_BUS_GBA;
    else
        return OF_ERR_INVALID_PARAM;

    /* Capability checks precede every command register write.  The locked
     * smoke profile therefore cannot start a bus cycle even if an app calls
     * this service directly. */
    if (!(hw & CART_HW_OP_READ) || !(hw & bus_bit))
        return OF_ERR_NOT_SUPPORTED;
    if (length > 32u || (bus == OF_CART_BUS_GBA &&
        ((address | length) & 1u)))
        return OF_ERR_BAD_RANGE;
#if CART_GB_MBC5_FULLROM_VERIFY
    if (bus != OF_CART_BUS_GB_GBC || length != 32u ||
        (address & 31u) != 0u || address >= 0x00200000u)
        return OF_ERR_BAD_RANGE;
#elif CART_GB_MBC5_IDENTIFY
    /* Legacy read is ROM-only and deliberately absent from identify91. */
    return OF_ERR_NOT_SUPPORTED;
#elif CART_GB_MBC5_BANK_PROBE
    if (bus != OF_CART_BUS_GB_GBC || length != 32u ||
        !svc_cart_gb_mbc5_bank_probe_allowed(address))
        return OF_ERR_BAD_RANGE;
#elif CART_GB_HEADER_PROBE
    if (bus != OF_CART_BUS_GB_GBC ||
        !((address == 0x00000100u && length == 32u) ||
          (address == 0x00000120u && length == 32u) ||
          (address == 0x00000140u && length == 16u)))
        return OF_ERR_BAD_RANGE;
#elif CART_GBA_HEADER_PROBE
    if (bus != OF_CART_BUS_GBA || address != 0x000000A0u || length != 32u)
        return OF_ERR_BAD_RANGE;
#elif CART_GBA_ROM4K_PROBE
    if (bus != OF_CART_BUS_GBA || length != 32u ||
        (address & 31u) != 0 || address > (4096u - 32u))
        return OF_ERR_BAD_RANGE;
#elif CART_GBA_ADDR_PROBE
    if (bus != OF_CART_BUS_GBA || length != 32u ||
        !svc_cart_gba_addr_probe_allowed(address))
        return OF_ERR_BAD_RANGE;
#endif
    if (CART_STATUS & CART_ST_BUSY)
        return OF_ERR_BUSY;

    uint64_t timeout_cycles =
        ((uint64_t)timeout_us * (uint64_t)CPU_FREQ_HZ) / 1000000u;
    if (timeout_cycles == 0)
        timeout_cycles = 1;
    if (timeout_cycles > 0xFFFFFFFFull)
        timeout_cycles = 0xFFFFFFFFull;

    /* The legacy read service is ROM-only.  Always program the appended
     * selector explicitly so a previous request can never leak its space
     * into a later transaction. */
    int select_rc = svc_cart_select(bus, CART_HW_SPACE_ROM);
    if (select_rc != OF_OK)
        return select_rc;
    CART_ADDRESS = address;
    CART_LENGTH = length;
    CART_TIMEOUT = (uint32_t)timeout_cycles;
    CART_COMMAND = CART_CMD_READ;

    /* The hardware watchdog is authoritative.  The later software deadline
     * is only containment for a wedged or mismatched register block. */
    uint64_t deadline = read_cycles() + timeout_cycles +
                        ((uint64_t)CPU_FREQ_HZ / 1000u);
    uint32_t status;
    do {
        status = CART_STATUS;
        if (!(status & CART_ST_BUSY))
            break;
        if (read_cycles() >= deadline) {
            CART_COMMAND = CART_CMD_ABORT;
            return OF_ERR_TIMEOUT;
        }
    } while (1);

    if (status & CART_ST_TIMEOUT)
        return OF_ERR_TIMEOUT;
    if (status & CART_ST_ABORTED)
        return OF_ERR_INVALID_STATE;
    if ((status & CART_ST_ERROR) || !(status & CART_ST_DONE))
        return OF_ERR_IO;
    if (CART_RX_LEVEL < length)
        return OF_ERR_IO;

    uint8_t *out_bytes = (uint8_t *)dst;
    uint32_t copied = 0;
    while (copied < length) {
        uint32_t available = CART_RX_LEVEL;
        if (available == 0)
            return OF_ERR_IO;
        uint32_t word = CART_RX_DATA;
        uint32_t take = length - copied;
        if (take > 4u)
            take = 4u;
        if (take > available)
            take = available;
        for (uint32_t i = 0; i < take; ++i)
            out_bytes[copied + i] = (uint8_t)(word >> (i * 8u));
        copied += take;
    }
    return OF_OK;
}

static int svc_cart_abort(void) {
    NOR108_LEGACY_GUARD();
    if (!svc_cart_present())
        return OF_ERR_NOT_SUPPORTED;
    CART_COMMAND = CART_CMD_ABORT;
    return svc_cart_wait_control(50000u, 1, 0);
}
#else
/* smoke.16 field boundary: identify the dedicated CBR1 register block, read
 * its capabilities, and permit only SAFE_IDLE and ABORT control commands.
 * All cartridge bus operations remain unreachable. */
static int svc_cart_present(void) {
    return (HW_FEATURES & HW_FEAT_CARTRIDGE) != 0 &&
           CART_ID == CART_ID_CBR1;
}

static int svc_cart_get_caps(void *out, uint32_t out_size) {
    of_cart_caps_t *caps = (of_cart_caps_t *)out;
    int rc = svc_cart_caps_init(out, out_size);
    if (rc != OF_OK)
        return rc;
    if (!svc_cart_present())
        return OF_ERR_NOT_SUPPORTED;

    /* The locked field profile may advertise only safe-idle, abort, and the
     * pin-inert protocol query subset.
     * Any bus/read bit or unknown bit fails closed instead of being exposed
     * to the application. */
    uint32_t hw = CART_CAPS;
    if (hw != (CART_HW_OP_SAFE | CART_HW_OP_ABORT | CART_HW_OP_QUERY))
        return OF_ERR_INVALID_STATE;
    caps->operation_mask = OF_CART_OP_SAFE_IDLE | OF_CART_OP_ABORT |
                           OF_CART_OP_QUERY;
    caps->writes_enabled = 0;
    caps->lifecycle_status = CART_LIFECYCLE;
    caps->host_b1_raw = CART_HOST_B1_RAW;
    caps->host_diag = CART_HOST_DIAG;
    svc_cart_caps_set_space_masks(caps, out_size, 0, 0);
    return OF_OK;
}

static int svc_cart_enter_safe_idle(void) {
    if (!svc_cart_present())
        return OF_ERR_NOT_SUPPORTED;
    CART_COMMAND = CART_CMD_SAFE_IDLE;
    return OF_OK;
}

static int svc_cart_read(uint32_t bus, uint32_t address, void *dst,
                         uint32_t length, uint32_t timeout_us) {
    (void)address;
    if (!dst || length == 0 || timeout_us == 0)
        return OF_ERR_INVALID_PARAM;
    if (bus != OF_CART_BUS_GB_GBC && bus != OF_CART_BUS_GBA)
        return OF_ERR_INVALID_PARAM;
    return OF_ERR_NOT_SUPPORTED;
}

static int svc_cart_abort(void) {
    if (!svc_cart_present())
        return OF_ERR_NOT_SUPPORTED;
    CART_COMMAND = CART_CMD_ABORT;
    return OF_OK;
}
#endif

/* The space selector is part of the transaction contract, not an address
 * decoration.  Only ROM can use the legacy read backend.  Save, EEPROM, and
 * RTC/GPIO paths stay closed until each has its own electrical state machine. */
static int svc_cart_read_space(uint32_t bus, uint32_t space,
                               uint32_t address, void *dst,
                               uint32_t length, uint32_t timeout_us) {
    NOR108_LEGACY_GUARD();
    if (!dst || length == 0 || timeout_us == 0)
        return OF_ERR_INVALID_PARAM;
    if (bus != OF_CART_BUS_GB_GBC && bus != OF_CART_BUS_GBA)
        return OF_ERR_INVALID_PARAM;
    if (space > OF_CART_SPACE_IDENTIFY)
        return OF_ERR_INVALID_PARAM;
    if (bus == OF_CART_BUS_GB_GBC &&
        space != OF_CART_SPACE_ROM &&
        space != OF_CART_SPACE_SAVE_PARALLEL &&
        space != OF_CART_SPACE_IDENTIFY)
        return OF_ERR_INVALID_PARAM;
#if CART_MMIO_ENABLED && (CART_GB_MBC5_IDENTIFY || CART_UI98)
    if ((bus == OF_CART_BUS_GB_GBC
#if CART_UI98
         || bus == OF_CART_BUS_GBA
#endif
        ) &&
        space == OF_CART_SPACE_IDENTIFY) {
#if CART_UI98
        /* UI98 fixed mapper trace: private selector, never arbitrary writes.
         * The RTL independently enforces the eleven-address sequence. */
        /* Private dump aperture: aligned 32-byte sequential reads of 2 MiB.
         * RTL additionally enforces ordering and only writes bank 1..127 to
         * 0x2000, restoring bank 1. No SRAM/RTC/NOR data writes are exposed. */
        int dump = bus == OF_CART_BUS_GB_GBC && address >= 0x200000u && address < 0x400000u;
        int general = bus == OF_CART_BUS_GB_GBC && address >= 0x10000000u && address < 0x18000000u;
        uint32_t kind=(address>>24)&7u;
        static const uint32_t limits[8]={32768,2097152,262144,2097152,4194304,8388608,4194304,1048576};
        int mapper = !general && !dump && bus == OF_CART_BUS_GB_GBC && address >= 0x10000u;
        uint32_t physical = general ? address & 0xffffffu : dump ? address - 0x200000u : mapper ? address - 0x10000u : address;
        if ((!general && !dump && !mapper && address != 0u) || ((general || dump) && (physical & 31u)) ||
            (general && physical>=limits[kind]) ||
            (mapper && physical != 0u && physical != 0x20u &&
             physical != 0x4000u && physical != 0x4020u &&
             physical != 0x100u && physical != 0x120u &&
             physical != 0x140u) || length != 32u)
            return OF_ERR_BAD_RANGE;
#else
        if (address != 0u || length != 32u)
            return OF_ERR_BAD_RANGE;
#endif
        if (!svc_cart_present())
            return OF_ERR_NOT_SUPPORTED;
        uint32_t hw = CART_CAPS;
        uint32_t identify_bit = 1u <<
            ((bus == OF_CART_BUS_GBA ? CART_HW_GBA_SPACE_SHIFT :
              CART_HW_GB_SPACE_SHIFT) + CART_HW_SPACE_IDENTIFY);
        if (!(hw & (bus == OF_CART_BUS_GBA ? CART_HW_BUS_GBA :
                    CART_HW_BUS_GB_GBC)) || !(hw & CART_HW_OP_READ) ||
            !(hw & identify_bit))
            return OF_ERR_NOT_SUPPORTED;
        if (CART_STATUS & CART_ST_BUSY)
            return OF_ERR_BUSY;

        uint64_t timeout_cycles =
            ((uint64_t)timeout_us * (uint64_t)CPU_FREQ_HZ) / 1000000u;
        if (timeout_cycles == 0)
            timeout_cycles = 1;
        if (timeout_cycles > 0xFFFFFFFFull)
            timeout_cycles = 0xFFFFFFFFull;

#if CART_UI98
        uint32_t selected_space = general ? 16u+kind : dump ? 6u : mapper ? 5u : CART_HW_SPACE_IDENTIFY;
#else
        uint32_t selected_space = CART_HW_SPACE_IDENTIFY;
#endif
        int select_rc = svc_cart_select(bus, selected_space);
        if (select_rc != OF_OK)
            return select_rc;
#if CART_UI98
        CART_ADDRESS = physical;
#else
        CART_ADDRESS = 0u;
#endif
        CART_LENGTH = 32u;
        CART_TIMEOUT = (uint32_t)timeout_cycles;
        CART_COMMAND = CART_CMD_READ;

        uint64_t deadline = read_cycles() + timeout_cycles +
                            ((uint64_t)CPU_FREQ_HZ / 1000u);
        uint32_t status;
        do {
            status = CART_STATUS;
            if (!(status & CART_ST_BUSY))
                break;
            if (read_cycles() >= deadline) {
                CART_COMMAND = CART_CMD_ABORT;
                return OF_ERR_TIMEOUT;
            }
        } while (1);
        if (status & CART_ST_TIMEOUT)
            return OF_ERR_TIMEOUT;
        if (status & CART_ST_ABORTED)
            return OF_ERR_INVALID_STATE;
        if ((status & CART_ST_ERROR) || !(status & CART_ST_DONE) ||
            CART_RX_LEVEL < 32u)
            return OF_ERR_IO;

        uint8_t *out_bytes = (uint8_t *)dst;
        for (uint32_t copied = 0; copied < 32u; copied += 4u) {
            if (CART_RX_LEVEL < 4u)
                return OF_ERR_IO;
            uint32_t word = CART_RX_DATA;
            out_bytes[copied] = (uint8_t)word;
            out_bytes[copied + 1u] = (uint8_t)(word >> 8);
            out_bytes[copied + 2u] = (uint8_t)(word >> 16);
            out_bytes[copied + 3u] = (uint8_t)(word >> 24);
        }
        return OF_OK;
    }
#endif
#if CART_SRAM137
#if CART_RTC167
    if(bus==OF_CART_BUS_GBA && space==OF_CART_SPACE_RTC_GPIO)
        return rtc167_read(address,dst,length,timeout_us);
#endif
    if (bus == OF_CART_BUS_GBA && space == OF_CART_SPACE_SAVE_PARALLEL) {
#if CART_SRAM138
#if CART_SAVE139 || CART_EEPROM139
        if (address & OF_CART_SAVE_FLASH128)
            return flash166_read(address, dst, length, timeout_us);
#endif
        if (address & OF_CART_SAVE_ARRAY64)
            return flash165_read(address, dst, length, timeout_us);
        if (address & OF_CART_SAVE_BANKED_1M)
            return sram138_read(address, dst, length, timeout_us);
#endif
        return sram137_read(address, dst, length, timeout_us);
    }
#endif
    if (space != OF_CART_SPACE_ROM)
        return OF_ERR_NOT_SUPPORTED;
    return svc_cart_read(bus, address, dst, length, timeout_us);
}

/* LK-compatible discovery subset.  Only QUERY_FW_INFO and PING are reachable;
 * the hardware rejects every other command and neither command owns cartridge
 * pins. */
static int svc_cart_query_word(uint32_t command, uint32_t argument,
                               uint32_t *response) {
    NOR108_LEGACY_GUARD();
    if (!response)
        return OF_ERR_INVALID_PARAM;
    if (!svc_cart_present())
        return OF_ERR_NOT_SUPPORTED;
    if (!(CART_CAPS & CART_HW_OP_QUERY))
        return OF_ERR_NOT_SUPPORTED;
    if (command != CART_CMD_QUERY_FW_INFO && command != CART_CMD_PING)
        return OF_ERR_INVALID_PARAM;
    if (CART_STATUS & CART_ST_BUSY)
        return OF_ERR_BUSY;

    CART_QUERY_ARG = argument;
    CART_COMMAND = command;
    int rc = svc_cart_wait_control(50000u, 0, 1);
    if (rc != OF_OK)
        return rc;
    *response = CART_QUERY_RESULT;
    return OF_OK;
}

static int svc_cart_query_info(void *out, uint32_t out_size) {
    of_cart_protocol_info_t *info = (of_cart_protocol_info_t *)out;
    if (!info || out_size < sizeof(*info))
        return OF_ERR_INVALID_PARAM;
    __builtin_memset(info, 0, sizeof(*info));
    info->struct_size = sizeof(*info);

    int rc = svc_cart_query_word(CART_CMD_QUERY_FW_INFO, 0, &info->magic);
    if (rc != OF_OK)
        return rc;
    rc = svc_cart_query_word(CART_CMD_QUERY_FW_INFO, 1, &info->version);
    if (rc != OF_OK)
        return rc;
    return svc_cart_query_word(CART_CMD_QUERY_FW_INFO, 2, &info->features);
}

static int svc_cart_ping(uint32_t value, uint32_t *response) {
    return svc_cart_query_word(CART_CMD_PING, value, response);
}

/* ====================================================================== */
/* Fixed-profile S29GL256S flash service                                  */
/* ====================================================================== */

#if CART_MMIO_ENABLED && CART_S29GL256_PROGRAMMER

#define CART_S29GL256_CAPS 0x02203F02u

static int svc_cart_flash_present(void) {
    return svc_cart_present() &&
#if CART_UI98
           CART_CAPS == 0x02313F03u;
#else
           CART_CAPS == CART_S29GL256_CAPS;
#endif
}

static int svc_cart_flash_geometry_present(void) {
    return CART_BUS == OF_CART_BUS_GBA &&
           CART_FLASH_BYTES == 0x02000000u &&
           CART_SECTOR_BYTES == 0x00020000u &&
           CART_BUFFER_BYTES == 512u;
}

static int svc_cart_flash_wait(uint32_t timeout_cycles) {
    uint64_t deadline = read_cycles() + timeout_cycles +
                        ((uint64_t)CPU_FREQ_HZ / 1000u);
    uint32_t status;
    do {
        status = CART_STATUS;
        if (!(status & CART_ST_BUSY))
            break;
        if (read_cycles() >= deadline) {
            CART_COMMAND = CART_CMD_ABORT;
            return OF_ERR_TIMEOUT;
        }
    } while (1);

    if (status & CART_ST_TIMEOUT)
        return OF_ERR_TIMEOUT;
    if (status & CART_ST_ABORTED)
        return OF_ERR_INVALID_STATE;
    if ((status & CART_ST_ERROR) || !(status & CART_ST_DONE))
        return OF_ERR_IO;
    return OF_OK;
}

static int svc_cart_flash_get_caps(void *out, uint32_t out_size) {
    NOR108_LEGACY_GUARD();
    of_cart_flash_caps_t *caps = (of_cart_flash_caps_t *)out;
    if (!caps || out_size < sizeof(*caps))
        return OF_ERR_INVALID_PARAM;
    __builtin_memset(caps, 0, sizeof(*caps));
    if (!svc_cart_flash_present())
        return OF_ERR_NOT_SUPPORTED;

    /* Identification starts from a confirmed inert state and explicitly
     * selects the GBA engine before reading engine-local geometry registers. */
    int rc = svc_cart_enter_safe_idle();
    if (rc != OF_OK)
        return rc;
    rc = svc_cart_select(OF_CART_BUS_GBA, CART_HW_SPACE_IDENTIFY);
    if (rc != OF_OK)
        return rc;
    if (!svc_cart_flash_geometry_present())
        return OF_ERR_NOT_SUPPORTED;

    uint32_t timeout_cycles = svc_cart_timeout_cycles(50000u);
    CART_ADDRESS = 0;
    CART_LENGTH = 32;
    CART_TIMEOUT = timeout_cycles;
    CART_COMMAND = CART_CMD_READ;

    rc = svc_cart_flash_wait(timeout_cycles);
    if (rc != OF_OK)
        return rc;
    if (CART_RX_LEVEL != 32u || !(CART_PROFILE & CART_PROFILE_OK))
        return OF_ERR_NOT_SUPPORTED;

    uint32_t snapshot[8];
    for (uint32_t i = 0; i < 8; ++i)
        snapshot[i] = CART_RX_DATA;

    caps->struct_size = sizeof(*caps);
    caps->version = OF_CART_FLASH_CAPS_VERSION;
    caps->command_set = OF_CART_FLASH_CMDSET_AMD;
    caps->flash_size_bytes = CART_FLASH_BYTES;
    caps->sector_size_bytes = CART_SECTOR_BYTES;
    caps->program_buffer_bytes = CART_BUFFER_BYTES;
    caps->profile_status = OF_CART_FLASH_PROFILE_OK;
    caps->id_prefix = snapshot[2];
    caps->id_tail = snapshot[3];
    caps->cfi_signature = snapshot[4];
    return OF_OK;
}

static int svc_cart_flash_begin(uint32_t command, uint32_t address,
                                uint32_t length, uint32_t timeout_us) {
    NOR108_LEGACY_GUARD();
    if (!timeout_us)
        return OF_ERR_INVALID_PARAM;
    if (!svc_cart_flash_present())
        return OF_ERR_NOT_SUPPORTED;
    int rc = svc_cart_select(OF_CART_BUS_GBA, CART_HW_SPACE_IDENTIFY);
    if (rc != OF_OK)
        return rc;
    if (!svc_cart_flash_geometry_present())
        return OF_ERR_NOT_SUPPORTED;
    if (!(CART_PROFILE & CART_PROFILE_OK))
        return OF_ERR_INVALID_STATE;
    if (CART_STATUS & CART_ST_BUSY)
        return OF_ERR_BUSY;

    uint32_t timeout_cycles = svc_cart_timeout_cycles(timeout_us);
    CART_ADDRESS = address;
    CART_LENGTH = length;
    CART_TIMEOUT = timeout_cycles;
    CART_ARM = CART_ARM_KEY_1;
    CART_ARM = CART_ARM_KEY_2;
    CART_COMMAND = command;
    return svc_cart_flash_wait(timeout_cycles);
}

static int svc_cart_flash_erase_sector(uint32_t address,
                                        uint32_t timeout_us) {
    if (address >= 0x02000000u || (address & 0x0001FFFFu) != 0)
        return OF_ERR_BAD_RANGE;
    return svc_cart_flash_begin(CART_CMD_ERASE_SECTOR, address,
                                0x00020000u, timeout_us);
}

static int svc_cart_flash_program(uint32_t address, const void *src,
                                   uint32_t length,
                                   uint32_t timeout_us) {
    NOR108_LEGACY_GUARD();
    if (!src || !timeout_us)
        return OF_ERR_INVALID_PARAM;
    if (length < 2u || length > 512u || (length & 1u) != 0u ||
        (address & 1u) != 0u || address > 0x02000000u - length ||
        (address & 511u) + length > 512u)
        return OF_ERR_BAD_RANGE;
    if (!svc_cart_flash_present())
        return OF_ERR_NOT_SUPPORTED;
    int rc = svc_cart_select(OF_CART_BUS_GBA, CART_HW_SPACE_IDENTIFY);
    if (rc != OF_OK)
        return rc;
    if (!svc_cart_flash_geometry_present())
        return OF_ERR_NOT_SUPPORTED;
    if (!(CART_PROFILE & CART_PROFILE_OK))
        return OF_ERR_INVALID_STATE;
    if (CART_STATUS & CART_ST_BUSY)
        return OF_ERR_BUSY;

    const uint8_t *bytes = (const uint8_t *)src;
    CART_TX_LEVEL = 0;
    for (uint32_t i = 0; i < length; i += 4) {
        uint32_t word = 0xFFFFFFFFu;
        word = (word & 0xFFFF0000u) |
               (uint32_t)bytes[i] |
               ((uint32_t)bytes[i + 1] << 8);
        if (i + 2u < length)
            word = (word & 0x0000FFFFu) |
                   ((uint32_t)bytes[i + 2] << 16) |
                   ((uint32_t)bytes[i + 3] << 24);
        CART_TX_DATA = word;
    }
    uint32_t staged_length = (length + 3u) & ~3u;
    if (CART_TX_LEVEL != staged_length) {
        CART_COMMAND = CART_CMD_DISARM;
        return OF_ERR_IO;
    }
    return svc_cart_flash_begin(CART_CMD_PROGRAM_32, address, length,
                                timeout_us);
}

static int svc_cart_flash_disarm(void) {
    NOR108_LEGACY_GUARD();
    if (!svc_cart_flash_present())
        return OF_ERR_NOT_SUPPORTED;
    if (CART_STATUS & CART_ST_BUSY) {
        CART_COMMAND = CART_CMD_ABORT;
        return svc_cart_wait_control(50000u, 1, 0);
    }
    int rc = svc_cart_select(OF_CART_BUS_GBA, CART_HW_SPACE_IDENTIFY);
    if (rc != OF_OK)
        return rc;
    CART_COMMAND = CART_CMD_DISARM;
    return svc_cart_wait_control(50000u, 0, 0);
}

#else

static int svc_cart_flash_get_caps(void *out, uint32_t out_size) {
    if (!out || out_size < sizeof(of_cart_flash_caps_t))
        return OF_ERR_INVALID_PARAM;
    __builtin_memset(out, 0, sizeof(of_cart_flash_caps_t));
    return OF_ERR_NOT_SUPPORTED;
}

static int svc_cart_flash_erase_sector(uint32_t address,
                                        uint32_t timeout_us) {
    (void)address; (void)timeout_us; return OF_ERR_NOT_SUPPORTED;
}

static int svc_cart_flash_program(uint32_t address, const void *src,
                                   uint32_t length,
                                   uint32_t timeout_us) {
    (void)address; (void)src; (void)length; (void)timeout_us;
    return OF_ERR_NOT_SUPPORTED;
}

static int svc_cart_flash_disarm(void) {
    return OF_ERR_NOT_SUPPORTED;
}

#endif

/* ======================================================================
 * Table population
 * ====================================================================== */

/* Single source of truth for the app-visible services table. Lives in
 * BSS (zero-initialized at boot), populated by services_table_init(). */
static struct of_services_table g_svc;

const struct of_services_table *services_table_get(void) {
    return &g_svc;
}

void services_table_init(void) {
    struct of_services_table *svc = &g_svc;

    svc->magic   = OF_SVC_MAGIC;
    svc->version = OF_SVC_VERSION;
    svc->count   = (sizeof(struct of_services_table) - 12) / sizeof(void *);

    /* Video */
    svc->video_init             = of_video_init;
    svc->video_get_surface      = of_video_get_surface;
    svc->video_flip             = of_video_flip;
    svc->video_wait_flip        = of_video_wait_flip;
    svc->video_vsync            = of_video_vsync;
    svc->video_set_palette      = svc_video_set_palette;
    svc->video_set_palette_bulk = of_video_set_palette_bulk;
    svc->video_set_palette_vga4 = of_video_set_palette_vga4;
    svc->video_clear            = of_video_clear;
    svc->video_flush_cache      = of_video_flush_cache;
    svc->video_set_display_mode = of_video_set_display_mode;
    svc->video_set_color_mode   = svc_video_set_color_mode;
    svc->video_acquire_next     = of_video_acquire_next;
    svc->video_buffer_addr      = of_video_buffer_addr;

    /* Input */
    svc->input_poll          = of_input_poll;
    svc->input_get_state     = svc_input_get_state;
    svc->input_poll_p0       = (void (*)(void *))of_input_poll_p0;
    svc->input_set_deadzone  = of_input_set_deadzone;

    /* Mixer */
    svc->mixer_init          = of_mixer_init;
    svc->mixer_play          = of_mixer_play;
    svc->mixer_stop          = of_mixer_stop;
    svc->mixer_stop_all      = of_mixer_stop_all;
    svc->mixer_set_volume    = of_mixer_set_volume;
    svc->mixer_set_pan       = of_mixer_set_pan;
    svc->mixer_voice_active  = of_mixer_voice_active;
    svc->mixer_pump          = of_mixer_pump;
    svc->mixer_set_loop      = of_mixer_set_loop;
    svc->mixer_set_rate      = of_mixer_set_rate;
    svc->mixer_set_rate_raw  = of_mixer_set_rate_raw;
    svc->mixer_set_vol_lr    = of_mixer_set_vol_lr;
    svc->mixer_set_bidi      = of_mixer_set_bidi;
    svc->mixer_get_position  = of_mixer_get_position;
    svc->mixer_set_position  = of_mixer_set_position;
    svc->mixer_set_voice     = of_mixer_set_voice;
    svc->mixer_set_voice_raw = of_mixer_set_voice_raw;
    svc->mixer_set_vol_rate  = of_mixer_set_volume_ramp;
    svc->mixer_poll_ended    = of_mixer_poll_ended;
    svc->mixer_alloc_samples = of_mixer_alloc_samples;
    svc->mixer_free_samples  = of_mixer_free_samples;
    svc->mixer_set_end_callback = svc_mixer_set_end_callback;
    svc->mixer_retrigger     = of_mixer_retrigger;
    svc->mixer_play_8bit     = of_mixer_play_8bit;
    svc->mixer_set_group     = of_mixer_set_group;
    svc->mixer_set_group_volume = of_mixer_set_group_volume;
    svc->mixer_set_master_volume = of_mixer_set_master_volume;
    svc->mixer_alloc_for_group = of_mixer_alloc_for_group;
    svc->mixer_voice_group   = of_mixer_voice_group;

    /* Audio */
    svc->audio_init     = of_audio_init;
    svc->audio_write    = of_audio_write;
    svc->audio_get_free = of_audio_get_free;
    svc->audio_stream_open  = of_audio_stream_open;
    svc->audio_stream_write = of_audio_stream_write;
    svc->audio_stream_ready = of_audio_stream_ready;
    svc->audio_stream_close = of_audio_stream_close;

    /* Timer */
    svc->timer_set_callback = svc_timer_set_callback;
    svc->timer_stop         = svc_timer_stop;
    svc->timer_get_us       = of_timer_get_us;
    svc->timer_get_ms       = of_timer_get_ms;
    svc->timer_delay_us     = of_timer_delay_us;

    /* Cache */
    svc->cache_flush       = of_cache_flush;
    svc->cache_clean_range = of_cache_clean_range;
    svc->cache_inval_range = of_cache_inval_range;

    /* Callbacks */
    svc->video_set_vsync_callback = svc_video_set_vsync_callback;

    /* File */
    svc->file_size    = svc_file_size;
    svc->file_size_fd = svc_file_size_fd;

    /* Filesystem */
    svc->file_slot_register = file_slot_register;

    /* SoundFont preload -- filled in by bank_preload() post-init */
    svc->smp_bank_preload_base = NULL;
    svc->smp_bank_preload_size = 0;

    /* Cache (append-only).  cbo.flush per line — writes back AND
     * invalidates dirty lines in the affected range so external AXI
     * masters (GPU m_rd_*, audio mixer voice fetch, …) reading DRAM
     * see the committed data.  Required because cbo.clean alone has
     * been unreliable on this VexiiRiscv config (see
     * project_audio_pan_flood / bank_preload comments). */
    svc->cache_flush_range = of_cache_flush_range;

    /* Input HID extensions */
    svc->input_get_keyboard_state = svc_input_get_keyboard_state;
    svc->input_read_mouse_state   = svc_input_read_mouse_state;

    /* Mixer stable handles */
    svc->mixer_play_h            = of_mixer_play_h;
    svc->mixer_play_8bit_h       = of_mixer_play_8bit_h;
    svc->mixer_alloc_for_group_h = of_mixer_alloc_for_group_h;
    svc->mixer_retrigger_h       = of_mixer_retrigger_h;
    svc->mixer_stop_h            = of_mixer_stop_h;
    svc->mixer_handle_active     = of_mixer_handle_active;
    svc->mixer_handle_group      = of_mixer_handle_group;
    svc->mixer_handle_voice      = of_mixer_handle_voice;
    svc->mixer_set_volume_h      = of_mixer_set_volume_h;
    svc->mixer_set_pan_h         = of_mixer_set_pan_h;
    svc->mixer_set_loop_h        = of_mixer_set_loop_h;
    svc->mixer_set_rate_h        = of_mixer_set_rate_h;
    svc->mixer_set_rate_raw_h    = of_mixer_set_rate_raw_h;
    svc->mixer_set_vol_lr_h      = of_mixer_set_vol_lr_h;
    svc->mixer_set_bidi_h        = of_mixer_set_bidi_h;
    svc->mixer_get_position_h    = of_mixer_get_position_h;
    svc->mixer_set_position_h    = of_mixer_set_position_h;
    svc->mixer_set_voice_h       = of_mixer_set_voice_h;
    svc->mixer_set_voice_raw_h   = of_mixer_set_voice_raw_h;
    svc->mixer_set_vol_rate_h    = of_mixer_set_volume_ramp_h;
    svc->mixer_poll_ended_h      = of_mixer_poll_ended_h;

    /* Video timing snapshot */
    svc->video_get_timing = of_video_get_timing;
    svc->video_set_refresh_vtotal = of_video_set_refresh_vtotal;
    svc->video_set_mode = of_video_set_mode;
    svc->video_get_mode = of_video_get_mode;
    svc->video_get_mode_count = of_video_get_mode_count;
    svc->video_get_mode_info = of_video_get_mode_info;
    svc->video_get_caps = of_video_get_caps;
    svc->video_check_mode = of_video_check_mode;

    /* OS/app configuration */
    svc->config_get = of_config_get;
    svc->config_get_int = of_config_get_int;
    svc->config_get_bool = of_config_get_bool;
    svc->config_next = of_config_next;

    /* File I/O idle hook: lets the app feed audio during blocking SD reads. */
    svc->file_set_idle_hook = of_file_set_idle_hook;

    /* Dock state */
    svc->input_is_docked = of_input_is_docked;

    /* Capability-gated cartridge service. */
    svc->cart_get_caps = svc_cart_get_caps;
    svc->cart_enter_safe_idle = svc_cart_enter_safe_idle;
    svc->cart_read = svc_cart_read;
    svc->cart_abort = svc_cart_abort;
    svc->cart_query_info = svc_cart_query_info;
    svc->cart_ping = svc_cart_ping;
    svc->cart_read_space = svc_cart_read_space;

    /* Generated output files.  NULL in default builds: applications can
     * feature-detect the append-only fields before any operation. */
#if CHISBURNER_OUTPUT_ENABLED
    svc->output_open = svc_output_open;
    svc->output_write = svc_output_write;
    svc->output_flush = svc_output_flush;
#else
    svc->output_open = NULL;
    svc->output_write = NULL;
    svc->output_flush = NULL;
#endif

    svc->cart_flash_get_caps = svc_cart_flash_get_caps;
    svc->cart_flash_erase_sector = svc_cart_flash_erase_sector;
    svc->cart_flash_program = svc_cart_flash_program;
    svc->cart_flash_disarm = svc_cart_flash_disarm;
#if CART_NOR108
    svc->nor_probe = nor108_probe;
#if CART_GB_SAVE175
    svc->gb175_capture = gb175_capture_service;
    svc->gb176_capture = gb176_capture_service;
    svc->gb177_capture = gb177_capture_service;
    svc->gb178_capture = gb178_capture_service;
    svc->gb179_capture = gb179_capture_service;
#if CART_GB_RTC186
    svc->gb186_read = gb186_read_service;
#if CART_GB_RTC189
    svc->gb189_write = gb189_write_service;
#endif
#endif
#if CART_GB_RESTORE180
    svc->gb180_restore = gb180_restore_service;
#endif
#endif
    svc->nor_begin = nor108_begin;
    svc->nor_erase = nor108_erase;
#if CART_RTC172 && CART_RTC167
    svc->rtc172_write = rtc172_write_service;
#endif
    svc->nor_program = nor108_program;
    svc->nor_finish = nor108_finish;
#if CART_EEPROM139
    svc->eeprom139_detect = eeprom139_detect_service;
    svc->eeprom139_capture = eeprom139_capture_service;
#endif
#if CART_SAVE158
    svc->sram160_restore = sram160_restore_service;
#endif
#if CART_EEPROM162
    svc->eeprom162_restore = eeprom162_restore_service;
#ifdef CB_SAVE171
    svc->save171_restore = save171_restore_service;
#endif
#if CART_FLASH170
    svc->flash170_restore = flash170_restore_service;
#endif
#endif
    svc->nor_verify_read = nor108_verify_read;
#if CHISBURNER_PSRAM_CACHE129
    svc->psram_cache_load = svc_psram_cache_load;
    svc->psram_cache_read = svc_psram_cache_read;
    svc->nor_begin_full = nor130_begin_full;
    svc->nor_erase_poll = nor130_erase_poll;
    svc->nor_program_poll = nor130_program_poll;
    svc->nor_verify_full_read = nor130_verify_full_read;
#if CART_STREAM131
    svc->nor_program_next = nor131_program_next;
    svc->nor_program_step = nor131_program_step;
    svc->nor_compare_start = nor131_compare_start;
    svc->nor_compare_result = nor131_compare_result;
#endif
#endif
#endif
}

void services_table_set_smp_bank(const void *base, uint32_t size) {
    g_svc.smp_bank_preload_base = base;
    g_svc.smp_bank_preload_size = size;
}
