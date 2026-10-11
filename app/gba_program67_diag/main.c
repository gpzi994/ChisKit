#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "of_cart.h"
#include "of.h"

#ifndef D67_BUILD_LABEL
#define D67_BUILD_LABEL "smoke.67"
#endif
#ifndef D67_PLATFORM_LINE
#define D67_PLATFORM_LINE "READ/CFI ONLY; NO ERASE/PROGRAM"
#endif

static of_cart_caps_t before67, after_flash67, after_safe67, final67;
static of_cart_flash_caps_t flash_first67, flash_retry67;
static of_cart_protocol_info_t protocol67;
static uint8_t identify67[32];
static int caps_before_rc67, protocol_rc67, ping_rc67;
static int flash_first_rc67, caps_after_flash_rc67;
static int safe_rc67, caps_after_safe_rc67, identify_rc67;
static int flash_retry_rc67, caps_final_rc67, final_safe_rc67;
static uint32_t ping67;

static int strict_cart67(const of_cart_caps_t *c) {
    const uint32_t ops = OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
        OF_CART_OP_ABORT | OF_CART_OP_QUERY | OF_CART_OP_ERASE_SECTOR |
        OF_CART_OP_PROGRAM;
    return c->struct_size == sizeof(*c) &&
           c->version == OF_CART_CAPS_VERSION &&
           c->bus_mask == OF_CART_BUS_MASK_GBA &&
           c->operation_mask == ops && c->max_read_bytes == 32u &&
           c->writes_enabled == 1u && c->gb_gbc_space_mask == 0u &&
           c->gba_space_mask ==
               (OF_CART_SPACE_MASK_ROM | OF_CART_SPACE_MASK_IDENTIFY) &&
           (c->lifecycle_status & 0x80000307u) == 0x80000303u;
}

static int strict_flash67(const of_cart_flash_caps_t *f) {
    return f->struct_size == sizeof(*f) &&
           f->version == OF_CART_FLASH_CAPS_VERSION &&
           f->command_set == OF_CART_FLASH_CMDSET_AMD &&
           f->flash_size_bytes == 0x02000000u &&
           f->sector_size_bytes == 0x00020000u &&
           f->program_buffer_bytes == 32u &&
           f->profile_status == OF_CART_FLASH_PROFILE_OK &&
           f->id_prefix == 0x227e0001u &&
           f->cfi_signature == 0x00520051u;
}

static void title67(unsigned page) {
    printf("\033[2J\033[H GBA TARGET DIAG %s %u/5\n", D67_BUILD_LABEL, page);
    printf("%s\n", D67_PLATFORM_LINE);
}

static void raw67(const of_cart_caps_t *c) {
    printf("HWFEAT %08lX CONTRACT %lu\n",
           (unsigned long)c->hw_features_raw,
           (unsigned long)c->hw_contract_rev);
    printf("ID %08lX CAPS %08lX\n",
           (unsigned long)c->cart_id_raw,
           (unsigned long)c->cart_caps_raw);
    printf("STATUS %08lX ERROR %08lX\n",
           (unsigned long)c->cart_status_raw,
           (unsigned long)c->cart_error_raw);
    printf("PROFILE %08lX\n", (unsigned long)c->cart_profile_raw);
    printf("GEOM %08lX %08lX %lu\n",
           (unsigned long)c->flash_bytes_raw,
           (unsigned long)c->sector_bytes_raw,
           (unsigned long)c->buffer_bytes_raw);
}

