#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mori="${MORI_DB_DIR:-../../references/MoriBunrnner-for-ESP32-S3/main/burner/db}"
mkdir -p build/nor112
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -Icore/nor -I"$mori" tests/test-mori112-equivalence.c \
 "$mori/burner_nor_db_data.c" "$mori/burner_nor_db_lookup.c" \
 core/nor/mori/chisburner_nor_db_data.c core/nor/mori/chisburner_nor_db_lookup.c \
 -o build/nor112/test-mori-equivalence
build/nor112/test-mori-equivalence > build/nor112/gba-scope.tsv
