#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save139
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 tests/test-save-flash139.c core/save/probe139.c core/save/flash139.c \
 -o build/save139/flash-test
build/save139/flash-test
gcc -std=c11 -D_GNU_SOURCE -Wall -Wextra -Werror -g -fsanitize=address,undefined \
 -Icore -Icore/nor -Icore/save -I.integration/openfpgaos/src/firmware/api \
 tests/test-save139-os.c core/save/probe139.c core/save/flash139.c core/nor/recipe.c \
 core/nor/mori/chisburner_nor_db_data.c core/nor/mori/chisburner_nor_db_lookup.c \
 -o build/save139/os-test
build/save139/os-test
