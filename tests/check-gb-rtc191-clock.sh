#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-rtc191-tests
verilator --cc --exe --build -j 4 -Wno-fatal -DCHISBURNER_SAVE158 -DCHISBURNER_TIMERS135 -DCHISBURNER_GB_RTC191 \
 --top-module chisburner_nor_bus108 --Mdir build/gb-rtc191-tests/clock \
 core/rtl/chisburner_nor_bus108.v "$PWD/tests/test-gb-rtc191-clock.cpp" >build/gb-rtc191-tests/clock-build.log 2>&1
build/gb-rtc191-tests/clock/Vchisburner_nor_bus108