static void page67(unsigned page) {
    title67(page);
    if (page == 1) {
        printf("INITIAL CART_GET_CAPS RC %d\n", caps_before_rc67);
        printf("ABI SIZE %lu VER %lu\n",
               (unsigned long)before67.struct_size,
               (unsigned long)before67.version);
        raw67(&before67);
        printf("BUS %08lX OPS %08lX WRITE %lu\n",
               (unsigned long)before67.bus_mask,
               (unsigned long)before67.operation_mask,
               (unsigned long)before67.writes_enabled);
        printf("SPACES GB %02lX GBA %02lX\n",
               (unsigned long)before67.gb_gbc_space_mask,
               (unsigned long)before67.gba_space_mask);
        printf("MAXREAD %lu LIFE %08lX\n",
               (unsigned long)before67.max_read_bytes,
               (unsigned long)before67.lifecycle_status);
    } else if (page == 2) {
        printf("PROTOCOL RC %d MAGIC %08lX\n", protocol_rc67,
               (unsigned long)protocol67.magic);
        printf("VERSION %08lX FEATURES %08lX\n",
               (unsigned long)protocol67.version,
               (unsigned long)protocol67.features);
        printf("PING RC %d VALUE %08lX\n", ping_rc67,
               (unsigned long)ping67);
        printf("FLASH CAPS FIRST RC %d\n", flash_first_rc67);
        printf("CMDSET %lu PROFILE %08lX\n",
               (unsigned long)flash_first67.command_set,
               (unsigned long)flash_first67.profile_status);
        printf("ID %08lX %08lX CFI %08lX\n",
               (unsigned long)flash_first67.id_prefix,
               (unsigned long)flash_first67.id_tail,
               (unsigned long)flash_first67.cfi_signature);
        printf("ABI %lu/%lu GEOM %08lX %08lX B%lu\n",
               (unsigned long)flash_first67.struct_size,
               (unsigned long)flash_first67.version,
               (unsigned long)flash_first67.flash_size_bytes,
               (unsigned long)flash_first67.sector_size_bytes,
               (unsigned long)flash_first67.program_buffer_bytes);
        printf("POST FLASH CAPS RC %d\n", caps_after_flash_rc67);
    } else if (page == 3) {
        printf("SNAPSHOT AFTER FIRST FLASH QUERY\n");
        raw67(&after_flash67);
        printf("SAFE IDLE RC %d\n", safe_rc67);
        printf("CAPS AFTER SAFE RC %d\n", caps_after_safe_rc67);
        printf("SAFE STATUS %08lX ERROR %08lX\n",
               (unsigned long)after_safe67.cart_status_raw,
               (unsigned long)after_safe67.cart_error_raw);
    } else if (page == 4) {
        printf("IDENTIFY SPACE READ RC %d\n", identify_rc67);
        printf("DATA0 %02X%02X%02X%02X DATA4 %02X%02X%02X%02X\n",
               identify67[0], identify67[1], identify67[2], identify67[3],
               identify67[4], identify67[5], identify67[6], identify67[7]);
        printf("FLASH CAPS RETRY RC %d\n", flash_retry_rc67);
        printf("RETRY ID %08lX CFI %08lX\n",
               (unsigned long)flash_retry67.id_prefix,
               (unsigned long)flash_retry67.cfi_signature);
        printf("FINAL CAPS RC %d SAFE RC %d\n",
               caps_final_rc67, final_safe_rc67);
        printf("FINAL STATUS %08lX ERROR %08lX\n",
               (unsigned long)final67.cart_status_raw,
               (unsigned long)final67.cart_error_raw);
    } else {
        const uint32_t ops = OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
            OF_CART_OP_ABORT | OF_CART_OP_QUERY | OF_CART_OP_ERASE_SECTOR |
            OF_CART_OP_PROGRAM;
        printf("STRICT FIELD DECISION\n");
        printf("C ABI%d BUS%d OPS%d READ%d\n",
               before67.struct_size == sizeof(before67) &&
                   before67.version == OF_CART_CAPS_VERSION,
               before67.bus_mask == OF_CART_BUS_MASK_GBA,
               before67.operation_mask == ops,
               before67.max_read_bytes == 32u);
        printf("C WRITE%d SPACE%d LIFE%d => %s\n",
               before67.writes_enabled == 1u,
               before67.gb_gbc_space_mask == 0u &&
                   before67.gba_space_mask ==
                       (OF_CART_SPACE_MASK_ROM | OF_CART_SPACE_MASK_IDENTIFY),
               (before67.lifecycle_status & 0x80000307u) == 0x80000303u,
               strict_cart67(&before67) ? "PASS" : "FAIL");
        printf("F RC%d ABI%d CMD%d GEOM%d\n", flash_first_rc67,
               flash_first67.struct_size == sizeof(flash_first67) &&
                   flash_first67.version == OF_CART_FLASH_CAPS_VERSION,
               flash_first67.command_set == OF_CART_FLASH_CMDSET_AMD,
               flash_first67.flash_size_bytes == 0x02000000u &&
                   flash_first67.sector_size_bytes == 0x00020000u);
        printf("F BUF%d PROF%d ID%d CFI%d => %s\n",
               flash_first67.program_buffer_bytes == 32u,
               flash_first67.profile_status == OF_CART_FLASH_PROFILE_OK,
               flash_first67.id_prefix == 0x227e0001u,
               flash_first67.cfi_signature == 0x00520051u,
               strict_flash67(&flash_first67) ? "PASS" : "FAIL");
        printf("NO ARM/ERASE/PROGRAM CALLS EXIST\n");
        printf("SEND PHOTOS OF ALL FIVE PAGES\n");
    }
    printf("LEFT/RIGHT: PAGE\n");
    fflush(stdout);
}

