#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/sdram114
verilator --cc --exe --build -j 4 --top-module chisburner_sdram_egress114 \
 --Mdir build/sdram114/egress core/rtl/chisburner_sdram_egress114.v \
 "$PWD/tests/test-sdram114-egress.cpp" >build/sdram114/egress-build.log 2>&1
build/sdram114/egress/Vchisburner_sdram_egress114
