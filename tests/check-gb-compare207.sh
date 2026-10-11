#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build/gb-compare207
iverilog -g2012 -Wall -s tb_gb_compare207 -o build/gb-compare207/gb.vvp \
 core/rtl/chisburner_gb_compare207.v tests/tb-gb-compare207.v \
 > build/gb-compare207/compile.log 2>&1
vvp build/gb-compare207/gb.vvp | tee build/gb-compare207/run.log
