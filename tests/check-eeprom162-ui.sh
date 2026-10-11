#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/eeprom162-test
gcc -std=c11 -Wall -Wextra -Werror -DCB_EEPROM162=1 -DCB_EEPROM139=1 \
 -DCB_SRAM137=1 -DCB_SRAM138=1 -Iapp/ui98 \
 -I.integration/openfpgaos/src/firmware/api \
 tests/test-eeprom162-ui.c app/ui98/ui.c app/ui98/model.c \
 app/ui98/font.c app/ui98/analysis.c -o build/eeprom162-test/ui
build/eeprom162-test/ui
