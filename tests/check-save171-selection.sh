#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save171-test
gcc -DCB_ROM164=1 -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api \
 tests/test-save171-selection.c app/gba_db55/db55.c app/gba_db55/db55_data.c \
 -o build/save171-test/selection
build/save171-test/selection
gcc -DCB_SAVE171=1 -DCB_ROM164=1 -DCB_FLASH170=1 -DCB_FLASH166=1 \
 -DCB_EEPROM162=1 -DCB_SRAM138=1 -DCB_SRAM137=1 -DCB_EEPROM139=1 \
 -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api \
 tests/test-save171-ui.c app/ui98/ui.c app/ui98/model.c app/ui98/font.c \
 app/ui98/analysis.c app/gba_db55/db55.c app/gba_db55/db55_data.c \
 -o build/save171-test/ui
build/save171-test/ui
