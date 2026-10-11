#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-rtc187-tests
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g -I.integration/openfpgaos/src/firmware/api tests/test-gb-rtc187.c -o build/gb-rtc187-tests/app
build/gb-rtc187-tests/app
bash tests/check-gb-rtc186.sh
bash tests/check-lang174.sh
