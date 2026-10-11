#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/nor108
gcc -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra -Werror -Wno-unused-function \
 -Wno-misleading-indentation -fsanitize=address,undefined -Iapp/ui98 \
 -I.integration/openfpgaos/src/firmware/api tests/test-ui108-write.c \
 app/ui98/analysis.c app/ui98/model.c app/ui98/ui.c app/ui98/font.c -o build/nor108/test-ui108-write
build/nor108/test-ui108-write "${1:?GBA reference ROM for its header}"
