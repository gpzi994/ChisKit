#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-mbc7-restore185-test
rtl_extra=()
c_extra=''
if [ "${1:-}" = rtc191 ]; then rtl_extra=(-DCHISBURNER_GB_RTC191); c_extra=' -DTEST_GB191'; fi
verilator --cc --exe --build -j 4 -Wno-fatal \
 "${rtl_extra[@]}" \
 -DCHISBURNER_GB_RESTORE185 -DCHISBURNER_GB_RESTORE180 -DCHISBURNER_GB_SAVE179 -DCHISBURNER_GB_SAVE177 -DCHISBURNER_GB_SAVE175 -DCHISBURNER_SRAM137 -DCHISBURNER_SRAM138 \
 -DCHISBURNER_SAVE139 -DCHISBURNER_SAVE158 -DCHISBURNER_TIMERS135 \
 -CFLAGS "-DTEST_GB185 -DTEST_GB180 -DTEST_GB179 -DTEST_GB177 -DTEST_GB175 -DTEST_SRAM137 -DTEST_SRAM138 -DTEST_SAVE139 -DTEST_SAVE158$c_extra" \
 --top-module chisburner_nor_bus108 --Mdir build/gb-mbc7-restore185-test/rtl \
 core/rtl/chisburner_nor_bus108.v "$PWD/tests/test-nor108-bus.cpp" >build/gb-mbc7-restore185-test/rtl-build.log 2>&1
build/gb-mbc7-restore185-test/rtl/Vchisburner_nor_bus108
