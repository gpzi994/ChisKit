#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-save177-test
verilator --cc --exe --build -j 4 -Wno-fatal \
 -DCHISBURNER_GB_SAVE177 -DCHISBURNER_GB_SAVE175 -DCHISBURNER_SRAM137 -DCHISBURNER_SRAM138 \
 -DCHISBURNER_SAVE139 -DCHISBURNER_SAVE158 -DCHISBURNER_TIMERS135 \
 -CFLAGS '-DTEST_GB177 -DTEST_GB175 -DTEST_SRAM137 -DTEST_SRAM138 -DTEST_SAVE139 -DTEST_SAVE158' \
 --top-module chisburner_nor_bus108 --Mdir build/gb-save177-test/rtl \
 core/rtl/chisburner_nor_bus108.v "$PWD/tests/test-nor108-bus.cpp" \
 >build/gb-save177-test/rtl-build.log 2>&1
build/gb-save177-test/rtl/Vchisburner_nor_bus108
bash tests/check-nor108-bus.sh
