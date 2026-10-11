#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/flash166-test
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined tests/test-flash166-os.c -o build/flash166-test/os
build/flash166-test/os
gcc -DCB_FLASH166=1 -std=c11 -Wall -Wextra -Werror -Wno-misleading-indentation -fsanitize=address,undefined tests/test-flash165.c -o build/flash166-test/app
build/flash166-test/app
gcc -DCB_FLASH166=1 -DCB_FLASH165=1 -DCB_ROM164=1 -DCB_EEPROM162=1 -DCB_EEPROM139=1 -DCB_SRAM137=1 -DCB_SRAM138=1 -std=c11 -Wall -Wextra -Werror -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api tests/test-eeprom162-ui.c app/ui98/ui.c app/ui98/model.c app/ui98/font.c app/ui98/analysis.c -o build/flash166-test/ui
build/flash166-test/ui
bash tests/check-flash165.sh
bash tests/check-sram137-os.sh
bash tests/check-eeprom162-service.sh
make -C app/ui98 FLASH166=1 ROM164=1 EEPROM162=1 STREAM131=1 GPU_LEAN132=1 GBA_IDENTIFY133=1 GB_IDENTIFY134=1 TIMERS135=1 BUILD_DIR="$PWD/build/flash166-app" OBJ_DIR="$PWD/build/flash166-app"
cd .integration/openfpgaos/src/firmware/os
make -j4 TARGET=pocket BLD=bld/flash166 \
 EXTRA_CFLAGS='-DCART_MMIO_ENABLED=1 -DCART_UI98=1 -DCART_NOR108=1 -DCART_NOR109=1 -DCART_NOR110=1 -DCART_STREAM131=1 -DCART_SRAM137=1 -DCART_SRAM138=1 -DCART_EEPROM139=1 -DCART_SAVE158=1 -DCART_EEPROM162=1 -DCART_S29GL256_PROGRAMMER=1 -DCHISBURNER_OUTPUT_ENABLED=1 -DCHISBURNER_PSRAM127=1 -DCHISBURNER_PSRAM_CACHE129=1' \
 bld/flash166/os.bin bld/flash166/firmware.mif
