#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-save177-test
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g \
 tests/test-gb-save177.c core/gb_save175/gb_save175.c -o build/gb-save177-test/core
build/gb-save177-test/core
bash tests/check-gb-save176.sh
