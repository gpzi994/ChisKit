#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "of_cart.h"
#include "sha256.h"

#define ROM_BYTES       2097152u
#define BLOCK_BYTES     32u
#define READ_TIMEOUT_US 50000u

static const uint8_t expected_sha256[32] = {
    0x8b,0xa5,0x33,0xbe,0x59,0x08,0x30,0x5a,
    0x0f,0x2f,0x04,0x11,0x7a,0x85,0x8a,0x91,
    0xd4,0x09,0x8e,0xd8,0xd4,0xd2,0xbe,0x6e,
    0x48,0xc3,0xc3,0xe9,0xdf,0x18,0x85,0xe7
};

static int digest_equal(const uint8_t *a, const uint8_t *b) {
    uint8_t different = 0;
    for (unsigned i = 0; i < 32; ++i)
        different |= a[i] ^ b[i];
    return different == 0;
}

static void print_digest(const uint8_t digest[32]) {
    for (unsigned i = 0; i < 32; ++i)
        printf("%02x", digest[i]);
    putchar('\n');
}

int main(void) {
    of_cart_caps_t caps;
    sha256_ctx_t sha;
    uint8_t block[BLOCK_BYTES];
    uint8_t digest[32] = {0};
    uint8_t header[4] = {0};
    uint32_t completed = 0;
    int read_rc = OF_OK;

    printf("\033[2J\033[H");
    printf("  GB/GBC MBC5 FULL ROM smoke.86\n");
    printf("  2 MiB / 128 BANKS / SHA-256\n");
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

    sha256_init(&sha);
    if (caps_ok && lifecycle_ok && safe_start) {
        for (uint32_t address = 0; address < ROM_BYTES;
             address += BLOCK_BYTES) {
            read_rc = of_cart_read_space(OF_CART_BUS_GB_GBC,
                                         OF_CART_SPACE_ROM, address,
                                         block, BLOCK_BYTES,
                                         READ_TIMEOUT_US);
            if (read_rc != OF_OK)
                break;
            sha256_update(&sha, block, BLOCK_BYTES);
            completed += BLOCK_BYTES;
            if (address <= 0x140u && address + BLOCK_BYTES > 0x149u) {
                header[0] = block[0x143u - address];
                header[1] = block[0x147u - address];
                header[2] = block[0x148u - address];
                header[3] = block[0x149u - address];
            }
            if ((completed & 0x3FFFFu) == 0u) {
                printf("  READ %lu / %u KiB\r",
                       (unsigned long)(completed >> 10), ROM_BYTES >> 10);
                fflush(stdout);
            }
        }
    } else {
        read_rc = OF_ERR_NOT_SUPPORTED;
    }

    if (completed == ROM_BYTES)
        sha256_final(&sha, digest);
    int safe_end = of_cart_enter_safe_idle() == OF_OK;
    int header_ok = header[0] == 0x80u && header[1] == 0x1Bu &&
                    header[2] == 0x06u && header[3] == 0x03u;
    int hash_ok = completed == ROM_BYTES &&
                  digest_equal(digest, expected_sha256);

    printf("\033[2J\033[H");
    printf("  GB/GBC MBC5 FULL ROM smoke.86\n");
    printf("  service/caps          %s\n", caps_ok ? "PASS" : "FAIL");
    printf("  lifecycle/start safe  %s\n",
           lifecycle_ok && safe_start ? "PASS" : "FAIL");
    printf("  exact 2 MiB read      %s\n",
           completed == ROM_BYTES && read_rc == OF_OK ? "PASS" : "FAIL");
    printf("  all 128 banks         %s\n",
           completed == ROM_BYTES ? "PASS" : "FAIL");
    printf("  CGB/MBC5 header       %s\n", header_ok ? "PASS" : "FAIL");
    printf("  SHA-256 exact         %s\n", hash_ok ? "PASS" : "FAIL");
    printf("  final safe idle       %s\n", safe_end ? "PASS" : "FAIL");
    printf("\n  ACTUAL SHA-256\n  ");
    print_digest(digest);
    printf("\n  FULL ROM VERIFY: %s\n",
           caps_ok && lifecycle_ok && safe_start &&
           completed == ROM_BYTES && read_rc == OF_OK &&
           header_ok && hash_ok && safe_end ? "PASS" : "FAIL");
    printf("  GBA PROFILE: UNCHANGED\n");
    printf("  NOR COMMANDS: ABSENT\n");
    fflush(stdout);
    while (1)
        usleep(100000);
#endif
}
