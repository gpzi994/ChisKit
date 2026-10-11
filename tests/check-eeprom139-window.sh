#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save139
for capture in 0 1 3 9 18 19; do
verilator --cc --exe --build -j 4 -Wno-fatal -GRX_CAPTURE_CYCLES="$capture" \
 -CFLAGS "-DCAPTURE_CYCLES=$capture" \
 --top-module chisburner_eeprom139 --Mdir "build/save139/eeprom-window-$capture" \
 core/rtl/chisburner_eeprom139.v "$PWD/tests/test-eeprom139-window.cpp" \
 >"build/save139/eeprom-window-$capture-build.log" 2>&1
"build/save139/eeprom-window-$capture/Vchisburner_eeprom139" | tee "build/save139/eeprom-window-$capture-result.log"
done
