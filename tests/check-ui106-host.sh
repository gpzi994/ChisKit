#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build
gcc -D_DEFAULT_SOURCE -std=c11 -Wall -Wextra -Werror -Wno-unused-function \
 -Wno-misleading-indentation -fsanitize=address,undefined -Iapp/ui98 \
 -I.integration/openfpgaos/src/firmware/api tests/test-ui106-dump.c \
 app/ui98/analysis.c app/ui98/model.c app/ui98/ui.c app/ui98/font.c -o build/ui106-dump-host
build/ui106-dump-host "${1:?Crystal reference ROM}" "${2:?Zelda reference ROM}"
