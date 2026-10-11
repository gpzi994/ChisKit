#include <stdio.h>
#include <unistd.h>

int main(void) {
    printf("\033[2J\033[H");
    printf("  ChisBurner\n");
    printf("  SDK BASELINE smoke.4\n");
    printf("  =====================\n\n");
    printf("  SDK BASELINE BOOT: PASS\n\n");
    printf("  Core, loader, OS and ELF are running.\n");
    printf("  Cartridge service is not exercised.\n");
    printf("  Keep the cartridge slot empty.\n");

    while (1)
        usleep(100000);
}
