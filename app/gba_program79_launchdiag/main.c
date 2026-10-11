#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "of_cart.h"
#include "of.h"

#define ROM_BYTES79 8705280L
#define B1_PLAY_CART79 (1u << 24)
#define B1_POWER_EXIT79 (1u << 16)
#define LIFE_READY_MASK79 0x80000307u
#define LIFE_READY_VALUE79 0x80000303u

static of_cart_caps_t launch79, target79;
static of_cart_flash_caps_t flash79;
static of_cart_protocol_info_t protocol79;
static uint8_t identify79[32];
static long slot_size79;
static uint32_t ping79;
static int launch_rc79, target_caps_rc79, protocol_rc79, ping_rc79, flash_rc79;
static int safe_rc79, identify_rc79;
static unsigned target_ran79;

static void title79(unsigned page) {
    printf("\033[2J\033[H GBA LAUNCH+ROM DIAG smoke.79b %u/5\n", page);
    printf("READ ONLY; NO FLASH COMMANDS\n");
}

static int launch_ready79(void) {
    const uint32_t required_b1 = B1_PLAY_CART79 | B1_POWER_EXIT79;
    return launch_rc79 == OF_OK &&
           (launch79.host_b1_raw & required_b1) == required_b1 &&
           (launch79.lifecycle_status & LIFE_READY_MASK79) ==
               LIFE_READY_VALUE79;
}

static int rom_ready79(void) {
    return slot_size79 == ROM_BYTES79;
}

static int target_ready79(void) {
    const uint32_t operations = OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
        OF_CART_OP_ABORT | OF_CART_OP_QUERY | OF_CART_OP_ERASE_SECTOR |
        OF_CART_OP_PROGRAM;
    return target_ran79 && target_caps_rc79 == OF_OK &&
           target79.struct_size == sizeof(target79) &&
           target79.version == OF_CART_CAPS_VERSION &&
           target79.bus_mask == OF_CART_BUS_MASK_GBA &&
           target79.operation_mask == operations &&
           target79.writes_enabled == 1u &&
           target79.max_read_bytes == 32u &&
           flash_rc79 == OF_OK &&
           flash79.struct_size == sizeof(flash79) &&
           flash79.version == OF_CART_FLASH_CAPS_VERSION &&
           flash79.command_set == OF_CART_FLASH_CMDSET_AMD &&
           flash79.flash_size_bytes == 0x02000000u &&
           flash79.sector_size_bytes == 0x00020000u &&
           flash79.program_buffer_bytes == 32u &&
           flash79.profile_status == OF_CART_FLASH_PROFILE_OK &&
           flash79.id_prefix == 0x227e0001u &&
           flash79.cfi_signature == 0x00520051u;
}

#ifndef OF_PC
static void refresh_launch79(void) {
    memset(&launch79, 0, sizeof(launch79));
    launch_rc79 = of_cart_get_caps(&launch79);
    slot_size79 = OF_SVC->file_size("slot:4");
}

static void probe_target79(void) {
    memset(&target79, 0, sizeof(target79));
    memset(&flash79, 0, sizeof(flash79));
    memset(&protocol79, 0, sizeof(protocol79));
    memset(identify79, 0, sizeof(identify79));
    protocol_rc79 = of_cart_query_info(&protocol79);
    ping_rc79 = of_cart_ping(0x13579BDFu, &ping79);
    target_caps_rc79 = of_cart_get_caps(&target79);
    flash_rc79 = of_cart_flash_get_caps(&flash79);
    safe_rc79 = of_cart_enter_safe_idle();
    identify_rc79 = of_cart_read_space(OF_CART_BUS_GBA,
                                        OF_CART_SPACE_IDENTIFY, 0,
                                        identify79, sizeof(identify79), 50000);
    target_ran79 = 1;
}
#else
static void fixture79(void) {
    launch_rc79 = OF_OK;
    launch79.host_b1_raw = 0x01018000u;
    launch79.lifecycle_status = LIFE_READY_VALUE79;
    launch79.host_diag = 0x0100013bu;
    slot_size79 = ROM_BYTES79;
    target79.struct_size = sizeof(target79);
    target79.version = OF_CART_CAPS_VERSION;
    target79.bus_mask = OF_CART_BUS_MASK_GBA;
    target79.operation_mask = OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
        OF_CART_OP_ABORT | OF_CART_OP_QUERY | OF_CART_OP_ERASE_SECTOR |
        OF_CART_OP_PROGRAM;
    target79.writes_enabled = 1;
    target79.max_read_bytes = 32;
    target_caps_rc79 = protocol_rc79 = ping_rc79 = flash_rc79 = OF_OK;
    safe_rc79 = identify_rc79 = OF_OK;
    protocol79.magic = OF_CART_PROTOCOL_MAGIC;
    protocol79.version = OF_CART_PROTOCOL_VERSION;
    ping79 = 0xECA86420u;
    flash79.struct_size = sizeof(flash79);
    flash79.version = OF_CART_FLASH_CAPS_VERSION;
    flash79.command_set = OF_CART_FLASH_CMDSET_AMD;
    flash79.flash_size_bytes = 0x02000000u;
    flash79.sector_size_bytes = 0x00020000u;
    flash79.program_buffer_bytes = 32;
    flash79.profile_status = OF_CART_FLASH_PROFILE_OK;
    flash79.id_prefix = 0x227e0001u;
    flash79.cfi_signature = 0x00520051u;
    identify79[0] = 0x01;
    identify79[1] = 0x00;
    identify79[2] = 0x7e;
    identify79[3] = 0x22;
    target_ran79 = 1;
}
#endif

