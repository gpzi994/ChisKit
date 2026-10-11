#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/eeprom162-regs
verilator --cc --exe --build -j 4 -Wno-fatal \
 --top-module chisburner_eeprom_regs139 --Mdir build/eeprom162-regs \
 core/rtl/chisburner_eeprom_regs139.v "$PWD/tests/test-eeprom162-regs.cpp" \
 >build/eeprom162-regs/build.log 2>&1
build/eeprom162-regs/Vchisburner_eeprom_regs139
