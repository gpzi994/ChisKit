#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/rom164-test
gcc -D_DEFAULT_SOURCE -DCB_ROM164=1 -std=c11 -O1 -g -Wall -Wextra -Werror \
 -Wno-unused-function -Wno-misleading-indentation -fsanitize=address,undefined \
 -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api \
 tests/test-rom164.c app/ui98/capacity164.c app/ui98/model.c app/ui98/ui.c \
 app/ui98/font.c app/ui98/analysis.c -o build/rom164-test/capacity-backup
build/rom164-test/capacity-backup "${1:?expanded 32 MiB Emerald fixture}"
gcc -DCB_ROM164=1 -DCB_EEPROM162=1 -DCB_EEPROM139=1 -DCB_SRAM137=1 -DCB_SRAM138=1 \
 -std=c11 -Wall -Wextra -Werror -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api \
 tests/test-eeprom162-ui.c app/ui98/ui.c app/ui98/model.c app/ui98/font.c \
 app/ui98/analysis.c -o build/rom164-test/save-ui
build/rom164-test/save-ui
