#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/save158
verilator --cc --exe --build -j 4 -Wno-fatal \
 +define+CHISBURNER_TIMERS135 +define+CHISBURNER_SRAM137 \
 +define+CHISBURNER_SRAM138 +define+CHISBURNER_SAVE139 \
 +define+CHISBURNER_SAVE158 \
 -CFLAGS '-DTEST_SRAM137 -DTEST_SRAM138 -DTEST_SAVE139 -DTEST_SAVE158' \
 --top-module chisburner_nor_bus108 --Mdir build/save158/bus \
 core/rtl/chisburner_nor_bus108.v "$PWD/tests/test-nor108-bus.cpp" \
 >build/save158/bus-build.log 2>&1
build/save158/bus/Vchisburner_nor_bus108
