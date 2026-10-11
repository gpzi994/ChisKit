#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/nor109
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -Icore/nor \
 tests/test-nor109-board.c core/nor/board109.c -o build/nor109/test-board
build/nor109/test-board
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -Icore/nor \
 tests/test-nor109-board-probe.c core/nor/board109.c core/nor/board_probe109.c core/nor/cfi.c \
 core/nor/mori/chisburner_nor_db_lookup.c core/nor/mori/chisburner_nor_db_data.c \
 -o build/nor109/test-board-probe
build/nor109/test-board-probe
gcc -DCART_NOR109=1 -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -Icore/nor -I.integration/openfpgaos/src/firmware/api tests/test-nor108-os.c \
 core/nor/recipe.c core/nor/board109.c core/nor/mori/chisburner_nor_db_data.c \
 core/nor/mori/chisburner_nor_db_lookup.c -o build/nor109/test-os-board
build/nor109/test-os-board
