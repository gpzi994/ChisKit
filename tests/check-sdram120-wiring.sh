#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/sdram120
verilator --cc --exe --build -j 4 --top-module chisburner_sdram_pocket120 \
 --Mdir build/sdram120/wiring core/rtl/chisburner_sdram_pocket120.v \
 core/rtl/chisburner_sdram_read117.v core/rtl/chisburner_sdram_control116.v \
 core/rtl/chisburner_sdram_interlock119.v core/rtl/chisburner_sdram_ingress113.v \
 core/rtl/chisburner_sdram_egress114.v core/rtl/chisburner_sdram_diag124.v "$PWD/tests/test-sdram120-wiring.cpp" \
 >build/sdram120/wiring-build.log 2>&1
build/sdram120/wiring/Vchisburner_sdram_pocket120
