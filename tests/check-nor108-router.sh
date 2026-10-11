#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
rtl=.integration/openfpgaos/src/fpga/targets/pocket
mkdir -p build/nor108
files=(core/rtl/chisburner_cart_auto108.v core/rtl/chisburner_nor_bus108.v core/rtl/chisburner_cart_auto98.v
 "$rtl/chisburner_cart_gb_readonly.v" "$rtl/chisburner_cart_gb_readonly_abstract.v"
 "$rtl/chisburner_cart_gb_mbc5_fullrom.v" "$rtl/chisburner_cart_gb_mbc5_identify.v"
 "$rtl/chisburner_cart_gba_readonly.v" "$rtl/chisburner_cart_gba_s29gl256.v")
targets=(private header program mapper3 mapper5 geom dump)
extra=()
suffix=""
if [ "${PROGRAM131:-0}" = 1 ]; then
 extra=(-DCHISBURNER_PROGRAM131)
 suffix="-stream131"
 for name in compare_engine131 program_path131 stream_regs131 nor_program131 nor_buffer131 nor_poll131 raw_adapter131 psram_stream131 psram_word_reader131 read_cdc131 psram_cache129; do
  files+=("core/rtl/chisburner_${name}.v")
 done
fi
if [ "$#" -gt 0 ]; then targets=("$@"); fi
if [ "${EEPROM139:-0}" = 1 ]; then
 if [ "${PROGRAM131:-0}" != 1 ]; then echo "EEPROM139 requires PROGRAM131" >&2; exit 1; fi
 extra+=(-DCHISBURNER_EEPROM139)
 suffix="$suffix-eeprom139"
 for name in eeprom139 eeprom_pins139 eeprom_regs139 eeprom_path139; do
  files+=("core/rtl/chisburner_${name}.v")
 done
fi
if [ "${SRAM137:-0}" = 1 ]; then
 extra+=(-DCHISBURNER_SRAM137 -DCHISBURNER_TIMERS135 -DCHISBURNER_GBA_IDENTIFY133 -DCHISBURNER_GB_IDENTIFY134)
 suffix="$suffix-sram137"
fi
for t in "${targets[@]}"; do
 test=tests/test-ui98-rtl.cpp
 flags="-I$PWD/tests/nor108_alias"
 if [ "${EEPROM139:-0}" = 1 ]; then flags="$flags -DTEST_EEPROM139"; fi
 if [ "${SRAM137:-0}" = 1 ]; then flags="$flags -DTEST_SRAM137"; fi
 if [ "${PROGRAM131:-0}" = 1 ]; then flags="$flags -DPROGRAM131_TEST"; fi
 case "$t" in
 private) test=tests/test-nor108-router.cpp;;
 program) test=tests/test-ui98-program-rtl.cpp;;
 mapper3) test=tests/test-ui98-mapper-rtl.cpp; flags="$flags -DMODEL_MBC3";;
 mapper5) test=tests/test-ui98-mapper-rtl.cpp; flags="$flags -DMODEL_MBC5";;
 geom) test=tests/test-ui98-geom-rtl.cpp;;
 dump) test=tests/test-ui107-dump-rtl.cpp;;
 esac
 verilator --cc --exe --build -j 4 -Wno-fatal --top-module chisburner_cart_auto108 \
  "${extra[@]}" -CFLAGS "$flags" --Mdir "build/nor108/router-$t$suffix" "${files[@]}" "$PWD/$test" \
  > "build/nor108/router-$t.log" 2>&1
 if [ "$t" = dump ]; then
  for kind in {0..7}; do "build/nor108/router-$t$suffix/Vchisburner_cart_auto108" "$kind"; done
 else
  "build/nor108/router-$t$suffix/Vchisburner_cart_auto108"
 fi
done
