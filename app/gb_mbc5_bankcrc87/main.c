#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "of_cart.h"
#include "bank_crc32.h"

#define ROM_BYTES       2097152u
#define BANK_BYTES      16384u
#define BLOCK_BYTES     32u
#define READ_TIMEOUT_US 50000u

int main(void) {
    of_cart_caps_t caps;
    uint8_t block[BLOCK_BYTES];
    uint8_t header[4] = {0};
    uint32_t completed = 0;
    uint32_t bank_crc = 0xffffffffu;
    uint32_t bad_banks = 0;
    uint32_t first_bad = 0xffffffffu;
    uint32_t first_actual = 0;
    uint32_t first_expected = 0;
    int read_rc = OF_OK;

    printf("\033[2J\033[H");
    printf("  GB/GBC MBC5 BANK CRC smoke.87\n");
    printf("  128 BANK CRC32 DIAGNOSTIC\n");
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

    if (caps_ok && lifecycle_ok && safe_start) {
        for (uint32_t address = 0; address < ROM_BYTES;
             address += BLOCK_BYTES) {
            read_rc = of_cart_read_space(OF_CART_BUS_GB_GBC,
                                         OF_CART_SPACE_ROM, address,
                                         block, BLOCK_BYTES,
                                         READ_TIMEOUT_US);
            if (read_rc != OF_OK)
                break;

            bank_crc = bank_crc32_update(bank_crc, block, BLOCK_BYTES);
            completed += BLOCK_BYTES;

            if (address <= 0x140u && address + BLOCK_BYTES > 0x149u) {
                header[0] = block[0x143u - address];
                header[1] = block[0x147u - address];
                header[2] = block[0x148u - address];
                header[3] = block[0x149u - address];
            }

            if ((completed & (BANK_BYTES - 1u)) == 0u) {
                uint32_t bank = (completed - 1u) / BANK_BYTES;
                uint32_t actual = bank_crc ^ 0xffffffffu;
                if (actual != mbc5_expected_bank_crc32[bank]) {
                    ++bad_banks;
                    if (first_bad == 0xffffffffu) {
                        first_bad = bank;
                        first_actual = actual;
                        first_expected = mbc5_expected_bank_crc32[bank];
                    }
                }
                bank_crc = 0xffffffffu;
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
    int header_ok = header[0] == 0x80u && header[1] == 0x1bu &&
                    header[2] == 0x06u && header[3] == 0x03u;
    int crc_ok = full_read && bad_banks == 0;

    printf("\033[2J\033[H");
    printf("  GB/GBC MBC5 BANK CRC smoke.87\n");
    printf("  service/caps          %s\n", caps_ok ? "PASS" : "FAIL");
    printf("  lifecycle/start safe  %s\n",
           lifecycle_ok && safe_start ? "PASS" : "FAIL");
    printf("  exact 2 MiB read      %s\n", full_read ? "PASS" : "FAIL");
    printf("  CGB/MBC5 header       %s\n", header_ok ? "PASS" : "FAIL");
    printf("  128 bank CRC32        %s\n", crc_ok ? "PASS" : "FAIL");
    printf("  final safe idle       %s\n", safe_end ? "PASS" : "FAIL");
    printf("\n  BAD BANKS: %lu\n", (unsigned long)bad_banks);
    if (bad_banks) {
        printf("  FIRST BAD BANK: %lu (0x%02lx)\n",
               (unsigned long)first_bad, (unsigned long)first_bad);
        printf("  ACTUAL CRC: %08lx\n", (unsigned long)first_actual);
        printf("  EXPECT CRC: %08lx\n", (unsigned long)first_expected);
    } else if (full_read) {
        printf("  ALL BANK DATA MATCHES FILE\n");
    } else {
        printf("  READ INCOMPLETE AT: %lu KiB\n",
               (unsigned long)(completed >> 10));
    }
    printf("\n  CRC DIAG: %s\n",
           caps_ok && lifecycle_ok && safe_start && full_read &&
           header_ok && crc_ok && safe_end ? "PASS" : "FAIL");
    printf("  GBA PROFILE: UNCHANGED\n");
    printf("  NOR COMMANDS: ABSENT\n");
    fflush(stdout);
    while (1)
        usleep(100000);
#endif
}
