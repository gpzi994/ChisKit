#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/eeprom162-test
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=undefined \
 -Icore -Icore/nor -Icore/save -I.integration/openfpgaos/src/firmware/api \
 tests/test-eeprom162-service.c -o build/eeprom162-test/service
build/eeprom162-test/service
