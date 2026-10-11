#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save139
verilator --cc --exe --build -j 4 -Wno-fatal \
 --top-module chisburner_eeprom_pins139 --Mdir build/save139/eeprom-pins \
 core/rtl/chisburner_eeprom139.v core/rtl/chisburner_eeprom_pins139.v \
 "$PWD/tests/test-eeprom-pins139.cpp" >build/save139/eeprom-pins-build.log 2>&1
build/save139/eeprom-pins/Vchisburner_eeprom_pins139
