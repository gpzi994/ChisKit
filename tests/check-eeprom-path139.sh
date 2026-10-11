#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save139
verilator --cc --exe --build -j 4 -Wno-fatal \
 --top-module chisburner_eeprom_path139 --Mdir build/save139/eeprom-path \
 core/rtl/chisburner_eeprom139.v core/rtl/chisburner_eeprom_pins139.v \
 core/rtl/chisburner_eeprom_regs139.v core/rtl/chisburner_eeprom_path139.v \
 "$PWD/tests/test-eeprom-path139.cpp" >build/save139/eeprom-path-build.log 2>&1
build/save139/eeprom-path/Vchisburner_eeprom_path139
