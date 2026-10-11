#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
rtl=.integration/openfpgaos/src/fpga/targets/pocket
files=(core/rtl/chisburner_cart_auto98.v "$rtl/chisburner_cart_gb_readonly.v" "$rtl/chisburner_cart_gb_readonly_abstract.v" "$rtl/chisburner_cart_gb_mbc5_fullrom.v" "$rtl/chisburner_cart_gb_mbc5_identify.v" "$rtl/chisburner_cart_gba_readonly.v" "$rtl/chisburner_cart_gba_s29gl256.v")
for t in header program mapper3 mapper5 geom; do
 test=tests/test-ui98-rtl.cpp
 flags=()
 case "$t" in
 program) test=tests/test-ui98-program-rtl.cpp;;
 mapper3) test=tests/test-ui98-mapper-rtl.cpp; flags=(-CFLAGS "-DMODEL_MBC3");;
 mapper5) test=tests/test-ui98-mapper-rtl.cpp; flags=(-CFLAGS "-DMODEL_MBC5");;
 geom) test=tests/test-ui98-geom-rtl.cpp;;
 esac
 verilator --cc --exe --build -j 4 -Wno-fatal --top-module chisburner_cart_auto98 "${flags[@]}" --Mdir "build/ui98-$t" "${files[@]}" "$PWD/$test" > "build/ui98-$t.log" 2>&1
 "build/ui98-$t/Vchisburner_cart_auto98"
done