#ifndef OF_PC
static void run67(void) {
    caps_before_rc67 = of_cart_get_caps(&before67);
    protocol_rc67 = of_cart_query_info(&protocol67);
    ping_rc67 = of_cart_ping(0x13579BDFu, &ping67);
    flash_first_rc67 = of_cart_flash_get_caps(&flash_first67);
    caps_after_flash_rc67 = of_cart_get_caps(&after_flash67);
    safe_rc67 = of_cart_enter_safe_idle();
    caps_after_safe_rc67 = of_cart_get_caps(&after_safe67);
    identify_rc67 = of_cart_read_space(OF_CART_BUS_GBA,
                                       OF_CART_SPACE_IDENTIFY, 0,
                                       identify67, sizeof(identify67), 50000);
    flash_retry_rc67 = of_cart_flash_get_caps(&flash_retry67);
    caps_final_rc67 = of_cart_get_caps(&final67);
    final_safe_rc67 = of_cart_enter_safe_idle();
}
#else
static void fixture67(void) {
    caps_before_rc67 = 0;
    before67.struct_size = OF_CART_CAPS_V5_SIZE;
    before67.version = OF_CART_CAPS_VERSION;
    before67.hw_features_raw = 2;
    before67.hw_contract_rev = 4;
    before67.cart_id_raw = 0x31424746u;
    before67.cart_caps_raw = 0x02203F02u;
    before67.cart_status_raw = 1;
    before67.cart_profile_raw = 1;
    before67.flash_bytes_raw = 0x02000000u;
    before67.sector_bytes_raw = 0x00020000u;
    before67.buffer_bytes_raw = 32;
    protocol67.magic = OF_CART_PROTOCOL_MAGIC;
    protocol67.version = OF_CART_PROTOCOL_VERSION;
    protocol67.features = OF_CART_PROTOCOL_QUERY | OF_CART_PROTOCOL_PING;
    ping67 = 0xECA86420u;
    flash_first_rc67 = OF_ERR_INVALID_STATE;
    after_flash67 = before67; after_flash67.cart_status_raw = 0x11;
    after_flash67.cart_error_raw = 3;
    after_safe67 = after_flash67; after_safe67.cart_status_raw = 1;
    final67 = after_flash67;
    identify_rc67 = OF_ERR_INVALID_STATE;
    flash_retry_rc67 = OF_ERR_INVALID_STATE;
}
#endif

int main(void) {
#ifdef OF_PC
    fixture67();
    for (unsigned page = 1; page <= 5; ++page) page67(page);
    return 0;
#else
    title67(1);
    printf("TAKING READ-ONLY HARDWARE SNAPSHOTS\n");
    printf("DO NOT REMOVE CARTRIDGE\n");
    fflush(stdout);
    run67();
    unsigned page = 1, previous = ~0u;
    page67(page);
    while (1) {
        of_input_poll_p0();
        unsigned held = _of_input_p0.buttons;
        unsigned edge = held & ~previous;
        previous = held;
        if (edge & OF_BTN_RIGHT) {
            page = page == 5 ? 1 : page + 1;
            page67(page);
        } else if (edge & OF_BTN_LEFT) {
            page = page == 1 ? 5 : page - 1;
            page67(page);
        }
        usleep(16000);
    }
#endif
}
