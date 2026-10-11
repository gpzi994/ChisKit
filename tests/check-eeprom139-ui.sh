#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save139
gcc -std=c11 -Wall -Wextra -Werror -DCB_SRAM137=1 -DCB_SRAM138=1 \
 -fsanitize=address,undefined -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api \
 tests/test-eeprom139-ui.c app/ui98/ui.c app/ui98/model.c app/ui98/font.c \
 app/ui98/analysis.c -o build/save139/eeprom-ui-test
build/save139/eeprom-ui-test
gcc -std=c11 -Wall -Wextra -Werror -DCB_SRAM137=1 -DCB_SRAM138=1 -DCB_EEPROM139=1 \
 -fsanitize=address,undefined -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api \
 tests/test-eeprom139-ui.c app/ui98/ui.c app/ui98/model.c app/ui98/font.c \
 app/ui98/analysis.c -o build/save139/eeprom-formal-ui-test
build/save139/eeprom-formal-ui-test
gcc -std=c11 -Wall -Wextra -Werror -DCB_SRAM137=1 -DCB_SRAM138=1 -DCB_EEPROM139_DIAG=1 \
 -fsanitize=address,undefined -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api \
 tests/test-eeprom139-ui.c app/ui98/ui.c app/ui98/model.c app/ui98/font.c \
 app/ui98/analysis.c -o build/save139/eeprom-diag-ui-test
build/save139/eeprom-diag-ui-test
