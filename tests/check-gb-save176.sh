#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-save176-test
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g \
 tests/test-gb-save176.c core/gb_save175/gb_save175.c \
 -o build/gb-save176-test/gb-save176
build/gb-save176-test/gb-save176
bash tests/check-gb-save175.sh
gcc -DCB_GB_SAVE176=1 -DCB_GB_SAVE175=1 -DCB_ROM164=1 -DCB_LANG174=1 \
 -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g \
 -I.integration/openfpgaos/src/firmware/api tests/test-gb-save175-app.c \
 app/ui98/ui.c app/ui98/model.c app/ui98/font.c app/ui98/analysis.c \
 -o build/gb-save176-test/app
build/gb-save176-test/app
