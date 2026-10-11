#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-save175-test
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g \
 tests/test-gb-save175.c core/gb_save175/gb_save175.c \
 -o build/gb-save175-test/gb-save175
build/gb-save175-test/gb-save175
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g -Icore \
 tests/test-gb-save175-os.c core/gb_save175/gb_save175.c core/gb_save175/mbc7179.c \
 -o build/gb-save175-test/gb-save175-os
build/gb-save175-test/gb-save175-os
gcc -DCB_GB_SAVE175=1 -DCB_ROM164=1 -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g \
 -I.integration/openfpgaos/src/firmware/api tests/test-gb-save175-app.c \
 app/ui98/ui.c app/ui98/model.c app/ui98/font.c app/ui98/analysis.c \
 -o build/gb-save175-test/gb-save175-app
build/gb-save175-test/gb-save175-app
