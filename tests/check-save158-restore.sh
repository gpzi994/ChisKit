#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save158
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -Icore tests/test-save158-restore.c core/save/restore158.c \
 -o build/save158/restore-test
build/save158/restore-test
