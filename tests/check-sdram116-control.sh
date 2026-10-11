#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/sdram116
verilator --cc --exe --build -j 4 --top-module chisburner_sdram_control116 \
 -GTIMEOUT_CYCLES=16 --Mdir build/sdram116/control \
 core/rtl/chisburner_sdram_control116.v "$PWD/tests/test-sdram116-control.cpp" \
 >build/sdram116/control-build.log 2>&1
build/sdram116/control/Vchisburner_sdram_control116
