#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save139
verilator --cc --exe --build -j 4 -Wno-fatal \
 --top-module chisburner_eeprom139 --Mdir build/save139/eeprom \
 core/rtl/chisburner_eeprom139.v "$PWD/tests/test-eeprom139-rtl.cpp" \
 >build/save139/eeprom-build.log 2>&1
build/save139/eeprom/Vchisburner_eeprom139
