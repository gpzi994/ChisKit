#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-save179-test
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g -Icore \
 tests/test-gb-save179.c core/gb_save175/gb_save175.c core/gb_save175/mbc7179.c \
 -o build/gb-save179-test/core
build/gb-save179-test/core
bash tests/check-gb-save178.sh
gcc -DCB_GB_SAVE179=1 -DCB_GB_SAVE178=1 -DCB_GB_SAVE177=1 -DCB_GB_SAVE176=1 -DCB_GB_SAVE175=1 -DCB_ROM164=1 -DCB_LANG174=1 \
 -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g \
 -I.integration/openfpgaos/src/firmware/api tests/test-gb-save175-app.c \
 app/ui98/ui.c app/ui98/model.c app/ui98/font.c app/ui98/analysis.c \
 -o build/gb-save179-test/app
build/gb-save179-test/app
