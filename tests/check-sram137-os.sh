#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/sram137
gcc -D_GNU_SOURCE -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -Icore/nor -I.integration/openfpgaos/src/firmware/api \
 tests/test-sram137-os.c core/nor/recipe.c \
 core/nor/mori/chisburner_nor_db_data.c core/nor/mori/chisburner_nor_db_lookup.c \
 -o build/sram137/test-os
build/sram137/test-os
