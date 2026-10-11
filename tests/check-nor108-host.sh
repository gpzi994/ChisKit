#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/nor108
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -Icore/nor tests/test-nor108.c core/nor/cfi.c \
 core/nor/mori/chisburner_nor_db_data.c core/nor/mori/chisburner_nor_db_lookup.c \
 -o build/nor108/test-nor108
build/nor108/test-nor108
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -Icore/nor tests/test-nor108-recipe.c core/nor/recipe.c \
 core/nor/mori/chisburner_nor_db_data.c core/nor/mori/chisburner_nor_db_lookup.c \
 -o build/nor108/test-nor108-recipe
build/nor108/test-nor108-recipe
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -Icore/nor tests/test-nor108-execute.c core/nor/execute.c core/nor/recipe.c \
 core/nor/mori/chisburner_nor_db_data.c core/nor/mori/chisburner_nor_db_lookup.c \
 -o build/nor108/test-nor108-execute
build/nor108/test-nor108-execute
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -Icore/nor tests/test-nor108-probe.c core/nor/probe.c core/nor/cfi.c core/nor/recipe.c \
 core/nor/mori/chisburner_nor_db_data.c core/nor/mori/chisburner_nor_db_lookup.c \
 -o build/nor108/test-nor108-probe
build/nor108/test-nor108-probe
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -Icore/nor -I.integration/openfpgaos/src/firmware/api tests/test-nor108-os.c core/nor/recipe.c \
 core/nor/mori/chisburner_nor_db_data.c core/nor/mori/chisburner_nor_db_lookup.c \
 -o build/nor108/test-nor108-os
build/nor108/test-nor108-os
