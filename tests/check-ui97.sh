#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
: "${1:?Supply a local GBA ROM path for header-only fixture testing}"
mkdir -p build/ui97-preview
gcc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g \
 -Iapp/ui97 -I.integration/openfpgaos/src/firmware/api tests/test-ui97.c \
 app/ui97/model.c app/ui97/ui.c app/ui97/font.c -o build/test-ui97
./build/test-ui97 "$1"
rtl=.integration/openfpgaos/src/fpga/targets/pocket
verilator --cc --exe --build -j 4 -Wno-fatal --top-module chisburner_cart_auto97 \
 --Mdir build/ui97-rtl core/rtl/chisburner_cart_auto97.v \
 "$rtl/chisburner_cart_gb_readonly.v" "$rtl/chisburner_cart_gb_readonly_abstract.v" \
 "$rtl/chisburner_cart_gba_readonly.v" "$PWD/tests/test-ui97-rtl.cpp" > build/ui97-rtl.log 2>&1
build/ui97-rtl/Vchisburner_cart_auto97
verilator --cc --exe --build -j 4 -Wno-fatal --top-module chisburner_cart_gba_s29gl256 \
 --Mdir build/ui97-gba-regress "$rtl/chisburner_cart_gba_readonly.v" \
 "$rtl/chisburner_cart_gba_s29gl256.v" "$PWD/tests/test-program65-rtl.cpp" > build/ui97-gba-regress.log 2>&1
build/ui97-gba-regress/Vchisburner_cart_gba_s29gl256
for mbc in 3 5; do
 verilator --cc --exe --build -j 4 -Wno-fatal --top-module chisburner_cart_gb_mbc5_fullrom \
  -GMAPPER_AUTO_PROFILE=1 -GDISCARD_FIRST_BLOCK=1 -GADDRESS_SETUP_CYCLES=1 \
  -GSELECT_SETUP_CYCLES=1 -GDATA_WAIT_CYCLES=1 -GRELEASE_CYCLES=1 \
  -GWRITE_SETUP_CYCLES=1 -GWRITE_LOW_CYCLES=1 -GWRITE_RELEASE_CYCLES=1 \
  -CFLAGS "-DMODEL_MBC${mbc}" --Mdir "build/ui97-mbc${mbc}-regress" \
  "$rtl/chisburner_cart_gb_mbc5_fullrom.v" "$PWD/tests/test-gb-mapper-auto93-rtl.cpp" \
  > "build/ui97-mbc${mbc}-regress.log" 2>&1
 "build/ui97-mbc${mbc}-regress/Vchisburner_cart_gb_mbc5_fullrom"
done
sha256sum app/ui97/model.c app/ui97/model.h app/ui97/ui.c app/ui97/ui.h app/ui97/font.c \
 core/rtl/chisburner_cart_auto97.v "$rtl/chisburner_cart_gba_readonly.v" \
 "$rtl/chisburner_cart_gba_s29gl256.v" "$rtl/chisburner_cart_gb_readonly.v" \
 "$rtl/chisburner_cart_gb_readonly_abstract.v" "$rtl/chisburner_cart_gb_mbc5_fullrom.v" \
 tests/test-ui97.c tests/test-ui97-rtl.cpp tests/test-program65-rtl.cpp \
 tests/test-gb-mapper-auto93-rtl.cpp > build/ui97-tested-sources.sha256
echo 'UI97 host, dual-bus RTL, GBA programmer, MBC3/MBC5 regression: ALL PASS'
