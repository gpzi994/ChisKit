#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save139
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -Iapp/ui98 -I.integration/openfpgaos/src/firmware/api \
 tests/test-eeprom161-backup.c -o build/save139/eeprom-backup161-test
build/save139/eeprom-backup161-test
