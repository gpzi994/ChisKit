#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/nor110
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -Icore/nor \
 tests/test-nor110-bank.c core/nor/bank110.c -o build/nor110/test-bank
build/nor110/test-bank
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -Icore/nor \
 tests/test-nor110-bank-probe.c core/nor/bank110.c core/nor/bank_probe110.c core/nor/recipe.c \
 core/nor/mori/chisburner_nor_db_lookup.c core/nor/mori/chisburner_nor_db_data.c \
 -o build/nor110/test-bank-probe
build/nor110/test-bank-probe
gcc -DCART_NOR109=1 -DCART_NOR110=1 -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -Icore/nor -I.integration/openfpgaos/src/firmware/api tests/test-nor108-os.c \
 core/nor/recipe.c core/nor/board109.c core/nor/bank110.c core/nor/bank_probe110.c \
 core/nor/mori/chisburner_nor_db_lookup.c core/nor/mori/chisburner_nor_db_data.c \
 -o build/nor110/test-os
build/nor110/test-os
