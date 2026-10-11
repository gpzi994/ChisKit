#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-save178-test
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g \
 tests/test-gb-save178.c core/gb_save175/gb_save175.c -o build/gb-save178-test/core
build/gb-save178-test/core
bash tests/check-gb-save177.sh
gcc -DCB_GB_SAVE178=1 -DCB_GB_SAVE177=1 -DCB_GB_SAVE176=1 -DCB_GB_SAVE175=1 -DCB_ROM164=1 -DCB_LANG174=1 \
 -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g \
 -I.integration/openfpgaos/src/firmware/api tests/test-gb-save175-app.c \
 app/ui98/ui.c app/ui98/model.c app/ui98/font.c app/ui98/analysis.c \
 -o build/gb-save178-test/app
build/gb-save178-test/app
