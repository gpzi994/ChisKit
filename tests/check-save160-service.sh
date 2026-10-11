#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save160
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=undefined \
 -Icore tests/test-save160-service.c core/save/restore158.c \
 -o build/save160/service-test
build/save160/service-test
