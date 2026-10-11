#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/timers135-tests
for p in 1 2 3 18 31 32 33 36 63 64 65 127 128; do
 iverilog -g2012 -DCHISBURNER_TIMERS135 -s tb_timers135 -Ptb_timers135.P=$p \
 -o build/timers135-tests/boundary tests/tb_timers135.sv core/rtl/chisburner_nor_bus108.v
 vvp build/timers135-tests/boundary
done
verilator --cc --exe --build -j 4 -Wno-fatal +define+CHISBURNER_TIMERS135 \
 --top-module chisburner_nor_bus108 --Mdir build/timers135-tests/bus \
 core/rtl/chisburner_nor_bus108.v "$PWD/tests/test-nor108-bus.cpp" \
 >build/timers135-tests/bus-build.log 2>&1
build/timers135-tests/bus/Vchisburner_nor_bus108
rtl=.integration/openfpgaos/src/fpga/targets/pocket
verilator --cc --exe --build -j 4 -Wno-fatal +define+CHISBURNER_TIMERS135 \
 +define+CHISBURNER_GB_IDENTIFY134 +define+CHISBURNER_GBA_IDENTIFY133 +define+CHISBURNER_PROGRAM131 \
 --top-module chisburner_cart_auto98 --Mdir build/timers135-tests/dump \
 core/rtl/chisburner_cart_auto98.v "$rtl/chisburner_cart_gb_mbc5_fullrom.v" \
 "$rtl/chisburner_cart_gba_readonly.v" "$PWD/tests/test-ui107-dump-rtl.cpp" \
 >build/timers135-tests/dump-build.log 2>&1
for kind in {0..7}; do build/timers135-tests/dump/Vchisburner_cart_auto98 "$kind"; done
