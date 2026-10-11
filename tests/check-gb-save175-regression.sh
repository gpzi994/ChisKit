#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
# Run the unchanged legacy ROM suite with the new GB aperture enabled too.
function iverilog(){ command iverilog -DCHISBURNER_GB_SAVE175 "$@"; }
export -f iverilog
bash tests/check-rom-stream131.sh
gcc -DCB_GB_SAVE175=1 -DCB_SAVE171=1 -DCB_ROM164=1 -DCB_FLASH170=1 -DCB_FLASH166=1 \
 -DCB_EEPROM162=1 -DCB_SRAM138=1 -DCB_SRAM137=1 -DCB_EEPROM139=1 \
 -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api \
 tests/test-save171-ui.c app/ui98/ui.c app/ui98/model.c app/ui98/font.c \
 app/ui98/analysis.c app/gba_db55/db55.c app/gba_db55/db55_data.c \
 -o build/gb-save175-test/gba-ui-enabled
build/gb-save175-test/gba-ui-enabled
