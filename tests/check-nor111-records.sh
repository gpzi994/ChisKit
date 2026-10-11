#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/nor111
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -Icore/nor \
 tests/audit-nor111-records.c core/nor/recipe.c \
 core/nor/mori/chisburner_nor_db_data.c core/nor/mori/chisburner_nor_db_lookup.c \
 -o build/nor111/audit-records
build/nor111/audit-records > build/nor111/records.tsv
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -Icore/nor \
 tests/test-mbcx111.c core/nor/mbcx111.c -o build/nor111/test-mbcx
build/nor111/test-mbcx
