#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save139
gcc -std=c11 -Wall -Wextra -Werror -g -fsanitize=address,undefined \
 -DOF_PC -I.integration/openfpgaos/src/firmware/api -Icore -Icore/nor -Icore/save tests/test-eeprom139-os.c core/save/eeprom139.c core/save/detect139.c -o build/save139/eeprom-os-test
build/save139/eeprom-os-test
