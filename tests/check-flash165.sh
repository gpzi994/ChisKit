#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/flash165-test
gcc -std=c11 -Wall -Wextra -Werror -Wno-misleading-indentation -fsanitize=address,undefined tests/test-flash165.c -o build/flash165-test/backup
build/flash165-test/backup
gcc -DCB_FLASH165=1 -DCB_ROM164=1 -DCB_EEPROM162=1 -DCB_EEPROM139=1 -DCB_SRAM137=1 -DCB_SRAM138=1 -std=c11 -Wall -Wextra -Werror -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api tests/test-eeprom162-ui.c app/ui98/ui.c app/ui98/model.c app/ui98/font.c app/ui98/analysis.c -o build/flash165-test/ui
build/flash165-test/ui
