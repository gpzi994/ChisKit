#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save137
${CC:-gcc} -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g \
  core/save/save.c tests/save-core-test.c -o build/save137/save-core-test
./build/save137/save-core-test
