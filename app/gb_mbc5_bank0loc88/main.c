#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "of_cart.h"
#include "chunk_crc32.h"

#define ROM_BYTES       2097152u
#define BANK0_BYTES     16384u
#define BLOCK_BYTES     32u
#define READ_TIMEOUT_US 50000u

#ifndef SMOKE_LABEL
#define SMOKE_LABEL "smoke.88"
#endif

#ifndef INITIAL_SETTLE_US
#define INITIAL_SETTLE_US 0u
#endif

int main(void) {
    of_cart_caps_t caps;
    uint8_t block[BLOCK_BYTES];
    uint8_t first_data[BLOCK_BYTES] = {0};
    uint32_t completed = 0;
    uint32_t bank0_crc = 0xffffffffu;
    uint32_t bad_chunks = 0;
    uint32_t first_bad_address = 0xffffffffu;
    uint32_t last_bad_address = 0xffffffffu;
    uint32_t first_actual_crc = 0;
    uint32_t first_expect_crc = 0;
    int read_rc = OF_OK;

    printf("\033[2J\033[H");
    printf("  GB/GBC BANK0 LOCATOR %s\n", SMOKE_LABEL);
    printf("  32-BYTE CRC / READ ONLY\n");
    printf("  NO NOR ERASE OR PROGRAM\n\n");

#ifdef OF_PC
    printf("  PC fallback closed: %s\n",
           of_cart_available() == 0 ? "PASS" : "FAIL");
    return 0;
#else
    int caps_rc = of_cart_get_caps(&caps);
    int caps_ok = caps_rc == OF_OK &&
                  caps.version == OF_CART_CAPS_VERSION &&
                  caps.bus_mask == OF_CART_BUS_MASK_GB_GBC &&
                  caps.operation_mask == (OF_CART_OP_SAFE_IDLE | OF_CART_OP_READ |
                                          OF_CART_OP_ABORT | OF_CART_OP_QUERY) &&
                  caps.max_read_bytes == BLOCK_BYTES &&
                  caps.writes_enabled == 0 &&
                  caps.gb_gbc_space_mask == OF_CART_SPACE_MASK_ROM &&
                  caps.gba_space_mask == 0;
    int lifecycle_ok = caps_rc == OF_OK &&
        (caps.lifecycle_status &
         (OF_CART_LIFECYCLE_STATE_MASK | OF_CART_LIFECYCLE_PLAY |
          OF_CART_LIFECYCLE_POWER | OF_CART_LIFECYCLE_READ_READY)) ==
        (OF_CART_LIFECYCLE_READY | OF_CART_LIFECYCLE_PLAY |
         OF_CART_LIFECYCLE_POWER | OF_CART_LIFECYCLE_READ_READY);
    int safe_start = of_cart_enter_safe_idle() == OF_OK;

    if (safe_start && INITIAL_SETTLE_US != 0u)
        usleep(INITIAL_SETTLE_US);

    if (caps_ok && lifecycle_ok && safe_start) {
        for (uint32_t address = 0; address < ROM_BYTES;
             address += BLOCK_BYTES) {
            read_rc = of_cart_read_space(OF_CART_BUS_GB_GBC,
                                         OF_CART_SPACE_ROM, address,
                                         block, BLOCK_BYTES,
                                         READ_TIMEOUT_US);
            if (read_rc != OF_OK)
                break;
            completed += BLOCK_BYTES;

            if (address < BANK0_BYTES) {
                uint32_t chunk = address / BLOCK_BYTES;
                uint32_t crc = chunk_crc32_update(0xffffffffu, block,
                                                  BLOCK_BYTES) ^ 0xffffffffu;
                bank0_crc = chunk_crc32_update(bank0_crc, block, BLOCK_BYTES);
                if (crc != bank0_expected_chunk_crc32[chunk]) {
                    ++bad_chunks;
                    last_bad_address = address;
                    if (first_bad_address == 0xffffffffu) {
                        first_bad_address = address;
                        first_actual_crc = crc;
                        first_expect_crc = bank0_expected_chunk_crc32[chunk];
                        for (unsigned i = 0; i < BLOCK_BYTES; ++i)
                            first_data[i] = block[i];
                    }
                }
            }

            if ((completed & 0x3ffffu) == 0u) {
                printf("  READ %lu / %u KiB\r",
                       (unsigned long)(completed >> 10), ROM_BYTES >> 10);
                fflush(stdout);
            }
        }
    } else {
        read_rc = OF_ERR_NOT_SUPPORTED;
    }

    int safe_end = of_cart_enter_safe_idle() == OF_OK;
    int full_read = completed == ROM_BYTES && read_rc == OF_OK;
    uint32_t bank0_actual = bank0_crc ^ 0xffffffffu;
    int bank0_ok = full_read && bank0_actual == 0xd0a2a03fu;
    int chunks_ok = full_read && bad_chunks == 0;

    printf("\033[2J\033[H");
    printf("  GB/GBC BANK0 LOCATOR %s\n", SMOKE_LABEL);
    printf("  service/caps    %s\n", caps_ok ? "PASS" : "FAIL");
    printf("  lifecycle/safe  %s\n",
           lifecycle_ok && safe_start ? "PASS" : "FAIL");
    if (INITIAL_SETTLE_US != 0u)
        printf("  initial settle  %lu ms\n",
               (unsigned long)(INITIAL_SETTLE_US / 1000u));
    printf("  exact 2 MiB     %s\n", full_read ? "PASS" : "FAIL");
    printf("  bank0 CRC32     %s  %08lx\n",
           bank0_ok ? "PASS" : "FAIL", (unsigned long)bank0_actual);
    printf("  32-byte chunks  %s\n", chunks_ok ? "PASS" : "FAIL");
    printf("  final safe      %s\n", safe_end ? "PASS" : "FAIL");

    printf("\n  BAD CHUNKS: %lu\n", (unsigned long)bad_chunks);
    if (bad_chunks) {
        printf("  FIRST: %04lx  LAST: %04lx\n",
               (unsigned long)first_bad_address,
               (unsigned long)last_bad_address);
        printf("  CRC ACT %08lx REF %08lx\n",
               (unsigned long)first_actual_crc,
               (unsigned long)first_expect_crc);
        printf("  FIRST BAD BLOCK DATA:\n  ");
        for (unsigned i = 0; i < BLOCK_BYTES; ++i) {
            printf("%02x", first_data[i]);
            if ((i & 7u) == 7u && i + 1u < BLOCK_BYTES)
                printf("\n  ");
            else if (i + 1u < BLOCK_BYTES)
                putchar(' ');
        }
        putchar('\n');
    } else if (full_read) {
        printf("  ALL BANK0 CHUNKS MATCH\n");
    } else {
        printf("  READ INCOMPLETE AT %lu KiB\n",
               (unsigned long)(completed >> 10));
    }
    printf("\n  GBA: UNCHANGED  NOR: ABSENT\n");
    fflush(stdout);
    while (1)
        usleep(100000);
#endif
}
