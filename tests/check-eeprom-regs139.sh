#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save139
verilator --cc --exe --build -j 4 -Wno-fatal \
 --top-module chisburner_eeprom_regs139 --Mdir build/save139/eeprom-regs \
 core/rtl/chisburner_eeprom_regs139.v "$PWD/tests/test-eeprom-regs139.cpp" \
 >build/save139/eeprom-regs-build.log 2>&1
build/save139/eeprom-regs/Vchisburner_eeprom_regs139
