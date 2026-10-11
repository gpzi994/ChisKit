#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save139
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -Icore -Icore/nor \
 tests/test-save-auto139.c core/save/eeprom139.c core/save/detect139.c -o build/save139/auto-test
build/save139/auto-test
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 tests/test-eeprom139.c core/save/eeprom139.c -o build/save139/eeprom-test
build/save139/eeprom-test
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 tests/test-save-detect139.c core/save/detect139.c -o build/save139/detect-test
build/save139/detect-test
