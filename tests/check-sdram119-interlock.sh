#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/sdram119
verilator --cc --exe --build -j 4 --top-module chisburner_sdram_interlock119 \
 -GTIMEOUT_CYCLES=1024 --Mdir build/sdram119/interlock \
 core/rtl/chisburner_sdram_interlock119.v "$PWD/tests/test-sdram119-interlock.cpp" \
 >build/sdram119/interlock-build.log 2>&1
build/sdram119/interlock/Vchisburner_sdram_interlock119
