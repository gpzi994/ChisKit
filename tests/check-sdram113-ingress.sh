#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/sdram113
for bits in 4 10; do
 verilator --cc --exe --build -j 4 --top-module chisburner_sdram_ingress113 \
  -GFIFO_BITS="$bits" -CFLAGS "-DFIFO_WORDS=$((1<<bits))" \
  --Mdir "build/sdram113/ingress-$bits" \
  core/rtl/chisburner_sdram_ingress113.v "$PWD/tests/test-sdram113-ingress.cpp" \
  >"build/sdram113/ingress-$bits-build.log" 2>&1
 "build/sdram113/ingress-$bits/Vchisburner_sdram_ingress113"
done
