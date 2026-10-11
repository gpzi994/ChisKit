#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/sdram117
verilator --cc --exe --build -j 4 --top-module chisburner_sdram_read117 \
 --Mdir build/sdram117/driver core/rtl/chisburner_sdram_read117.v \
 core/rtl/chisburner_sdram_control116.v core/rtl/chisburner_sdram_egress114.v \
 "$PWD/tests/test-sdram117-driver.cpp" >build/sdram117/driver-build.log 2>&1
build/sdram117/driver/Vchisburner_sdram_read117
