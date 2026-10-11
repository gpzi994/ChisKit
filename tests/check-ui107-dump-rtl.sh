#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build
rtl=.integration/openfpgaos/src/fpga/targets/pocket
files=(core/rtl/chisburner_cart_auto98.v "$rtl/chisburner_cart_gb_readonly.v" "$rtl/chisburner_cart_gb_readonly_abstract.v" "$rtl/chisburner_cart_gb_mbc5_fullrom.v" "$rtl/chisburner_cart_gb_mbc5_identify.v" "$rtl/chisburner_cart_gba_readonly.v" "$rtl/chisburner_cart_gba_s29gl256.v")
verilator --cc --exe --build -j 4 -Wno-fatal --top-module chisburner_cart_auto98 --Mdir build/ui107-dump "${files[@]}" "$PWD/tests/test-ui107-dump-rtl.cpp" > build/ui107-dump.log 2>&1
for kind in {0..7}; do build/ui107-dump/Vchisburner_cart_auto98 "$kind"; done
