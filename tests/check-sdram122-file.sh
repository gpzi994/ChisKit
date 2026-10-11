#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/sdram122
g++ -std=c++17 -Wall -Wextra -Werror -fsanitize=address,undefined \
 tests/test-sdram122-file.cpp -o build/sdram122/test-file
build/sdram122/test-file
