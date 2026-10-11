#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/lang174-test
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=undefined tests/test-settings174.c -o build/lang174-test/settings
build/lang174-test/settings
gcc -DCB_LANG174=1 -DCB_SAVE171=1 -DCB_RTC173=1 -DCB_FLASH170=1 -DCB_FLASH166=1 -DCB_FLASH165=1 -DCB_ROM164=1 -DCB_EEPROM162=1 -DCB_EEPROM139=1 -DCB_SRAM137=1 -DCB_SRAM138=1 -std=c11 -Wall -Wextra -Werror -fsanitize=undefined -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api tests/test-lang174.c app/ui98/ui.c app/ui98/model.c app/ui98/font.c app/ui98/analysis.c app/gba_db55/db55.c app/gba_db55/db55_data.c -o build/lang174-test/ui
build/lang174-test/ui
