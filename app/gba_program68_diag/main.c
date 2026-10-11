#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "of_cart.h"
#include "of.h"

typedef struct hw_snapshot68 {
    uint32_t hw_features;
    uint32_t hw_contract;
    uint32_t id;
    uint32_t caps;
    uint32_t status;
    uint32_t error;
    uint32_t lifecycle;
    uint32_t host_b1;
    uint32_t host_diag;
    uint32_t profile;
    uint32_t flash_bytes;
    uint32_t sector_bytes;
    uint32_t buffer_bytes;
} hw_snapshot68_t;

static of_cart_caps_t caps68;
static of_cart_flash_caps_t flash_first68, flash_retry68;
static of_cart_protocol_info_t protocol68;
static hw_snapshot68_t initial68, post_flash68, post_safe68, final68;
static uint8_t identify68[32];
static int caps_rc68, protocol_rc68, ping_rc68, flash_first_rc68;
static int safe_rc68, identify_rc68, flash_retry_rc68, final_safe_rc68;
static uint32_t ping68;

#ifndef OF_PC
#define D68_REG32(a) (*(volatile uint32_t *)(uintptr_t)(a))
#define D68_SYSREG_BASE     0x40000000u
#define D68_CART_BASE       (D68_SYSREG_BASE + 0x1a0u)
#define D68_PROGRAM_BASE    (D68_SYSREG_BASE + 0x184u)

static void snapshot68(hw_snapshot68_t *s) {
    s->hw_features = D68_REG32(D68_SYSREG_BASE + 0x98u);
    s->hw_contract = D68_REG32(D68_SYSREG_BASE + 0x8cu);
    s->id = D68_REG32(D68_CART_BASE + 0x00u);
    s->caps = D68_REG32(D68_CART_BASE + 0x04u);
    s->status = D68_REG32(D68_CART_BASE + 0x08u);
    s->error = D68_REG32(D68_CART_BASE + 0x28u);
    s->lifecycle = D68_REG32(D68_CART_BASE + 0x2cu);
    s->host_b1 = D68_REG32(D68_CART_BASE + 0x30u);
    s->host_diag = D68_REG32(D68_CART_BASE + 0x34u);
    s->profile = D68_REG32(D68_PROGRAM_BASE + 0x0cu);
    s->flash_bytes = D68_REG32(D68_PROGRAM_BASE + 0x10u);
    s->sector_bytes = D68_REG32(D68_PROGRAM_BASE + 0x14u);
    s->buffer_bytes = D68_REG32(D68_PROGRAM_BASE + 0x18u);
}
#endif

static void title68(unsigned page) {
    printf("\033[2J\033[H GBA TARGET DIAG smoke.68 %u/5\n", page);
    printf("KNOWN-GOOD smoke.66 OS; READ ONLY\n");
}

static void raw68(const hw_snapshot68_t *s) {
    printf("HWFEAT %08lX CONTRACT %lu\n",
           (unsigned long)s->hw_features, (unsigned long)s->hw_contract);
    printf("ID %08lX CAPS %08lX\n",
           (unsigned long)s->id, (unsigned long)s->caps);
    printf("STATUS %08lX ERROR %08lX\n",
           (unsigned long)s->status, (unsigned long)s->error);
    printf("LIFE %08lX B1 %08lX\n",
           (unsigned long)s->lifecycle, (unsigned long)s->host_b1);
    printf("HDIAG %08lX PROFILE %08lX\n",
           (unsigned long)s->host_diag, (unsigned long)s->profile);
    printf("GEOM %08lX %08lX %lu\n",
           (unsigned long)s->flash_bytes, (unsigned long)s->sector_bytes,
           (unsigned long)s->buffer_bytes);
}

