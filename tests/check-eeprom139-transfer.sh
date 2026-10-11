#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save139
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined \
 -I.integration/openfpgaos/src/firmware/api tests/test-eeprom139-transfer.c \
 -o build/save139/eeprom-transfer-test
build/save139/eeprom-transfer-test