static void page79(unsigned page) {
    title79(page);
    if (page == 1) {
        printf("LAUNCH CAPS RC %d\n", launch_rc79);
        printf("B1 %08lX LIFE %08lX\n",
               (unsigned long)launch79.host_b1_raw,
               (unsigned long)launch79.lifecycle_status);
        printf("HDIAG %08lX\n", (unsigned long)launch79.host_diag);
        printf("PLAY %u POWER %u READY %u\n",
               !!(launch79.host_b1_raw & B1_PLAY_CART79),
               !!(launch79.host_b1_raw & B1_POWER_EXIT79),
               launch_ready79());
        if (!launch_ready79()) {
            printf("QUIT; START WITH PLAY CARTRIDGE\n");
            printf("DO NOT PICK ROM DURING STARTUP\n");
        }
    } else if (page == 2) {
        printf("RUNTIME ROM SLOT 4\n");
        printf("SIZE %ld EXPECT %ld\n", slot_size79, ROM_BYTES79);
        printf("ROM READY %u\n", rom_ready79());
        if (!rom_ready79()) {
            printf("OPEN CORE SETTINGS\n");
            printf("LOAD PROGRAM ROM (SLOT 4)\n");
            printf("THEN PRESS A TO RESCAN\n");
        } else {
            printf("ROM SLOT EXACT SIZE PASS\n");
        }
    } else if (page == 3) {
        printf("TARGET PROBE RAN %u CAPS RC %d\n",
               target_ran79, target_caps_rc79);
        printf("ABI %lu/%lu BUS %08lX\n",
               (unsigned long)target79.struct_size,
               (unsigned long)target79.version,
               (unsigned long)target79.bus_mask);
        printf("OPS %08lX WRITE %lu READ %lu\n",
               (unsigned long)target79.operation_mask,
               (unsigned long)target79.writes_enabled,
               (unsigned long)target79.max_read_bytes);
        printf("STATUS %08lX ERROR %08lX\n",
               (unsigned long)target79.cart_status_raw,
               (unsigned long)target79.cart_error_raw);
    } else if (page == 4) {
        printf("PROTO %d MAGIC %08lX VER %08lX\n",
               protocol_rc79, (unsigned long)protocol79.magic,
               (unsigned long)protocol79.version);
        printf("PING %d VALUE %08lX\n", ping_rc79,
               (unsigned long)ping79);
        printf("FLASH %d CMD %lu PROFILE %08lX\n",
               flash_rc79, (unsigned long)flash79.command_set,
               (unsigned long)flash79.profile_status);
        printf("ID %08lX CFI %08lX\n",
               (unsigned long)flash79.id_prefix,
               (unsigned long)flash79.cfi_signature);
        printf("GEOM %08lX %08lX B%lu\n",
               (unsigned long)flash79.flash_size_bytes,
               (unsigned long)flash79.sector_size_bytes,
               (unsigned long)flash79.program_buffer_bytes);
        printf("SAFE %d IDENT %d %02X%02X%02X%02X\n",
               safe_rc79, identify_rc79, identify79[0], identify79[1],
               identify79[2], identify79[3]);
    } else {
        printf("COMBINED DECISION\n");
        printf("PLAY CARTRIDGE %s\n", launch_ready79() ? "PASS" : "FAIL");
        printf("RUNTIME ROM SLOT %s\n", rom_ready79() ? "PASS" : "FAIL");
        printf("S29GL256S TARGET %s\n", target_ready79() ? "PASS" : "FAIL");
        if (!launch_ready79())
            printf("RELAUNCH USING PLAY CARTRIDGE\n");
        else if (!rom_ready79())
            printf("LOAD ROM IN CORE SETTINGS; A\n");
        else if (!target_ready79())
            printf("SEND PHOTOS OF PAGES 3 AND 4\n");
        else
            printf("ALL READ-ONLY PREFLIGHTS PASS\n");
        printf("NO ARM/ERASE/PROGRAM CALLS EXIST\n");
    }
    printf("LEFT/RIGHT: PAGE; A: RESCAN\n");
    fflush(stdout);
}

int main(void) {
#ifdef OF_PC
    fixture79();
    for (unsigned page = 1; page <= 5; ++page)
        page79(page);
    return 0;
#else
    refresh_launch79();
    if (launch_ready79() && rom_ready79())
        probe_target79();
    unsigned page = 1;
    unsigned previous = ~0u;
    page79(page);
    while (1) {
        of_input_poll_p0();
        unsigned held = _of_input_p0.buttons;
        unsigned edge = held & ~previous;
        previous = held;
        if (edge & OF_BTN_A) {
            refresh_launch79();
            if (launch_ready79() && rom_ready79() && !target_ran79)
                probe_target79();
            page79(page);
        } else if (edge & OF_BTN_RIGHT) {
            page = page == 5 ? 1 : page + 1;
            page79(page);
        } else if (edge & OF_BTN_LEFT) {
            page = page == 1 ? 5 : page - 1;
            page79(page);
        }
        usleep(16000);
    }
#endif
}