static void page68(unsigned page) {
    title68(page);
    if (page == 1) {
        printf("INITIAL DIRECT MMIO SNAPSHOT\n");
        raw68(&initial68);
        printf("CART_GET_CAPS RC %d\n", caps_rc68);
        printf("CAPS ABI SIZE %lu VER %lu\n",
               (unsigned long)caps68.struct_size,
               (unsigned long)caps68.version);
    } else if (page == 2) {
        printf("PROTOCOL RC %d MAGIC %08lX\n", protocol_rc68,
               (unsigned long)protocol68.magic);
        printf("VERSION %08lX FEATURES %08lX\n",
               (unsigned long)protocol68.version,
               (unsigned long)protocol68.features);
        printf("PING RC %d VALUE %08lX\n", ping_rc68,
               (unsigned long)ping68);
        printf("FLASH CAPS FIRST RC %d\n", flash_first_rc68);
        printf("CMDSET %lu PROFILE %08lX\n",
               (unsigned long)flash_first68.command_set,
               (unsigned long)flash_first68.profile_status);
        printf("ID %08lX %08lX CFI %08lX\n",
               (unsigned long)flash_first68.id_prefix,
               (unsigned long)flash_first68.id_tail,
               (unsigned long)flash_first68.cfi_signature);
    } else if (page == 3) {
        printf("AFTER FIRST FLASH CAPS QUERY\n");
        raw68(&post_flash68);
        printf("SAFE IDLE RC %d\n", safe_rc68);
        printf("AFTER SAFE STATUS/ERROR\n");
        printf("%08lX %08lX PROFILE %08lX\n",
               (unsigned long)post_safe68.status,
               (unsigned long)post_safe68.error,
               (unsigned long)post_safe68.profile);
    } else if (page == 4) {
        printf("IDENTIFY SPACE READ RC %d\n", identify_rc68);
        printf("DATA0 %02X%02X%02X%02X DATA4 %02X%02X%02X%02X\n",
               identify68[0], identify68[1], identify68[2], identify68[3],
               identify68[4], identify68[5], identify68[6], identify68[7]);
        printf("FLASH CAPS RETRY RC %d\n", flash_retry_rc68);
        printf("RETRY ID %08lX CFI %08lX\n",
               (unsigned long)flash_retry68.id_prefix,
               (unsigned long)flash_retry68.cfi_signature);
        printf("FINAL SAFE RC %d\n", final_safe_rc68);
        printf("FINAL STATUS %08lX ERROR %08lX\n",
               (unsigned long)final68.status,
               (unsigned long)final68.error);
    } else {
        printf("DECISION SUMMARY\n");
        printf("CART CAPS %d FIRST CFI %d\n",
               caps_rc68, flash_first_rc68);
        printf("IDENTIFY READ %d CFI RETRY %d\n",
               identify_rc68, flash_retry_rc68);
        if (caps_rc68)
            printf("FAIL AT CART CAPABILITY CONTRACT\n");
        else if (flash_first_rc68)
            printf("CAPS PASS; FAIL AT FLASH IDENTIFY\n");
        else
            printf("CAPS AND FLASH IDENTIFY PASS\n");
        printf("NO ARM/ERASE/PROGRAM CALLS\n");
        printf("SEND PHOTOS OF ALL FIVE PAGES\n");
    }
    printf("LEFT/RIGHT: PAGE\n");
    fflush(stdout);
}

#ifndef OF_PC
static void run68(void) {
    snapshot68(&initial68);
    caps_rc68 = of_cart_get_caps(&caps68);
    protocol_rc68 = of_cart_query_info(&protocol68);
    ping_rc68 = of_cart_ping(0x13579bdfu, &ping68);
    flash_first_rc68 = of_cart_flash_get_caps(&flash_first68);
    snapshot68(&post_flash68);
    safe_rc68 = of_cart_enter_safe_idle();
    snapshot68(&post_safe68);
    identify_rc68 = of_cart_read_space(OF_CART_BUS_GBA,
                                       OF_CART_SPACE_IDENTIFY, 0,
                                       identify68, sizeof(identify68), 50000);
    flash_retry_rc68 = of_cart_flash_get_caps(&flash_retry68);
    snapshot68(&final68);
    final_safe_rc68 = of_cart_enter_safe_idle();
}
#else
static void fixture68(void) {
    initial68.hw_features = 0x22u;
    initial68.hw_contract = 4u;
    initial68.id = 0x31424746u;
    initial68.caps = 0x02203f02u;
    initial68.status = 1u;
    initial68.lifecycle = 0x81000203u;
    initial68.profile = 1u;
    initial68.flash_bytes = 0x02000000u;
    initial68.sector_bytes = 0x00020000u;
    initial68.buffer_bytes = 32u;
    post_flash68 = initial68;
    post_flash68.status = 0x11u;
    post_flash68.error = 3u;
    post_safe68 = initial68;
    final68 = post_flash68;
    caps68.struct_size = OF_CART_CAPS_V4_SIZE;
    caps68.version = 4u;
    protocol68.magic = OF_CART_PROTOCOL_MAGIC;
    protocol68.version = OF_CART_PROTOCOL_VERSION;
    protocol68.features = OF_CART_PROTOCOL_QUERY | OF_CART_PROTOCOL_PING;
    ping68 = 0xeca86420u;
    flash_first_rc68 = OF_ERR_INVALID_STATE;
    identify_rc68 = OF_ERR_INVALID_STATE;
    flash_retry_rc68 = OF_ERR_INVALID_STATE;
}
#endif

int main(void) {
#ifdef OF_PC
    fixture68();
    for (unsigned page = 1; page <= 5; ++page) page68(page);
    return 0;
#else
    title68(1);
    printf("TAKING READ-ONLY HARDWARE SNAPSHOTS\n");
    printf("DO NOT REMOVE CARTRIDGE\n");
    fflush(stdout);
    run68();
    unsigned page = 1, previous = ~0u;
    page68(page);
    while (1) {
        of_input_poll_p0();
        unsigned held = _of_input_p0.buttons;
        unsigned edge = held & ~previous;
        previous = held;
        if (edge & OF_BTN_RIGHT) {
            page = page == 5 ? 1 : page + 1;
            page68(page);
        } else if (edge & OF_BTN_LEFT) {
            page = page == 1 ? 5 : page - 1;
            page68(page);
        }
        usleep(16000);
    }
#endif
}
