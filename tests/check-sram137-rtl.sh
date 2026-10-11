#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/sram137
verilator --cc --exe --build -j 4 -Wno-fatal \
 +define+CHISBURNER_TIMERS135 +define+CHISBURNER_SRAM137 -CFLAGS -DTEST_SRAM137 \
 --top-module chisburner_nor_bus108 --Mdir build/sram137/bus \
 core/rtl/chisburner_nor_bus108.v "$PWD/tests/test-nor108-bus.cpp" \
 >build/sram137/bus-build.log 2>&1
build/sram137/bus/Vchisburner_nor_bus108
